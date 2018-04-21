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

    // Checks if the collider intersects with a sphere with the specified centre
    // and radius.
    // If true, the ColliderHit struct will be filled in.
    virtual bool checkForCollision(Point3 point, float radius, ColliderHit &hit) const = 0;

private:

};