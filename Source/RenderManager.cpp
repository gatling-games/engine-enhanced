#include "RenderManager.h"

#include <GLFW/glfw3.h>
#include "imgui.h"

RenderManager::RenderManager()
    : allowedShaderFeatures_(~0u)
{
    // Set default opengl settings
    glEnable(GL_CULL_FACE);
}

bool RenderManager::isFeatureGloballyEnabled(ShaderFeature feature) const
{
    return (feature & allowedShaderFeatures_) != 0;
}

void RenderManager::globallyEnableFeature(ShaderFeature feature)
{
    allowedShaderFeatures_ |= feature;
}

void RenderManager::globallyDisableFeature(ShaderFeature feature)
{
    allowedShaderFeatures_ &= ~feature;
}

void RenderManager::globallyToggleFeature(ShaderFeature feature)
{
    if (isFeatureGloballyEnabled(feature))
    {
        globallyDisableFeature(feature);
    }
    else
    {
        globallyEnableFeature(feature);
    }
}

ShaderFeatureList RenderManager::filterFeatureList(ShaderFeatureList list) const
{
    return list & allowedShaderFeatures_;
}

void RenderManager::render()
{
    // Individual renderers are currently rendered on-demand.
}