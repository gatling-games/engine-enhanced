#version 330

// Scene uniform buffer
layout(std140) uniform scene_data
{
    uniform vec3 _AmbientColor;
    uniform vec3 _LightColor;
    uniform vec3 _LightDirection;
};

// Camera uniform buffer
layout(std140) uniform camera_data
{
    uniform vec2 _ScreenResolution;
    uniform vec3 _CameraPosition;
    uniform mat4x4 _ViewProjectionMatrix;
    uniform mat4x4 _ClipToWorld;
};

// Per-draw uniform buffer.
layout(std140) uniform per_draw_data
{
    uniform mat4x4 _LocalToWorld;
};

#ifdef VERTEX_SHADER


// Vertex attributes
layout(location = 0) in vec4 _position;
layout(location = 1) in vec3 _normal;
layout(location = 2) in vec4 _tangent;
layout(location = 3) in vec2 _texcoord;

// Interpolated values to fragment shader
out vec3 worldNormal;
out vec2 texcoord;

void main()
{
    // Project the vertex position to clip space
    gl_Position = _ViewProjectionMatrix * (_LocalToWorld * _position);
    
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
    return max(0.0, dot(worldNormal, _LightDirection)) * _LightColor * surface.rgb;
}

void main()
{
    // Use the main texture for the surface color
    vec4 col = texture(_MainTexture, texcoord);
    
	// Compute lambert direct light and flat ambient light
    vec3 directLight = LambertLight(col, worldNormal);
    vec3 ambientLight = col.rgb * _AmbientColor;
    vec3 finalColor = directLight + ambientLight;
	
	// Output the final color
    fragColor = vec4(finalColor.rgb, 1.0);
}

#endif // FRAGMENT_SHADER