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

bool SphereCollider::checkForCollision(const Point3 &point) const
{
	// Put the point into local space
	const Point3 p = gameObject()->transform()->worldToLocal() * point;

	// Check it against the sphere
	return (Point3::sqrDistance(Point3::origin(), p) < radius_ * radius_);
}
