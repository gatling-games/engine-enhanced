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

    bool hmdIsPresent();

    void initVR();
    bool initCompositor();
    void shutdown();

    bool enabled() const { return vrEnabled_; }

    std::string getTrackedDeviceString(vr::IVRSystem *hmd, vr::TrackedDeviceIndex_t device, vr::TrackedDeviceProperty prop, vr::TrackedPropertyError *error = nullptr);

private:
    bool vrEnabled_;

    vr::IVRSystem *hmd_;
    vr::IVRRenderModels *renderModels_;
};