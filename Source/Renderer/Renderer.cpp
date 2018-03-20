#include "Renderer.h"

#include <GL/gl3w.h>

#include <assert.h>

#include "Math/Random.h"
#include "RenderManager.h"
#include "ResourceManager.h"
#include "SceneManager.h"
#include "Utils/Clock.h"
#include "Scene/Transform.h"

Renderer::Renderer()
    : Renderer(Framebuffer::backbuffer())
{

}

Renderer::Renderer(const Framebuffer* targetFramebuffer)
    : targetFramebuffer_(targetFramebuffer),
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

    // Load skybox shader and mesh
    skyboxShader_ = ResourceManager::instance()->load<Shader>("Resources/Shaders/SkyboxPass.shader");
    skyboxMesh_ = ResourceManager::instance()->load<Mesh>("Resources/Meshes/skybox.obj");
    skyTransmittanceShader_ = ResourceManager::instance()->load<Shader>("Resources/Shaders/Sky/PrecomputeTransmittance.shader");

    // Generate the sky transmittance lut on startup.
    // It should be ok for the entire app lifetime and shouldn't need to be remade.
    regenerateSkyTransmittanceLUT();

    // Generate the random poisson disks on startup.
    for(int i = 0; i < 16; ++i)
    {
        const Vector2 disk = random_in_unit_circle();
        poissonDisks_[i] = Vector4(disk.x, disk.y, 0.0f, 0.0f);
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
    terrainDetailsUniformBuffer_.use();

    // Ensure the contents of the uniform buffers is up to date
    // The per-draw buffer is handled separately
    updateSceneUniformBuffer();
    updateCameraUniformBuffer(camera);

    // Wireframe debugging mode needs to be handled separately.
    if (RenderManager::instance()->debugMode() == RenderDebugMode::Wireframe)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        // We want to render directly to the target framebuffer
        targetFramebuffer_->use();

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

    // Render the shadow map prior to the main render passes
    if (RenderManager::instance()->filterFeatureList(SF_Shadows | SF_DebugShadows | SF_DebugShadowCascades) != 0)
    {
        shadowMap_.updatePosition(camera, targetFramebuffer_->width() / (float)targetFramebuffer_->height());
        shadowMap_.bind();
        for (int cascade = 0; cascade < ShadowMap::CASCADE_COUNT; ++cascade)
        {
            shadowMap_.cascadeFramebuffer(cascade).use();
            executeGeometryPass(shadowMap_.cascadeCamera(cascade), SF_HighTessellation | SF_Cutout);
        }
    }

    // Ensure the gbuffer exists and is ok
    createGBuffer();

    // Render each opaque object into the gbuffer textures
    gbufferFramebuffer_.use();
    executeGeometryPass(camera, ALL_SHADER_FEATURES);

    // Render ambient occlusion into the gbuffer, before computing lighting
    if (RenderManager::instance()->isFeatureGloballyEnabled(SF_AmbientOcclusion))
    {
        executeDeferredAmbientOcclusionPass();
    }

    // Compute lighting into final render target
    targetFramebuffer_->use();
    executeDeferredLightingPass();

    // Render the water on top of the geometry using alpha blending
    executeWaterPass();

    // Show any debugging modes
    if (RenderManager::instance()->debugMode() != RenderDebugMode::None)
    {
        executeDeferredDebugPass();
    }

    // Finally render the skybox
    executeSkyboxPass(camera);
}

void Renderer::createGBuffer()
{
    // If there is already an ok gbuffer, we dont need to do anything
    // Check with the first texture - if it is ok, the whole thing is ok.
    if (gbufferTextures_[0] != nullptr
        && gbufferTextures_[0]->width() == targetFramebuffer_->width()
        && gbufferTextures_[0]->height() == targetFramebuffer_->height())
    {
        // GBuffer already exists and is the correct resolution.
        return;
    }

    // First destroy any existing gbuffer textures
    destroyGBuffer();

    // Create the textures    
    gbufferTextures_[0] = new Texture(TextureFormat::RGBA8, targetFramebuffer_->width(), targetFramebuffer_->height());
    gbufferTextures_[1] = new Texture(TextureFormat::RGBA1010102, targetFramebuffer_->width(), targetFramebuffer_->height());
    assert(GBUFFER_RENDER_TARGETS == 2); // should be one higher than the last index

    // Bind the gbuffer textures for sampling in deferred passes
    for (int i = 0; i < GBUFFER_RENDER_TARGETS; ++i)
    {
        // Slot 15 is reserved for the depth texture.
        // Start with gbuffer0 in slot 14, gbuffer1 in slot 13, etc.
        gbufferTextures_[i]->bind(14 - i);
    }

    // Bind the depth texture as target 15.
    GLint depthTexture;
    glGetNamedFramebufferAttachmentParameteriv(targetFramebuffer_->glid(), GL_DEPTH_ATTACHMENT, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &depthTexture);
    glActiveTexture(GL_TEXTURE15);
    glBindTexture(GL_TEXTURE_2D, depthTexture);

    // Set up the framebuffer
    // Use the target framebuffer's depth texture and the gbuffer textures
    gbufferFramebuffer_.attachDepthTextureFromFramebuffer(targetFramebuffer_);
    gbufferFramebuffer_.attachColorTexturesMRT(GBUFFER_RENDER_TARGETS, (const Texture**)gbufferTextures_);
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

void Renderer::updateCameraUniformBuffer(const Camera* camera) const
{
    // Find out the resolution and aspect ratio of the framebuffer
    const float width = (float)targetFramebuffer_->width();
    const float height = (float)targetFramebuffer_->height();
    const float aspect = width / height;

    // Gather the new contents of the camera buffer
    CameraUniformData data;
    data.screenResolution = Vector4(width, height, 1.0f / width, 1.0f / height);
    data.cameraPosition = Vector4(camera->gameObject()->transform()->positionWorld());
    data.worldToClip = camera->getWorldToCameraMatrix(aspect);
    data.clipToWorld = camera->getCameraToWorldMatrix(aspect);

    // Update the uniform buffer.
    cameraUniformBuffer_.update(data);
}

void Renderer::updatePerDrawUniformBuffer(const Matrix4x4 &localToWorld, const Material* material) const
{
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
    updateCameraUniformBuffer(camera);

    // Ensure that depth testing and depth write are on
    glEnable(GL_DEPTH_TEST);
    glDepthMask(true);

    // First, clear the depth buffer - don't need to clear color buffer as skybox will cover background
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
    // Full screen passes don't use depth testing
    glDisable(GL_DEPTH_TEST);
    glDepthMask(false);

    // Draw the full screen mesh
    fullScreenMesh_->bind();
    shader->bindVariant(shaderFeatures);
    glDrawElements(GL_TRIANGLES, fullScreenMesh_->elementsCount(), GL_UNSIGNED_SHORT, (void*)0);
}

void Renderer::executeDeferredAmbientOcclusionPass() const
{
    // We only want to render into the occlusion gbuffer channel (gbuffer 0 alpha).
    glColorMask(false, false, false, true);

    // Render the ao shader full screen
    executeFullScreen(deferredAmbientOcclusionShader_, ALL_SHADER_FEATURES);

    // Reset the color mask when done
    glColorMask(true, true, true, true);
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
