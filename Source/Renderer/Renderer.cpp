#include "Renderer.h"

#include <GL/gl3w.h>

#include <assert.h>

#include "RenderManager.h"
#include "ResourceManager.h"
#include "SceneManager.h"
#include "Utils/Clock.h"

Renderer::Renderer()
    : Renderer(Framebuffer::backbuffer())
{

}

Renderer::Renderer(const Framebuffer* targetFramebuffer)
    : targetFramebuffer_(targetFramebuffer),
    gbufferTextures_(),
    gbufferFramebuffer_(),
    sceneUniformBuffer_(UniformBufferType::SceneBuffer),
    cameraUniformBuffer_(UniformBufferType::CameraBuffer),
    perDrawUniformBuffer_(UniformBufferType::PerDrawBuffer),
    terrainUniformBuffer_(UniformBufferType::TerrainBuffer)
{
    fullScreenMesh_ = ResourceManager::instance()->load<Mesh>("Resources/Meshes/full_screen_mesh.mesh");

    // Load the shaders required for each render pass
    standardShader_ = ResourceManager::instance()->load<Shader>("Resources/Shaders/Standard.shader");
    terrainShader_ = ResourceManager::instance()->load<Shader>("Resources/Shaders/Terrain.shader");
    deferredLightingShader_ = ResourceManager::instance()->load<Shader>("Resources/Shaders/Deferred-Lighting.shader");
    deferredDebugShader_ = ResourceManager::instance()->load<Shader>("Resources/Shaders/Deferred-Debug.shader");

    // Load skybox shader and mesh
    skyboxShader_ = ResourceManager::instance()->load<Shader>("Resources/Shaders/SkyboxPass.shader");
    skyboxMesh_ = ResourceManager::instance()->load<Mesh>("Resources/Meshes/skybox.obj");
    skyboxCloudThicknessTexture_ = ResourceManager::instance()->load<Texture>("Resources/Textures/cloud_thickness.psd");
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
    updateCameraUniformBuffer(camera);

    // Ensure the gbuffer exists and is ok
    createGBuffer();

    // Render each opaque object into the gbuffer textures
    gbufferFramebuffer_.use();
    executeDeferredGBufferPass();

    // Compute lighting into final render target
    targetFramebuffer_->use();
    executeDeferredLightingPass();

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
    // Gather the new contents of the scene buffer
    SceneUniformData data;
    data.ambientLightColor = Color(0.6f, 0.6f, 0.6f);
    data.lightColor = Color(1.0f, 1.0f, 1.0f);
    data.toLightDirection = Vector4(Vector3(1.0f, 1.0f, 1.0f).normalized());
    data.skyTopColor = Color(0.23f, 0.66f, 0.86f);
    data.skyHorizonColor = Color(0.5f, 0.55f, 0.84f);
    data.sunParams = Vector4(4.0f, 512.0f, 0.0f, 0.0f); // x = falloff, y = size

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

void Renderer::updatePerDrawUniformBuffer(const StaticMesh* draw, const Texture* albedoTexture, const Texture* normalMapTexture) const
{
    // Gather the new contents of the per-draw buffer
    PerDrawUniformData data;
    data.localToWorld = draw->gameObject()->transform()->localToWorld();
    data.colorSmoothness = draw->material()->color();
    data.colorSmoothness.a = draw->material()->smoothness();
    data.albedoTexture = (albedoTexture == nullptr) ? 0 : albedoTexture->bindlessHandle();
    data.normalMapTexture = (normalMapTexture == nullptr) ? 0 : normalMapTexture->bindlessHandle();

    // Update the uniform buffer.
    perDrawUniformBuffer_.update(data);
}

void Renderer::updateTerrainUniformBuffer(const Terrain* terrain) const
{
    TerrainUniformData data;
    Vector3 dimens = terrain->gameObject()->terrain()->terrainDimensions();
    data.terrainSize = Vector4(dimens.x, dimens.y, dimens.z, 1.0f);

    data.terrainCoordinateOffsetScale = Vector4(0.0f, 0.0f, 1.0f, 1.0f);
    Vector2 texScale = terrain->gameObject()->terrain()->textureWrapping();

    //Invert Texture scale
    data.textureScale = Vector4(dimens.x / texScale.x, dimens.z / texScale.y, 1.0f, 1.0f);
    terrainUniformBuffer_.update(data);
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

void Renderer::executeDeferredGBufferPass() const
{
    // Ensure that depth testing and depth write are on
    glEnable(GL_DEPTH_TEST);
    glDepthMask(true);

    // First, clear the depth buffer - don't need to clear color buffer as skybox will cover background
    glClear(GL_DEPTH_BUFFER_BIT);

    // Draw every static mesh component in the scene with the standard shaders
    for (StaticMesh* staticMesh : SceneManager::instance()->staticMeshes())
    {
        // Skip instances with no material
        if(staticMesh->material() == nullptr)
        {
            continue;
        }

        // Use the correct standard shader variant
        standardShader_->bindVariant(staticMesh->material()->supportedFeatures());

        // Set the correct mesh
        // Textures are bound via bindless texture handles
        staticMesh->mesh()->bind();
        
        // Update the per draw uniform buffer
        updatePerDrawUniformBuffer(staticMesh, staticMesh->material()->albedoTexture(), staticMesh->material()->normalMapTexture());

        // Draw the mesh
        glDrawElements(GL_TRIANGLES, staticMesh->mesh()->elementsCount(), GL_UNSIGNED_SHORT, (void*)0);
    }

    //Draw terrain
    terrainShader_->bindVariant(ALL_SHADER_FEATURES);
    auto terrains = SceneManager::instance()->terrains();
    for (unsigned int i = 0; i < terrains.size(); ++i)
    {
        //Get the terrain element
        auto terrain = terrains[i];

        //Set mesh and heightmap
        terrain->mesh()->bind();
        terrain->heightmap()->bind(0);
        terrain->texture()->bind(1);
        terrain->normalMap()->bind(4);
        //THIS IS A HACK REMOVE LATER
        ResourceManager::instance()->load<Texture>("Resources/Textures/terrain_snow.psd")->bind(2);
        ResourceManager::instance()->load<Texture>("Resources/Textures/terrain_rock.png")->bind(3);
        ResourceManager::instance()->load<Texture>("Resources/Textures/terrain_snow_normals.png")->bind(5);
        ResourceManager::instance()->load<Texture>("Resources/Textures/terrain_rock_normals.tga")->bind(6);

        updateTerrainUniformBuffer(terrain);

        glDrawElements(GL_TRIANGLES, terrain->mesh()->elementsCount(), GL_UNSIGNED_SHORT, (void*)0);
    }
}

void Renderer::executeDeferredLightingPass() const
{
    executeFullScreen(deferredLightingShader_, ALL_SHADER_FEATURES);
}

void Renderer::executeDeferredDebugPass() const
{
    RenderDebugMode mode = RenderManager::instance()->debugMode();
    executeFullScreen(deferredDebugShader_, (ShaderFeatureList)mode);
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
    skyboxCloudThicknessTexture_->bind(0);

    // Compute scale for skydome - must ensure it's big enough without exceeding far clipping plane
    const float farPlane = camera->getFarPlaneDistance();
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
