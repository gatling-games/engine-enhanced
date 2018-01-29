
#include "UniformBuffers.inc.shader"

#define USE_SCREENSPACE_VERT_SHADER
#define USE_GBUFFER_READ
#include "Deferred.inc.shader"

#include "PhysicallyBasedShading.inc.shader"

#ifdef FRAGMENT_SHADER

out vec4 fragColor;

void main()
{
    // Sample the gBuffer textures
    SurfaceProperties surface = readGBuffer();

    // Retrive the world space position by sampling the depth buffer
    vec3 worldPosition = readGBufferWorldPos();

    // Compute the view vector and half vector from the world position
    vec3 viewDir = _CameraPosition.xyz - worldPosition;
    float viewDistance = length(viewDir);
    viewDir /= viewDistance;
    vec3 halfVector = normalize(_LightDirection.xyz + viewDir);

    // Ambient term
    vec3 light = surface.diffuseColor * _AmbientColor.rgb * 0.2;

    // We are using a cook-torrance based brdf, based on [Lazarov2013].
    // Like lazarov, it makes sense to split into 3 components
    // D - normal distribution function, including (PI / 4).
    // F - fresnel effect
    // V - visibility function, including divide by (n.l)(n.v)
    
    // The microfacet brdf needs the following dot products
    float ndoth = max(dot(halfVector, surface.worldNormal), 0.0);
    float ndotl = max(dot(surface.worldNormal, _LightDirection.xyz), 0.0);
    float ndotv = max(dot(surface.worldNormal, viewDir), 0.0);
    float ldoth = dot(_LightDirection.xyz, halfVector); // Angle always < 90, so no clamp needed

    // Lazarov suggests encoding gloss using 8192.0^smoothness
    float a = exp2(13.0 * surface.gloss); // Equivalent to pow(8192.0, surface.gloss) [Lazarov13];
    float D = normalizedBlinnPhong(a, ndoth);
    float F = schlickFresnel(NON_METALLIC_SPECULAR, ldoth);
    float V = schlickSmithVisibility(a, ndotl, ndotv);

    // Combine the D, F and V terms for the specular BRDF.
#ifdef SPECULAR_ON
    float fs = F * V * D;
    light += fs * _LightColor.rgb * ndotl;
#endif
    
    // For the diffuse brdf, we support either disney's model or 
    // lambert diffuse.
    // Disney's model looks better, giving a slight sheen to the edges of
    // rough surfaces and darkening the edges of smooth ones, but it is
    // more computationally expensive (its disney, its expected).
#if 1
    float fd = disneyDiffuse(ldoth, ndotl, ndotv, 1.0 - surface.gloss);
#else
    float fd = lambertDiffuse(ndotl);
#endif
    
    // Use the diffuse brdf for diffuse lighting
    light += fd * _LightColor.rgb * surface.diffuseColor;

    // Add fog based on view distance.
#ifdef FOG_ON
    vec3 fogColor = vec3(0.5, 0.5, 0.5);
    float fogDensity = 0.0025 * viewDistance;
    fogDensity = 1.0 - exp2(-fogDensity * fogDensity);
    light = mix(light, fogColor, fogDensity);
#endif

    fragColor = vec4(light, 0.0);
}

#endif // FRAGMENT_SHADER