#ifndef ATMOSPHERE_SHADER_CODE_INCLUDED
#define ATMOSPHERE_SHADER_CODE_INCLUDED

#include "Common.inc.shader"

//////////////////////////////////////////////////////////////////
// Atmospheric scattering code.
//
// All equations and constants are based on "Precomputed Atmospheric Scattering" [Bruneton 2008]
// unless otherwise noted.
//
// A few marked parts are based on "Physically Based Sky, Atmosphere and Cloud Rendering 
// in Frostbite" [Hilliare16]
//////////////////////////////////////////////////////////////////

// Look up table textures
layout(binding = 5) uniform sampler2D _TransmittanceLUT;

// Constants for atmospheric scattering in Earth's atmosphere.
// The constants below match the names and values from Bruneton08

// The earth's radius at ground height
const float Rg = 6360.0 * 1000.0; // 6360 km

// The earth's radius at the top of the atmosphere
const float Rt = 6420.0 * 1000.0; // 6420 km

// The thickness of the atmosphere if its density was uniform
const float HR = 8.0 * 1000.0; // 8 km

// The wavelengths of red, green and blue light.
const vec3 Lambda = vec3(680.0, 550.0, 440.0) * 0.000000001;

// The rayleigh scattering coefficient for r, g, and b light at ground level
const vec3 RayleighS = vec3(0.0000058, 0.0000135, 0.0000331); // Scattered % per m

// The rayleigh extinction coefficient for rgb light
const vec3 RayleighE = RayleighS;

// The mie scattering coefficient at ground level
const float MieScattering = 0.00002;

// The mie absorption coefficient, relative to mie scattering.
const float MieAbsorption = 1.11; // [bruneton08]

// The ozone absorption coefficient (ozone has no scattering, only absorption)
const vec3 OzoneAbsorption = vec3(3.426, 8.298, 0.356) * 0.0000006; // From [Hilliare16]

/*
 * Computes the rayleigh scattering coefficient at the given altitude h
 * Returns the coefficients for red, green and blue light.
 */
vec3 rayleighScattering(float h)
{
    // The thickness of the atmosphere if its density was uniform
    const float Hr = 8000.0; // 8km

    // The rayleigh scattering coeff decreases exponentially with height
    return RayleighS * exp(-h / Hr);
}

/*
 * Computes the rayleigh phase function
 */
float rayleighPhase(float costheta)
{
    return 3.0 / (16.0 * M_PI) * (1.0 + costheta * costheta);
}

/*
 * Computes the mie scattering coefficient at the given altitude h
 * Returns the coefficients for red, green and blue light.
 */
float mieScattering(float h)
{
    // Use a smaller height scale than rayleigh
    // Bruneton suggests 1200
    const float Hr = 1200.0; // 1.2 km

    // The mie scattering coeff decreases exponentially with height
    return MieScattering * exp(-h / Hr);
}

/*
 * Computes the mie phase function
 */
float miePhase(float costheta)
{
    const float g = 0.5;

    return 3.0 / (8.0 * M_PI) * ((1.0 - g*g) * (1.0 + costheta * costheta))
        / ((2.0 + g*g) * pow(1.0 + g*g - 2.0 * g * costheta, 1.5));
}

/*
 * Computes the ozone absorption at the specified altitude h
 * Based on [Hilliare16]
 */
vec3 ozoneAbsorption(float h)
{
    // [Hilliare16] found that the ozone result was better if it 
    // followed the same distribution as rayleigh scattering.

    // The thickness of the atmosphere if its density was uniform
    const float Hr = 8000.0; // 8km

    // The rayleigh scattering coeff decreases exponentially with height
    return OzoneAbsorption * exp(-h / Hr);
}

/*
 * Computes the sum of all absorption coefficients at the altitude h
 */
vec3 absorptionAtAltitude(float h)
{
    // Rayleigh is only scattering, so absorption = scattering
    vec3 rayleigh = rayleighScattering(h);

    // Mie scattering
    float mie = MieAbsorption * mieScattering(h);

    // Ozone absorption
    vec3 ozone = ozoneAbsorption(h);

    return clamp(rayleigh + vec3(mie) + ozone, 0.0, 1.0);
}

/*
 * Computes the point on the top of the atmosphere that ray x + tv will hit.
 * x is the origin position, including Rg.
 */
