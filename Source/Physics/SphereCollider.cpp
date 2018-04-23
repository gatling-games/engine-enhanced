#include "SphereCollider.h"

#include "imgui.h"

#include "Scene/Transform.h"

SphereCollider::SphereCollider(GameObject* gameObject)
    : Collider(gameObject),
    radius_(1.0f),
    offset_(Vector3::zero())
{

}

void SphereCollider::drawProperties()
{
    ImGui::DragFloat("Radius", &radius_, 0.001f, 0.0001f);
    ImGui::DragFloat3("Offset", &offset_.x, 0.001f);
}

void SphereCollider::serialize(PropertyTable& table)
{
    table.serialize("radius", radius_, 1.0f);
    table.serialize("offset", offset_, Vector3::zero());
}

void SphereCollider::setRadius(float value)
{
    radius_ = value;
}

void SphereCollider::setOffset(const Vector3& offset)
{
    offset_ = offset;
}

bool SphereCollider::checkForCollision(Point3 point, ColliderHit& hit) const
{
    // Get the centre of the sphere collider in world space
    const Point3 colliderCentre = gameObject()->transform()->localToWorld() * Point3(offset_);

    // Find the vector from the collider centre to the point
    const Vector3 colliderToPoint = point - colliderCentre;

    // Scale the radius by the transform scale
	// This code assumes the scale is uniform in x, y and z
    const float sphereColliderRadius = radius_ * gameObject()->transform()->scaleWorld().x;

    // A hit occurs if the 2 radii are greater than the distance between the colliders
    if(colliderToPoint.magnitude() < sphereColliderRadius)
    {
        hit.position = colliderCentre + colliderToPoint.normalized() * sphereColliderRadius;
        hit.normal = colliderToPoint.normalized();
        return true;
    }
    else
    {
        return false;
    }
}
