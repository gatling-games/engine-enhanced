#pragma once

#include "Editor/EditorPanel.h"
#include "Renderer/Texture.h"
#include "Renderer/Framebuffer.h"

class GamePanel : public EditorPanel
{
public:
    GamePanel();
    ~GamePanel();

    // EditorPanel overrides
    std::string name() const override { return "Game Panel"; }
    void draw() override;

private:
    Framebuffer* frameBuffer_;
    Texture* depthBuffer_;
    Texture* colorBuffer_;

    void createFramebuffer(int width, int height);
};