vec3 PointOnAtmosphereTop(vec3 x, vec3 v)
{
    float a = dot(v, v);
    float b = 2.0 * dot(x, v);
    float c = dot(x, x) - Rt * Rt;
    float t = (-b + sqrt(b * b - 4.0 * a * c)) / (2.0 * a);

    return x + t * v;
}

/*
 * Finds the transmittance of light arriving at x from infinitely far in direction v.
 * x includes Rg.
 */
vec3 TDirection(vec3 x, vec3 v)
{
#ifdef GENERATING_TRANSMITTANCCE_LUT
    // Find the position where x + tv intersects the top of the atmosphere
    vec3 x0 = PointOnAtmosphereTop(x, v);

    // Integrate along the view ray
    vec3 absorptionSum = vec3(0.0);
    const float stepCount = 128.0;
    vec3 stepSize = (x0 - x) / (stepCount - 1.0);
    for (float step = 0.0; step < stepCount; ++step)
    {
        // Find the position we are evaluating
        vec3 p = x + stepSize * step;

        // Determine the altitude of point p
        float h = length(p) - Rg;

        absorptionSum += absorptionAtAltitude(h) * length(stepSize);
    }

    // The transmission is e ^ -absorption
    return exp(-absorptionSum);
#endif

    // When the lut is already made, its much quicker.
    // This uses the parameterisation proposed by [Bruneton08]
    // The LUT generation code is in Shaders/Sky/PrecomputeTransmittance.shader
    float r = length(x);
    float u = dot(x, v) / r;
    return texture(_TransmittanceLUT, vec2((r - Rg) / (Rt - Rg), u * 0.5 + 0.5)).rgb;
}

/*
 * Finds the transmittance between points x and y
 * Both points include Rg
 */
vec3 TPointToPoint(vec3 x, vec3 y)
{
    return exp(-length(x - y) * RayleighS);

    // Determine the view vector from x to y
    vec3 v = normalize(y - x);

    // Use the identity T(x, y) = TLookup(x, v) / TLookup(y, v) [Bruneton08]
    return TDirection(x, v) / TDirection(y, v);
}

/*
 * Computes the in-scattered light arriving along the ray x + tv,
 * where the ray goes to the top of the atmosphere.
 */
vec3 InScatteringDirection(vec3 x, vec3 v)
{
    // Find the position where x + tv intersects the top of the atmosphere
    vec3 x0 = PointOnAtmosphereTop(x, v);

    // Integrate along the view ray
    vec3 scatteringSum = vec3(0.0);
    const float stepCount = 32.0;
    vec3 stepSize = (x0 - x) / (stepCount - 1.0);
    for (float step = 0.0; step < stepCount; ++step)
    {
        vec3 y = x + stepSize * step;
        float u = max(0.0, dot(normalize(v), _LightDirectionIntensity.xyz));
        
        // Rayleigh in-scattering 
        scatteringSum += TPointToPoint(x, y) * rayleighScattering(y.y - Rg) * rayleighPhase(u) * TDirection(y, _LightDirectionIntensity.xyz) * length(stepSize);

        // Mie in-scattering
        scatteringSum += TPointToPoint(x, y) * mieScattering(y.y - Rg) * miePhase(u) * TDirection(y, _LightDirectionIntensity.xyz) * length(stepSize);
    }

    return scatteringSum;
}

vec3 InScatteringPointToPoint(vec3 x, vec3 y)
{
    // Find the position where x + tv intersects the top of the atmosphere
    vec3 x0 = y;
    vec3 v = normalize(y - x);

    // Integrate along the view ray
    vec3 scatteringSum = vec3(0.0);
    const float stepCount = 16.0;
    vec3 stepSize = (x0 - x) / (stepCount - 1.0);
    for (float step = 0.0; step < stepCount; ++step)
    {
        vec3 y = x + stepSize * step;
        float u = max(0.0, dot(normalize(v), _LightDirectionIntensity.xyz));
        
        // Rayleigh in-scattering 
        scatteringSum += TPointToPoint(x, y) * rayleighScattering(y.y - Rg) * rayleighPhase(u) * TDirection(y, _LightDirectionIntensity.xyz) * length(stepSize);

        // Mie in-scattering
        scatteringSum += TPointToPoint(x, y) * mieScattering(y.y - Rg) * miePhase(u) * TDirection(y, _LightDirectionIntensity.xyz) * length(stepSize);
    }

    return scatteringSum;
}

#endif // ATMOSPHERE_SHADER_CODE_INCLUDED