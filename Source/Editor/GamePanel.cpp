#include "GamePanel.h"

#include <imgui.h>

#include "Renderer/UniformBuffer.h"

GamePanel::GamePanel(ResourceManager* resourceManager)
    : frameBuffer_(nullptr)
    , depthBuffer_(nullptr)
    , colorBuffer_(nullptr)
    , resourceManager_(resourceManager)
    , objects_()
    , camera_(Point3(0.0f, 0.0f, 0.0f), Quaternion::identity())
{

}

GamePanel::~GamePanel()
{
    // Delete any existing framebuffer
    if (frameBuffer_ != nullptr)
    {
        delete frameBuffer_;
        delete depthBuffer_;
        delete colorBuffer_;
    }
}

void GamePanel::draw()
{
    // On first draw, load the scene and camera
    if(objects_.empty())
    {
        Shader* cubeShader = resourceManager_->load<Shader>("Shader/ForwardPass.shader");
        Mesh* cubeMesh = resourceManager_->load<Mesh>("Meshes/cube.obj");
       // Texture* cubeTexture = resourceManager_->load<Texture>("Textures/cube.png");
        Material* cubeMaterial = new Material();
        cubeMaterial->shader = cubeShader;
        cubeMaterial->texture = nullptr;
        SceneObject cubeSO;
        cubeSO.mesh = cubeMesh;
        cubeSO.material = cubeMaterial;
        cubeSO.localToWorld = Matrix4x4::trs(Vector3(0.0f, 0.0f, 10.0f), Quaternion::identity(), Vector3::one());
        objects_.push_back(cubeSO);
    }

    frameBuffer_->use();

    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(180.0f, 0.0f, 180.0f, 1.0f);

    // Setup the camera uniform buffer
    CameraUniformData cud;
    cud.cameraPosition = Vector4(camera_.getPosition());
    cud.worldToClip = camera_.getWorldToCameraMatrix(1.0);
    UniformBuffer<CameraUniformData> ub(UniformBufferType::CameraBuffer);
    ub.update(cud);
    ub.use();

    for(unsigned int i = 0; i < objects_.size(); ++i)
    {
        // Set the correct mesh, shader, texture
        objects_[i].mesh->bind();
        // objects_[i].material->texture->bind(0);
        objects_[i].material->shader->bind();

        // Draw the mesh
        glDrawElements(GL_TRIANGLES, objects_[i].mesh->elementsCount(), GL_UNSIGNED_SHORT, (void*)0);
    }

    Framebuffer::backbuffer()->use();

    // Get texture size
    const Rect rect = size();

    // Draw the texture
    ImGui::Image(reinterpret_cast<ImTextureID>(colorBuffer_->glid()), ImVec2(rect.width, rect.height));
}

void GamePanel::onResize(int width, int height)
{
    // Delete any existing framebuffer
    if (frameBuffer_ != nullptr)
    {
        delete frameBuffer_;
        delete depthBuffer_;
        delete colorBuffer_;
    }

    // Create new framebuffer and textures with new panel dimensions
    frameBuffer_ = new Framebuffer();
    depthBuffer_ = new Texture(TextureFormat::Depth, width, height);
    colorBuffer_ = new Texture(TextureFormat::RGB8, width, height);

    // Attach new textures to new framebuffer
    frameBuffer_->attachDepthTexture(depthBuffer_);
    frameBuffer_->attachColorTexture(colorBuffer_);
}