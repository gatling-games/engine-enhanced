#pragma once

#include <vector>

#include "Application.h"
#include "Renderer/Renderer.h"

class RenderManager : public ApplicationModule
{
public:
    RenderManager();
    ~RenderManager();

    std::string name() const override { return "Render Manager"; }
    void drawDebugMenu() override;

    Renderer* createRenderer();

    void render();

private:
    std::vector<Renderer*> renderers_;
};
