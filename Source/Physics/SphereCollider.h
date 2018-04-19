#pragma once

#include "Collider.h"

class SphereCollider : public Collider
{
public:
    SphereCollider(GameObject* gameObject);

    // The radius of the sphere collider
    // The collider is also affected by the transform scale and position.
    float radius() const { return radius_; }
    void setRadius(float value);

private:
    float radius_;
};