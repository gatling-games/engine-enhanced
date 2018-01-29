#include "Camera.h"

#include <imgui.h>

#include "Scene/Transform.h"

Camera::Camera(GameObject* gameObject)
    : Component(gameObject)
{
    nearPlaneDistance_ = 0.1f;
    farPlaneDistance_ = 500.0f;
    horizontalFOV_ = 60.0f;
}

void Camera::drawProperties()
{
    ImGui::DragFloat("Near Plane", &nearPlaneDistance_, 0.1f, 0.001f, 100.0f);
    ImGui::DragFloat("Far Plane", &farPlaneDistance_, 1.0f, 50.0f, 10000.0f);
    ImGui::DragFloat("Horiz FOV", &horizontalFOV_, 1.0f, 1.0f, 180.0f);
}

void Camera::serialize(PropertyTable &table)
{
    table.serialize("Near Plane", nearPlaneDistance_, 0.1f);
    table.serialize("Far Plane", farPlaneDistance_, 1000.0f);
    table.serialize("Horiz FOV", horizontalFOV_, 60.0f);
}

float Camera::getNearPlaneDistance() const
{
    return nearPlaneDistance_;
}

void Camera::setNearPlaneDistance(const float& distance)
{
    nearPlaneDistance_ = distance;
}

float Camera::getFarPlaneDistance() const
{
    return farPlaneDistance_;
}

void Camera::setFarPlaneDistance(const float& distance)
{
    farPlaneDistance_ = distance;
}

float Camera::getHorizontalFOV() const
{
    return horizontalFOV_;
}

void Camera::setHorizontalFOV(const float& FOV)
{
    horizontalFOV_ = FOV;
}

Matrix4x4 Camera::getWorldToCameraMatrix(float aspectRatio) const
{
    const Transform* transform = gameObject()->findComponent<Transform>();
    const Matrix4x4 worldToLocal = transform->worldToLocal();
    const Matrix4x4 projection = Matrix4x4::perspective(horizontalFOV_, aspectRatio, nearPlaneDistance_, farPlaneDistance_);

    return projection * worldToLocal;
}

Matrix4x4 Camera::getCameraToWorldMatrix(float aspectRatio) const
{
    const Transform* transform = gameObject()->findComponent<Transform>();
    const Matrix4x4 localToWorld = transform->localToWorld();
    const Matrix4x4 inverseProjection = Matrix4x4::perspectiveInverse(horizontalFOV_, aspectRatio, nearPlaneDistance_, farPlaneDistance_);

    return localToWorld * inverseProjection;
}
