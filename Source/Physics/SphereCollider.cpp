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

bool SphereCollider::checkForCollision(Point3 point, float radius, ColliderHit& hit) const
{
    // Get the local space centre of the collider in world space
    const Point3 localSpaceCentre(offset_);
    const Point3 worldSpaceCentre = gameObject()->transform()->localToWorld() * localSpaceCentre;

    // Find the vector between the two centres
    const Vector3 colliderToPoint = point - worldSpaceCentre;

    // Scale the radius by the transfrm scale
    const float radiusScale = gameObject()->transform()->scaleWorld();

    // A hit occurs if the 2 radii are greater than the distance between the colliders
    if(colliderToPoint.magnitude() < (radius + radius_))
    {
        std::cout << "Point at " << point << std::endl;
        std::cout << "Point radius " << radius << std::endl;
        std::cout << "Collider at " << worldSpaceCentre << std::endl;
        std::cout << "Collider radius " << radius_ << std::endl;
        printf("Collision with %s \n", gameObject()->name().c_str());

        hit.position = worldSpaceCentre + colliderToPoint.normalized() * radius_;
        hit.normal = colliderToPoint.normalized();
        return true;
    }
    else
    {
        return false;
    }
}
