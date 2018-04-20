#pragma once

#include "Collider.h"
#include "Math/Vector3.h"

class SphereCollider : public Collider
{
public:
    SphereCollider(GameObject* gameObject);

    void drawProperties() override;
    void serialize(PropertyTable& table) override;

    // The radius of the sphere collider
    // The collider is also affected by the transform scale and position.
    float radius() const { return radius_; }
    void setRadius(float value);

    // The offset of the sphere
    // This offsets the origin of the sphere from the transform origin
    Vector3 offset() const { return offset_; }
    void setOffset(const Vector3 &offset);

private:
    float radius_;
    Vector3 offset_;
};
