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

    // Water settings
    uniform vec3 _WaterColor;
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
    uniform vec4 _TerrainTextureOffsetScale; // XY is offset, ZW is scale
    uniform vec4 _TerrainSize; // XYZ, W is layercount(int)
    uniform vec4 _TextureScale; // XY
    uniform vec4 _TerrainColor[MAX_TERRAIN_LAYERS]; //Terrain color from material
    uniform vec4 _TerrainLayerBlendData[MAX_TERRAIN_LAYERS]; // xy = offset and scale for altitude blend, zw for slope
    sampler2D _TerrainHeightmap; //Texture containing the terrain heightmap
    sampler2D _TerrainTextures[MAX_TERRAIN_LAYERS]; //Terrain albedo textures
    sampler2D _TerrainNormalMapTextures[MAX_TERRAIN_LAYERS]; //Terrai normal Textures
    uniform float _WaterDepth;
};


#endif // UNIFORM_BUFFERS_INCLUDED