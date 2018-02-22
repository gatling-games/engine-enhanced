#ifndef SHADOWS_SHADER_CODE_INCLUDED
#define SHADOWS_SHADER_CODE_INCLUDED

#include "UniformBuffers.inc.shader"

layout (binding = 10) uniform sampler2DArrayShadow _ShadowMapTexture;

int DetermineShadowCascade(float distance)
{
    return distance < _CascadeMaxDistances.y
        ? (distance < _CascadeMaxDistances.x ? 0 : 1)
        : (distance < _CascadeMaxDistances.z ? 2 : 3);
}

float SampleSunShadow(vec3 worldPosition, float distance)
{
    // First, determine which shadow cascade the point falls into.
    int cascade = DetermineShadowCascade(distance);

    // Convert the world position into shadow coords
    vec4 shadowCoord = _WorldToShadow[cascade] * vec4(worldPosition, 1.0);
    shadowCoord.xyw = shadowCoord.xyz;
    shadowCoord.z = float(cascade);

#ifdef SOFT_SHADOWS
    // Soft shadows enabled, use a PCF filter with a large number of taps.
    // This could be optimised to use a smaller number of lookups to get the same result,
    // by carefully placing each tap (they each actually get 4 texels and perform a bilinear lerp),
    // but this works and it fast enough for now.
    float offset = 1.0 / float(textureSize(_ShadowMapTexture, 0).x) * 0.5;
    float sum = 0.0;
    for (float xOffset = -offset * 3.0; xOffset <= offset * 3.00001; xOffset += offset)
    {
        for (float yOffset = -offset * 3.0; yOffset <= offset * 3.00001; yOffset += offset)
        {
            sum += texture(_ShadowMapTexture, shadowCoord + vec4(xOffset, yOffset, 0.0, 0.0));
        }
    }
    
    return sum / 49.0;
#else
    // Soft shadows disabled, use a single tap.
    return texture(_ShadowMapTexture, shadowCoord);
#endif

    // Sample the shadow map at that point
}

#endif // SHADOWS_SHADER_CODE_INCLUDED