#pragma once

#include "GameObject.h"
#include "Component.h"

#include "Math/Matrix4x4.h"

class Camera : public Component
{
public:
    explicit Camera(GameObject* gameObject);
    ~Camera() override { }

    // Draws the camera properties fold out
    void drawProperties() override;

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
