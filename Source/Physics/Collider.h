#pragma once

#include "Scene/Component.h"

#include "Math/Vector3.h"
#include "Math/Point3.h"

class Collider : public Component
{
public:
    explicit Collider(GameObject* gameObject);

    // Checks if a world-space point intersects with the collider.
    // If true, the ColliderHit struct will be filled in.
    virtual bool checkForCollision(const Point3 &point) const = 0;

private:

};