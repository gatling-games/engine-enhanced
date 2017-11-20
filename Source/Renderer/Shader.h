#pragma once
#include <GL/gl3w.h>
#include "ResourceManager.h"

class Shader : public Resource
{
public:
    Shader(ResourceID shaderId);
    ~Shader();

    void load(std::ifstream& file) override;
    void unload() override;


    GLuint program() const { return program_; }
    GLuint vertexShader() const { return vertexShader_; }
    GLuint fragmentShader() const { return fragmentShader_; }

    void bind();

private:
    bool loaded_;

    GLuint program_;
    GLuint vertexShader_;
    GLuint fragmentShader_;

    GLint mainTextureLoc_;

    // Handles preprocessing for a shader source code string.
    // This performs eg. version header adding and #defines
    std::string preprocessSource(GLenum shaderStage, const std::string &originalSource) const;

    bool compileShader(GLenum type, const char* shader, GLuint &id);
    bool checkShaderErrors(GLuint shaderID);
    bool checkLinkerErrors(GLuint programID);
    
};
