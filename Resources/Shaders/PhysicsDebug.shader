
#include "UniformBuffers.inc.shader"




#ifdef VERTEX_SHADER

layout(location = 0) in vec4 _position;

void main()
{
    gl_Position = _ViewProjectionMatrix * (_LocalToWorld * _position);
}

#endif // VERTEX_SHADER




#ifdef FRAGMENT_SHADER

out vec4 fragColor;

void main()
{
    fragColor = vec4(0.0, 0.0, 1.0, 1.0);
}

#endif // FRAGMENT_SHADER