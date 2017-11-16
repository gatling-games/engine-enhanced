#include "Shader.h"

#include <Gl/gl3w.h>
#include "ResourceManager.h"
#include <string>
#include <iostream>

Shader::Shader(ResourceID id)
{
    if(!compileShader(GL_VERTEX_SHADER, id, vertexShader_))
    {
        
    }

    if (!compileShader(GL_FRAGMENT_SHADER, id, fragmentShader_))
    {

    }
    //Create Program
    program_ = glCreateProgram();
    glAttachShader(program_, vertexShader_);
    glAttachShader(program_, fragmentShader_);
    glLinkProgram(program_);

    mainTextureLoc_ = glGetUniformLocation(program_, "_MainTexture");
}


Shader::~Shader()
{
    glDeleteProgram(program_);
    glDeleteShader(vertexShader_);
    glDeleteShader(fragmentShader_);
}

void Shader::bind()
{
    //TODO
    glUseProgram(program_);
    glUniform1i(mainTextureLoc_, 0);
}

bool Shader::compileShader(GLenum type, ResourceID shaderId, GLuint& id)
{
    id = glCreateShader(type);
    glShaderSource(id, 1, NULL, NULL);
    glCompileShader(id);
    return true;
}

