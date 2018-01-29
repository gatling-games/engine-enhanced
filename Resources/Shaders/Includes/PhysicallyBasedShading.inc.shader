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

#endif // PHYSICALLY_BASED_SHADING_INCLUDED