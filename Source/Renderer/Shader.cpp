#include "Shader.h"

#include <Gl/gl3w.h>

#include <assert.h>
#include <string>
#include <sstream>
#include <iostream>
#include <regex>

#include "ResourceManager.h"
#include "RenderManager.h"

ShaderInclude::ShaderInclude(ResourceID resourceID)
    : Resource(resourceID),
    previouslyLoaded_(false)
{
    
}

void ShaderInclude::load(std::ifstream &file)
{
    // Load in source file to variable for later use in variants.
    std::stringstream fileStringStream;
    fileStringStream << file.rdbuf();
    originalSource_ = fileStringStream.str();

    // If the include was previously loaded, this reload may have affected
    // shader variants that are currently loaded.
    // Remove all loaded shader variants. They will then be re-created on
    // demand, ensuring that they use up-to-date includes.
    if(previouslyLoaded_)
    {
        for(Shader* shader : ResourceManager::instance()->loadedResourcesOfType<Shader>())
        {
            shader->unloadAllVariants();
        }
    }
}

void ShaderInclude::unload()
{
    // Mark the shader as previously loaded.
    previouslyLoaded_ = true;
}

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

    // If detected in the source code, also compile a tessellation control shader
    if (vertexSource.find("TESS_CONTROL_SHADER") != std::string::npos)
    {
        const std::string tessEvaluationSource = preprocessSource(GL_TESS_CONTROL_SHADER, originalSource);
        GLuint tessEvaluationShader;
        if (!compileShader(GL_TESS_CONTROL_SHADER, tessEvaluationSource.c_str(), tessEvaluationShader))
        {
            printf("Failed to compile tessellation control shader \n");
        }
        else
        {
            glAttachShader(program_, tessEvaluationShader);
        }
    }

    // If detected in the source code, also compile a tessellation evaluation shader
    if(vertexSource.find("TESS_EVALUATION_SHADER") != std::string::npos)
    {
        const std::string tessEvaluationSource = preprocessSource(GL_TESS_EVALUATION_SHADER, originalSource);
        GLuint tessEvaluationShader;
        if(!compileShader(GL_TESS_EVALUATION_SHADER, tessEvaluationSource.c_str(), tessEvaluationShader))
        {
            printf("Failed to compile tessellation evaluation shader \n");
        }
        else
        {
            glAttachShader(program_, tessEvaluationShader);
        }
    }

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

    // Set texture locations
    setTextureLocation("_MainTexture", 0);

    // Specify that tessellation is always on triangles (aka patches of 3)
    glPatchParameteri(GL_PATCH_VERTICES, 3);
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
    if (hasFeature(SF_HighTessellation)) defines += "#define HIGH_TESSELLATION \n";
    if (hasFeature(SF_DebugGBufferAlbedo)) defines += "#define DEBUG_GBUFFER_ALBEDO \n";
    if (hasFeature(SF_DebugGBufferOcclusion)) defines += "#define DEBUG_GBUFFER_OCCLUSION \n";
    if (hasFeature(SF_DebugGBufferNormals)) defines += "#define DEBUG_GBUFFER_NORMALS \n";
    if (hasFeature(SF_DebugGBufferGloss)) defines += "#define DEBUG_GBUFFER_GLOSS \n";

    return defines;
}

std::string ShaderVariant::preprocessSource(GLenum shaderStage, const std::string &originalSource) const
{
    // Construct the processed shader string from start to finish.
    // First, start with the version define
    std::string finalSource = "#version 450 \n";
    finalSource += "#extension ARB_bindless_texture:require \n";

    // Next, add a stage-specific define
    assert(shaderStage == GL_VERTEX_SHADER || shaderStage == GL_FRAGMENT_SHADER 
        || shaderStage == GL_TESS_CONTROL_SHADER || shaderStage == GL_TESS_EVALUATION_SHADER);
    if (shaderStage == GL_VERTEX_SHADER)
    {
        finalSource += "#define VERTEX_SHADER 1 \n";
    }
    else if (shaderStage == GL_FRAGMENT_SHADER)
    {
        finalSource += "#define FRAGMENT_SHADER 1 \n";
    }
    else if(shaderStage == GL_TESS_CONTROL_SHADER)
    {
        finalSource += "#define TESS_CONTROL_SHADER 1 \n";
    }
    else if(shaderStage == GL_TESS_EVALUATION_SHADER)
    {
        finalSource += "#define TESS_EVALUATION_SHADER 1 \n";
    }

    // Add feature-specific defines next in the file.
    finalSource += createFeatureDefines();

    // Finally, include the actual shader source code.
    finalSource += originalSource;

    // We need to resolve #include'd files in the source code.
    // To do this, search the source file for #include statements and keep
    // resolving them until there are no resolves left to do.
    const std::regex includeRegex(R"(#include "[0-9a-zA-Z\.\-_]+")");
    std::smatch includeMatch;
    while(std::regex_search(finalSource, includeMatch, includeRegex))
    {
        // Extract the name of the include file from the include directive
        const std::string includeName = includeMatch.str().substr(10, includeMatch.str().size() - 11);
        const std::string includePath = "Resources/Shaders/Includes/" + includeName;

        // Load the shader include at that path
        const ResourcePPtr<ShaderInclude> includeResource = ResourceManager::instance()->load<ShaderInclude>(includePath);

        // Replace the include directive with the actual source code
        finalSource.replace(includeMatch.position(), includeName.length() + 11, includeResource->originalSource());
    }

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
    unloadAllVariants();
}

void Shader::unloadAllVariants()
{
    //Unloads all shader variants calls their destructor
    loadedVariants_.clear();
}

void Shader::bindVariant(ShaderFeatureList features)
{
    // Ensure only globally enabled features are used
    features = RenderManager::instance()->filterFeatureList(features);

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
