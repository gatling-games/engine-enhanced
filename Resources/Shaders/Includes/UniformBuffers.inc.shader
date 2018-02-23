#ifndef UNIFORM_BUFFERS_INCLUDED
#define UNIFORM_BUFFERS_INCLUDED
#define MAX_TERRAIN_LAYERS 32

// Scene uniform buffer
layout(std140, binding = 0) uniform scene_data
{
    uniform vec4 _AmbientColor;
    uniform vec4 _LightColor;
    uniform vec4 _LightDirection;
    uniform vec4 _SkyTopColor;
    uniform vec4 _SkyHorizonColor;
    uniform vec4 _SunParams; // x = size, y = falloff
    uniform vec4 _Time; // x = time, y = 1/time

    // Fog settings
    uniform vec4 _FogColor;
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

    sampler2D _TerrainHeightmap;
};


#endif // UNIFORM_BUFFERS_INCLUDED