#pragma once

#include "Editor/EditorPanel.h"
#include "Renderer/Texture.h"
#include "Renderer/Framebuffer.h"
#include "Renderer/UniformBuffer.h"

class GamePanel : public EditorPanel
{
public:
    GamePanel();
    ~GamePanel();

    // EditorPanel overrides
    std::string name() const override { return "Game Panel"; }
    void draw() override;
    void onResize(int width, int height) override;

private:
    UniformBuffer<SceneUniformData> sceneUniformBuffer_;
    UniformBuffer<CameraUniformData> cameraUniformBuffer_;
    UniformBuffer<PerDrawUniformData> perDrawUniformBuffer_;

    Framebuffer* frameBuffer_;
    Texture* depthBuffer_;
    Texture* colorBuffer_;
};
