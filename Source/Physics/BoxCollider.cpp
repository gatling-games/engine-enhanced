#include "BoxCollider.h"
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
