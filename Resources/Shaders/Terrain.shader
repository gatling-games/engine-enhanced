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
    //XY
    uniform vec4 _TerrainSize;
};

// Per-draw uniform buffer.
layout(std140) uniform per_draw_data
{
    uniform mat4x4 _LocalToWorld;
};

#ifdef VERTEX_SHADER


// Vertex attributes
uniform sampler2D _heightmap;
layout(location = 0) in vec4 _position;
layout(location = 1) in vec3 _normal;
layout(location = 2) in vec4 _tangent;
layout(location = 3) in vec2 _texcoord;

// Interpolated values to fragment shader
out vec3 worldNormal;
out vec2 texcoord;

void main()
{
    vec3 offset = vec3(1.0, 1.0, 0.0);
    float y = texture(_heightmap, _texcoord).r *20;
    // Project the vertex position to clip space
    gl_Position = _ViewProjectionMatrix * vec4(_position.x, y, _position.z, 1);
    
    // Get the normal in world space
    worldNormal = normalize(mat3(_LocalToWorld) * _normal);
	
    // Texcoord does not need to be modified.
    texcoord = _texcoord;
}

#endif // VERTEX_SHADER

#ifdef FRAGMENT_SHADER

// Texture inputs
uniform sampler2D _MainTexture;

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
    vec4 col = texture(_MainTexture, texcoord);
    //fragColor = sin(col.r*30) > 0.8 ? vec4(1.0) : vec4(0.0);
    //return;

	// Compute lambert direct light and flat ambient light
    vec3 directLight = LambertLight(col, worldNormal);
    vec3 ambientLight = col.rgb * _AmbientColor.rgb;
    vec3 finalColor = directLight + ambientLight;
	
	// Output the final color
    fragColor = vec4(finalColor.rgb, 1.0);
    fragColor.xyz = worldNormal * 0.5 + 0.5;
}

#endif // FRAGMENT_SHADER