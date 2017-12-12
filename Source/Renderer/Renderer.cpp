#include "Renderer.h"

#include <GL/gl3w.h>

#include "ResourceManager.h"
#include "SceneManager.h"

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
	data.textureScale = Vector4(texScale.x, texScale.y, 1.0f, 1.0f);
	terrainUniformBuffer_.update(data);
}

void Renderer::executeForwardPass() const
{
    // The forward pass renders into the target framebuffer
    targetFramebuffer_->use();

    // First, clear the colour and depth buffers
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

		updateTerrainUniformBuffer(terrain);

        glDrawElements(GL_TRIANGLES, terrain->mesh()->elementsCount(), GL_UNSIGNED_SHORT, (void*)0);
    }
}