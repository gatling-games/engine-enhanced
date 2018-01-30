#ifndef UNIFORM_BUFFERS_INCLUDED
#define UNIFORM_BUFFERS_INCLUDED

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
};

//Terrain uniform buffer
layout(std140) uniform terrain_data
{
    uniform vec4 _TerrainCoordinateOffsetScale; // XY is offset, ZW is scale
    uniform vec4 _TerrainSize; // XYZW, w is normal scale
    uniform vec4 _TextureScale; // XY
};


#endif // UNIFORM_BUFFERS_INCLUDED