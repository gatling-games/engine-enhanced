#include "VRManager.h"

VRManager::VRManager()
    : leftEyeTexture_(nullptr),
    rightEyeTexture_(nullptr)
{
    vr::EVRInitError error = vr::VRInitError_None;
    hmd_ = vr::VR_Init(&error, vr::VRApplication_Scene);

    renderModels_ = (vr::IVRRenderModels*)vr::VR_GetGenericInterface(vr::IVRRenderModels_Version, &error);
}

VRManager::~VRManager()
{
    shutdown();
}

void VRManager::shutdown()
{
    if(hmd_)
    {
        vr::VR_Shutdown();
        hmd_ = nullptr;
    }
}

