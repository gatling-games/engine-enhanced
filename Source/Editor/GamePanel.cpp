#include "GamePanel.h"

#include <imgui.h>

GamePanel::GamePanel()
{
    frameBuffer_ = new Framebuffer();
    colorBuffer_ = new Texture(TextureFormat::RGBA8, 400, 400);

    colour_ = new float;
    *colour_ = 0.0f;

    frameBuffer_->attachColorTexture(colorBuffer_);
}

GamePanel::~GamePanel()
{
    delete frameBuffer_;
    delete colorBuffer_;
    delete colour_;
}


void GamePanel::draw()
{
    frameBuffer_->use();

    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(static_cast<GLfloat>(*colour_), 0.0f, static_cast<GLfloat>(*colour_), 1.0f);

    Framebuffer::backbuffer()->use();

    // Draw the texture
    ImGui::Image(reinterpret_cast<ImTextureID>(colorBuffer_->glid()), ImVec2(500.0f, 300.0f));
    ImGui::SliderFloat("Colour", colour_, 0.0f, 1.0f);
}