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

void main()
{
	//Treat the X and Z of the terrain position as the tex coords
	texcoord = _position.xz*_TerrainCoordinateOffsetScale.zw+_TerrainCoordinateOffsetScale.xy;
    float y = texture(_heightmap, _position.xz).r * _TerrainSize.y;
    // Project the vertex position to clip space
    gl_Position = _ViewProjectionMatrix * vec4(_position.x*_TerrainSize.x, y, _position.z*_TerrainSize.z, 1);
    
	ivec2 heightmapRes = textureSize(_heightmap, 0);
	vec2 heightmapTexelSize = 1.0 / heightmapRes;

    // Get the normal in world space
	vec4 h;
	h.x = texture(_heightmap, texcoord + heightmapTexelSize * vec2(0.0, -1.0)).r * _TerrainSize.y;
	h.y = texture(_heightmap, texcoord + heightmapTexelSize * vec2(-1.0, 0.0)).r * _TerrainSize.y;
	h.z = texture(_heightmap, texcoord + heightmapTexelSize * vec2(1.0, 0.0)).r * _TerrainSize.y;
	h.w = texture(_heightmap, texcoord + heightmapTexelSize * vec2(0.0, 1.0)).r * _TerrainSize.y;
	worldNormal.z = h.x - h.w;
	worldNormal.x = h.y - h.z;
	worldNormal.y = _TerrainSize.w;
	worldNormal = normalize(worldNormal);  
}

#endif // VERTEX_SHADER

#ifdef FRAGMENT_SHADER

// Texture inputs
layout (binding=0) uniform sampler2D _HeightmapTexture;
layout (binding=1) uniform sampler2D _Texture;

// Interpolated values from vertex shader
in vec3 worldNormal;
in vec2 texcoord;

// Final colour
out vec4 fragColor;

vec3 LambertLight(vec4 surface, vec3 worldNormal)
{
    return max(0.0, dot(worldNormal, _LightDirection.xyz)) * _LightColor.rgb * surface.rgb;
}

void main()
{
    // Use the main texture for the surface color
    vec4 col = texture(_Texture, texcoord*_TextureScale.xy);
	// Compute lambert direct light and flat ambient light
    vec3 directLight = LambertLight(col, worldNormal);
    vec3 ambientLight = col.rgb * _AmbientColor.rgb;
    vec3 finalColor = directLight + ambientLight;
	
	// Output the final color
    fragColor = vec4(finalColor.rgb, 1.0);
	//fragColor.xyz = LambertLight(vec4(0.5), worldNormal);
}

#endif // FRAGMENT_SHADER