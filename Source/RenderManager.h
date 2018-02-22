#pragma once

#include "Application.h"

#include "Renderer/Shader.h"
#include "Utils/Singleton.h"

enum class RenderDebugMode
{
    None = 0,
    Wireframe = SF_DebugWireframe,
    Albedo = SF_DebugGBufferAlbedo,
    Occlusion = SF_DebugGBufferOcclusion,
    Gloss = SF_DebugGBufferGloss,
    Normals = SF_DebugGBufferNormals,
    Shadows = SF_DebugShadows,
    ShadowCascades = SF_DebugShadowCascades,
};

class RenderManager : public Singleton<RenderManager>
{
public:
    RenderManager();
                                                                                                                                                                                                                   
    // Enables and disables shader features globally.
    // Shader features will not be used unless enabled globally.
    bool isFeatureGloballyEnabled(ShaderFeature feature) const;
    void globallyEnableFeature(ShaderFeature feature);
    void globallyDisableFeature(ShaderFeature feature);
    void globallyToggleFeature(ShaderFeature feature);

    // Filters a shader feature list to include only ones that
    // are globally enabled.
    ShaderFeatureList filterFeatureList(ShaderFeatureList list) const;

    // Gets or sets the current debugging mode
    RenderDebugMode debugMode() const { return debugMode_; }
    void setDebugMode(RenderDebugMode mode) { debugMode_ = mode; }

    // Called each frame to perform per-frame rendering tasks.
    void render();

private:
    // Globally enabled shader features.
    // Features that are not globally enabled cannot be used.
    ShaderFeatureList allowedShaderFeatures_;
    
    // The current deferred debugging mode.
    RenderDebugMode debugMode_;

    // Adds a menu item for toggling a global shader feature.
    void addShaderFeatureMenuItem(ShaderFeature feature, const std::string &name);

    // Adds a menu item for switching to a debugging mode.
    void addDebugModeMenuItem(RenderDebugMode mode, const std::string &name);
};
