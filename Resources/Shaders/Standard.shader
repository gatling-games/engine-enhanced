
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
out vec2 texcoord;

// Tangent to world space matrix used for normal mapping
#ifdef NORMAL_MAP_ON
out vec3 tangentToWorld[3];
#else
out vec3 worldNormal;
#endif

void main()
{
	// Project the vertex position to clip space
	gl_Position = _ViewProjectionMatrix * (_LocalToWorld * _position);

#ifdef NORMAL_MAP_ON
	// Get the normal, tangent and bitangent in world space
	vec3 worldNormal = normalize(mat3(_LocalToWorld) * _normal);
	vec3 worldTangent = normalize(mat3(_LocalToWorld) * _tangent.xyz);
	vec3 worldBitangent = cross(worldNormal, worldTangent);

	// Construct a (worldtangent, worldnormal, worldbitangent) basis
	// Used in the fragment shader to change the tangent space normal to world space.
	tangentToWorld[0] = vec3(worldTangent.x, worldBitangent.x, worldNormal.x);
	tangentToWorld[1] = vec3(worldTangent.y, worldBitangent.y, worldNormal.y);
	tangentToWorld[2] = vec3(worldTangent.z, worldBitangent.z, worldNormal.z);
#else
	// No normal mapping. Send the world space normal directly to the fragment shader.
	worldNormal = normalize(mat3(_LocalToWorld) * _normal);
#endif

	// Texcoord does not need to be modified.
	texcoord = _texcoord;
}

#endif // VERTEX_SHADER

#ifdef FRAGMENT_SHADER

// Texture inputs
layout(binding = 0) uniform sampler2D _MainTexture;
layout(binding = 1) uniform sampler2D _NormalMap;

// Interpolated values from vertex shader
in vec2 texcoord;

// Tangent to world space matrix used for normal mapping
#ifdef NORMAL_MAP_ON
in vec3 tangentToWorld[3];
#else
in vec3 worldNormal;
#endif

void main()
{
    SurfaceProperties surface;
    surface.occlusion = 1.0;

    // Sample the albedo texture for the diffuse color
#ifdef TEXTURE_ON
    vec4 diffuseGloss = texture(_MainTexture, texcoord);
    surface.diffuseColor = diffuseGloss.rgb * _Color.rgb;
    surface.gloss = diffuseGloss.a;
#else
    surface.diffuseColor = _Color.rgb;
    surface.gloss = _Color.a;
#endif

	// Sample the normal map and convert to world space
#ifdef NORMAL_MAP_ON
	vec3 tangentNormal = unpackDXT5nm(texture(_NormalMap, texcoord));
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