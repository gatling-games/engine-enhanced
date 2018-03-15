#pragma once

#include "Framebuffer.h"
#include "Texture.h"
#include "UniformBuffer.h"

class GameObject;
class Camera;
class Transform;

struct ShadowCascade
{
    // Cascade distance
    float minDistance;
    float maxDistance;

    // The framebuffer for each cascade
    Framebuffer framebuffer;

    // Camera used to render the cascade
    Transform* cameraTransform;
    Camera* camera;
};

class ShadowMap
{
public:
    static const int RESOLUTION = 4096; // Cannot increase past 16384 on most hardware
    static const int CASCADE_COUNT = 4; // Increasing past 4 will require reworking the uniform buffer layout
    const float DEPTH_BIAS_PER_CASCADE[CASCADE_COUNT] = { 0.0004f, 0.0005f, 0.001f, 0.0015f };
    const float SHADOW_DRAW_DISTANCE = 300.0f;

public:
    ShadowMap();

    // Do not allow the shadowmap class to copied
    ShadowMap(const ShadowMap&) = delete;
    ShadowMap& operator=(const ShadowMap&) = delete;

    // Gets the framebuffer for a cascade index
    Framebuffer& cascadeFramebuffer(int cascade);
    Camera* cascadeCamera(int cascade);

    // Binds the texture and uniform buffer
    void bind();

    // Moves the shadow map camera to match the specified view camera
    void updatePosition(const Camera* viewCamera, float viewCameraAspect);

private:
    // A single array texture stores all of the shadow cascades
    ArrayTexture texture_;

    // The uniform buffer containg the shadow render settings
    UniformBuffer<ShadowUniformData> uniformBuffer_;

    // The data for each shadow cascade
    ShadowCascade cascades_[CASCADE_COUNT];

    // Computes the start distance of a shadow cascade
    float getCascadeMin(int cascade) const;

    // Computes the end distance of a shadow cascade
    float getCascadeMax(int cascade) const;
};