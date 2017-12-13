#pragma once

#include <GL/gl3w.h>

#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/Matrix4x4.h"
#include "Math/Color.h"

// Enum for specifying buffer type to UniformBuffer constructor
enum class UniformBufferType
{
    SceneBuffer = 0,
    CameraBuffer = 1,
    ShadowsBuffer = 2,
    PerDrawBuffer = 3,
    PerMaterialBuffer = 4,
    TerrainBuffer = 5
};

// Plain old uniform data for scene
struct SceneUniformData
{
    Color ambientLightColor;
    Color lightColor;
    Vector4 toLightDirection;
    Color skyTopColor;
    Color skyHorizonColor;
    Vector4 sunParams; // x = falloff, y = size
    Vector4 time; // x = time, y = 1/time
};

// Plain old uniform data for camera
struct CameraUniformData
{
    Vector4 screenResolution;
    Vector4 cameraPosition;
    Matrix4x4 worldToClip;
    Matrix4x4 clipToWorld;
};

struct TerrainUniformData
{
    Vector4 terrainCoordinateOffsetScale;
    Vector4 terrainSize;
	Vector4 textureScale;
};

// Plain old uniform data for converting object local coordinates to world space
struct PerDrawUniformData
{
    Matrix4x4 localToWorld;
};

struct PerMaterialUniformData
{
    
};

template <typename T>
class UniformBuffer
{
public:
    explicit UniformBuffer(UniformBufferType type)
    {
        // Set buffer type
        type_ = type;

        // Generate buffer and set buffer ID
        glCreateBuffers(1, &bufferID_);
    }

    ~UniformBuffer()
    {
        if (bufferID_ != 0)
        {
            glDeleteBuffers(1, &bufferID_);
        }
    }

    // Prevent the buffer being copied
    UniformBuffer(const UniformBuffer&) = delete;
    UniformBuffer& operator=(const UniformBuffer&) = delete;

    // Allow the buffer to be moved
    UniformBuffer(UniformBuffer&& other)
    {
        // Steal the contents of other
        type_ = other.type_;
        bufferID_ = other.bufferID_;
        
        // Reset other
        other.type_ = 0;
        other.bufferID_ = 0;
    }

    UniformBuffer& operator=(UniformBuffer&& other)
    {
        if (this != &other)
        {
            // Steal the contents of other
            type_ = other.type_;
            bufferID_ = other.bufferID_;

            // Reset other
            other.type_ = 0;
            other.bufferID_ = 0;
        }

        return *this;
    }

    // Bind a buffer object to memory on GPU and create/populate buffer data store
    void update(const T &data) const
    {
        glNamedBufferData(bufferID_, sizeof(T), &data, GL_DYNAMIC_DRAW);
    }

    // Bind buffer to usage slot governed by buffer type
    void use() const
    {
        glBindBufferBase(GL_UNIFORM_BUFFER, static_cast<GLuint>(type_), bufferID_);
    }

private:
    UniformBufferType type_;
    GLuint bufferID_;
};