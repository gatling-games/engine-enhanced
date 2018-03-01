
#include "UniformBuffers.inc.shader"

#define USE_GBUFFER_WRITE
#include "Deferred.inc.shader"

#ifdef VERTEX_SHADER

layout(binding = 8) uniform sampler2D _TerrainHeightmap;

layout(location = 0) in vec4 _position;
layout(location = 3) in vec2 _texcoord;

out vec3 worldNormal;
out vec2 texcoord;

void main()
{
    // Rotate the local position pseudo-randomly
    // Just base the rotation on the instance id
    float rotationRad = float(gl_InstanceID) * 0.1;
    float sinRotation = sin(rotationRad);
    float cosRotation = cos(rotationRad);
    vec3 localPosition = vec3(
        _position.x * cosRotation - _position.z * sinRotation,
        _position.y,
        _position.x * sinRotation + _position.z * cosRotation
    );

    // Apply the scale and offset to the local-space position
    vec4 instanceOffsetScale = _TerrainDetailPositions[gl_InstanceID];
    vec3 offset = instanceOffsetScale.xyz;
    float scale = instanceOffsetScale.a;
    vec3 worldPosition = (localPosition * scale) + offset;

    // Project the world position to clip space
    gl_Position = _ViewProjectionMatrix * vec4(worldPosition, 1.0);

    vec3 normalizedPosition = worldPosition / _TerrainSize.xyz;

    // Compute the offset from the normalized position to get the adjacent heightmap pixels
    ivec2 heightmapRes = textureSize(_TerrainHeightmap, 0);
    vec2 heightmapTexelSize = 1.0 / heightmapRes;

    // Determine the gradient along x and z at the vertex position
    float x1 = texture(_TerrainHeightmap, normalizedPosition.xz + heightmapTexelSize * vec2(-1.0, 0.0)).r;
    float x2 = texture(_TerrainHeightmap, normalizedPosition.xz + heightmapTexelSize * vec2(1.0, 0.0)).r;
    float z1 = texture(_TerrainHeightmap, normalizedPosition.xz + heightmapTexelSize * vec2(0.0, -1.0)).r;
    float z2 = texture(_TerrainHeightmap, normalizedPosition.xz + heightmapTexelSize * vec2(0.0, 1.0)).r;
    float dydx = x2 - x1;
    float dydz = z2 - z1;
    dydx *= _TerrainSize.y;
    dydz *= _TerrainSize.y;
    dydx /= (2.0 * heightmapTexelSize.x) * _TerrainSize.x;
    dydz /= (2.0 * heightmapTexelSize.y) * _TerrainSize.z;

    // Determine the tangents along the X and Z
    vec3 worldTangent = normalize(vec3(1.0, dydx, 0.0));
    vec3 worldBitangent = normalize(vec3(0.0, dydz, 1.0));

    // Cross product to get the world normal
    worldNormal = cross(worldBitangent, worldTangent);

    // Texcoord does not need to be modified.
    texcoord = _texcoord;
}

#endif // VERTEX_SHADER

#ifdef FRAGMENT_SHADER

in vec3 worldNormal;
in vec2 texcoord;

void main()
{
    SurfaceProperties surface;
    surface.occlusion = 1.0;

    // Sample the albedo texture for the diffuse color
#ifdef TEXTURE_ON
    vec4 diffuseAlpha = texture(_AlbedoTexture, texcoord);
    surface.diffuseColor = diffuseAlpha.rgb * _Color.rgb;

#ifdef ALPHA_TEST_ON
    // Use alpha test rendering
    if (diffuseAlpha.a < 0.5)
    {
        discard;
    }
#endif

#else
    surface.diffuseColor = _Color.rgb;
#endif

    // The a channel is used for opacity, so always use a constant gloss term.
    surface.gloss = _Color.a;

    // Grass doesn't use normal mapping
    surface.worldNormal = worldNormal;

    // Output surface properties to the gbuffer
    writeToGBuffer(surface);
}

#endif // FRAGMENT_SHADER