// Scene uniform buffer
layout(std140) uniform scene_data
{
    uniform vec4 _AmbientColor;
    uniform vec4 _LightColor;
    uniform vec4 _LightDirection;
};

// Camera uniform buffer
layout(std140) uniform camera_data
{
    uniform vec4 _ScreenResolution;
    uniform vec4 _CameraPosition;
    uniform mat4x4 _ViewProjectionMatrix;
    uniform mat4x4 _ClipToWorld;
};

//Terrain uniform buffer
layout(std140) uniform terrain_data
{
    //XY is offset, ZW is scale
    uniform vec4 _TerrainCoordinateOffsetScale;
    //XYZW, w is normal scale
    uniform vec4 _TerrainSize;
	//XY 
	uniform vec4 _TextureScale;
};

// Per-draw uniform buffer.
layout(std140) uniform per_draw_data
{
    uniform mat4x4 _LocalToWorld;
};

#ifdef VERTEX_SHADER

// Vertex attributes
layout (binding = 0) uniform sampler2D _heightmap;
layout (location = 0) in vec4 _position;

// Interpolated values to fragment shader
out vec3 worldNormal;
out vec2 texcoord;
out float height;

void main()
{
	//Treat the X and Z of the terrain position as the tex coords
	texcoord = _position.xz*_TerrainCoordinateOffsetScale.zw+_TerrainCoordinateOffsetScale.xy;
    height = texture(_heightmap, _position.xz).r * _TerrainSize.y;
    // Project the vertex position to clip space
    gl_Position = _ViewProjectionMatrix * vec4(_position.x*_TerrainSize.x, height, _position.z*_TerrainSize.z, 1);
    
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
in vec3 worldNormal;
in vec2 texcoord;
in float height;

// Final colour
out vec4 fragColor;

vec3 LambertLight(vec4 surface, vec3 worldNormal)
{
    return max(0.0, dot(worldNormal, _LightDirection.xyz)) * _LightColor.rgb * surface.rgb;
}

void main()
{
    // Use the main texture for the surface color
    vec4 baseDiffuse = texture(_Texture, texcoord*_TextureScale.xy);
	vec4 rockDiffuse = texture(_RockTex, texcoord*_TextureScale.xy);
	vec4 snowDiffuse = texture(_SnowTex, texcoord*_TextureScale.xy);

	float rockLerp = clamp((1.0 - worldNormal.y) * 10.0 - 8.0,0.0,1.0);
	float snowLerp = clamp(height * 2.0 - 50,0.0,1.0);


	//fragColor = vec4(rockLerp);
	vec4 col = mix(mix(baseDiffuse, rockDiffuse, rockLerp), snowDiffuse, snowLerp);
	// Compute lambert direct light and flat ambient light
    vec3 directLight = LambertLight(col, worldNormal);
    vec3 ambientLight = col.rgb * _AmbientColor.rgb;
    vec3 finalColor = directLight + ambientLight;
	
	// Output the final color
    fragColor = vec4(finalColor.rgb, 1.0);
	//fragColor.xyz = LambertLight(vec4(0.5), worldNormal);

	//fragColor.xyz = worldNormal.yyy * 0.5 + 0.5;
}

#endif // FRAGMENT_SHADER