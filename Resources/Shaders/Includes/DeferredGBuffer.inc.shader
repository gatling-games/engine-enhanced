#ifndef DEFERRED_GBUFFER_INCLUDED
#define DEFERRED_GBUFFER_INCLUDED 1

#include "UniformBuffers.inc.shader"
#include "Common.inc.shader"

// GBuffer Layout
// RT0: Albedo (RGB), Gloss (A)
// RT1: Normals (XYZ, 10 bits), Translucency (a, 2 bits)

// Stores surface properties into the gbuffer format
void packGBuffer(SurfaceProperties surface, out vec4 gbuffer0, out vec4 gbuffer1)
{
    gbuffer0 = vec4(surface.diffuseColor, surface.gloss);
    gbuffer1 = vec4(surface.worldNormal * 0.5 + 0.5, surface.translucency);
}

// Retrieves surface properties from the gbuffer format
SurfaceProperties unpackGBuffer(vec4 gbuffer0, vec4 gbuffer1)
{
    SurfaceProperties surface;
    surface.diffuseColor = gbuffer0.rgb;
    surface.occlusion = 1.0;
    surface.gloss = gbuffer0.a;
    surface.translucency = gbuffer1.a;
    surface.worldNormal = normalize(gbuffer1.xyz * 2.0 - 1.0);
    return surface;
}


// Helper code for writing to the GBuffer
// To use from a fragment shader, define USE_GBUFFER_WRITE
#if defined(FRAGMENT_SHADER) && defined(USE_GBUFFER_WRITE)

    layout(location = 0) out vec4 gbuffer0;
    layout(location = 1) out vec4 gbuffer1;

    // Packs and writes the specified surface properties to the gbuffer.
    void writeToGBuffer(SurfaceProperties surface)
    {
        packGBuffer(surface, gbuffer0, gbuffer1);
    }

#endif


// Helper code for reading from the GBuffer
// To use from a fragment shader, define USE_GBUFFER_READ
#if defined(FRAGMENT_SHADER) && defined(USE_GBUFFER_READ)

    layout(binding = 15) uniform sampler2D _CameraDepthTexture;
    layout(binding = 14) uniform sampler2D _GBuffer0;
    layout(binding = 13) uniform sampler2D _GBuffer1;

    // Samples the gbuffer depth texture and reconstructs the world position.
    vec3 readGBufferWorldPos()
    {
        vec2 texcoord = vec2(gl_FragCoord.xy) * _ScreenResolution.zw;

        // Sample the camera depth texture
        float depth = texture(_CameraDepthTexture, texcoord).r;

        // Construct the clip position of the pixel fragment
        vec4 clipPos = vec4(texcoord, depth, 1.0);

        // Convert the clip pos to world space
        vec4 worldPos = _ClipToWorld * clipPos;
        return worldPos.xyz / worldPos.w;
    }

    // Reads and unpacks the gbuffer for the current pixel.
    // Only avaliable in a fragment shader with USE_GBUFFER_READ defined.
    SurfaceProperties readGBuffer()
    {
        // Use the built in gl_FragCoord variable to sample the textures
        ivec2 coord = ivec2(gl_FragCoord.xy);
        vec4 gbuffer0 = texelFetch(_GBuffer0, coord, 0);
        vec4 gbuffer1 = texelFetch(_GBuffer1, coord, 0);

        // Unpack surface properties from the gbuffer
        return unpackGBuffer(gbuffer0, gbuffer1);
    }

#endif


#endif // DEFERRED_GBUFFER_INCLUDED