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
    ivec2 tileIndex = ivec2(gl_InstanceID % _TerrainTileCount.x, gl_InstanceID / _TerrainTileCount.x); //_TerrainTileCount

    // Compute normalized position of the terrain. This ranges from 0,1 in XYZ
    // Use the x and z and take the y from the heightmap
    vec3 normalizedPosition = _position.xyz;
    normalizedPosition.xz += tileIndex;
    normalizedPosition.xz /= _TerrainTileCount.xy;
    normalizedPosition.y = texture(_TerrainHeightmap, normalizedPosition.xz).g;

    worldPosition = vec4(normalizedPosition * _TerrainSize.xyz, 1.0);

    // Project the vertex position to clip space
    gl_Position = _ViewProjectionMatrix * worldPosition;

    // Compute the Texture coordinates from world position
    texcoord = normalizedPosition.xz * _TextureScale.xy;// *_TerrainTextureOffsetScale.zw + _TerrainTextureOffsetScale.xy;

    // Compute the offset from the normalized position to get the adjacent heightmap pixels
    ivec2 heightmapRes = textureSize(_TerrainHeightmap, 0);
    vec2 heightmapTexelSize = 1.0 / heightmapRes;

    // Determine the gradient along x and z at the vertex position
    float x1 = texture(_TerrainHeightmap, normalizedPosition.xz + heightmapTexelSize * vec2(-1.0, 0.0)).g;
    float x2 = texture(_TerrainHeightmap, normalizedPosition.xz + heightmapTexelSize * vec2(1.0, 0.0)).g;
    float z1 = texture(_TerrainHeightmap, normalizedPosition.xz + heightmapTexelSize * vec2(0.0, -1.0)).g;
    float z2 = texture(_TerrainHeightmap, normalizedPosition.xz + heightmapTexelSize * vec2(0.0, 1.0)).g;
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

    // Compute the tangent and bitangent to make a tangent to world space matrix
#ifdef NORMAL_MAP_ON
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

    // Sample and blend albedo textures for each texture layer
    float layerLerp;
    vec4 baseAlbedo = texture(_TerrainTextures[0], texcoord) * _TerrainColor[0];
    for (int i = 1; i < _TerrainSize.w; ++i)
    {
        vec4 temp = baseAlbedo;
        vec4 layerAlbedo = texture(_TerrainTextures[i], texcoord) * _TerrainColor[i];
        //slopemin
        layerLerp = clamp((1.0 - worldNormal.y) * 5.0 - sin(radians(_TerrainSlopeAltitudeData[i].x)), 0.0, 1.0);
        baseAlbedo = mix(baseAlbedo, layerAlbedo, layerLerp);
        //slopemax
        layerLerp = clamp((-1.0 + worldNormal.y) / (1 / 5.0) + sin(radians(_TerrainSlopeAltitudeData[i].y)), 0.0, 1.0);
        baseAlbedo = mix(temp, baseAlbedo, layerLerp);
        //altmin
        layerLerp = clamp(worldPosition.y * 2.0 - _TerrainSlopeAltitudeData[i].z, 0.0, 1.0);
        baseAlbedo = mix(temp, baseAlbedo, layerLerp);
        //altmax
        layerLerp = clamp(-worldPosition.y / (1/2.0) + _TerrainSlopeAltitudeData[i].w, 0.0, 1.0);
        baseAlbedo = mix(temp, baseAlbedo, layerLerp);
    }
#ifdef TEXTURE_ON
    surface.diffuseColor = baseAlbedo.rgb;
    surface.gloss = baseAlbedo.a;
#else
    surface.diffuseColor = baseColor.rgb;
    surface.gloss = 0.2;
#endif

#ifdef NORMAL_MAP_ON
    // Sample and blend the normal map textures for each texture layer
    vec3 baseNormal = unpackDXT5nm(texture(_TerrainNormalMapTextures[0], texcoord));
    for (int i = 1; i < _TerrainSize.w; ++i)
    {
        vec3 temp = baseNormal;
        vec3 layerNormal = unpackDXT5nm(texture(_TerrainNormalMapTextures[i], texcoord));
        //slopemin
        layerLerp = clamp((1.0 - worldNormal.y) * 5.0 - sin(radians(_TerrainSlopeAltitudeData[i].x)), 0.0, 1.0);
        baseNormal = mix(baseNormal, layerNormal, layerLerp);
        //slopemax
        layerLerp = clamp((-1.0 + worldNormal.y) / (1 / 5.0) + sin(radians(_TerrainSlopeAltitudeData[i].y)), 0.0, 1.0);
        baseNormal = mix(temp, baseNormal, layerLerp);

        //altmin
        layerLerp = clamp(worldPosition.y * 2.0 - _TerrainSlopeAltitudeData[i].z, 0.0, 1.0);
        baseNormal = mix(temp, baseNormal, layerLerp);
        //altmax
        layerLerp = clamp(-worldPosition.y / (1 / 2.0) + _TerrainSlopeAltitudeData[i].w, 0.0, 1.0);
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