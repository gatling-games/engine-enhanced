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
    void setupCameras();
    void setupFramebuffers();

    void frameStart();
    void renderToHmd(GLint leftEye, GLint rightEye);
    void updateHmdPose();

    const Matrix4x4 getCurrentViewProjectionMatrix(vr::Hmd_Eye eye);

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

    Matrix4x4 eyePosLeft_;
    Matrix4x4 eyeProjectionLeft_;

    Matrix4x4 eyePosRight_;
    Matrix4x4 eyeProjectionRight_;

    const Matrix4x4 getHmdMatrixProjectionEye(vr::Hmd_Eye eye);
    const Matrix4x4 getHmdMatrixPoseEye(vr::Hmd_Eye eye);
    const Matrix4x4 getHmdMatrixPoseEyeInverse(vr::Hmd_Eye eye);
};
