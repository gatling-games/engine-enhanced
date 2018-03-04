#include "Renderer.h"

#include <GL/gl3w.h>

#include <assert.h>

#include "Math/Random.h"
#include "RenderManager.h"
#include "ResourceManager.h"
#include "SceneManager.h"
#include "Utils/Clock.h"
#include "Scene/Transform.h"
#include "Scene/Shield.h"

Renderer::Renderer()
    : Renderer(Framebuffer::backbuffer())
{

}

Renderer::Renderer(Framebuffer* targetFramebuffer)
    : Renderer(std::vector<Framebuffer*> { targetFramebuffer })
{

}

Renderer::Renderer(std::vector<Framebuffer*> targetFramebuffers)
    : targetFramebuffers_(targetFramebuffers),
    gbufferTextures_(),
    gbufferFramebuffer_(),
    shadowMap_(),
    sceneUniformBuffer_(UniformBufferType::SceneBuffer),
    cameraUniformBuffer_(UniformBufferType::CameraBuffer),
    perDrawUniformBuffer_(UniformBufferType::PerDrawBuffer),
    terrainUniformBuffer_(UniformBufferType::TerrainBuffer),
    terrainDetailsUniformBuffer_(UniformBufferType::TerrainDetailsBuffer),
    skyTransmittanceLUT_(TextureFormat::RGB16F, 256, 256)
{
    fullScreenMesh_ = ResourceManager::instance()->load<Mesh>("Resources/Meshes/full_screen_mesh.mesh");

    // Load the shaders required for each render pass
    standardShader_ = ResourceManager::instance()->load<Shader>("Resources/Shaders/Standard.shader");
    terrainShader_ = ResourceManager::instance()->load<Shader>("Resources/Shaders/Terrain.shader");
    terrainDetailMeshShader_ = ResourceManager::instance()->load<Shader>("Resources/Shaders/TerrainDetail.shader");
    waterShader_ = ResourceManager::instance()->load<Shader>("Resources/Shaders/Water.shader");
    deferredAmbientOcclusionShader_ = ResourceManager::instance()->load<Shader>("Resources/Shaders/Deferred-AmbientOcclusion.shader");
    deferredLightingShader_ = ResourceManager::instance()->load<Shader>("Resources/Shaders/Deferred-Lighting.shader");
    deferredDebugShader_ = ResourceManager::instance()->load<Shader>("Resources/Shaders/Deferred-Debug.shader");

    // Load shield rendering resources
    shieldShader_ = ResourceManager::instance()->load<Shader>("Resources/Shaders/Shield.shader");
    shieldFlowTexture_ = ResourceManager::instance()->load<Texture>("Resources/Textures/shield_flow.tga");
    shieldOpacityTexture_ = ResourceManager::instance()->load<Texture>("Resources/Textures/shield_opacity.tga");
    shieldMesh_ = ResourceManager::instance()->load<Mesh>("Resources/Meshes/sphere.obj");

    // Load skybox shader and mesh
    skyboxShader_ = ResourceManager::instance()->load<Shader>("Resources/Shaders/SkyboxPass.shader");
    skyboxMesh_ = ResourceManager::instance()->load<Mesh>("Resources/Meshes/skybox.obj");
    skyTransmittanceShader_ = ResourceManager::instance()->load<Shader>("Resources/Shaders/Sky/PrecomputeTransmittance.shader");

    // Generate the sky transmittance lut on startup.
    // It should be ok for the entire app lifetime and shouldn't need to be remade.
    regenerateSkyTransmittanceLUT();

    // Generate the random poisson disks on startup.
    for (int i = 0; i < 16; ++i)
    {
        const Vector2 disk = random_in_unit_circle();
        poissonDisks_[i] = Vector4(disk.x, disk.y, 0.0f, 0.0f);
    }

    // Find the depth texture id for each framebuffer
    // They are needed for the render loop later.
    for(Framebuffer* framebuffer : targetFramebuffers_)
    {
        GLint depthTexture;
        glGetNamedFramebufferAttachmentParameteriv(framebuffer->glid(), GL_DEPTH_ATTACHMENT, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &depthTexture);
        framebufferDepthTextures_.push_back(depthTexture);
    }
}

