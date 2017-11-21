#include "GamePanel.h"

#include <imgui.h>

GamePanel::GamePanel()
    : frameBuffer_(nullptr)
    , depthBuffer_(nullptr)
    , colorBuffer_(nullptr)
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

    glClearColor(180.0f, 0.0f, 180.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

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