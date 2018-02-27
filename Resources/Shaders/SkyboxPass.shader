
#include "Common.inc.shader"
#include "UniformBuffers.inc.shader"

#ifdef VERTEX_SHADER

layout(location = 0) in vec4 _position;

out vec3 viewDirUnnormalized;

void main()
{
    gl_Position = _ViewProjectionMatrix * (_LocalToWorld * _position);
    viewDirUnnormalized = _position.xyz;
    viewDirUnnormalized.y = max(0.0, viewDirUnnormalized.y);
}

#endif // VERTEX_SHADER

#ifdef FRAGMENT_SHADER

// Based on "Precomputed Atmospheric Scattering" [Bruneton 2008]
#include "Atmosphere.inc.shader"

in vec3 viewDirUnnormalized;
out vec4 fragColor;

void main()
{
    vec3 viewPos = _CameraPosition.xyz;
    viewPos.y += Rg;
    vec3 viewDir = normalize(viewDirUnnormalized);

    // Determine the direct light from the sun
    float isInSun = clamp(pow(dot(viewDir, _LightDirectionIntensity.xyz), 1024.0), 0.0, 1.0);
    float sunIntensity = _LightDirectionIntensity.w;
    vec3 L0 = isInSun * sunIntensity * TDirection(viewPos, _LightDirectionIntensity.xyz);
  
    // Add the in-scattered light along the view
    L0 += InScatteringDirection(viewPos, viewDir) * _LightDirectionIntensity.w;

    // Add the components together
    fragColor = vec4(L0, 1.0);
}

#endif // FRAGMENT_SHADER