Renderer::~Renderer()
{
    destroyGBuffer();
}

void Renderer::renderFrame(const Camera* camera)
{
    // Ensure the correct uniform buffers are bound
    sceneUniformBuffer_.use();
    cameraUniformBuffer_.use();
    perDrawUniformBuffer_.use();
    terrainUniformBuffer_.use();

    // Ensure the contents of the uniform buffers is up to date
    // The per-draw buffer is handled separately
    updateSceneUniformBuffer();

    // Compute the aspect ratio using one of the framebuffers
    // All of the framebuffers are the same size anyway
    const float aspectRatio = targetFramebuffers_[0]->width() / (float)targetFramebuffers_[0]->height();

    // Render the shadow map prior to the main render passes
    if (RenderManager::instance()->filterFeatureList(SF_Shadows | SF_DebugShadows | SF_DebugShadowCascades) != 0)
    {
        shadowMap_.updatePosition(camera, aspectRatio);
        shadowMap_.bind();

        for (int cascade = 0; cascade < ShadowMap::CASCADE_COUNT; ++cascade)
        {
            shadowMap_.cascadeFramebuffer(cascade).use();
            updateCameraUniformBuffer(shadowMap_.cascadeCamera(cascade), 0);
            executeGeometryPass(shadowMap_.cascadeCamera(cascade), SF_HighTessellation);
        }
    }

    // Ensure the gbuffer exists and is ok
    createGBuffer();

    // Bind the gbuffer textures for sampling in deferred passes
    // The gbuffer textures are shared between eyes, but the depth
    // textures are not, so bind the correct ones later.
    for (int i = 0; i < GBUFFER_RENDER_TARGETS; ++i)
    {
        // Slot 15 is reserved for the depth texture.
        // Start with gbuffer0 in slot 14, gbuffer1 in slot 13, etc.
        gbufferTextures_[i]->bind(14 - i);
    }

    // Draw each of the bound framebuffers
    // There is one per eye, so either 1 (no vr) or 2 (vr).
    for (unsigned int fb = 0; fb < targetFramebuffers_.size(); ++fb)
    {
        // Set the camera parameters for the current camera + eye
        updateCameraUniformBuffer(camera, fb);

        // Each target framebuffer uses a different depth texture, so
        // bind the correct one to slot 15
        glActiveTexture(GL_TEXTURE15);
        glBindTexture(GL_TEXTURE_2D, framebufferDepthTextures_[fb]);

        // Wireframe debugging mode needs to be handled separately.
        if (RenderManager::instance()->debugMode() == RenderDebugMode::Wireframe)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

            // We want to render directly to the target framebuffer
            targetFramebuffers_[fb]->use();

            // Normally, the guffer pass only needs to clear depth, not colour.
            // When rendering a wireframe we need to clear the color too
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            executeGeometryPass(camera, ALL_SHADER_FEATURES);
            executeWaterPass();

            // Ensure wireframe rendering is turned off again
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            return;
        }

        // Render each opaque object into the gbuffer textures
        gbufferFramebuffer_.use();
        executeGeometryPass(camera, ALL_SHADER_FEATURES);

        // Render ambient occlusion into the gbuffer, before computing lighting
        if (RenderManager::instance()->isFeatureGloballyEnabled(SF_AmbientOcclusion))
        {
            executeDeferredAmbientOcclusionPass();
        }

        // Now, we need to combine deferred lighting, sky, water etc into the target framebuffer
        targetFramebuffers_[fb]->use();

        // If we are not rendering the sky, clear the screen to black
        if (RenderManager::instance()->isFeatureGloballyEnabled(SF_Sky) == false)
        {
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
        }

        // Compute lighting into final render target
        executeDeferredLightingPass();

        // Render the water on top of the geometry using alpha blending
        executeWaterPass();

        // Show any debugging modes
        if (RenderManager::instance()->debugMode() != RenderDebugMode::None)
        {
            executeDeferredDebugPass();
        }

        // Finally render the skybox
        if (RenderManager::instance()->isFeatureGloballyEnabled(SF_Sky))
        {
            executeSkyboxPass(camera);
        }

        // Alpha blended shields are then rendered on top of the water and sky
        if (RenderManager::instance()->debugMode() == RenderDebugMode::None)
        {
            executeShieldPass();
        }
    }
}

