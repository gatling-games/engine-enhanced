
#include "UniformBuffers.inc.shader"

#define USE_SCREENSPACE_VERT_SHADER
#define USE_GBUFFER_READ
#include "Deferred.inc.shader"

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
}

#endif // FRAGMENT_SHADER