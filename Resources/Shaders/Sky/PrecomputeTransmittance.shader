
#ifdef VERTEX_SHADER

layout(location = 0) in vec4 _position;
out vec2 texcoord;

void main()
{
    gl_Position = _position;

    // _position ranges from -1 to 1
    texcoord = _position.xy * 0.5 + 0.5;
}

#endif // VERTEX_SHADER

#ifdef FRAGMENT_SHADER

#define GENERATING_TRANSMITTANCCE_LUT
#include "Atmosphere.inc.shader"

in vec2 texcoord;
out vec4 fragColor;

void main()
{
    // Use the LUT parameterisation proposed by [Bruneton08]
    // r = length(x), where x is the position of the start of the ray
    // u = v.x / r, where v is the normalized view vector
    // note that v is normalized, so u = cos(angle between x and v)

    // r is in the range Rg to Rt
    float r = texcoord.x * (Rt - Rg) + Rg;

    // u is in the range -1 to 1
    float u = texcoord.y * 2.0 - 1.0;

    // Construct a combination of x and v that are valid for the current 
    // values of r and u
    vec3 x = vec3(0.0, r, 0.0);
    vec3 v = vec3(sqrt(1.0 - u*u), u, 0.0);

    // Find the actual transmittance from x in direction v and output it
    vec3 T = TDirection(x, v);
    fragColor = vec4(T, 1.0);
}

#endif // FRAGMENT_SHADER