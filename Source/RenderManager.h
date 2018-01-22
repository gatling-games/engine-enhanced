#pragma once

#include "Application.h"

#include "Renderer/Shader.h"
#include "Utils/Singleton.h"

class RenderManager : public ApplicationModule, public Singleton<RenderManager>
{
public:
    RenderManager();

    std::string name() const override { return "Render Manager"; }

    // Enables and disables shader features globally.
    // Shader features will not be used unless enabled globally.
    bool isFeatureGloballyEnabled(ShaderFeature feature) const;
    void globallyEnableFeature(ShaderFeature feature);
    void globallyDisableFeature(ShaderFeature feature);
    void globallyToggleFeature(ShaderFeature feature);

    // Filters a shader feature list to include only ones that
    // are globally enabled.
    ShaderFeatureList filterFeatureList(ShaderFeatureList list) const;

    // Called each frame to perform per-frame rendering tasks.
    void render();

private:
    // Globally enabled shader features.
    // Features that are not globally enabled cannot be used.
    ShaderFeatureList allowedShaderFeatures_;
};
