#pragma once

#include "Collider.h"
#include "Math/Vector3.h"

class SphereCollider : public Collider
{
public:
    explicit SphereCollider(GameObject* gameObject);

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

    // Checks if the sphere is intersecting with a point with the specified radius.
    bool checkForCollision(const Point3 &point) const override;

private:
    float radius_;
    Vector3 offset_;
};
