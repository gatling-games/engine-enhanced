#ifndef COMMON_SHADER_CODE_INCLUDED
#define COMMON_SHADER_CODE_INCLUDED 1

// The properties that describe a surface and the way it is lit.
struct SurfaceProperties
{
    vec3 diffuseColor;
    vec3 worldNormal;
};

#endif // COMMON_SHADER_CODE_INCLUDED