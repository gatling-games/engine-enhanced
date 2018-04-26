#ifndef TERRAIN_SHADER_CODE_INCLUDED
#define TERRAIN_SHADER_CODE_INCLUDED

#include "UniformBuffers.inc.shader"

layout(binding = 8) uniform sampler2D _TerrainHeightmap;
layout(binding = 9) uniform sampler2D _TerrainDestructionmap;

#endif // TERRAIN_SHADER_CODE_INCLUDED
