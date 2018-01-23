#ifndef DEFERRED_LIBRARY_INCLUDED
#define DEFERRED_LIBRARY_INCLUDED

#include "DeferredGBuffer.inc.shader"


// The default vertex shader for full screen shaders
// To use, just define #USE_SCREENSPACE_VERT_SHADER
#if defined(VERTEX_SHADER) && defined(USE_SCREENSPACE_VERT_SHADER)

    layout(location = 0) in vec4 _position;

    void main()
    {
        gl_Position = _position;
    }

#endif

#endif // DEFERRED_LIBRARY_INCLUDED