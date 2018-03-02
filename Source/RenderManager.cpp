#include "RenderManager.h"

#include <GLFW/glfw3.h>
#include "imgui.h"

#include "Editor/MainWindowMenu.h"

RenderManager::RenderManager()
    : allowedShaderFeatures_(~0u),
    debugMode_(RenderDebugMode::None)
{
    // Set default opengl settings
    glEnable(GL_CULL_FACE);

    // Ensure linear space rendering is used
    glEnable(GL_FRAMEBUFFER_SRGB);

    // Set up menu items for toggling render features
    addShaderFeatureMenuItem(SF_Texture, "Textures");
    addShaderFeatureMenuItem(SF_NormalMap, "Normal Maps");
    addShaderFeatureMenuItem(SF_Specular, "Specular Highlights");
    addShaderFeatureMenuItem(SF_Cutout, "Alpha Cutout");
    addShaderFeatureMenuItem(SF_Fog, "Fog");
    addShaderFeatureMenuItem(SF_Shadows, "Shadows");
    addShaderFeatureMenuItem(SF_SoftShadows, "Soft Shadows");
    addShaderFeatureMenuItem(SF_ShadowCascadeBlending, "Shadow Cascade Blending");
    addShaderFeatureMenuItem(SF_HighTessellation, "Extra Tessellation");
    addShaderFeatureMenuItem(SF_TerrainDetailMeshes, "Terrain Details");

    // Set up menu items for showing debugging modes
    addDebugModeMenuItem(RenderDebugMode::None, "None");
    addDebugModeMenuItem(RenderDebugMode::Wireframe, "Wireframe");
    addDebugModeMenuItem(RenderDebugMode::Depth, "Depth");
    addDebugModeMenuItem(RenderDebugMode::Albedo, "Albedo");
    addDebugModeMenuItem(RenderDebugMode::Gloss, "Gloss");
    addDebugModeMenuItem(RenderDebugMode::Normals, "Normals");
    addDebugModeMenuItem(RenderDebugMode::Occlusion, "Occlusion");
    addDebugModeMenuItem(RenderDebugMode::Shadows, "Shadows");
    addDebugModeMenuItem(RenderDebugMode::ShadowCascades, "Shadow Cascades");
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

void RenderManager::addShaderFeatureMenuItem(ShaderFeature feature, const std::string &name)
{
    MainWindowMenu::instance()->addMenuItem(
        "View/Shader Feature/" + name,
        [=] { globallyToggleFeature(feature); },
        [=] { return isFeatureGloballyEnabled(feature); }
    );
}

void RenderManager::addDebugModeMenuItem(RenderDebugMode mode, const std::string &name)
{
    MainWindowMenu::instance()->addMenuItem(
        "View/Debug Mode/" + name,
        [=] { setDebugMode(mode); },
        [=] { return debugMode() == mode; }
    );
}