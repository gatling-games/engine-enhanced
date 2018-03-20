#pragma once

#include <GL/gl3w.h>

#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/Matrix4x4.h"
#include "Math/Color.h"

// For bindlesstexturehandle
#include "Renderer/Texture.h"
#include "Scene/Terrain.h"

// Enum for specifying buffer type to UniformBuffer constructor
enum class UniformBufferType
{
    SceneBuffer = 0,
    CameraBuffer = 1,
    ShadowsBuffer = 2,
    PerDrawBuffer = 3,
    PerMaterialBuffer = 4,
    TerrainBuffer = 5,
    TerrainDetailsBuffer = 6,
};

// Plain old uniform data for scene
struct SceneUniformData
{
    Color ambientLightColor;

    // The primary light direction and intensity
    // xyz = to light dir, normalized
    // w = light intensity
    Vector4 lightDirectionIntensity;

    Vector4 time; // x = time, y = 1/time

    // Fog settings
    float fogDensity;
    float fogHeightFalloff;

    // Ambient occlusion settings
    float ambientOcclusionDistance;
    float ambientOcclusionFalloff;
    Vector4 ambientOcclusionPoissonDisks[16];
};

// Plain old uniform data for camera
struct CameraUniformData
{
    Vector4 screenResolution;
    Vector4 cameraPosition;
    Matrix4x4 worldToClip;
    Matrix4x4 clipToWorld;
};

// Data relating to shadowmapping
// Stored inside the ShadowMap class
struct ShadowUniformData
{
    float cascadeMaxDistances[4];
    Matrix4x4 worldToShadow[4];
};

struct TerrainUniformData
{
    Vector4 terrainSize;
	
    // rgb = Water color, a = water depth
    Vector4 waterColorDepth;

    // Per-layer data
    Vector4 terrainLayerBlendData[Terrain::MAX_LAYERS];
    Vector4 terrainLayerScale[Terrain::MAX_LAYERS]; // xy for scale, zw unused
    Color terrainLayerColours[Terrain::MAX_LAYERS];
    BindlessTextureHandle terrainTextures[Terrain::MAX_LAYERS*2]; // x2 to pad each value to 16 bytes
    BindlessTextureHandle terrainNormalMapTextures[Terrain::MAX_LAYERS*2];  // x2 to pad each value to 16 bytes
};

// Plain old uniform data for converting object local coordinates to world space
struct PerDrawUniformData
{
    Matrix4x4 localToWorld;
    Color colorSmoothness;
    BindlessTextureHandle albedoTexture;
    BindlessTextureHandle normalMapTexture;
};

struct TerrainDetailsData
{
    Vector4 detailPositions[DetailBatch::MaxInstancesPerBatch];
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