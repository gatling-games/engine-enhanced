#include "Camera.h"

#include "Scene/Transform.h"

Camera::Camera(const GameObjectID gameObjectID)
    : GameObjectComponent(gameObjectID)
{
    nearPlaneDistance_ = 0.1f;
    farPlaneDistance_ = 500.0f;
    horizontalFOV_ = 60.0f;
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
    const Transform* transform = gameObject()->transformComponent();
    const Matrix4x4 worldToLocal = transform->worldToLocal();
    const Matrix4x4 projection = Matrix4x4::perspective(horizontalFOV_, aspectRatio, nearPlaneDistance_, farPlaneDistance_);

    return projection * worldToLocal;
}
