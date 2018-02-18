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

float tessAtPoint(vec4 normalizedPosition)
{
    vec3 worldPosition = normalizedPosition.xyz * _TerrainSize.xyz;
    vec3 toCamera = _CameraPosition.xyz - worldPosition;
    return clamp(64.0 - length(toCamera) * 0.15, 4.0, 64.0);
}

void main()
{
    if (gl_InvocationID == 0)
    {
        // Base the outer tessellation on the midpoint of each edge
        // This ensures that edges shared between triangles receive the same tessellation factor
        // on both sides of the edge, preventing gaps.
        gl_TessLevelOuter[0] = tessAtPoint((gl_in[1].gl_Position + gl_in[2].gl_Position) / 2.0);
        gl_TessLevelOuter[1] = tessAtPoint((gl_in[2].gl_Position + gl_in[0].gl_Position) / 2.0);
        gl_TessLevelOuter[2] = tessAtPoint((gl_in[0].gl_Position + gl_in[1].gl_Position) / 2.0);

        // Make the inner tessellation level the average of the outer ones.
        gl_TessLevelInner[0] = (gl_TessLevelOuter[0] + gl_TessLevelOuter[1] + gl_TessLevelOuter[2]) / 3.0;
        gl_TessLevelInner[1] = gl_TessLevelInner[0];
    }

    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}

#endif // TESS_CONTROL_SHADER

#ifdef TESS_EVALUATION_SHADER

layout(triangles, fractional_odd_spacing, ccw) in;

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
    // Compute normalized position of the terrain. This ranges from 0,1 in XYZ
    // Use the x and z and take the y from the heightmap
    vec4 normalizedPosition = gl_in[0].gl_Position * gl_TessCoord.x
        + gl_in[1].gl_Position * gl_TessCoord.y
        + gl_in[2].gl_Position * gl_TessCoord.z;;
    normalizedPosition.y = texture(_TerrainHeightmap, normalizedPosition.xz).g;

    // Scale by the terrain size to get the world position
    worldPosition = vec4(normalizedPosition.xyz * _TerrainSize.xyz, 1.0);

    // Project the vertex position to clip space
    gl_Position = _ViewProjectionMatrix * worldPosition;

    // Compute the Texture coordinates from world position
    texcoord = normalizedPosition.xz * _TextureScale.xy;

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

#endif // TESS_EVALUATION_SHADER

#ifdef FRAGMENT_SHADER

// Interpolated values from vertex shader
in vec4 worldPosition;
in vec3 worldNormal;
in vec2 texcoord;

// Tangent to world space matrix used for normal mapping
#ifdef NORMAL_MAP_ON
in vec3 tangentToWorld[3];
#endif

/*
 * Samples the albedo and normal map textures for the specified terrain layer.
 */
void sampleLayer(int index, out vec4 albedoSmoothness, out vec3 tangentNormal)
{
#ifdef TEXTURE_ON
    albedoSmoothness = texture(_TerrainTextures[index], texcoord) * _TerrainColor[index];
#else
    albedoSmoothness = _TerrainColor[index];
#endif

#ifdef NORMAL_MAP_ON
    tangentNormal = unpackDXT5nm(texture(_TerrainNormalMapTextures[index], texcoord));
#else
    tangentNormal = vec3(0.0, 0.0, 1.0);
#endif
}

/*
 * Computes the altitude blend factor for the given altitude.
 * The blend is between 0 and 1 and increases linearly across the blend area.
 */
float altitudeBlend(int layer, float altitude)
{
    return clamp((altitude - _TerrainLayerBlendData[layer].x) * _TerrainLayerBlendData[layer].y, 0.0, 1.0);
}

/*
* Computes the slope blend factor for the given slope.
* The blend is between 0 and 1 and increases linearly across the blend angle.
*/
float slopeBlend(int layer, float slope)
{
    return clamp((slope - _TerrainLayerBlendData[layer].z) * _TerrainLayerBlendData[layer].w, 0.0, 1.0);
}

void main()
{
    // Start by sampling the base layer
    vec4 albedoSmoothness;
    vec3 tangentNormal;
    sampleLayer(0, albedoSmoothness, tangentNormal);

    // Sample each additional layer and blend them on top of lower layers.
    for (int layer = 1; layer < _TerrainSize.w; ++layer)
    {
        // Determine the blend factors for slope and altitude
        // Combine the factors to get the final blend factor.
        float altitudeAlpha = altitudeBlend(layer, worldPosition.y);
        float slopeAlpha = slopeBlend(layer, worldNormal.y);
        float alpha = altitudeAlpha * slopeAlpha;

        // Sample the textures for the layer
        vec4 layerAlbedoSmoothness;
        vec3 layerTangentNormal;
        sampleLayer(layer, layerAlbedoSmoothness, layerTangentNormal);

        // Blend the layer data on top of lower layers.
        albedoSmoothness = mix(albedoSmoothness, layerAlbedoSmoothness, alpha);
        tangentNormal = mix(tangentNormal, layerTangentNormal, alpha);
    }

    // Pack the data into the surface structure.
    SurfaceProperties surface;
    surface.diffuseColor = albedoSmoothness.rgb;
    surface.gloss = albedoSmoothness.a;
    surface.occlusion = 1.0;

#ifdef NORMAL_MAP_ON
    surface.worldNormal.x = dot(tangentNormal, tangentToWorld[0]);
    surface.worldNormal.y = dot(tangentNormal, tangentToWorld[1]);
    surface.worldNormal.z = dot(tangentNormal, tangentToWorld[2]);
#else
    surface.worldNormal = worldNormal;
#endif

    // Output surface properties to the gbuffer
    writeToGBuffer(surface);
}

#endif // FRAGMENT_SHADER