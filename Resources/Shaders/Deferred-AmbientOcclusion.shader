
#include "Common.inc.shader"
#include "UniformBuffers.inc.shader"

#define USE_SCREENSPACE_VERT_SHADER
#define USE_GBUFFER_READ
#include "Deferred.inc.shader"

#ifdef FRAGMENT_SHADER

layout(binding = 8) uniform sampler2D _TerrainHeightmap;

out vec4 fragColor;

/*
* Computes a 2d rotation matrix that is different per pixel
* Uses "interleaved gradient noise" from
* "Next Generation PostProcessing in Call of Duty: Advanced Warfare", [Jimemez14]
*/
mat2 randomRotationIGL()
{
    // From [Jimenez14], slide 123
    vec3 magic = vec3(0.06711056, 0.00583715, 52.9829189);
    float rotation = fract(magic.z * fract(dot(gl_FragCoord.xy, magic.xy)));
    float sinRotation = sin(rotation);
    float cosRotation = cos(rotation);

    return mat2(sinRotation, cosRotation, -cosRotation, sinRotation);
}

/*
 * Returns the terrain world space y at the specified world pos.
 */
float sampleHeightmap(vec3 worldPos)
{
    return texture(_TerrainHeightmap, worldPos.xz / _TerrainSize.xz).r * _TerrainSize.y - _WaterColorDepth.a;
}

void main()
{
    // Sample the gBuffer textures
    SurfaceProperties surface = readGBuffer();

    // Retrive the world space position by sampling the depth buffer
    vec3 worldPosition = readGBufferWorldPos();

    // This is inspired by the ambient occlusion system explained in
    // "Rendering the World of Far Cry 4" [McAuley2015] (From GDC15)
    // The approach is a ssao-style sampling of a heightfield
    // we already have a terrain heightfield, with (almost) the entire
    // landscape in it, so just sample that.
    // The heightfield should really be blurred, but this seems to work ok.

    // We use poisson disk sampling of the heightmap, like [McAuley2015]
    // Compute the rotation matrix for the disks for this pixel
    // We use interleaved gradient noise [Jimenez15] to rotate the precomputed disks
    mat2 diskRotation = randomRotationIGL(); // different per gl_fragCoord

    const int tapCount = 16;
    float sum = 0.0;
    for (int tap = 0; tap < tapCount; ++tap)
    {
        // Offset the world position to get the height sampling pos
        vec2 offset = (diskRotation * _AmbientOcclusionPoissonDisks[tap]) * _AmbientOcclusionDistance;
        vec3 samplingPosition = worldPosition + vec3(offset.x, 0.0, offset.y);

        // Sample the heightmap at that point
        float heightmapHeight = sampleHeightmap(samplingPosition);

        // If the point is lower than the heightmap, it isnt visible
        const float bias = 0.0005;
        if (heightmapHeight < worldPosition.y + bias)
        {
            sum += 1.0;
        }
    }

    // The ao is based on the % of unoccluded samples
    // Also provide a power ramp for artistic control
    float ambientOcclusion = pow(min(1.0, sum / float(tapCount) * 1.2), _AmbientOcclusionFalloff);

    // Output the ao into the gbuffer
    fragColor = vec4(ambientOcclusion);
}

#endif // FRAGMENT_SHADER