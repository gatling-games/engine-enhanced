// Scene uniform buffer
layout(std140) uniform scene_data
{
    uniform vec4 _AmbientColor;
    uniform vec4 _LightColor;
    uniform vec4 _LightDirection;
    uniform vec4 _SkyTopColor;
    uniform vec4 _SkyHorizonColor;
};

// Camera uniform buffer
layout(std140) uniform camera_data
{
    uniform vec4 _ScreenResolution;
    uniform vec4 _CameraPosition;
    uniform mat4x4 _ViewProjectionMatrix;
    uniform mat4x4 _ClipToWorld;
};

// Per-draw uniform buffer.
layout(std140) uniform per_draw_data
{
    uniform mat4x4 _LocalToWorld;
};

#ifdef VERTEX_SHADER

// Vertex attributes
layout(location = 0) in vec4 _position;

// Interpolated values to fragment shader
out vec4 skyColor;

void main()
{
    // Project the vertex position to clip space
    gl_Position = _ViewProjectionMatrix * (_LocalToWorld * _position);

    // Interpolate between the horizon and sky top colors
    skyColor = mix(_SkyHorizonColor, _SkyTopColor, _position.y);
}

#endif // VERTEX_SHADER

#ifdef FRAGMENT_SHADER

// Interpolated values from vertex shader
in vec4 skyColor;

// Final colour
out vec4 fragColor;

void main()
{	
	// Output the final color
    fragColor = skyColor;
}

#endif // FRAGMENT_SHADER