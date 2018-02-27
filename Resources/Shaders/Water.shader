
#include "Common.inc.shader"
#include "UniformBuffers.inc.shader"
#include "PhysicallyBasedShading.inc.shader"

#ifdef VERTEX_SHADER

// Vertex attributes
layout(location = 0) in vec4 _position;

void main()
{
    gl_Position = _position;
}

#endif // VERTEX_SHADER

#ifdef TESS_CONTROL_SHADER

layout(vertices = 3) out;

void main()
{
    if (gl_InvocationID == 0)
    {
        // Use the same amount of tessellation across the entire water surface to prevent
        // popping / waving artifacts

#ifdef HIGH_TESSELLATION
        float tessFactor = 64.0;
#else
        float tessFactor = 8.0;
#endif

        gl_TessLevelOuter[0] = tessFactor;
        gl_TessLevelOuter[1] = tessFactor;
        gl_TessLevelOuter[2] = tessFactor;
        gl_TessLevelInner[0] = tessFactor;
        gl_TessLevelInner[1] = tessFactor;
    }

    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}

#endif // TESS_CONTROL_SHADER

#ifdef TESS_EVALUATION_SHADER

layout(triangles, fractional_odd_spacing, ccw) in;

// Interpolated values to fragment shader
out float heightAboveTerrain;
out vec3 worldPosition;
out vec3 worldNormal;
out vec2 texcoord;

// Tangent to world space matrix used for normal mapping
#ifdef NORMAL_MAP_ON
out vec3 tangentToWorld[3];
#endif

/*
 * Computes the total vertical displacement of the water at the specified point
 * Based on "Effective Water Simulation from Physical Models" by Mark Finch (GPU Gems)
 */
float getWaveDisplacement(vec2 position)
{
    const int waveCount = 2;

    // The maximum amplitude of each wave
    float waveAmplitudes[waveCount] = {
        0.4,
        0.3
    };

    // A vector perpendicular to the direction of travel of each wave
    vec2 waveDirections[waveCount] = {
        normalize(vec2(1.0, 1.0)),
        normalize(vec2(-1.0, 0.0))
    };

    // The wavelength of each wave
    float waveWavelengths[waveCount] = {
        140.0,
        100.0
    };

    // The frequency of each wave
    // frequency * wavelength = 2 * PI
    float waveFrequencies[waveCount] = {
        2.0 * M_PI / waveWavelengths[0],
        2.0 * M_PI / waveWavelengths[1]
    };

    // The phase function of each wave
    // = wave speed * 2*PI / wavelength
    float wavePhaseFunctions[waveCount] = {
        15.0 * (2.0 * M_PI) / waveWavelengths[0],
        25.0 * (2.0 * M_PI) / waveWavelengths[1]
    };

    float displacement = 0.0;
    for (int i = 0; i < waveCount; i++)
    {
        displacement += waveAmplitudes[i] * sin(dot(waveDirections[i], position) * waveFrequencies[i] + _Time.x * wavePhaseFunctions[i]);
    }

    return displacement;
}

