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
    virtual std::string name() const { return "Game Panel"; }
    virtual void draw();

private:
    Framebuffer* frameBuffer_;
    Texture* depthBuffer_;
    Texture* colorBuffer_;
};
