#ifndef PHYSICALLY_BASED_SHADING_INCLUDED
#define PHYSICALLY_BASED_SHADING_INCLUDED

// Assume non-metallic surfaces have a specular color rf0 of 0.04 linear
#define NON_METALLIC_SPECULAR 0.04

/*
 * Normalized blinn-phong visibility function
 * Based on the black ops 1 brdf (Lazarov 2013 course notes)
 */
float normalizedBlinnPhong(float a, float ndoth)
{
    return (a + 2.0) / (8.0) * pow(ndoth, a);
}

/*
 * Schlick Smith visibility function
 * Again, based on the adaptations from Lazarov 2013
 */
float schlickSmithVisibility(float a, float ndotl, float ndotv)
{
    float k = 2.0 / sqrt(M_PI * (a + 2.0));
    k = 1.0 / sqrt((M_PI / 4.0) * a + (M_PI / 2.0));
    return 1.0 / ((ndotl * (1.0 - k) + k) * (ndotv * (1.0 - k) + k));
}

/*
 * An approximation to schlick fresnel.
 * From "Getting More Physical in Call of Duty Black Ops 2" (Lazarov 2013) course notes.
 */
float schlickFresnel_lazarov2013(float f0, float x)
{
    return f0 + (1.0 - f0) * exp2(-10.0 * x);
}

/*
 * Schlick approximation for fresnel.
 */
float schlickFresnel(float f0, float x)
{
#if 0
    return f0 + (1.0 - f0) * pow(1.0 - x, 5.0);
#endif

    // Use the lazarov approximation
    return schlickFresnel_lazarov2013(f0, x);
}

/*
 * Diffuse term used for Disney pbs.
 * From "Physically Based Shading at Disney" (Burley 2012) course notes.
 */
float disneyDiffuse(float ldoth, float ndotl, float ndotv, float roughness)
{
    float fd90 = 0.5 + 2.0 * ldoth * ldoth * roughness;
    float fresnel0 = 1.0 + (fd90 - 1.0) * pow(1.0 - ndotl, 5.0);
    float fresnel1 = 1.0 + (fd90 - 1.0) * pow(1.0 - ndotv, 5.0);
    return fresnel0 * fresnel1 * ndotl;
}

/*
 * Diffuse term for standard lambertian lighting.
 */
float lambertDiffuse(float ndotl)
{
    return ndotl;
}

vec3 PhysicallyBasedBRDF(SurfaceProperties surface, vec3 viewDir)
{
    vec3 light = vec3(0.0);

    vec3 halfVector = normalize(_LightDirection.xyz + viewDir);

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
    return light;
}

#endif // PHYSICALLY_BASED_SHADING_INCLUDED