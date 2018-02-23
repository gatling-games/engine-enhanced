#include "UniformBuffers.inc.shader"

#define USE_GBUFFER_WRITE
#include "Deferred.inc.shader"

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
out vec4 worldPosition;
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
        1.0,
        0.7
    };

    // A vector perpendicular to the direction of travel of each wave
    vec2 waveDirections[waveCount] = {
        normalize(vec2(1.0, 1.0)),
        normalize(vec2(-1.0, 0.0))
    };

    // The wavelength of each wave
    float waveWavelengths[waveCount] = {
        40.0,
        30.0
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
        4.0 * (2.0 * M_PI) / waveWavelengths[0],
        3.0 * (2.0 * M_PI) / waveWavelengths[1]
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
    worldPosition = vec4(normalizedPosition.xyz * _TerrainSize.xyz, 1.0);

    // Find the water depth by comparing the terrain heightmap to the water height
    float terainHeight = texture(_TerrainHeightmap, normalizedPosition.xz).g * _TerrainSize.y - _WaterDepth;
    heightAboveTerrain = worldPosition.y - terainHeight;

    // Offset each vertex up and down over time to simulate waves
    // Determine the normal by taking the derivative of the waves and combining
    worldPosition.y += getWaveDisplacement(worldPosition.xz);

    // Project the vertex position to clip space
    gl_Position = _ViewProjectionMatrix * worldPosition;

    // Compute the Texture coordinates from world position
    texcoord = normalizedPosition.xz * _TextureScale.xy;

    // Cross product to get the world normal
    worldNormal = vec3(0.0, 1.0, 0.0);
    vec3 worldTangent = vec3(1.0, 0.0, 0.0);
    vec3 worldBitangent = vec3(0.0, 0.0, 1.0);

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
in vec4 worldPosition;
in vec3 worldNormal;
in vec2 texcoord;

// Tangent to world space matrix used for normal mapping
#ifdef NORMAL_MAP_ON
in vec3 tangentToWorld[3];
#endif

out vec4 fragColor;

void main()
{
    // Start by sampling the base layer
    vec4 albedoSmoothness = vec4(0.5, 0.5, 1.0, 0.0);
    vec3 tangentNormal = vec3(0.0, 0.0, 1.0);

    // Pack the data into the surface structure.
    SurfaceProperties surface;
    surface.diffuseColor = _WaterColor;
    surface.gloss = mix(0.1, 0.8, heightAboveTerrain / _WaterDepth);
    surface.occlusion = 1.0;
    surface.worldNormal = vec3(0.0, 1.0, 0.0);

    // Output surface properties to the gbuffer
    writeToGBuffer(surface);
}

#endif // FRAGMENT_SHADER