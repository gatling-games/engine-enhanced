#pragma once

#include "Collider.h"
#include "Math/Vector3.h"

class BoxCollider : public Collider
{
public:
    BoxCollider(GameObject* gameObject);

    // The size of the box
    // The box is centered on the gameobject origin
    // The collider is also affected by the transform scale, position and rotation.
    Vector3 size() const { return size_; }
    void setSize(const Vector3 &size);

private:
    Vector3 size_;
};