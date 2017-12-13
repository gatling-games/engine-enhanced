#ifndef LIGHTING_INCLUDED
#define LIGHTING_INCLUDED

#include "UniformBuffers.inc.shader"

// The properties that describe a surface and the way it is lit.
struct SurfaceProperties
{
    vec3 diffuseColor;
    vec3 worldNormal;
};

vec4 ComputeLighting(SurfaceProperties surface)
{
    // Use lambertian direct lighting
    float ndotl = max(dot(surface.worldNormal, _LightDirection.xyz), 0.0);
    vec3 directLight = ndotl * surface.diffuseColor * _LightColor.rgb;

    // Use flat ambient lighting
    vec3 ambientLight = surface.diffuseColor * _AmbientColor.rgb;

    // For now, just use lambert + ambient
    return vec4(directLight + ambientLight, 1.0);
}

#endif // LIGHTING_INCLUDED