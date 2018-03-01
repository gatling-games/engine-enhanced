#ifndef UNIFORM_BUFFERS_INCLUDED
#define UNIFORM_BUFFERS_INCLUDED
#define MAX_TERRAIN_LAYERS 32

// Scene uniform buffer
layout(std140, binding = 0) uniform scene_data
{
    uniform vec4 _AmbientColor;

    // The primary light direction and intensity
    // xyz = to light dir, normalized
    // w = light intensity
    uniform vec4 _LightDirectionIntensity;

    // x = time, y = 1/time
    uniform vec4 _Time;

    // Fog settings
    uniform float _FogDensity;
    uniform float _FogHeightFalloff;
};

// Camera uniform buffer
layout(std140, binding = 1) uniform camera_data
{
    uniform vec4 _ScreenResolution; // xyzw = width, height, 1/width, 1/height
    uniform vec4 _CameraPosition;
    uniform mat4x4 _ViewProjectionMatrix;
    uniform mat4x4 _ClipToWorld;
};

layout(std140, binding = 2) uniform shadows_data
{
    uniform vec4 _CascadeMaxDistances;
    uniform mat4x4 _WorldToShadow[4];
};

// Per-draw uniform buffer.
layout(std140, binding = 3) uniform per_draw_data
{
    uniform mat4x4 _LocalToWorld;
    uniform vec4 _Color; // rgb = color, a = smoothness
    sampler2D _AlbedoTexture;
    sampler2D _NormalMapTexture;
};

//Terrain uniform buffer
layout(std140, binding = 5) uniform terrain_data
{
    uniform vec4 _TerrainSize; // XYZ, W is layercount   
	
    // The base colour and maximum depth of the terrain water
    // rgb = base color
    // a = max depth, m
    uniform vec4 _WaterColorDepth;
	
    // The blending settings for each terrain layer
    // x = altitude border, y = altitude transition
    // z = slope border, w = slope transition
    uniform vec4 _TerrainLayerBlendData[MAX_TERRAIN_LAYERS];

    // The texture scale/offset for each layer material
    uniform vec4 _TerrainLayerScale[MAX_TERRAIN_LAYERS];

    // The material base colour for each terrain layer
    uniform vec4 _TerrainColor[MAX_TERRAIN_LAYERS];

    // The albedo / smoothness texture handle for each layer
    sampler2D _TerrainTextures[MAX_TERRAIN_LAYERS];

    // The tangent space normal map texture handle for each layer
    sampler2D _TerrainNormalMapTextures[MAX_TERRAIN_LAYERS];
};

// Terrain details uniform buffer.
// The buffer is updated once per details batch.
layout(std140, binding = 6) uniform terrain_details_data
{
    // World-space offsets for each terrain detail.
    // Once per detail mesh instance, up to 1024 in total.
    uniform vec4 _TerrainDetailPositions[1024];
};

#endif // UNIFORM_BUFFERS_INCLUDED