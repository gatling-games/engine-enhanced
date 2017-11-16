#pragma once
#include <GL/gl3w.h>
#include "ResourceManager.h"

class Shader
{
public:
    Shader(ResourceID shaderId);
    ~Shader();

    GLuint program() const { return program_; }
    GLuint vertexShader() const { return vertexShader_; }
    GLuint fragmentShader() const { return fragmentShader_; }

    void bind();

private:
    GLuint program_;
    GLuint vertexShader_;
    GLuint fragmentShader_;

    GLint mainTextureLoc_;

    bool compileShader(GLenum type, ResourceID shaderId, GLuint &id);
    
};
