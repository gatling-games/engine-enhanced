#ifndef LIGHTING_INCLUDED
#define LIGHTING_INCLUDED

#include "UniformBuffers.inc.shader"

// The properties that describe a surface and the way it is lit.
struct SurfaceProperties
{
    vec3 diffuseColor;
    vec3 worldPosition;
    vec3 worldNormal;
};

vec4 ComputeLighting(SurfaceProperties surface)
{
    // Use lambertian direct lighting
    float ndotl = max(dot(surface.worldNormal, _LightDirection.xyz), 0.0);
    vec3 directLight = ndotl * surface.diffuseColor * _LightColor.rgb;

    // Use flat ambient lighting
    vec3 ambientLight = surface.diffuseColor * _AmbientColor.rgb;

    // Get the combined lighting
    vec3 light = directLight + ambientLight;

    // Add fog
    float viewDist = length(surface.worldPosition - _CameraPosition.xyz);
    float fogDensity = 0.0115 * viewDist;
    fogDensity = 1.0 - exp2(-fogDensity * fogDensity);

    // Apply fog to the fragColor
    vec3 fogColor = vec3(0.5, 0.5, 0.5);
    return vec4(mix(light, fogColor, fogDensity), 1.0);
}

#endif // LIGHTING_INCLUDED