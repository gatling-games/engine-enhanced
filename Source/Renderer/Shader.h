#pragma once

#include <GL/gl3w.h>
#include  <vector>

#include "ResourceManager.h"
#include "Renderer/UniformBuffer.h"

enum ShaderFeature
{
    SF_Texture = 1,

    SF_NormalMap = 2,

    SF_Specular = 4,

    SF_Cutout = 8,

    SF_Fog = 16,

    // Causes a large amount of tessellation to be used on the terrain
    SF_HighTessellation = 32,

    // Enables shadow sampling for the sun
    SF_Shadows = 64,

    // Enables PCF shadows with a high filter count. Removes shadow map aliasing.
    SF_SoftShadows = 128,

    // GBuffer debugging modes
    SF_DebugGBufferAlbedo = 256,
    SF_DebugGBufferOcclusion = 512,
    SF_DebugGBufferNormals = 1024,
    SF_DebugGBufferGloss = 2048,

    // Shadow debugging modes for viewing raw shadow sampling & cascade splits
    SF_DebugShadows = 4096,
    SF_DebugShadowCascades = 8192,

    // Wireframe rendering mode
    SF_DebugWireframe = 16384,
};

typedef unsigned int ShaderFeatureList;
const ShaderFeatureList ALL_SHADER_FEATURES = ~0u;

// Handles a .inc.shader resource
// It is simply glsl source code that can be #include'd into a shader variant.
class ShaderInclude : public Resource
{
public:
    ShaderInclude(ResourceID resourceID);

    // Gets the source code for the include.
    const std::string originalSource() const { return originalSource_; }

    // Resource loading and unloading
    void load(std::ifstream &file) override;
    void unload() override;

private:
    std::string originalSource_;
    bool previouslyLoaded_;
};

class ShaderVariant
{
public:
    ShaderVariant(ShaderFeatureList features, const std::string &originalSource);
    ~ShaderVariant();

    // Prevent a shader variant from being copied
    ShaderVariant(const ShaderVariant&) = delete;
    ShaderVariant& operator=(const ShaderVariant&) = delete;

    // Allow a variant to be moved
    ShaderVariant(ShaderVariant&& other);
    ShaderVariant& operator=(ShaderVariant&& other);

    // Gets the features in the variant.
    ShaderFeatureList features() const { return features_; }

    // Sets the variant as the active shader program.
    void bind() const;

private:
    ShaderFeatureList features_;
    GLuint program_;

    bool hasFeature(ShaderFeature feature) const;
    std::string createFeatureDefines() const;

    // Handles preprocessing for a shader source code string.
    // This function does the following:
    //     - adds version header
    //     - Adds shader stage #define
    //     - Adds #define for enabled features
    //     - Resolves #include statements
    std::string preprocessSource(GLenum shaderStage, const std::string &originalSource) const;

    bool compileShader(GLenum type, const char* shader, GLuint &id);
    bool checkShaderErrors(GLuint shaderID);
    bool checkLinkerErrors(GLuint programID);

    // Sets the texture slot for a sampler with the specified name.
    void setTextureLocation(const char* textureName, int slot);
};

class Shader : public Resource
{
public:
    Shader(ResourceID shaderId);
    ~Shader();

    void load(std::ifstream& file) override;
    void unload() override;

    // Unloads all shader variants.
    // They will be recreated when needed
    void unloadAllVariants();

    // Finds or created a variant with the given feature list
    // and binds it as the active gl program.
    void bindVariant(ShaderFeatureList features);

private:
    std::vector<ShaderVariant> loadedVariants_;
    std::string originalSource_;
};
