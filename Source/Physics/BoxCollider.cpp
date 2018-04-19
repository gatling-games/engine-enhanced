#include "BoxCollider.h"

BoxCollider::BoxCollider(GameObject* gameObject)
    : Collider(gameObject)
{

}

void BoxCollider::setSize(const Vector3 &size)
{
    size_ = size;
}