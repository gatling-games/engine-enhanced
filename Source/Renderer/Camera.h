#pragma once
#include "Math/Point3.h"
#include "Math/Quaternion.h"
#include "Math/Matrix4x4.h"
#include "Math/Vector3.h"

class Camera
{
public:
    Camera(const Point3 &position, const Quaternion &rotation);

    Point3 getPosition() const;
    void setPosition(const Point3 &position);

    Quaternion getRotation() const;
    void setRotation(const Quaternion &rotation);

    float getNearPlaneDistance() const;
    void setNearPlaneDistance(const float &distance);

    float getFarPlaneDistance() const;
    void setFarPlaneDistance(const float &distance);

    float getHorizontalFOV() const;
    void setHorizontalFOV(const float &FOV);

    Matrix4x4 getWorldToCameraMatrix(float aspectRatio) const;

private:
    Point3 position_;
    Quaternion rotation_;
    float nearPlaneDistance_;
    float farPlaneDistance_;
    float horizontalFOV_;
};