void Renderer::createGBuffer()
{
    for (int fb = 0; fb < targetFramebuffers_.size(); ++fb)
    {
        // If there is already an ok gbuffer, we dont need to do anything
        // Check with the first texture - if it is ok, the whole thing is ok.
        if (gbufferTextures_[0] != nullptr
            && gbufferTextures_[0]->width() == targetFramebuffers_[fb]->width()
            && gbufferTextures_[0]->height() == targetFramebuffers_[fb]->height())
        {
            // GBuffer already exists and is the correct resolution.
            return;
        }

        // First destroy any existing gbuffer textures
        destroyGBuffer();

        // Create the textures    
        gbufferTextures_[0] = new Texture(TextureFormat::RGBA8, targetFramebuffers_[fb]->width(), targetFramebuffers_[fb]->height());
        gbufferTextures_[1] = new Texture(TextureFormat::RGBA8, targetFramebuffers_[fb]->width(), targetFramebuffers_[fb]->height());
        assert(GBUFFER_RENDER_TARGETS == 2); // should be one higher than the last index

        // Set up the framebuffer
        // Use the target framebuffer's depth texture and the gbuffer textures
        gbufferFramebuffer_.attachDepthTextureFromFramebuffer(targetFramebuffers_[fb]);
        gbufferFramebuffer_.attachColorTexturesMRT(GBUFFER_RENDER_TARGETS, (const Texture**)gbufferTextures_);
    }
}

void Renderer::destroyGBuffer()
{
    if (gbufferTextures_[0] != nullptr)
    {
        for (int i = 0; i < GBUFFER_RENDER_TARGETS; ++i)
        {
            delete gbufferTextures_[i];
            gbufferTextures_[i] = nullptr;
        }
    }
}

void Renderer::updateSceneUniformBuffer() const
{
    const Scene* scene = SceneManager::instance()->currentScene();

    // Gather the new contents of the scene buffer
    SceneUniformData data;
    data.ambientLightColor = scene->ambientLight();
    data.lightDirectionIntensity = Vector4((scene->sunRotation() * Vector3(0.0f, 0.0f, -1.0f)).normalized(), scene->sunIntensity());
    data.fogDensity = scene->fogDensity();
    data.fogHeightFalloff = scene->fogHeightFalloff();
    data.ambientOcclusionDistance = scene->ambientOcclusionDistance();
    data.ambientOcclusionFalloff = scene->ambientOcclusionFalloff();

    // Copy the poisson disks into the scene data
    for(int disk = 0; disk < 16; ++disk)
    {
        data.ambientOcclusionPoissonDisks[disk] = poissonDisks_[disk];
    }

    // Send time to shader for cloud texture scrolling
    const float time = Clock::instance()->time();
    data.time = Vector4(time, 1.0f / time, 0.0f, 0.0f);

    // Update the uniform buffer
    sceneUniformBuffer_.update(data);
}

void Renderer::updateCameraUniformBuffer(const Camera* camera, int targetFrameBuffer) const
{
    // Find out the resolution and aspect ratio of the framebuffer
    const float width = (float)targetFramebuffers_[targetFrameBuffer]->width();
    const float height = (float)targetFramebuffers_[targetFrameBuffer]->height();
    const float aspect = width / height;

    // Gather the new contents of the camera buffer
    CameraUniformData data;
    data.screenResolution = Vector4(width, height, 1.0f / width, 1.0f / height);
    data.cameraPosition = Vector4(camera->gameObject()->transform()->positionWorld());

    // If VR
    if (targetFramebuffers_.size() == 2)
    {
        if (targetFrameBuffer == 0)
        {
            data.worldToClip = camera->getWorldToCameraMatrix(aspect, EyeType::LeftEye);
            data.clipToWorld = camera->getCameraToWorldMatrix(aspect, EyeType::LeftEye);
        }
        else if (targetFrameBuffer == 1)
        {
            data.worldToClip = camera->getWorldToCameraMatrix(aspect, EyeType::RightEye);
            data.clipToWorld = camera->getCameraToWorldMatrix(aspect, EyeType::RightEye);
        }
    }
    else
    {
        data.worldToClip = camera->getWorldToCameraMatrix(aspect);
        data.clipToWorld = camera->getCameraToWorldMatrix(aspect);
    }

    // Update the uniform buffer.
    cameraUniformBuffer_.update(data);
}

