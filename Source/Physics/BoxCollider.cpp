#include "BoxCollider.h"

#include "Math/Vector3.h"
#include "Scene/Transform.h"
#include "Utils/ImGuiExtensions.h"

BoxCollider::BoxCollider(GameObject* gameObject)
	: Collider(gameObject),
	size_(Vector3::one()),
	offset_(Vector3::zero())
{

}

void BoxCollider::drawProperties()
{
	ImGui::DragFloat3("Size", &size_.x, 0.001f);
	ImGui::DragFloat3("Offset", &offset_.x, 0.001f);
}

void BoxCollider::serialize(PropertyTable& table)
{
	table.serialize("size", size_, Vector3::one());
	table.serialize("offset", offset_, Vector3::zero());
}

void BoxCollider::setSize(const Vector3 &size)
{
	size_ = size;
}

void BoxCollider::setOffset(const Vector3& offset)
{
	offset_ = offset;
}

bool BoxCollider::checkForCollision(const Point3 &point) const
{
	// Put the point into local space
	const Point3 p = gameObject()->transform()->worldToLocal() * point;

	// Check it against the bounding box
	return (p.x > (size_.x * -0.5f + offset_.x)
		&& p.x < (size_.x * 0.5f + offset_.x)
		&& p.y >(size_.y * -0.5f + offset_.y)
		&& p.y < (size_.y * 0.5f + offset_.y)
		&& p.z >(size_.z * -0.5f + offset_.z)
		&& p.z < (size_.z * 0.5f + offset_.z));
}
