#include "VRManager.h"
#include "SceneManager.h"
#include "Utils/Console.h"

VRManager::VRManager()
    : vrEnabled_(false),
    hmd_(nullptr)
{
    if (hmdIsPresent() && vr::VR_IsRuntimeInstalled())
    {
        initVR();
        initCompositor();
        setupFramebuffers();
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
    hmd_->GetRecommendedRenderTargetSize(&width_, &height_);
    renderModels_ = (vr::IVRRenderModels*)vr::VR_GetGenericInterface(vr::IVRRenderModels_Version, &error);

    if(error != vr::VRInitError_None)
    {
        log("Error calling VR_GetGenericInterface \n");
    }

    std::string driver = getTrackedDeviceString(hmd_, vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_TrackingSystemName_String);
    std::string display = getTrackedDeviceString(hmd_, vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_SerialNumber_String);
    log("Connected to %s %s.\n", driver.c_str(), display.c_str());
}

bool VRManager::initCompositor()
{
    if (!vr::VRCompositor())
    {
        log("Compositor initialization failed");
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

void VRManager::setupFramebuffers()
{
    for (int i = 0; i < 2; ++i)
    {
        frameBuffers_.push_back(new Framebuffer());
        // Create new framebuffer and textures with new panel dimensions
        depthBuffers_[i] = new Texture(TextureFormat::Depth, width_, height_);
        colorBuffers_[i] = new Texture(TextureFormat::RGB8_SRGB, width_, height_);

        // Attach new textures to new framebuffer
        frameBuffers_[i]->attachDepthTexture(depthBuffers_[i]);
        frameBuffers_[i]->attachColorTexture(colorBuffers_[i]);
    } 

    // Then create a renderer for the framebuffer.
    renderer_ = new Renderer(frameBuffers_);
    
}

void VRManager::frameStart()
{
    if (!hmd_)
    {
        return;
    }

    const Camera* cam = SceneManager::instance()->mainCamera();
    if (!cam)
    {
        return;
    }

    renderer_->renderFrame(cam);
    renderToHmd(colorBuffers_[0]->glid(), colorBuffers_[1]->glid());
}


void VRManager::renderToHmd(GLint leftEye, GLint rightEye)
{
    if (!hmd_)
    {
        return;
    }
    // Update HMD's position as part of the main render loop
    updateHmdPose();

    vr::Texture_t leftEyeTexture = { (void*)(uintptr_t)leftEye, vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
    vr::Texture_t rightEyeTexture = { (void*)(uintptr_t)rightEye, vr::TextureType_OpenGL, vr::ColorSpace_Gamma };

    if(vr::VRCompositor()->Submit(vr::Eye_Left, &leftEyeTexture) != vr::VRCompositorError_None)
    {
        throw;
    }

    if(vr::VRCompositor()->Submit(vr::Eye_Right, &rightEyeTexture) != vr::VRCompositorError_None)
    {
        throw;
    }  
}

void VRManager::updateHmdPose()
{
    if (!hmd_)
    {
        return;
    }

    vr::VRCompositor()->WaitGetPoses(trackedDevicePoses_, vr::k_unMaxTrackedDeviceCount, NULL, 0);

    if (trackedDevicePoses_[vr::k_unTrackedDeviceIndex_Hmd].bPoseIsValid)
    {
        vr::HmdMatrix34_t mat = trackedDevicePoses_[vr::k_unTrackedDeviceIndex_Hmd].mDeviceToAbsoluteTracking;
        Matrix4x4 matrix;

        matrix.setCol(0, mat.m[0][0], mat.m[1][0], mat.m[2][0], 0.0f);
        matrix.setCol(1, mat.m[0][1], mat.m[1][1], mat.m[2][1], 0.0f);
        matrix.setCol(2, -mat.m[0][2], -mat.m[1][2], -mat.m[2][2], 0.0f);
        matrix.setCol(3, mat.m[0][3], mat.m[1][3], mat.m[2][3], 1.0f);
        
        // openvr expects that negative z is forward
        // We use positive z, so flip the coords before applying the matrix
        hmdPose_ = matrix.invert() * Matrix4x4::scale(Vector3(1.0f, 1.0f, -1.0f));
    }
}

Matrix4x4 VRManager::getProjectionMatrix(EyeType eye, float nearPlane, float farPlane) const
{
    assert(eye != EyeType::None);
    
    vr::HmdMatrix44_t proj = hmd_->GetProjectionMatrix(eye == EyeType::LeftEye ? vr::Eye_Left : vr::Eye_Right, nearPlane, farPlane);
    Matrix4x4 matrix;
    matrix.setCol(0, proj.m[0][0], proj.m[1][0], proj.m[2][0], proj.m[3][0]);
    matrix.setCol(1, proj.m[0][1], proj.m[1][1], proj.m[2][1], proj.m[3][1]);
    matrix.setCol(2, -proj.m[0][2], -proj.m[1][2], -proj.m[2][2], -proj.m[3][2]);
    matrix.setCol(3, proj.m[0][3], proj.m[1][3], proj.m[2][3], proj.m[3][3]);
    return matrix;
}

Matrix4x4 VRManager::getEyeMatrix(EyeType eye) const
{
    assert(eye != EyeType::None);

    vr::HmdMatrix34_t em = hmd_->GetEyeToHeadTransform(eye == EyeType::LeftEye ? vr::Eye_Left : vr::Eye_Right);
    Matrix4x4 matrix = Matrix4x4::identity();
    matrix.setCol(3, -em.m[0][3], -em.m[1][3], -em.m[2][3], 1.0f);

    // Merge the toeye matrix with the camera->head matrix
    return matrix * hmdPose_;
}