void Renderer::updatePerDrawUniformBuffer(const Matrix4x4 &localToWorld, const Material* material) const
{
    // Use the default material if none was specified
    if(material == nullptr)
    {
        material = ResourceManager::instance()->load<Material>("Resources/Materials/default.material");
    }

    // Gather the new contents of the per-draw buffer
    PerDrawUniformData data;
    data.localToWorld = localToWorld;
    data.colorSmoothness = material->color();
    data.colorSmoothness.a = material->smoothness();
    data.albedoTexture = (material->albedoTexture() == nullptr) ? 0 : material->albedoTexture()->bindlessHandle();
    data.normalMapTexture = (material->normalMapTexture() == nullptr) ? 0 : material->normalMapTexture()->bindlessHandle();

    // Update the uniform buffer.
    perDrawUniformBuffer_.update(data);
}

void Renderer::updateTerrainUniformBuffer(const Terrain* terrain) const
{
    TerrainUniformData data;
    data.terrainSize = Vector4(terrain->size().x, terrain->size().y, terrain->size().z, (float)terrain->layerCount());
    data.waterColorDepth = Vector4(terrain->waterColor().r, terrain->waterColor().g, terrain->waterColor().b, terrain->waterDepth());

    for (int i = 0; i < terrain->layerCount(); ++i)
    {
        const TerrainLayer& layer = terrain->layers()[i];

        data.terrainLayerBlendData[i].x = layer.altitudeBorder;
        data.terrainLayerBlendData[i].y = 1.0f / layer.altitudeTransition;
        data.terrainLayerBlendData[i].z = layer.slopeBorder;
        data.terrainLayerBlendData[i].w = layer.slopeHardness * 20.0f;
        data.terrainLayerScale[i].x = terrain->size().x / layer.textureTileSize.x;
        data.terrainLayerScale[i].y = terrain->size().z / layer.textureTileSize.y;
        data.terrainLayerColours[i] = layer.material->color();
        data.terrainTextures[i * 2] = (layer.material->albedoTexture() == nullptr) ? 0 : layer.material->albedoTexture()->bindlessHandle();
        data.terrainNormalMapTextures[i * 2] = (layer.material->normalMapTexture() == nullptr) ? 0 : layer.material->normalMapTexture()->bindlessHandle();
    }

    terrainUniformBuffer_.update(data);
}

void Renderer::updateTerrainDetailsUniformBuffer(const DetailBatch& details) const
{
    TerrainDetailsData detailsData;
    std::memcpy(detailsData.detailPositions, details.instancePositions, sizeof(Vector4) * details.count);
    terrainDetailsUniformBuffer_.update(detailsData);
}

