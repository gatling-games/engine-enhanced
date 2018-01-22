
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

    // Use lambertian direct lighting
    float ndotl = max(dot(surface.worldNormal, _LightDirection.xyz), 0.0);
    vec3 directLight = ndotl * surface.diffuseColor * _LightColor.rgb;
    vec3 ambientLight = surface.diffuseColor * _AmbientColor.rgb;
    fragColor = vec4(directLight + ambientLight, 1.0);
}

#endif // FRAGMENT_SHADER