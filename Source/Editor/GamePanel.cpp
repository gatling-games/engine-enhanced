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
    // Determine the size of the region we need to render for
    const ImVec2 renderTextureSize = ImGui::GetContentRegionAvail();
    if(frameBuffer_ == nullptr 
        || colorBuffer_->width() != renderTextureSize.x 
        || colorBuffer_->height() != renderTextureSize.y)
    {
        createFramebuffer(renderTextureSize.x, renderTextureSize.y);
    }

    frameBuffer_->use();

    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(180.0f, 0.0f, 180.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

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

    // Draw the texture
    ImGui::Image(reinterpret_cast<ImTextureID>(colorBuffer_->glid()), ImGui::GetContentRegionAvail());
}

void GamePanel::createFramebuffer(int width, int height)
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