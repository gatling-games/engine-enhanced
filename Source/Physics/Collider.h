#pragma once

#include "Scene/Component.h"

#include "Math/Vector3.h"
#include "Math/Point3.h"

struct ColliderHit
{
    Point3 position;
    Vector3 normal;
};

class Collider : public Component
{
public:
    explicit Collider(GameObject* gameObject);

    // Checks if a world-space point intersects with the collider.
    // If true, the ColliderHit struct will be filled in.
    virtual bool checkForCollision(Point3 point, ColliderHit &hit) const = 0;

private:

};