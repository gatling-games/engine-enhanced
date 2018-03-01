#pragma once

#include "GameObject.h"
#include "Component.h"

#include "Math/Point3.h"
#include "Math/Vector3.h"
#include "Math/Quaternion.h"
#include "Math/Matrix4x4.h"

class Transform : public Component
{
public:
    explicit Transform(GameObject* gameObject);
    virtual ~Transform();

    // Draws the transform properties fold out
    void drawProperties() override;

    // Override of Component::serialize().
    // Handles component serialization
    void serialize(PropertyTable &table) override;

    // Return parent transform
    Transform* parentTransform() const { return parent_; }

    // Return child transform vector
    const std::vector<Transform*>& children() const { return children_; }

    // Transform position / rotation / scale in local space
    Point3 positionLocal() const { return position_; }
    Quaternion rotationLocal() const { return rotation_; }
    Vector3 scaleLocal() const { return scale_; }

    // Transform position / rotation / scale in world space
    // world space = local space for objects with no parent
    Point3 positionWorld() const;
    Quaternion rotationWorld() const;
    Vector3 scaleWorld() const;

    // Transformation matrices
    Matrix4x4 worldToLocal() const { return worldToLocal_; }
    Matrix4x4 localToWorld() const { return localToWorld_; }

    // Object axis in world space
    Vector3 left() const;
    Vector3 right() const;
    Vector3 forwards() const;
    Vector3 backwards() const;
    Vector3 up() const;
    Vector3 down() const;

    // Changes parent transform for manipulating transform hierarchy
    void setParentTransform(Transform* parent);

    // Callback when transform is changed for child notification and matrix recomputation
    void onTransformChanged();

    // Directly sets the transformation TRS values
    void setPositionLocal(const Point3 &pos);
    void setRotationLocal(const Quaternion &rot);
    void setScaleLocal(const Vector3 &scale);

    // Translates the object by the vector
    void translateLocal(const Vector3 &translation);

    // Rotates object around a vector in local/world space
    void rotateLocal(float angle, const Vector3& axis);
    void rotateWorld(float angle, const Vector3& axis);

private:
    // Local space transformation values
    Point3 position_;
    Quaternion rotation_;
    Vector3 scale_;

    // Parent transform
    Transform* parent_;

    // Child transforms
    std::vector<Transform*> children_;

    // Cached transformation matrices
    Matrix4x4 worldToLocal_;
    Matrix4x4 localToWorld_;
    void recomputeMatrices();

    // For adding and removal of child transforms
    // Called when parent transforms are set by children
    void addChild(Transform* child);
    void removeChild(Transform* child);
};
