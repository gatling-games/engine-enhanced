
#include "UniformBuffers.inc.shader"

#define USE_GBUFFER_WRITE
#include "Deferred.inc.shader"

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

void main()
{
    // Output surface properties to the gbuffer
    SurfaceProperties surface;
    surface.diffuseColor = texture(_MainTexture, texcoord).rgb;
    surface.worldNormal = worldNormal;
    writeToGBuffer(surface);
}

#endif // FRAGMENT_SHADER