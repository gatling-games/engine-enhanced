#include "VRManager.h"
#include "SceneManager.h"

VRManager::VRManager()
    : vrEnabled_(false),
    hmd_(nullptr)
{
    if (hmdIsPresent() && vr::VR_IsRuntimeInstalled())
    {
        initVR();
        initCompositor();
        setupCameras();
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

void VRManager::setupCameras()
{
    eyeProjectionLeft_ = getHmdMatrixProjectionEye(vr::Eye_Left);
    eyeProjectionRight_ = getHmdMatrixProjectionEye(vr::Eye_Right);
    eyePosLeft_ = getHmdMatrixPoseEye(vr::Eye_Left);
    eyePosRight_ = getHmdMatrixPoseEye(vr::Eye_Right);
}


void VRManager::setupFramebuffers()
{
    for (int i = 0; i < 2; ++i)
    {
        frameBuffers_->push_back(new Framebuffer());
        // Create new framebuffer and textures with new panel dimensions
        depthBuffers_[i] = new Texture(TextureFormat::Depth, width_, height_);
        colorBuffers_[i] = new Texture(TextureFormat::RGB8_SRGB, width_, height_);

        // Attach new textures to new framebuffer
        frameBuffers_->at(i)->attachDepthTexture(depthBuffers_[i]);
        frameBuffers_->at(i)->attachColorTexture(colorBuffers_[i]);
    } 

    // Then create a renderer for the framebuffer.
    renderer_ = new Renderer(*frameBuffers_);
    
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

    vr::TrackedDevicePose_t trackedDevicePose[vr::k_unMaxTrackedDeviceCount];
    vr::VRCompositor()->WaitGetPoses(trackedDevicePose, vr::k_unMaxTrackedDeviceCount, nullptr, 0);

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

    vr::VRCompositor()->PostPresentHandoff();
    //updateHmdPose();
}

void VRManager::updateHmdPose()
{
    if (!hmd_)
    {
        return;
    }

    //vr::VRCompositor()->WaitGetPoses()
}


const Matrix4x4 VRManager::getCurrentViewProjectionMatrix(vr::Hmd_Eye eye)
{
    Matrix4x4 mat;
    if (eye == vr::Eye_Left)
    {
        mat = eyeProjectionLeft_ * eyePosLeft_;
    }
    else if (eye == vr::Eye_Right)
    {
        mat = eyeProjectionRight_ * eyePosRight_;
    }
    return mat;
}


const Matrix4x4 VRManager::getHmdMatrixProjectionEye(vr::Hmd_Eye eye)
{
    if (!hmd_)
    {
        return Matrix4x4();
    }

    vr::HmdMatrix44_t mat = hmd_->GetProjectionMatrix(eye, 0.01f, 10000.0f);

    Matrix4x4 matrix;
    matrix.setCol(0, mat.m[0][0], mat.m[1][0], mat.m[2][0], mat.m[3][0]);
    matrix.setCol(1, mat.m[0][1], mat.m[1][1], mat.m[2][1], mat.m[3][1]);
    matrix.setCol(2, mat.m[0][2], mat.m[1][2], mat.m[2][2], mat.m[3][2]);
    matrix.setCol(3, mat.m[0][3], mat.m[1][3], mat.m[2][3], mat.m[3][3]);
    return matrix;
}

const Matrix4x4 VRManager::getHmdMatrixPoseEye(vr::Hmd_Eye eye)
{
    if (!hmd_)
    {
        return Matrix4x4();
    }
    vr::HmdMatrix34_t mat = hmd_->GetEyeToHeadTransform(eye);
    Matrix4x4 matrix;

    // This is only a transformation matrix, so we can invert values here.
    matrix.setRow(0, mat.m[0][0], mat.m[1][0], mat.m[2][0], 0.0f);
    matrix.setRow(1, mat.m[0][1], mat.m[1][1], mat.m[2][1], 0.0f);
    matrix.setRow(2, mat.m[0][2], mat.m[1][2], mat.m[2][2], 0.0f);
    matrix.setRow(3, -mat.m[0][3], -mat.m[1][3], -mat.m[2][3], 1.0f);

    return matrix;
}

const Matrix4x4 VRManager::getHmdMatrixPoseEyeInverse(vr::Hmd_Eye eye)
{
    if (!hmd_)
    {
        return Matrix4x4();
    }
    vr::HmdMatrix34_t mat = hmd_->GetEyeToHeadTransform(eye);
    Matrix4x4 matrix;

    // This is only a transformation matrix, so we can invert values here.
    matrix.setRow(0, mat.m[0][0], mat.m[1][0], mat.m[2][0], 0.0f);
    matrix.setRow(1, mat.m[0][1], mat.m[1][1], mat.m[2][1], 0.0f);
    matrix.setRow(2, mat.m[0][2], mat.m[1][2], mat.m[2][2], 0.0f);
    matrix.setRow(3, mat.m[0][3], mat.m[1][3], mat.m[2][3], 1.0f);

    return matrix;
}