void main()
{
    // Compute normalized position of the terrain. This ranges from 0,1 in XYZ
    // Use the x and z and take the y from the heightmap
    vec4 normalizedPosition = gl_in[0].gl_Position * gl_TessCoord.x
        + gl_in[1].gl_Position * gl_TessCoord.y
        + gl_in[2].gl_Position * gl_TessCoord.z;;

    // Make the water extend further than the terrain
    normalizedPosition.xz = normalizedPosition.xz * 8.0 - 4.0;

    // Scale by the terrain size to get the world position
    worldPosition = normalizedPosition.xyz * _TerrainSize.xyz;

    // Find the water depth by comparing the terrain heightmap to the water height
    float terainHeight = texture(_TerrainHeightmap, normalizedPosition.xz).g * _TerrainSize.y - _WaterDepth;
    heightAboveTerrain = worldPosition.y - terainHeight;

    // Offset each vertex up and down over time to simulate waves
    // Determine the normal by taking the derivative of the waves and combining
    worldPosition.y += getWaveDisplacement(worldPosition.xz) * heightAboveTerrain;

    // Project the vertex position to clip space
    gl_Position = _ViewProjectionMatrix * vec4(worldPosition, 1.0);

    // Compute the Texture coordinates from world position
    texcoord = normalizedPosition.xz * _TextureScale.xy * 0.05 + _Time.x * 0.01;

    // Determine the gradient along x and z at the vertex position
    float x1 = getWaveDisplacement(worldPosition.xz + vec2(-0.05, 0.0));
    float x2 = getWaveDisplacement(worldPosition.xz + vec2(0.05, 0.0));
    float z1 = getWaveDisplacement(worldPosition.xz + vec2(0.0, -0.05));
    float z2 = getWaveDisplacement(worldPosition.xz + vec2(0.0, 0.05));
    float dydx = (x2 - x1) / 0.1;
    float dydz = (z2 - z1) / 0.1;

    // Determine the tangents along the X and Z
    vec3 worldTangent = normalize(vec3(1.0, dydx, 0.0));
    vec3 worldBitangent = normalize(vec3(0.0, dydz, 1.0));

    // Cross product to get the world normal
    worldNormal = cross(worldBitangent, worldTangent);

    // Compute the tangent and bitangent to make a tangent to world space matrix
#ifdef NORMAL_MAP_ON
    tangentToWorld[0] = vec3(worldTangent.x, worldBitangent.x, worldNormal.x);
    tangentToWorld[1] = vec3(worldTangent.y, worldBitangent.y, worldNormal.y);
    tangentToWorld[2] = vec3(worldTangent.z, worldBitangent.z, worldNormal.z);
#endif
}

#endif // TESS_EVALUATION_SHADER

#ifdef FRAGMENT_SHADER

// Interpolated values from vertex shader
in float heightAboveTerrain;
in vec3 worldPosition;
in vec3 worldNormal;
in vec2 texcoord;

// Tangent to world space matrix used for normal mapping
#ifdef NORMAL_MAP_ON
in vec3 tangentToWorld[3];
#endif

out vec4 fragColor;

void main()
{
    // Pack the data into the surface structure.
    SurfaceProperties surface;
    surface.diffuseColor = _WaterColor;
    surface.gloss = mix(0.4, 0.75, heightAboveTerrain / _WaterDepth);
    surface.occlusion = 1.0;

    // Perform normal mapping
#ifdef NORMAL_MAP_ON
    vec3 tangentNormal = unpackDXT5nm(texture(_TerrainNormalMapTextures[1], texcoord));
    surface.worldNormal.x = dot(tangentNormal, tangentToWorld[0]);
    surface.worldNormal.y = dot(tangentNormal, tangentToWorld[1]);
    surface.worldNormal.z = dot(tangentNormal, tangentToWorld[2]);
#else
    surface.worldNormal = worldNormal;
#endif

    // Compute the view vector and half vector from the world position
    vec3 viewDirUnnormalized = _CameraPosition.xyz - worldPosition;
    float viewDistance = length(viewDirUnnormalized);
    vec3 viewDir = viewDirUnnormalized / viewDistance;

    // The water shader is rendered with alpha blending and may have a large amount of overdraw
    // Use a simplified lighting model.
    vec3 light = (dot(_LightDirection.xyz, surface.worldNormal) * 0.5 + 0.5) * _LightColor.rgb * surface.diffuseColor;
#ifdef SPECULAR_ON
    vec3 halfVector = normalize(_LightDirection.xyz + viewDir);
    float ndoth = max(dot(halfVector, surface.worldNormal), 0.0);
    float ldoth = dot(_LightDirection.xyz, halfVector); // Angle always < 90, so no clamp needed
    float fs = normalizedBlinnPhong(500.0, ndoth) * schlickFresnel(NON_METALLIC_SPECULAR, ldoth);
    light += fs * _LightColor.rgb;
#endif

    fragColor = vec4(light, 1.0 - exp(-0.5 * heightAboveTerrain));
}

#endif // FRAGMENT_SHADER