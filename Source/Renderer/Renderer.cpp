#include "Renderer.h"

#include <GL/gl3w.h>

#include "ResourceManager.h"
#include "SceneManager.h"
#include "Utils/Clock.h"

Renderer::Renderer()
    : Renderer(Framebuffer::backbuffer())
{

}

Renderer::Renderer(const Framebuffer* targetFramebuffer)
    : targetFramebuffer_(targetFramebuffer),
    sceneUniformBuffer_(UniformBufferType::SceneBuffer),
    cameraUniformBuffer_(UniformBufferType::CameraBuffer),
    perDrawUniformBuffer_(UniformBufferType::PerDrawBuffer),
    terrainUniformBuffer_(UniformBufferType::TerrainBuffer)
{
    // Load the shaders required for each render pass
    forwardShader_ = ResourceManager::instance()->load<Shader>("Resources/Shaders/ForwardPass.shader");

    // Load skybox shader and mesh
    skyboxShader_ = ResourceManager::instance()->load<Shader>("Resources/Shaders/SkyboxPass.shader");
    skyboxMesh_ = ResourceManager::instance()->load<Mesh>("Resources/Meshes/skybox.obj");
    skyboxCloudThicknessTexture_ = ResourceManager::instance()->load<Texture>("Resources/Textures/cloud_thickness.psd");
    terrainShader_ = ResourceManager::instance()->load<Shader>("Resources/Shaders/Terrain.shader");
}

void Renderer::renderFrame(const Camera* camera) const
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

    // Execute each render pass in turn.
    executeForwardPass();
    executeSkyboxPass(camera);
}

void Renderer::updateSceneUniformBuffer() const
{
    // Get the scene we are rendering
    const SceneManager* scene = SceneManager::instance();

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
	data.terrainSize = Vector4(dimens.x,dimens.y,dimens.z,normalScale);

	data.terrainCoordinateOffsetScale = Vector4(0.0f, 0.0f, 1.0f, 1.0f);
	Vector2 texScale = terrain->gameObject()->terrain()->textureWrapping();

	//Invert Texture scale
	data.textureScale = Vector4(dimens.x/texScale.x, dimens.z/texScale.y, 1.0f, 1.0f);
	terrainUniformBuffer_.update(data);
}

void Renderer::executeForwardPass() const
{
    // The forward pass renders into the target framebuffer
    targetFramebuffer_->use();

    // First, clear the depth buffer - don't need to clear color buffer as skybox will cover background
    glClear(GL_DEPTH_BUFFER_BIT);

    // Ensure that depth testing and backface culling
    // are turned on for this pass
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // Ensure the forward shader is being used (enable all features.
    forwardShader_->bindVariant(~0);

    // Draw every static mesh component in the scene with the forward shaders
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
}

void Renderer::executeSkyboxPass(const Camera* camera) const
{
    // Ensure skybox shader is being used
    skyboxShader_->bindVariant(~0);

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


    //Draw terrain
    terrainShader_->bindVariant(~0);
    auto terrains = SceneManager::instance()->terrains();
    for (unsigned int i =0; i < terrains.size(); ++i)
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