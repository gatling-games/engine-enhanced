#pragma once

#include "GameObject.h"
#include "Math/Matrix4x4.h"

class Camera : public GameObjectComponent
{
public:
    Camera(const GameObjectID gameObjectID);

    float getNearPlaneDistance() const;
    void setNearPlaneDistance(const float &distance);

    float getFarPlaneDistance() const;
    void setFarPlaneDistance(const float &distance);

    float getHorizontalFOV() const;
    void setHorizontalFOV(const float &FOV);

    Matrix4x4 getWorldToCameraMatrix(float aspectRatio) const;

private:
    float nearPlaneDistance_;
    float farPlaneDistance_;
    float horizontalFOV_;
};
