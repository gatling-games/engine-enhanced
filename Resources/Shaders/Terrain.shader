
#include "UniformBuffers.inc.shader"
#include "Lighting.inc.shader"

#ifdef VERTEX_SHADER

// Vertex attributes
layout (binding = 0) uniform sampler2D _heightmap;
layout (location = 0) in vec4 _position;

// Interpolated values to fragment shader
out vec4 worldPosition;
out vec3 worldNormal;
out vec2 texcoord;

void main()
{
	//Treat the X and Z of the terrain position as the tex coords
	texcoord = _position.xz * _TerrainCoordinateOffsetScale.zw + _TerrainCoordinateOffsetScale.xy;

    // Compute the world position of the terrain.
    // Use the x and z and take the y from the heightmap
    worldPosition = vec4(_position.x, texture(_heightmap, _position.xz).r, _position.z, 1.0);
    worldPosition.xyz *= _TerrainSize.xyz;

    // Project the vertex position to clip space
    gl_Position = _ViewProjectionMatrix * worldPosition;
    
	ivec2 heightmapRes = textureSize(_heightmap, 0);
	vec2 heightmapTexelSize = 1.0 / heightmapRes;

    // Get the normal in world space
	vec4 h;
	h.x = texture(_heightmap, _position.xz + heightmapTexelSize * vec2(0.0, -1.0)).r * _TerrainSize.y;
	h.y = texture(_heightmap, _position.xz + heightmapTexelSize * vec2(-1.0, 0.0)).r * _TerrainSize.y;
	h.z = texture(_heightmap, _position.xz + heightmapTexelSize * vec2(1.0, 0.0)).r * _TerrainSize.y;
	h.w = texture(_heightmap, _position.xz + heightmapTexelSize * vec2(0.0, 1.0)).r * _TerrainSize.y;
	worldNormal.z = h.w - h.x;
	worldNormal.x = h.z - h.y;
	worldNormal.y = _TerrainSize.w;
	worldNormal = normalize(worldNormal);  
}

#endif // VERTEX_SHADER

#ifdef FRAGMENT_SHADER

// Texture inputs
layout (binding=0) uniform sampler2D _HeightmapTexture;
layout (binding=1) uniform sampler2D _Texture;
layout (binding = 2) uniform sampler2D _SnowTex;
layout (binding = 3) uniform sampler2D _RockTex;

// Interpolated values from vertex shader
in vec4 worldPosition;
in vec3 worldNormal;
in vec2 texcoord;

// Final colour
out vec4 fragColor;

void main()
{
    // Sample the diffuse textures for each texture layer
    vec4 baseDiffuse = texture(_Texture, texcoord*_TextureScale.xy);
	vec4 rockDiffuse = texture(_RockTex, texcoord*_TextureScale.xy);
	vec4 snowDiffuse = texture(_SnowTex, texcoord*_TextureScale.xy);

    // Work out interpolation factors for the rock and snow layers
    // Based on altitude and slope (ak world y)
	float rockLerp = clamp((1.0 - worldNormal.y) * 10.0 - 8.0,0.0,1.0);
	float snowLerp = clamp(worldPosition.y * 2.0 - 50,0.0,1.0);

    // Blend the rock and snow textures into the base texture
    vec4 combinedDiffuse = mix(mix(baseDiffuse, rockDiffuse, rockLerp), snowDiffuse, snowLerp);

    // Gather surface properties
    SurfaceProperties surface;
    surface.diffuseColor = combinedDiffuse.rgb;
    surface.worldPosition = worldPosition;
    surface.worldNormal = worldNormal;

    // Output the final lit color
    fragColor = ComputeLighting(surface);
}

#endif // FRAGMENT_SHADER