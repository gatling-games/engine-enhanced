#pragma once
#include <GL/gl3w.h>
#include  <vector>
#include "ResourceManager.h"


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
    ShaderVariant(ShaderFeatureList features);
    ~ShaderVariant();

    ShaderFeatureList features() const { return features_; }

    void load(const std::string originalSource);
    void unload();

    bool hasFeature(ShaderFeature feature) const;
    std::string createFeatureDefines() const;

    GLuint program() const { return program_; }
    GLuint vertexShader() const { return vertexShader_; }
    GLuint fragmentShader() const { return fragmentShader_; }

    void bind() const;

private:
    bool loaded_;
    GLuint program_;
    GLuint vertexShader_;
    GLuint fragmentShader_;
    ShaderFeatureList features_;

    // Handles preprocessing for a shader source code string.
    // This performs eg. version header adding and #defines
    std::string preprocessSource(GLenum shaderStage, const std::string &originalSource) const;

    bool compileShader(GLenum type, const char* shader, GLuint &id);
    bool checkShaderErrors(GLuint shaderID);
    bool checkLinkerErrors(GLuint programID);
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
    bool loaded_;
    std::vector<ShaderVariant> loadedVariants_;
    std::string originalSource_;
};
