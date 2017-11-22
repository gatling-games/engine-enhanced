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
    // Re-render the framebuffer on each draw
    renderer_->renderFrame(SceneManager::instance()->mainCamera());

    // Draw the framebuffer's colour texture into the panel.
    ImGui::Image(reinterpret_cast<ImTextureID>(colorBuffer_->glid()), ImVec2(size().width, size().height), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
}

void GamePanel::onResize(int width, int height)
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