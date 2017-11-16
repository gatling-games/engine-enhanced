#include "Camera.h"

Camera::Camera(const Point3 &position, const Quaternion &rotation)
{
    position_ = position;
    rotation_ = rotation;

    nearPlaneDistance_ = 0.1f;
    farPlaneDistance_ = 500.0f;
    horizontalFOV_ = 60.0f;
}

Point3 Camera::getPosition() const
{
    return position_;
}

void Camera::setPosition(const Point3 &position)
{
    position_ = position;
}

Quaternion Camera::getRotation() const
{
    return rotation_;
}

void Camera::setRotation(const Quaternion &rotation)
{
    rotation_ = rotation;
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
    const Matrix4x4 worldToLocal = Matrix4x4::trsInverse(position_, rotation_, Vector3::zero());
    const Matrix4x4 projection = Matrix4x4::perspective(horizontalFOV_, aspectRatio, nearPlaneDistance_, farPlaneDistance_);

    return projection * worldToLocal;
}
