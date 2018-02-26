
#include "Common.inc.shader"
#include "UniformBuffers.inc.shader"

#ifdef VERTEX_SHADER

layout(location = 0) in vec4 _position;

out vec3 viewDirUnnormalized;

void main()
{
    gl_Position = _ViewProjectionMatrix * (_LocalToWorld * _position);
    viewDirUnnormalized = _position.xyz;
    viewDirUnnormalized.y = max(0.0, viewDirUnnormalized.y);
}

#endif // VERTEX_SHADER

#ifdef FRAGMENT_SHADER

// Based on "Precomputed Atmospheric Scattering" [Bruneton 2008]

#include "Atmosphere.inc.shader"

in vec3 viewDirUnnormalized;
out vec4 fragColor;

bool isViewDirInSun(vec3 viewDir)
{
    return dot(viewDir, _LightDirectionIntensity.xyz) > 0.997;
}

vec3 inScattering(vec3 y, vec3 v)
{
    vec3 sum = vec3(0.0);

    const int stepCount = 64;
    for (int i = 0; i < stepCount; ++i)
    {
        vec3 dir = _SphereIntegralPoints[i].xyz;
        vec3 L = _LightDirectionIntensity.w * TDirection(y, _LightDirectionIntensity.xyz);
        sum += rayleighScattering(y.y - Rg) * max(rayleighPhase(dot(dir, v)), 0.0) * L;
        sum += mieScattering(y.y - Rg) * max(miePhase(dot(dir, v)), 0.0) * L;
    }

    return sum / float(stepCount) * 4 * M_PI;
}

vec3 S(vec3 x, vec3 v)
{
    // Find the position where x + tv intersects the top of the atmosphere
    vec3 x0 = PointOnAtmosphereTop(x, v);

    // Integrate along the view ray
    vec3 scatteringSum = vec3(0.0);
    const float stepCount = 16.0;
    vec3 stepSize = (x0 - x) / (stepCount - 1.0);
    for (float step = 0.0; step < stepCount; ++step)
    {
        vec3 y = x + stepSize * step;
        
        // Rayleigh in-scattering 
        scatteringSum += TPointToPoint(x, y) * rayleighScattering(y.y - Rg) * rayleighPhase(dot(normalize(v), _LightDirectionIntensity.xyz)) * TDirection(y, _LightDirectionIntensity.xyz) * _LightDirectionIntensity.w * length(stepSize);

        // Mie in-scattering
        scatteringSum += TPointToPoint(x, y) * mieScattering(y.y - Rg) * miePhase(dot(normalize(v), _LightDirectionIntensity.xyz)) * TDirection(y, _LightDirectionIntensity.xyz) * _LightDirectionIntensity.w * length(stepSize);
    }

    return scatteringSum;
}

void main()
{
    vec3 viewPos = _CameraPosition.xyz;
    viewPos.y += Rg;
    vec3 viewDir = normalize(viewDirUnnormalized);

    // Determine the direct light from the sun
    vec3 L0 = clamp(pow(dot(viewDir, _LightDirectionIntensity.xyz), 1024.0), 0.0, 1.0) * _LightDirectionIntensity.w * TDirection(viewPos, _LightDirectionIntensity.xyz);
    L0 += S(viewPos, viewDir);

    // Add the components together
    fragColor = vec4(L0, 1.0);
}

#endif // FRAGMENT_SHADER