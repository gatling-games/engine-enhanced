#include "Rocket.h"

#include "Scene/Transform.h"
#include "Physics/Rigidbody.h"

#include "SceneManager.h"

#include "imgui.h"

Rocket::Rocket(GameObject* gameObject)
    : Component(gameObject),
    transform_(gameObject->createComponent<Transform>()),
    speed_(200.0f)
{
    transform_->setRotationLocal(Quaternion::identity());
}

void Rocket::drawProperties()
{
    ImGui::DragFloat("Rocket speed", &speed_, 0.1f);
}

void Rocket::handleCollision(Collider* collider)
{
    // Delete gameObject on collision
    delete gameObject();
}