void Renderer::executeGeometryPass(const Camera* camera, ShaderFeatureList shaderFeatures) const
{
    // Ensure that depth testing and depth write are on
    glEnable(GL_DEPTH_TEST);
    glDepthMask(true);

    // We only need to clear the depth buffer, and not the color buffer
    // This pass is rendering into the gbuffer and the non-rendered areas are not used.
    glClear(GL_DEPTH_BUFFER_BIT);

    // Draw every static mesh component in the scene with the standard shaders
    for (StaticMesh* staticMesh : SceneManager::instance()->staticMeshes())
    {
        // Skip instances with no material
        if (staticMesh->material() == nullptr || staticMesh->mesh() == nullptr)
        {
            continue;
        }

        // Use the correct standard shader variant
        standardShader_->bindVariant(staticMesh->material()->supportedFeatures() & shaderFeatures);

        // Set the correct mesh
        // Textures are bound via bindless texture handles
        staticMesh->mesh()->bind();

        // Update the per draw uniform buffer
        const Matrix4x4 localToWorld = staticMesh->gameObject()->transform()->localToWorld();
        updatePerDrawUniformBuffer(localToWorld, staticMesh->material());

        // Draw the mesh
        glDrawElements(GL_TRIANGLES, staticMesh->mesh()->elementsCount(), GL_UNSIGNED_SHORT, (void*)0);
    }

    // Draw terrain
    const Terrain* terrain = SceneManager::instance()->terrain();
    if (terrain != nullptr)
    {
        terrainShader_->bindVariant(shaderFeatures);

        //Set mesh and heightmap
        terrain->mesh()->bind();
        terrain->heightmap()->bind(8);
        updateTerrainUniformBuffer(terrain);

        // Render the terrain with tessellation
        glDrawElements(GL_PATCHES, terrain->mesh()->elementsCount(), GL_UNSIGNED_SHORT, (void*)0);
    }

    // Draw terrain details
    if (terrain != nullptr
        && (shaderFeatures & SF_TerrainDetailMeshes) != 0
        && RenderManager::instance()->isFeatureGloballyEnabled(SF_TerrainDetailMeshes)
        && terrain->detailMaterial() != nullptr
        && terrain->detailMesh() != nullptr)
    {
        // Use the terrain's detail mesh
        terrain->detailMesh()->bind();
        const int elementsCount = terrain->detailMesh()->elementsCount();

        // Use the terrain's detail material
        updatePerDrawUniformBuffer(Matrix4x4::identity(), terrain->detailMaterial());

        // Use the terrain's detail shader
        terrainDetailMeshShader_->bindVariant(terrain->detailMaterial()->supportedFeatures() & shaderFeatures);

        // Render each terrain details batch
        const Point3 cameraPosition = camera->gameObject()->transform()->positionWorld();
        const float distanceScale = RenderManager::instance()->isFeatureGloballyEnabled(SF_ExtraTerrainDetails) ? 6.0f : 1.0f;
        for (const DetailBatch& batch : terrain->detailBatches())
        {
            // Skip batches that are further than the draw distance
            if ((batch.bounds.centre() - cameraPosition).sqrMagnitude() > batch.drawDistance * batch.drawDistance * distanceScale)
            {
                continue;
            }

            // Draw the batch data using an instanced draw call
            updateTerrainDetailsUniformBuffer(batch);
            glDrawElementsInstanced(GL_TRIANGLES, elementsCount, GL_UNSIGNED_SHORT, (void*)0, batch.count);
        }
    }
}

void Renderer::executeFullScreen(Shader* shader, ShaderFeatureList shaderFeatures) const
{
    // "Full Screen" passes should write to all pixels that are not sky.
    // To do this, we render a full screen quad at the maximum depth
    // and only render where the quad is further than the depth buffer value
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_GREATER);

    // Ensure that we arent' writing depth
    glDepthMask(false);

    // Draw the full screen mesh
    fullScreenMesh_->bind();
    shader->bindVariant(shaderFeatures);
    glDrawElements(GL_TRIANGLES, fullScreenMesh_->elementsCount(), GL_UNSIGNED_SHORT, (void*)0);

    // Put the depth function back to normal
    glDepthFunc(GL_LESS);
}

void Renderer::executeDeferredAmbientOcclusionPass() const
{
    // We only want to render into the occlusion gbuffer channel (gbuffer 0 alpha).
    glColorMask(false, false, false, true);

    // Use min blending for alpha (= occlusion) so existing ao information is not lost
    glEnable(GL_BLEND);
    glBlendEquation(GL_MIN);

    // Render the ao shader full screen
    executeFullScreen(deferredAmbientOcclusionShader_, ALL_SHADER_FEATURES);

    // Reset the color mask when done
    glColorMask(true, true, true, true);

    // Reset blending state
    glDisable(GL_BLEND);
}

void Renderer::executeDeferredLightingPass() const
{
    executeFullScreen(deferredLightingShader_, ALL_SHADER_FEATURES);
}

void Renderer::executeDeferredDebugPass() const
{
    RenderDebugMode mode = RenderManager::instance()->debugMode();
    executeFullScreen(deferredDebugShader_, (ShaderFeatureList)mode | SF_SoftShadows);
}

