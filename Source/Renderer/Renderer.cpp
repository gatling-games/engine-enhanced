#include "Renderer.h"

#include <GL/gl3w.h>

#include <assert.h>

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
    for(int i = 0; i < GBUFFER_RENDER_TARGETS; ++i)
    {
        // Start with gbuffer0 in slot 15, gbuffer1 in slot 14, etc.
        gbufferTextures_[i]->bind(15 - i);
    }

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
    const int width = targetFramebuffer_->width();
    const int height = targetFramebuffer_->height();
    const float aspect = width / (float)height;

    // Gather the new contents of the camera buffer
    CameraUniformData data;
    data.worldToClip = camera->getWorldToCameraMatrix(aspect);
    data.cameraPosition = Vector4(camera->gameObject()->transform()->positionWorld());

    // Update the uniform buffer.
    cameraUniformBuffer_.update(data);
}

void Renderer::updatePerDrawUniformBuffer(const StaticMesh* draw) const
{
    // Gather the new contents of the per-draw buffer
    PerDrawUniformData data;
    data.localToWorld = draw->gameObject()->transform()->localToWorld();

    // Update the uniform buffer.
    perDrawUniformBuffer_.update(data);
}

void Renderer::updateTerrainUniformBuffer(const Terrain* terrain) const
{
    TerrainUniformData data;
    Vector3 dimens = terrain->gameObject()->terrain()->terrainDimensions();
    float normalScale = terrain->gameObject()->terrain()->normalScale();
    data.terrainSize = Vector4(dimens.x, dimens.y, dimens.z, normalScale);

    data.terrainCoordinateOffsetScale = Vector4(0.0f, 0.0f, 1.0f, 1.0f);
    Vector2 texScale = terrain->gameObject()->terrain()->textureWrapping();
    data.textureScale = Vector4(texScale.x, texScale.y, 1.0f, 1.0f);
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

    // Ensure the standard shader is being used (enable all features.
    standardShader_->bindVariant(ALL_SHADER_FEATURES);

    // Draw every static mesh component in the scene with the standard shaders
    auto staticMeshes = SceneManager::instance()->staticMeshes();
    for (unsigned int i = 0; i < staticMeshes.size(); ++i)
    {
        // Get the static mesh to draw
        auto staticMesh = staticMeshes[i];

        // Set the correct mesh and texture
        staticMesh->mesh()->bind();
        staticMesh->texture()->bind(0);

        // Update the per draw uniform buffer
        updatePerDrawUniformBuffer(staticMesh);

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
        //THIS IS A HACK REMOVE LATER
        ResourceManager::instance()->load<Texture>("Resources/Textures/terrain_snow.psd")->bind(2);
        ResourceManager::instance()->load<Texture>("Resources/Textures/terrain_rock.png")->bind(3);

        updateTerrainUniformBuffer(terrain);

        glDrawElements(GL_TRIANGLES, terrain->mesh()->elementsCount(), GL_UNSIGNED_SHORT, (void*)0);
    }
}

void Renderer::executeDeferredLightingPass() const
{
    executeFullScreen(deferredLightingShader_, ALL_SHADER_FEATURES);
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