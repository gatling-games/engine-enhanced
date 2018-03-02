
#include "UniformBuffers.inc.shader"

#define USE_GBUFFER_WRITE
#include "Deferred.inc.shader"

#ifdef VERTEX_SHADER

layout(binding = 8) uniform sampler2D _TerrainHeightmap;

layout(location = 0) in vec4 _position;
layout(location = 1) in vec3 _normal;
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

    // Apply a wind offset to the world position
    float offsetMagnitude = sin(_Time.x * 1.5 + dot(worldPosition, vec3(0.2))) * _position.y;
    vec2 offsetDir = vec2(0.45, 0.2);
    worldPosition.xz += offsetDir * offsetMagnitude;

    // Project the world position to clip space
    gl_Position = _ViewProjectionMatrix * vec4(worldPosition, 1.0);

    // Cross product to get the world normal
    worldNormal = vec3(
        _normal.x * cosRotation - _normal.z * sinRotation,
        _normal.y,
        _normal.x * sinRotation + _normal.z * cosRotation
    );

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

    // Make grass partially translucent
    surface.translucency = 0.666;

    // Grass doesn't use normal mapping
    surface.worldNormal = worldNormal;

    // Output surface properties to the gbuffer
    writeToGBuffer(surface);
}

#endif // FRAGMENT_SHADER