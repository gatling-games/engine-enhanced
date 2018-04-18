#include "Camera.h"

#include <imgui.h>

#define _USE_MATH_DEFINES
#include <math.h>

#include "Scene/Transform.h"
#include "VRManager.h"

Camera::Camera(GameObject* gameObject)
    : Component(gameObject),
    type_(CameraType::Perspective),
    nearPlane_(0.1f),
    farPlane_(15000.0f),
    orthographicSize_(100.0f),
    fov_(60.0f)
{

}

void Camera::drawProperties()
{
    ImGui::DragFloat("Near Plane", &nearPlane_, 0.1f, -10000.0f, 15000.0f);
    ImGui::DragFloat("Far Plane", &farPlane_, 1.0f, -10000.0f, 15000.0f);

    if (type() == CameraType::Orthographic)
    {
        ImGui::DragFloat("Ortho Size", &orthographicSize_, 0.5f, 1.0f, 50000.0f);
    }

    if (type() == CameraType::Perspective)
    {
        ImGui::DragFloat("Perspective FOV", &fov_, 1.0f, 1.0f, 180.0f);
    }
}

void Camera::serialize(PropertyTable &table)
{
    table.serialize("near_plane", nearPlane_, 0.1f);
    table.serialize("far_plane", farPlane_, 15000.0f);
    table.serialize("fov", fov_, 60.0f);
}

void Camera::getFrustumCorners(float distance, Point3* corners, float aspect) const
{
    // Currently supports perspective mode only
    assert(type_ == CameraType::Perspective);

    // Calculate the frustum width and height at the specified distance
    float halfFov = (fov_ / 2.0f) * ((float)M_PI / 180.0f);
    float halfHeight = distance * tanf(halfFov);
    float halfWidth = halfHeight * aspect;

    // Get the 4 corners in local space
    // Looking down positive z
    corners[0] = Point3(halfWidth, halfHeight, distance);
    corners[1] = Point3(halfWidth, -halfHeight, distance);
    corners[2] = Point3(-halfWidth, halfHeight, distance);
    corners[3] = Point3(-halfWidth, -halfHeight, distance);
}

void Camera::setType(CameraType type)
{
    type_ = type;
}

void Camera::setNearPlane(float nearPlane)
{
    nearPlane_ = nearPlane;
}

void Camera::setFarPlane(float farPlane)
{
    farPlane_ = farPlane;
}

void Camera::setOrthographicSize(float size)
{
    orthographicSize_ = size;
}

void Camera::setFov(float fov)
{
    fov_ = fov;
}

Matrix4x4 Camera::getWorldToCameraMatrix(float aspectRatio, EyeType eye) const
{
    const Transform* transform = gameObject()->findComponent<Transform>();
    const Matrix4x4 worldToLocal = transform->worldToLocal();
    
    Matrix4x4 projection;
    if (type_ == CameraType::Perspective)
    {
        projection = Matrix4x4::perspective(fov_, aspectRatio, nearPlane_, farPlane_);
    }
    else
    {
        float l = -orthographicSize_ / 2.0f;
        float r = orthographicSize_ / 2.0f;
        float t = orthographicSize_ / 2.0f;
        float b = -orthographicSize_ / 2.0f;

        projection = Matrix4x4::orthographic(l, r, b, t, nearPlane_, farPlane_);
    }

    Matrix4x4 eyeMat = Matrix4x4::identity();
    if(eye != EyeType::None)
    {
        projection = VRManager::instance()->getProjectionMatrix(eye, nearPlane_, farPlane_);
        eyeMat = VRManager::instance()->getEyeMatrix(eye);
        eyeMat = eyeMat * VRManager::instance()->getHmdMatrix();
    }

    return projection * eyeMat * worldToLocal;
}
