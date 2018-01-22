#pragma once

#include "Editor/EditorPanel.h"

#include "Renderer/Texture.h"
#include "Renderer/Framebuffer.h"
#include "Renderer/Renderer.h"

class GamePanel : public EditorPanel
{
public:
    GamePanel();
    ~GamePanel();

    // EditorPanel overrides
    std::string name() const override { return "Game Panel"; }
    virtual void drawMenu(const std::string menuName) override;
    void draw() override;

private:
    Framebuffer* frameBuffer_;
    Texture* depthBuffer_;
    Texture* colorBuffer_;
    Renderer* renderer_;
	
    void createFramebuffer(int width, int height);

    // Draws a toggle in the view menu for globally toggling a shader feature
    void drawFeatureToggle(ShaderFeature feature, const char* label) const;
};
