#include "Shader.h"

#include <Gl/gl3w.h>
#include "ResourceManager.h"
#include <assert.h>
#include <string>
#include <sstream>
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

    // The single file contains the vertex and fragment shader source code,
    // #ifdef VERTEX_SHADER and #ifdef FRAGMENT_SHADER are used to separate
    // the source code for each stage.
    std::stringstream fileStringStream;
    fileStringStream << file.rdbuf();
    const std::string originalSource = fileStringStream.str();

    // Apply preprocessing to the source code for each shader stage.
    // This adds stage-specific and variant-specific #defines and handles
    // custom preprocessor steps (eg adding extra source code and the version header).
    const std::string vertexSource = preprocessSource(GL_VERTEX_SHADER, originalSource);
    const std::string fragmentSource = preprocessSource(GL_FRAGMENT_SHADER, originalSource);

    // Attempt to compile the vertex shader code.
    if (!compileShader(GL_VERTEX_SHADER, vertexSource.c_str(), vertexShader_))
    {
        printf("Failed to compile vertex shader");
    }

    // Attempt to compile the fragment shader code.
    if (!compileShader(GL_FRAGMENT_SHADER, fragmentSource.c_str(), fragmentShader_))
    {
        printf("Failed to compile fragment shader");
    }

    // Create and link the shader program.
    program_ = glCreateProgram();
    glAttachShader(program_, vertexShader_);
    glAttachShader(program_, fragmentShader_);
    glLinkProgram(program_);

    // Check that the program linking succeeded.
    if (!checkLinkerErrors(program_))
    {
        printf("Failed to create program \n");
    }

    // Set uniform block binding
    setUniformBufferBinding("scene_data", UniformBufferType::SceneBuffer);
    setUniformBufferBinding("camera_data", UniformBufferType::CameraBuffer);
    setUniformBufferBinding("shadows_data", UniformBufferType::ShadowsBuffer);
    setUniformBufferBinding("per_draw_data", UniformBufferType::PerDrawBuffer);
    setUniformBufferBinding("per_material_data", UniformBufferType::PerMaterialBuffer);

    // Set texture locations
    setTextureLocation("_MainTexture", 0);

    // Set loaded flag
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
}

std::string Shader::preprocessSource(GLenum shaderStage, const std::string &originalSource) const
{
    // Construct the processed shader string from start to finish.
    // First, start with the version define
    std::string finalSource = "#version 450 \n";

    // Next, add a stage-specific define
    assert(shaderStage == GL_VERTEX_SHADER || shaderStage == GL_FRAGMENT_SHADER);
    if (shaderStage == GL_VERTEX_SHADER)
    {
        finalSource += "#define VERTEX_SHADER 1 \n";
    }
    else if (shaderStage == GL_FRAGMENT_SHADER)
    {
        finalSource += "#define FRAGMENT_SHADER 1 \n";
    }

    // Finally, include the actual shader source code.
    finalSource += originalSource;

    // Done. Return the preprocessed source code.
    return finalSource;
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
        printf("Shader Error. Log: %s \n", log);
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

void Shader::setUniformBufferBinding(const char *blockName, UniformBufferType type)
{
    const GLuint blockIndex = glGetUniformBlockIndex(program_, blockName);

    if (blockIndex != GL_INVALID_INDEX)
    {
        glUniformBlockBinding(program_, blockIndex, (GLint)type);
    }
}

void Shader::setTextureLocation(const char* textureName, int slot)
{
    const GLuint textureIndex = glGetUniformLocation(program_, "_MainTexture");
    if(textureIndex != -1)
    {
        glUniform1i(textureIndex, slot);
    }
}
