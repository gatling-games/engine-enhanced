#ifndef TERRAIN_SHADER_CODE_INCLUDED
#define TERRAIN_SHADER_CODE_INCLUDED

#include "UniformBuffers.inc.shader"

layout(binding = 8) uniform sampler2D _TerrainHeightmap;
layout(binding = 9) uniform sampler2D _TerrainDestructionmap;

float getHeightmapHeight(vec2 coords)
{
    float heightmap = texture(_TerrainHeightmap, coords / _TerrainSize.xz).r;
    float destruction = texture(_TerrainDestructionmap, coords / _TerrainSize.xz).r;

    // Scale and reduce based on water level
    return max((heightmap - destruction), 0) * _TerrainSize.y -_WaterColorDepth.a;
}

vec3 getWorldNormal(vec2 coords, out vec3 worldTangent, out vec3 worldBitangent)
{
    vec2 texcoord = coords;

    // Compute the offset from the normalized position to get the adjacent heightmap pixels
    ivec2 heightmapRes = textureSize(_TerrainHeightmap, 0);
    vec2 heightmapTexelSize = 1.0 / heightmapRes;

    // Determine the gradient along x and z at the vertex position
    float x1 = texture(_TerrainHeightmap, texcoord + heightmapTexelSize * vec2(-1.0, 0.0)).r;
    float x2 = texture(_TerrainHeightmap, texcoord + heightmapTexelSize * vec2(1.0, 0.0)).r;
    float z1 = texture(_TerrainHeightmap, texcoord + heightmapTexelSize * vec2(0.0, -1.0)).r;
    float z2 = texture(_TerrainHeightmap, texcoord + heightmapTexelSize * vec2(0.0, 1.0)).r;
    float dydx = x2 - x1;
    float dydz = z2 - z1;
    dydx *= _TerrainSize.y;
    dydz *= _TerrainSize.y;
    dydx /= (2.0 * heightmapTexelSize.x) * _TerrainSize.x;
    dydz /= (2.0 * heightmapTexelSize.y) * _TerrainSize.z;

    // Determine the tangents along the X and Z
    worldTangent = normalize(vec3(1.0, dydx, 0.0));
    worldBitangent = normalize(vec3(0.0, dydz, 1.0));

    return cross(worldBitangent, worldTangent);
}
#endif // TERRAIN_SHADER_CODE_INCLUDED
