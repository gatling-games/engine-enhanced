#include "SphereCollider.h"

SphereCollider::SphereCollider(GameObject* gameObject)
    : Collider(gameObject)
{

}

void SphereCollider::setRadius(float value)
{
    radius_ = value;
}