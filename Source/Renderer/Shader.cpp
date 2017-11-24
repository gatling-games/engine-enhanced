#include "Shader.h"

#include <Gl/gl3w.h>
#include "ResourceManager.h"
#include <assert.h>
#include <string>
#include <sstream>
#include <iostream>

ShaderVariant::ShaderVariant(ShaderFeatureList features, const std::string &originalSource)
    : features_(features)
{
    // The single file contains the vertex and fragment shader source code,
    // #ifdef VERTEX_SHADER and #ifdef FRAGMENT_SHADER are used to separate
    // the source code for each stage.
    // Apply preprocessing to the source code for each shader stage.
    // This adds stage-specific and variant-specific #defines and handles
    // custom preprocessor steps (eg adding extra source code and the version header).
    const std::string vertexSource = preprocessSource(GL_VERTEX_SHADER, originalSource);
    const std::string fragmentSource = preprocessSource(GL_FRAGMENT_SHADER, originalSource);

    // Attempt to compile the vertex shader code.
    GLuint vertexShader;
    if (!compileShader(GL_VERTEX_SHADER, vertexSource.c_str(), vertexShader))
    {
        printf("Failed to compile vertex shader");
    }

    // Attempt to compile the fragment shader code.
    GLuint fragmentShader;
    if (!compileShader(GL_FRAGMENT_SHADER, fragmentSource.c_str(), fragmentShader))
    {
        printf("Failed to compile fragment shader");
    }

    // Create and link the shader program.
    program_ = glCreateProgram();
    glAttachShader(program_, vertexShader);
    glAttachShader(program_, fragmentShader);
    glLinkProgram(program_);

    // We are using program id 0 to mean no program.
    // Ensure that opengl does not create a program with id 0.
    assert(program_ != 0);

    // Check that the program linking succeeded.
    if (!checkLinkerErrors(program_))
    {
        printf("Failed to create program \n");
    }

    // We no longer need the vertex and fragment shader objects
    // as the shader program is now compiled.
    glDetachShader(program_, vertexShader);
    glDetachShader(program_, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Set uniform block binding
    setUniformBufferBinding("scene_data", UniformBufferType::SceneBuffer);
    setUniformBufferBinding("camera_data", UniformBufferType::CameraBuffer);
    setUniformBufferBinding("shadows_data", UniformBufferType::ShadowsBuffer);
    setUniformBufferBinding("per_draw_data", UniformBufferType::PerDrawBuffer);
    setUniformBufferBinding("per_material_data", UniformBufferType::PerMaterialBuffer);

    // Set texture locations
    setTextureLocation("_MainTexture", 0);
}

ShaderVariant::~ShaderVariant()
{
    if (program_ != 0)
    {
        glDeleteProgram(program_);
    }
}

ShaderVariant::ShaderVariant(ShaderVariant&& other)
{
    // Steal the contents of other
    features_ = other.features_;
    program_ = other.program_;

    // Reset other
    other.features_ = 0;
    other.program_ = 0;
}

ShaderVariant& ShaderVariant::operator=(ShaderVariant&& other)
{
    if (this != &other)
    {
        // Steal the contents of other
        features_ = other.features_;
        program_ = other.program_;

        // Reset other
        other.features_ = 0;
        other.program_ = 0;
    }

    return *this;
}

void ShaderVariant::bind() const
{
    glUseProgram(program_);
}

bool ShaderVariant::hasFeature(ShaderFeature feature) const
{
    return (features_ & feature) != 0;
}

std::string ShaderVariant::createFeatureDefines() const
{
    std::string defines = "";

    // Shader feature defines
    if (hasFeature(SF_Texture)) defines += "#define TEXTURE_ON \n";
    if (hasFeature(SF_NormalMap)) defines += "#define NORMAL_MAP_ON \n";
    if (hasFeature(SF_Specular)) defines += "#define SPECULAR_ON \n";
    if (hasFeature(SF_Cutout)) defines += "#define ALPHA_TEST_ON \n";
    if (hasFeature(SF_Fog)) defines += "#define FOG_ON \n";

    return defines;
}

std::string ShaderVariant::preprocessSource(GLenum shaderStage, const std::string &originalSource) const
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
    finalSource += createFeatureDefines();
    // Finally, include the actual shader source code.
    finalSource += originalSource;

    // Done. Return the preprocessed source code.
    return finalSource;
}

bool ShaderVariant::compileShader(GLenum type, const char* shader, GLuint& id)
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

bool ShaderVariant::checkShaderErrors(GLuint shaderID)
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

bool ShaderVariant::checkLinkerErrors(GLuint programID)
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

void ShaderVariant::setUniformBufferBinding(const char *blockName, UniformBufferType type)
{
    const GLuint blockIndex = glGetUniformBlockIndex(program_, blockName);

    if (blockIndex != GL_INVALID_INDEX)
    {
        glUniformBlockBinding(program_, blockIndex, (GLint)type);
    }
}

void ShaderVariant::setTextureLocation(const char* textureName, int slot)
{
    const GLuint textureIndex = glGetUniformLocation(program_, textureName);
    if (textureIndex != -1)
    {
        glProgramUniform1i(program_, textureIndex, slot);
    }
}

//Shader
Shader::Shader(ResourceID id)
    : Resource(id)
{

}

Shader::~Shader()
{
    unload();
}

void Shader::load(std::ifstream& file)
{
    // Unload any loaded shader variants
    unload();

    //Load in source file to variable for later use in variants.
    std::stringstream fileStringStream;
    fileStringStream << file.rdbuf();
    originalSource_ = fileStringStream.str();
}

void Shader::unload()
{
    //Unloads all shader variants calls their destructor
    loadedVariants_.clear();
}

void Shader::bindVariant(ShaderFeatureList features)
{
    //Loop through all variants to see if it already exists.
    for (int variant = 0; variant < loadedVariants_.size(); ++variant)
    {
        ShaderVariant& current = loadedVariants_[variant];
        if (current.features() == features)
        {
            current.bind();
            return;
        }
    }

    //If not, compile and bind new variant, and add it to the list.
    loadedVariants_.push_back(ShaderVariant(features, originalSource_));
    loadedVariants_.back().bind();
}
