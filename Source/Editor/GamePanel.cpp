#include "GamePanel.h"

#include <imgui.h>

#include "SceneManager.h"
#include "Scene/Camera.h"

GamePanel::GamePanel()
    : frameBuffer_(nullptr)
    , depthBuffer_(nullptr)
    , colorBuffer_(nullptr)
    , renderer_(nullptr)
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
        delete renderer_;
    }
}

void GamePanel::draw()
{
    // Determine the size of the region we need to render for
    // and create / recreate the framebuffer when needed.
    const ImVec2 renderTextureSize = ImGui::GetContentRegionAvail();
    if (frameBuffer_ == nullptr
        || colorBuffer_->width() != renderTextureSize.x
        || colorBuffer_->height() != renderTextureSize.y)
    {
        createFramebuffer(renderTextureSize.x, renderTextureSize.y);
    }

    // Re-render the framebuffer on each draw
    renderer_->renderFrame(SceneManager::instance()->mainCamera());

    // Draw the texture
    ImGui::Image(reinterpret_cast<ImTextureID>(colorBuffer_->glid()), ImGui::GetContentRegionAvail(), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
}

void GamePanel::createFramebuffer(int width, int height)
{
    // Delete any existing framebuffer
    if (frameBuffer_ != nullptr)
    {
        delete frameBuffer_;
        delete depthBuffer_;
        delete colorBuffer_;
        delete renderer_;
    }

    // Create new framebuffer and textures with new panel dimensions
    frameBuffer_ = new Framebuffer();
    depthBuffer_ = new Texture(TextureFormat::Depth, width, height);
    colorBuffer_ = new Texture(TextureFormat::RGB8, width, height);

    // Attach new textures to new framebuffer
    frameBuffer_->attachDepthTexture(depthBuffer_);
    frameBuffer_->attachColorTexture(colorBuffer_);

    // Then create a renderer for the framebuffer.
    renderer_ = new Renderer(frameBuffer_);
}