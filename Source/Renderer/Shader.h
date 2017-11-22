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
};

typedef unsigned int ShaderFeatureList;

class ShaderVariant
{
public:
    ShaderVariant(ShaderFeatureList features, const std::string &originalSource);
    ~ShaderVariant();

    ShaderFeatureList features() const { return features_; }

    GLuint program() const { return program_; }
    GLuint vertexShader() const { return vertexShader_; }
    GLuint fragmentShader() const { return fragmentShader_; }

    void bind() const;

private:
    ShaderFeatureList features_;
    GLuint program_;
    GLuint vertexShader_;
    GLuint fragmentShader_;

    bool hasFeature(ShaderFeature feature) const;
    std::string createFeatureDefines() const;

    // Handles preprocessing for a shader source code string.
    // This performs eg. version header adding and #defines
    std::string preprocessSource(GLenum shaderStage, const std::string &originalSource) const;

    bool compileShader(GLenum type, const char* shader, GLuint &id);
    bool checkShaderErrors(GLuint shaderID);
    bool checkLinkerErrors(GLuint programID);
	
	// Binds a uniform buffer type to the shader ubo block with the specified name
    void setUniformBufferBinding(const char *blockName, UniformBufferType type);
	
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

    void bindVariant(ShaderFeatureList features);

private:
    std::vector<ShaderVariant> loadedVariants_;
    std::string originalSource_;
};
