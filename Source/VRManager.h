#pragma once

#include <openvr.h>

#include "Utils/Singleton.h"
#include "Renderer/Texture.h"
#include "Math/Matrix4x4.h"
#include "Scene/Camera.h"
#include "Renderer/Framebuffer.h"
#include "Renderer/Renderer.h"

class VRManager : public Singleton<VRManager>
{
public:
    VRManager();
    ~VRManager();

    bool hmdIsPresent();

    void initVR();
    bool initCompositor();
    void shutdown();

    bool enabled() const { return vrEnabled_; }

    std::string getTrackedDeviceString(vr::IVRSystem *hmd, vr::TrackedDeviceIndex_t device, vr::TrackedDeviceProperty prop, vr::TrackedPropertyError *error = nullptr);
    void setupFramebuffers();

    void frameStart();
    void renderToHmd(GLint leftEye, GLint rightEye);
    void updateHmdPose();
    
    Matrix4x4 getProjectionMatrix(EyeType eye, float nearPlane, float farPlane) const;
    Matrix4x4 getInverseProjectionMatrix(EyeType eye, float nearPlane, float farPlane) const;
    Matrix4x4 getEyeMatrix(EyeType eye) const;
    Matrix4x4 getInverseEyeMatrix(EyeType eye) const;

private:
    bool vrEnabled_;

    std::vector<Framebuffer*> frameBuffers_;
    Texture* depthBuffers_[2];
    Texture* colorBuffers_[2];
    Renderer* renderer_;

    vr::IVRSystem *hmd_;
    vr::IVRRenderModels *renderModels_;
    uint32_t width_;
    uint32_t height_;
};
