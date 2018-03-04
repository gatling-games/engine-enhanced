#ifndef COMMON_SHADER_CODE_INCLUDED
#define COMMON_SHADER_CODE_INCLUDED 1

#include "UniformBuffers.inc.shader"

#define M_PI 3.14159265359

// The properties that describe a surface and the way it is lit.
struct SurfaceProperties
{
    vec3 diffuseColor;
    float occlusion;
    float gloss;
    float translucency;
    vec3 worldNormal;
};

/*
 * Normal map textures are stored in the DXT5nm format.
 * This function unpacks the texture into xyz tangent space (z is up).
 */
vec3 unpackDXT5nm(vec4 compressed)
{
	vec3 n;
	n.xy = compressed.ag * 2.0 - 1.0;
	n.z = sqrt(1.0 - n.x*n.x - n.y*n.y);
	return n;
}

/*
 * Computes volumetric fog for the given unnormalized cameraToWorld vector.
 * Integrates fog for a density _FogDensity * e ^ (-_FogHeightFalloff * y) at a given point.
 * Based on "Real-time Atmospheric Effects in Games" [Wenzel 2006, Crytek] 
 */
float computeVolumetricFog(vec3 cameraPos, vec3 cameraToWorld, float cameraToWorldDist)
{
    float fogDensityAtViewer = exp(-_FogHeightFalloff * cameraPos.y);
    float integral = fogDensityAtViewer * cameraToWorldDist;

    float slopeThreshold = 0.001;
    if (abs(cameraToWorld.y) > slopeThreshold)
    {
        float cdy = _FogHeightFalloff * cameraToWorld.y;
        integral *= (1.0 - exp(-cdy)) / cdy;
    }

    return 1.0 - exp(-_FogDensity * integral);
}

/*
 * Applies fog to the incoming radiance, based on the specified fog density.
 * The fog density can be calculated using computeVolumetricFog() 
 */
vec3 applyVolumetricFog(vec3 light, float fogDensity, vec3 fogColor)
{
    return mix(light, fogColor, fogDensity);
}

#endif // COMMON_SHADER_CODE_INCLUDED