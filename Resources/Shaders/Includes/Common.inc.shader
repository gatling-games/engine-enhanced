#ifndef COMMON_SHADER_CODE_INCLUDED
#define COMMON_SHADER_CODE_INCLUDED 1

#define M_PI 3.14159265359

// The properties that describe a surface and the way it is lit.
struct SurfaceProperties
{
    vec3 diffuseColor;
    float occlusion;
    float gloss;
    vec3 worldNormal;
};

/*
 * Normal map textures are stored in the DXT5nm format.
 * This function unpacks the texture into xyz tangent space (z is up).
 */
vec3 unpackDXT5nm(vec4 compressed)
{
	vec3 n;
	n.xy = compressed.ag * 2.0 - 1.0;
	n.z = sqrt(1.0 - n.x*n.x - n.y*n.y);
	return n;
}

#endif // COMMON_SHADER_CODE_INCLUDED