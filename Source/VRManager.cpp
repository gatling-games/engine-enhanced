#include "VRManager.h"

VRManager::VRManager()
    : vrEnabled_(false),
    hmd_(nullptr)
{
    if (hmdIsPresent() && vr::VR_IsRuntimeInstalled())
    {
        initVR();
        initCompositor();
        vrEnabled_ = true;
    }
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

bool VRManager::hmdIsPresent()
{
    return vr::VR_IsHmdPresent();
}

void VRManager::initVR()
{
    vr::EVRInitError error = vr::VRInitError_None;
    hmd_ = vr::VR_Init(&error, vr::VRApplication_Scene);

    renderModels_ = (vr::IVRRenderModels*)vr::VR_GetGenericInterface(vr::IVRRenderModels_Version, &error);

    std::string driver = getTrackedDeviceString(hmd_, vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_TrackingSystemName_String);
    std::string display = getTrackedDeviceString(hmd_, vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_SerialNumber_String);
    printf("Connected to %s %s.\n", driver.c_str(), display.c_str());
}


bool VRManager::initCompositor()
{
    vr::EVRInitError error = vr::VRInitError_None;

    if (!vr::VRCompositor())
    {
        printf("Compositor initialization failed");
        return false;
    }
    return true;
}

std::string VRManager::getTrackedDeviceString(vr::IVRSystem* hmd, vr::TrackedDeviceIndex_t device, vr::TrackedDeviceProperty prop, vr::TrackedPropertyError* error)
{
    uint32_t unRequiredBufferLen = hmd->GetStringTrackedDeviceProperty(device, prop, nullptr, 0, error);
    if (unRequiredBufferLen == 0)
    {
        return "";
    }

    char* buffer = new char[unRequiredBufferLen];
    unRequiredBufferLen = hmd->GetStringTrackedDeviceProperty(device, prop, buffer, unRequiredBufferLen, error);
    std::string result = buffer;
    delete[] buffer;
    return result;
}

void VRManager::renderToHmd(GLint leftEye, GLint rightEye)
{
    if (!hmd_)
    {
        return;
    }

    vr::TrackedDevicePose_t trackedDevicePose[vr::k_unMaxTrackedDeviceCount];
    vr::VRCompositor()->WaitGetPoses(trackedDevicePose, vr::k_unMaxTrackedDeviceCount, nullptr, 0);

    vr::Texture_t leftEyeTexture = { (void*)(uintptr_t)leftEye, vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
    vr::Texture_t rightEyeTexture = { (void*)(uintptr_t)rightEye, vr::TextureType_OpenGL, vr::ColorSpace_Gamma };

    vr::VRCompositor()->Submit(vr::Eye_Left, &leftEyeTexture);
    vr::VRCompositor()->Submit(vr::Eye_Right, &rightEyeTexture);

    vr::VRCompositor()->PostPresentHandoff();

}
