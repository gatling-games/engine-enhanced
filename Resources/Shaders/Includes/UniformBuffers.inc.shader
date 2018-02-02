#ifndef UNIFORM_BUFFERS_INCLUDED
#define UNIFORM_BUFFERS_INCLUDED
#define MAX_TERRAIN_LAYERS 32

// Scene uniform buffer
layout(std140) uniform scene_data
{
    uniform vec4 _AmbientColor;
    uniform vec4 _LightColor;
    uniform vec4 _LightDirection;
    uniform vec4 _SkyTopColor;
    uniform vec4 _SkyHorizonColor;
    uniform vec4 _SunParams; // x = size, y = falloff
    uniform vec4 _Time; // x = time, y = 1/time
};

// Camera uniform buffer
layout(std140) uniform camera_data
{
    uniform vec4 _ScreenResolution; // xyzw = width, height, 1/width, 1/height
    uniform vec4 _CameraPosition;
    uniform mat4x4 _ViewProjectionMatrix;
    uniform mat4x4 _ClipToWorld;
};

// Per-draw uniform buffer.
layout(std140) uniform per_draw_data
{
    uniform mat4x4 _LocalToWorld;
    uniform vec4 _Color; // rgb = color, a = smoothness
    sampler2D _AlbedoTexture;
    sampler2D _NormalMapTexture;
};

//Terrain uniform buffer
layout(std140) uniform terrain_data
{
    uniform vec4 _TerrainCoordinateOffsetScale; // XY is offset, ZW is scale
    uniform vec4 _TerrainSize; // XYZW, W is layercount(int)
    uniform vec4 _TextureScale; // XY
    uniform vec4 _SlopeAltitudeData[MAX_TERRAIN_LAYERS]; //XY is min max slope height, ZW is min max altitude
    sampler2D _TerrainHeightmap;
    sampler2D _TerrainTextures[MAX_TERRAIN_LAYERS];
    sampler2D _TerrainNormalMapTextures[MAX_TERRAIN_LAYERS];
};


#endif // UNIFORM_BUFFERS_INCLUDED