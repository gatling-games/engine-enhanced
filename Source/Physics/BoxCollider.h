#pragma once

#include "Collider.h"
#include "Math/Vector3.h"

class BoxCollider : public Collider
{
public:
    explicit BoxCollider(GameObject* gameObject);

    void drawProperties() override;
    void serialize(PropertyTable& table) override;

    // The size of the box
    // The collider is also affected by the transform scale, position and rotation.
    Vector3 size() const { return size_; }
    void setSize(const Vector3 &size);

    // The offset of the box
    // This offsets the origin of the box from the transform origin
    Vector3 offset() const { return offset_; }
    void setOffset(const Vector3 &offset);

private:
    Vector3 size_;
    Vector3 offset_;
};