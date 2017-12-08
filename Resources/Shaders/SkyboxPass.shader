// Scene uniform buffer
layout(std140) uniform scene_data
{
    uniform vec4 _AmbientColor;
    uniform vec4 _LightColor;
    uniform vec4 _LightDirection;
    uniform vec4 _SkyTopColor;
    uniform vec4 _SkyHorizonColor;
    uniform vec4 _SunParams; // x = size; y = falloff; z&w = unused
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
out vec3 cameraToSky;

void main()
{
    // Project the vertex position to clip space
    gl_Position = _ViewProjectionMatrix * (_LocalToWorld * _position);

    // Interpolate between the horizon and sky top colors
    skyColor = mix(_SkyHorizonColor, _SkyTopColor, _position.y);

    // Set camera to sky vector to skybox vertex position
    cameraToSky = _position.xyz;
}

#endif // VERTEX_SHADER

#ifdef FRAGMENT_SHADER

uniform sampler2D _MainTexture;

// Interpolated values from vertex shader
in vec4 skyColor;
in vec3 cameraToSky;

// Final colour
out vec4 fragColor;

void main()
{	
    // Cloud plane parameters
    const float cloudPlaneHeight = 0.1;
    const float cloudWind = 20.0;
    const vec2 cloudScroll = vec2(0.055, 0.005);

    // Calculate cloud texture coordinates (scrolling not yet implemented)
    vec2 cloudTexCoord = (cloudPlaneHeight / cameraToSky.y) * cameraToSky.xz;
    cloudTexCoord += cloudScroll * 0.0 * cloudWind;

    // Get cloud texture from main texture slot
    vec4 clouds = texture(_MainTexture, cloudTexCoord);

    // Interpolate in the sun colour when the view direction is near the sun direction
    float viewDotSun = max(dot(cameraToSky, _LightDirection.xyz), 0.0);
    float sunLerp = pow(viewDotSun, _SunParams.y);

    // Scale sun lerp upward and clamp at 1.0 to sharpen intensity falloff
    sunLerp = min(sunLerp*_SunParams.x, 1.0);

	// Add clouds and output the final color
    fragColor = mix(skyColor, _LightColor, sunLerp);
    fragColor += clouds;
}

#endif // FRAGMENT_SHADER