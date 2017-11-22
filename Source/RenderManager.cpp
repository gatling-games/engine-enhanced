#include "RenderManager.h"

#include <GLFW/glfw3.h>
#include "imgui.h"

RenderManager::RenderManager()
{
    renderers_.push_back(createRenderer());
}

RenderManager::~RenderManager()
{
    for (int renderIndex = 0; renderIndex < renderers_.size(); renderIndex++)
    {
        delete renderers_[renderIndex];
    }
}


void RenderManager::drawDebugMenu()
{
}

Renderer* RenderManager::createRenderer()
{
    Renderer* renderer = new Renderer;

    return renderer;
}

void RenderManager::render()
{
    // Individual renderers are currently rendered on-demand.
}