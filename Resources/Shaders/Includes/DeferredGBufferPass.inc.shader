#ifndef DEFERRED_GBUFFERPASS_INCLUDED
#define DEFERRED_GBUFFERPASS_INCLUDED 1

#include "Deferred.inc.shader"

// GBuffer render target outputs
layout (location = 0) out vec4 gBuffer0;
layout (location = 1) out vec4 gBuffer1;
layout (location = 2) out vec4 gBuffer2;

// Writes surface properties into the gbuffer.
void outputToGBuffer(SurfaceProperties surface)
{
    // GBuffer layout:
    // RT0 [ diffuseColor.r    diffuseColor.g    diffuseColor.b    occlusion  ]
    // RT1 [ specularColor.r   specularColor.g   specularColor.b   smoothness ]
    // RT2 [ normal.x (10 bit) normal.y (10 bit) normal.z (10 bit) unused     ]
    gBuffer0 = vec4(surface.diffuseColor, 0.0);
    gBuffer1 = vec4(0.0, 0.0, 0.0, 0.2);
    gBuffer2 = vec4(surface.worldNormal * 0.5 + 0.5, 0.0);
}

#endif // DEFERRED_GBUFFERPASS_INCLUDED