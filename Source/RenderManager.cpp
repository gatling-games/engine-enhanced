#include "RenderManager.h"

#include <GLFW/glfw3.h>
#include "imgui.h"

#include "Editor/MainWindowMenu.h"

RenderManager::RenderManager()
    : vsyncEnabled_(true),
    allowedShaderFeatures_(~0u),
    debugMode_(RenderDebugMode::None)
{
    // Set default opengl settings
    glEnable(GL_CULL_FACE);

    // Ensure linear space rendering is used
    glEnable(GL_FRAMEBUFFER_SRGB);

    // Default to vsync on
    glfwSwapInterval(1);

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
    addShaderFeatureMenuItem(SF_ExtraTerrainDetails, "Extra Terrain Details");
    addShaderFeatureMenuItem(SF_Translucency, "Translucency");
    addShaderFeatureMenuItem(SF_AmbientOcclusion, "Ambient Occlusion");
    addShaderFeatureMenuItem(SF_Sky, "Sky");
    
    // Set up menu items for showing debugging modes
    addDebugModeMenuItem(RenderDebugMode::None, "None");
    addDebugModeMenuItem(RenderDebugMode::Wireframe, "Wireframe");
    addDebugModeMenuItem(RenderDebugMode::Depth, "Depth");
    addDebugModeMenuItem(RenderDebugMode::Albedo, "Albedo");
    addDebugModeMenuItem(RenderDebugMode::Gloss, "Gloss");
    addDebugModeMenuItem(RenderDebugMode::Normals, "Normals");
    addDebugModeMenuItem(RenderDebugMode::Occlusion, "Occlusion");
    addDebugModeMenuItem(RenderDebugMode::Translucency, "Translucency");
    addDebugModeMenuItem(RenderDebugMode::Shadows, "Shadows");
    addDebugModeMenuItem(RenderDebugMode::ShadowCascades, "Shadow Cascades");

    // Set up a menu item for toggling vsync
    MainWindowMenu::instance()->addMenuItem(
        "View/VSync",
        [&] { glfwSwapInterval(vsyncEnabled_ ? 0 : 1); vsyncEnabled_ = !vsyncEnabled_; },
        [&] { return vsyncEnabled_; }
    );
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