void Renderer::executeWaterPass() const
{
    // This pass requires a terrain in the scene
    const Terrain* terrain = SceneManager::instance()->terrain();
    if (terrain == nullptr)
    {
        return;
    }

    // Ensure that depth testing and depth write are on
    glEnable(GL_DEPTH_TEST);
    glDepthMask(true);

    // Use alpha blending
    // The blend factor is output from the water fragment shader
    glEnable(GL_BLEND);
    glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

    // Render the terrain mesh, using the water shader, with tessellation
    waterShader_->bindVariant(ALL_SHADER_FEATURES);
    terrain->mesh()->bind();
    terrain->heightmap()->bind(8);
    glDrawElements(GL_PATCHES, terrain->mesh()->elementsCount(), GL_UNSIGNED_SHORT, (void*)0);

    // Reset blending state
    glDisable(GL_BLEND);
}

void Renderer::executeSkyboxPass(const Camera* camera) const
{
    // Ensure that depth testing is turned on, but dont write depth
    glEnable(GL_DEPTH_TEST);
    glDepthMask(false);

    // Ensure skybox shader is being used
    skyboxShader_->bindVariant(ALL_SHADER_FEATURES);

    // Ensure skybox mesh is being used
    skyboxMesh_->bind();

    // Bind the sky lookup textures
    skyTransmittanceLUT_.bind(5);

    // Compute scale for skydome - must ensure it's big enough without exceeding far clipping plane
    const float farPlane = camera->farPlane();
    const float skyboxScaleSqr = (1.0f / 3.0f) * farPlane * farPlane;
    const float skyboxScale = sqrtf(skyboxScaleSqr);
    const Matrix4x4 scaleMatrix = Matrix4x4::scale(Vector3(skyboxScale, skyboxScale, skyboxScale));

    // Compute position of skydome -
    // ensure the skybox is centered on the camera
    const Matrix4x4 translationMatrix = Matrix4x4::translation(camera->gameObject()->transform()->positionWorld());

    // Set the local to world matrix in per draw data
    PerDrawUniformData data;
    data.localToWorld = translationMatrix * scaleMatrix;
    perDrawUniformBuffer_.update(data);

    // Draw skybox mesh
    glDrawElements(GL_TRIANGLES, skyboxMesh_->elementsCount(), GL_UNSIGNED_SHORT, (void*)0);
}

void Renderer::executeShieldPass() const
{
    // Ensure that depth testing is turned on, but dont write depth
    glEnable(GL_DEPTH_TEST);
    glDepthMask(false);

    // Use alpha blending
    glEnable(GL_BLEND);
    glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

    // Use the shield texture and shield mesh
    shieldFlowTexture_->bind(6);
    shieldOpacityTexture_->bind(7);
    shieldMesh_->bind();

    // Use the shield shader
    shieldShader_->bindVariant(ALL_SHADER_FEATURES);

    // Render each shield
    for (const Shield* shield : SceneManager::instance()->shields())
    {
        // Create the shield matrix using the transform + radius.
        const Matrix4x4 transformMat = shield->gameObject()->transform()->localToWorld();
        const Matrix4x4 radiusMat = Matrix4x4::scale(Vector3(shield->radius(), shield->radius(), shield->radius()));
        const Matrix4x4 localToWorld = transformMat * radiusMat;

        // Draw the shield
        updatePerDrawUniformBuffer(localToWorld, nullptr);
        glDrawElements(GL_TRIANGLES, shieldMesh_->elementsCount(), GL_UNSIGNED_SHORT, (void*)0);
    }

    // Reset blending state
    glDisable(GL_BLEND);
}

void Renderer::regenerateSkyTransmittanceLUT()
{
    // Ensure the lut wrap + clamp settings are correct
    skyTransmittanceLUT_.setFilterMode(TextureFilterMode::Bilinear);
    skyTransmittanceLUT_.setWrapMode(TextureWrapMode::Clamp);

    // We run this process on the GPU.
    // The result is stored in the texture, so make a framebuffer for it.
    Framebuffer fbo;
    fbo.attachColorTexture(&skyTransmittanceLUT_);

    // Render to the fbo
    fbo.use();
    executeFullScreen(skyTransmittanceShader_, ALL_SHADER_FEATURES);
}
