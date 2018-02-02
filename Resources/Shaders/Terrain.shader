#include "UniformBuffers.inc.shader"

#define USE_GBUFFER_WRITE
#include "Deferred.inc.shader"

#ifdef VERTEX_SHADER

// Vertex attributes
layout(location = 0) in vec4 _position;

// Interpolated values to fragment shader
out vec4 worldPosition;
out vec3 worldNormal;
out vec2 texcoord;

// Tangent to world space matrix used for normal mapping
#ifdef NORMAL_MAP_ON
out vec3 tangentToWorld[3];
#endif

void main()
{
    //Treat the X and Z of the terrain position as the tex coords
    texcoord = _position.xz * _TerrainCoordinateOffsetScale.zw + _TerrainCoordinateOffsetScale.xy;

    // Compute the world position of the terrain.
    // Use the x and z and take the y from the heightmap
    worldPosition = vec4(_position.x, texture(_TerrainHeightmap, _position.xz).r, _position.z, 1.0);
    worldPosition.xyz *= _TerrainSize.xyz;

    // Project the vertex position to clip space
    gl_Position = _ViewProjectionMatrix * worldPosition;

    ivec2 heightmapRes = textureSize(_TerrainHeightmap, 0);
    vec2 heightmapTexelSize = 1.0 / heightmapRes;

    // Get the normal in world space
    vec4 h;
    h.x = texture(_TerrainHeightmap, _position.xz+ heightmapTexelSize * vec2(0.0, -1.0)).r * _TerrainSize.y;
    h.y = texture(_TerrainHeightmap, _position.xz+ heightmapTexelSize * vec2(-1.0, 0.0)).r * _TerrainSize.y;
    h.z = texture(_TerrainHeightmap, _position.xz+ heightmapTexelSize * vec2(1.0, 0.0)).r * _TerrainSize.y;
    h.w = texture(_TerrainHeightmap, _position.xz+ heightmapTexelSize * vec2(0.0, 1.0)).r * _TerrainSize.y;
    worldNormal.z = h.w - h.x;
    worldNormal.x = h.z - h.y;
    worldNormal.y = 2.0;
    worldNormal = normalize(worldNormal);

    // Compute the tangent and bitangent to make a tangent to world space matrix
#ifdef NORMAL_MAP_ON
    vec3 worldTangent = cross(worldNormal, vec3(1.0, 0.0, 0.0));
    vec3 worldBitangent = cross(worldNormal, worldTangent);
    tangentToWorld[0] = vec3(worldTangent.x, worldBitangent.x, worldNormal.x);
    tangentToWorld[1] = vec3(worldTangent.y, worldBitangent.y, worldNormal.y);
    tangentToWorld[2] = vec3(worldTangent.z, worldBitangent.z, worldNormal.z);
#endif
}

#endif // VERTEX_SHADER

#ifdef FRAGMENT_SHADER

// Interpolated values from vertex shader
in vec4 worldPosition;
in vec3 worldNormal;
in vec2 texcoord;

// Tangent to world space matrix used for normal mapping
#ifdef NORMAL_MAP_ON
in vec3 tangentToWorld[3];
#endif

void main()
{
    SurfaceProperties surface;
    surface.occlusion = 1.0;

#ifdef TEXTURE_ON

    // Sample and blend albedo textures for each texture layer
    float layerLerp;
    vec4 baseAlbedo = texture(_TerrainTextures[0], texcoord * _TextureScale.xy);
    for (int i = 1; i < _TerrainSize.w; ++i)
    {
        vec4 temp = baseAlbedo;
        layerLerp = clamp(worldPosition.y * 2.0 - _SlopeAltitudeData[i].z, 0.0, 1.0);
        vec4 layerAlbedo = texture(_TerrainTextures[i], texcoord * _TextureScale.xy);
        baseAlbedo = mix(baseAlbedo, layerAlbedo, layerLerp);
        layerLerp = clamp(-worldPosition.y / (1/2.0) + _SlopeAltitudeData[i].w, 0.0, 1.0);
        baseAlbedo = mix(temp, baseAlbedo, layerLerp);
    }

    surface.diffuseColor = baseAlbedo.rgb;
    surface.gloss = baseAlbedo.a;
#else
    surface.diffuseColor = vec3(1.0);
    surface.gloss = 0.2;
#endif

#ifdef NORMAL_MAP_ON
    // Sample and blend the normal map textures for each texture layer
    vec3 baseNormal = unpackDXT5nm(texture(_TerrainNormalMapTextures[0], texcoord * _TextureScale.xy));
    for (int i = 1; i < _TerrainSize.w; ++i)
    {
        vec3 temp = baseNormal;
        layerLerp = clamp(worldPosition.y * 2.0 - _SlopeAltitudeData[i].z, 0.0, 1.0);
        vec3 layerNormal = unpackDXT5nm(texture(_TerrainNormalMapTextures[i], texcoord * _TextureScale.xy));
        baseNormal = mix(baseNormal, layerNormal, layerLerp);
        layerLerp = clamp(-worldPosition.y / (1 / 2.0) + _SlopeAltitudeData[i].w, 0.0, 1.0);
        baseNormal = mix(temp, baseNormal, layerLerp);
    }

    // Convert the normal to world space
    surface.worldNormal.x = dot(baseNormal, tangentToWorld[0]);
    surface.worldNormal.y = dot(baseNormal, tangentToWorld[1]);
    surface.worldNormal.z = dot(baseNormal, tangentToWorld[2]);
#else
    surface.worldNormal = worldNormal;
#endif

    // Output surface properties to the gbuffer
    writeToGBuffer(surface);
}

#endif // FRAGMENT_SHADER