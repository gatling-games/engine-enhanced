#include "Shader.h"

#include <Gl/gl3w.h>
#include "ResourceManager.h"
#include <string>
#include <memory>
#include <iostream>

Shader::Shader(ResourceID id)
    : Resource(id),
    loaded_(false)
{
    
}

Shader::~Shader()
{
    if (loaded_)
    {
        unload();
    }
}

void Shader::load(std::ifstream& file)
{
    if (loaded_)
    {
        unload();
    }
    
    //Get file length
    file.seekg(0, file.end);
    size_t size = file.tellg();
    file.seekg(0, file.beg);

    //Read file data into array
    std::unique_ptr<char[]> sourceData(new char[size]);
    file.read((char*)sourceData.get(), size);

    //Split Vertex and Fragment shaders
    char *vertex = "#ifdef VERTEX_SHADER\n";
    char *fragment = "ifdef FRAGMENT_SHADER\n";
    const char *vshader[2] = { vertex, (char*)sourceData.get() };
    const char *fshader[2] = { fragment, (char*)sourceData.get() };

    //Compile shaders
    if (!compileShader(GL_VERTEX_SHADER, *vshader, vertexShader_))
    {
        printf("Failed to compile vertex shader");
    }

    if (!compileShader(GL_FRAGMENT_SHADER, *fshader, fragmentShader_))
    {
        printf("Failed to compile fragment shader");
    }

    //Create Program
    program_ = glCreateProgram();
    glAttachShader(program_, vertexShader_);
    glAttachShader(program_, fragmentShader_);
    glLinkProgram(program_);

    if (!checkLinkerErrors(program_))
    {
        printf("Failed to create program \n");
    }

    mainTextureLoc_ = glGetUniformLocation(program_, "_MainTexture");

    //Set load flag
    loaded_ = true;
}

void Shader::unload()
{
    glDeleteProgram(program_);
    glDeleteShader(vertexShader_);
    glDeleteShader(fragmentShader_);

    loaded_ = false;
}

void Shader::bind()
{
    glUseProgram(program_);
    glUniform1i(mainTextureLoc_, 0);
}

bool Shader::compileShader(GLenum type, const char* shader, GLuint& id)
{
    //Setup and compile shader
    id = glCreateShader(type);
    glShaderSource(id, 1, &shader, NULL);
    glCompileShader(id);

    if (!checkShaderErrors(id))
    {
        printf("Error compiling shader with ID: %d \n", id);
        return false;
    }
    return true;
}

bool Shader::checkShaderErrors(GLuint shaderID)
{
    //Check shader compile
    GLint ok;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &ok);

    if (!ok)
    {
        GLint logLength;
        glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &logLength);

        char* log = new char[logLength];
        glGetShaderInfoLog(shaderID, logLength, NULL, log);
        printf("Shader Error. Log: &s \n", log);
        delete[] log;

        return false;
    }

    return true;
}

bool Shader::checkLinkerErrors(GLuint programID)
{
    //Check for linking errors
    GLint linked = 0;
    glGetProgramiv(programID, GL_LINK_STATUS, (int*)&linked);

    if (!linked)
    {
        GLint logLength;
        glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &logLength);

        //Print error
        char* log = new char[logLength];
        glGetProgramInfoLog(programID, logLength, NULL, log);
        printf("Program link error. Log %s \n", log);
        delete[] log;

        return false;
    }
    return true;
}