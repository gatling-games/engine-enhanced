
#include "UniformBuffers.inc.shader"

#define USE_SCREENSPACE_VERT_SHADER
#define USE_GBUFFER_READ
#include "Deferred.inc.shader"

#include "Shadows.inc.shader"

#ifdef FRAGMENT_SHADER

out vec4 fragColor;

void main()
{
	// Sample the gBuffer textures
	SurfaceProperties surface = readGBuffer();

    // Diffuse color mode
#ifdef DEBUG_GBUFFER_ALBEDO
	fragColor = vec4(surface.diffuseColor, 1.0);
#endif

    // Gloss mode
#ifdef DEBUG_GBUFFER_GLOSS
    fragColor = vec4(surface.gloss);
#endif

    // Normals mode
#ifdef DEBUG_GBUFFER_NORMALS
    fragColor = vec4(surface.worldNormal * 0.5 + 0.5, 1.0);
#endif

    // Occlusion mode
#ifdef DEBUG_GBUFFER_OCCLUSION
    fragColor = vec4(surface.occlusion);
#endif

    // Shadows mode
#ifdef DEBUG_SHADOWS
    float distance = length(_CameraPosition.xyz - readGBufferWorldPos());
    vec3 worldPosition = readGBufferWorldPos();
    fragColor = vec4(SampleSunShadow(worldPosition, distance) * clamp(dot(_LightDirection.xyz, surface.worldNormal) * 5.0, 0.0, 1.0));
#endif

    // Shadow cascade splits mode
#ifdef DEBUG_SHADOW_CASCADES
    float distance = length(_CameraPosition.xyz - readGBufferWorldPos());
    int cascade = DetermineShadowCascade(distance);

    // Per-Cascade colours
    if (cascade == 0) fragColor = vec4(106.0, 8.0, 8.0, 255.0) / 256.0;
    if (cascade == 1) fragColor = vec4(239.0, 225.0, 67.0, 255.0) / 256.0;
    if (cascade == 2) fragColor = vec4(37.0, 67.0, 107.0, 255.0) / 256.0;
    if (cascade == 3) fragColor = vec4(4.0, 91.0, 22.0, 255.0) / 256.0;
    fragColor.rgb = mix(fragColor.rgb, surface.diffuseColor, 0.25);
#endif
}

#endif // FRAGMENT_SHADER