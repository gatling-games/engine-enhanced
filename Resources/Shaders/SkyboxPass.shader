
#include "UniformBuffers.inc.shader"

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

float getCloudPlane(float cloudPlaneHeight, vec2 cloudScroll, float timeOffset)
{
    // Calculate cloud texture coordinates (scrolling not yet implemented)
    vec2 cloudTexCoord = (cloudPlaneHeight / cameraToSky.y) * cameraToSky.xz;
    cloudTexCoord += cloudScroll * (_Time.x + timeOffset);

    // Get cloud texture from main texture slot
    float cloudLerp = texture(_MainTexture, cloudTexCoord).r;

    return clamp(cloudLerp * 2.5 - 1.2, 0.0, 1.0);
}

void main()
{	
    // If the point is below the horizon, make it dark brown.
    if (cameraToSky.y < 0.0)
    {
        fragColor = vec4(0.02, 0.01, 0.01, 1.0);
        return;
    }

    // Cloud plane parameters
    const float cloudPlaneHeight = 0.8;
    const vec2 cloudScroll1 = vec2(0.1, 0.025);
    const vec2 cloudScroll2 = vec2(0.07, 0.030);

    float cloudLerp1 = getCloudPlane(0.4, cloudScroll1, 0.0f);
    float cloudLerp2 = getCloudPlane(0.2, cloudScroll2, 40.0f);

    const vec4 cloudColor = vec4(1.0, 1.0, 1.0, 1.0);

    // Get cloud texture from main texture slot
    float cloudLerp = min(cloudLerp1 + cloudLerp2, 1.0);

    // Interpolate in the sun colour when the view direction is near the sun direction
    float viewDotSun = max(dot(cameraToSky, _LightDirection.xyz), 0.0);
    float sunLerp = pow(viewDotSun, _SunParams.y);

    // Scale sun lerp upward and clamp at 1.0 to sharpen intensity falloff
    sunLerp = min(sunLerp*_SunParams.x, 1.0);

	// Add clouds and output the final color
    const vec4 skyWithClouds = mix(skyColor, cloudColor, cloudLerp);
    fragColor = mix(skyWithClouds, _LightColor, sunLerp);
}

#endif // FRAGMENT_SHADER