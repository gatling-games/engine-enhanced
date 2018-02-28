#pragma once

#include "GameObject.h"
#include "Component.h"

#include "Math/Matrix4x4.h"

enum class CameraType
{
    Perspective,
    Orthographic
};

enum class EyeType
{
    LeftEye,
    RightEye,
    None
};

class Camera : public Component
{
public:
    explicit Camera(GameObject* gameObject);
    ~Camera() override { }

    // Draws the camera properties fold out
    void drawProperties() override;

    // Override of Component::serialize().
    // Handles component serialization
    void serialize(PropertyTable &table) override;

    // Gets basic settings
    CameraType type() const { return type_; }
    float nearPlane() const { return nearPlane_; }
    float farPlane() const { return farPlane_; }
    float orthographicSize() const { return orthographicSize_; }
    float fov() const { return fov_; }

    // Gets the 4 corners of the view frustum at the specified distance.
    // Perspective mode only. The points returned are in local space.
    void getFrustumCorners(float distance, Point3* corners, float aspect) const;

    // Changes the type of camera
    void setType(CameraType type);

    // Sets near and far clipping planes
    void setNearPlane(float nearPlane);
    void setFarPlane(float farPlane);

    // Sets the orthographic projection to a specified square size
    void setOrthographicSize(float size);

    // Changes the vertical fov
    void setFov(float fov);

    // Gets the combined world -> clip space transformation matrix
    Matrix4x4 getWorldToCameraMatrix(float aspectRatio, EyeType eye = EyeType::None) const;

    // Gets the inverse of the world -> clip space matrix
    Matrix4x4 getCameraToWorldMatrix(float aspectRatio) const;

private:
    CameraType type_;

    // Near and far clipping planes
    float nearPlane_;
    float farPlane_;

    // Orthographic size
    float orthographicSize_;

    // Vertical fov
    float fov_;
};
