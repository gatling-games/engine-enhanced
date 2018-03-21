#ifndef DEFERRED_LIBRARY_INCLUDED
#define DEFERRED_LIBRARY_INCLUDED

#include "DeferredGBuffer.inc.shader"


// The default vertex shader for full screen shaders
// To use, just define #USE_SCREENSPACE_VERT_SHADER
#if defined(VERTEX_SHADER) && defined(USE_SCREENSPACE_VERT_SHADER)

    layout(location = 0) in vec4 _position;

    void main()
    {
        // Always ensure the full screen quad is at max depth
        gl_Position = vec4(_position.xy, 1.0, 1.0);
    }

#endif

#endif // DEFERRED_LIBRARY_INCLUDED