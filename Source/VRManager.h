#pragma once

#include <openvr.h>

#include "Utils/Singleton.h"
#include "Renderer/Texture.h"
#include "Math/Matrix4x4.h"
#include "Scene/Camera.h"

class VRManager : public Singleton<VRManager>
{
public:
    VRManager();
    ~VRManager();

    bool initCompositor();
    void shutdown();

    void renderToHMD();

    void setLeftEyeTexture(Texture* Texture);
    void setRightEyeTexture(Texture* Texture);

    void setupCameras();

    Matrix4x4 getHMDMatrixProjectionEye(vr::Hmd_Eye eye);
    Matrix4x4 getHMDMatrixPoseEye(vr::Hmd_Eye eye);
    std::string getTrackedDeviceString(vr::IVRSystem *hmd, vr::TrackedDeviceIndex_t device, vr::TrackedDeviceProperty prop, vr::TrackedPropertyError *error = nullptr);

private:
    Texture * leftEyeTexture_;
    Camera* leftEyeCamera_;
    Matrix4x4 leftEyeProjectionMatrix_;
    Matrix4x4 leftEyePoseMatrix_;

    Texture* rightEyeTexture_;
    Camera* rightEyeCamera_;
    Matrix4x4 rightEyeProjectionMatrix_;
    Matrix4x4 rightEyePoseMatrix_;

    uint32_t renderWidth_;
    uint32_t renderHeight_;

    vr::IVRSystem *hmd_;
    vr::IVRRenderModels *renderModels_;
};