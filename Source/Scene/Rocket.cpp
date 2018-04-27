#include "Rocket.h"

#include "Scene/Transform.h"
#include "Physics/Rigidbody.h"

#include "SceneManager.h"

#include "imgui.h"

Rocket::Rocket(GameObject* gameObject)
    : Component(gameObject),
    transform_(gameObject->createComponent<Transform>()),
    speed_(100.0f)
{
    // Initialise rotation
    transform_->setRotationLocal(Quaternion::identity());

    // Create rigidbody for collisions
    gameObject->createComponent<Rigidbody>();
    Rigidbody* collider = gameObject->findComponent<Rigidbody>();

    // Calculate velocity and send to rigidbody component
    Vector3 velocity = transform_->forwards() * speed_;
    collider->setVelocity(velocity);
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

void Rocket::setRocketSpeed(float speed)
{
    speed_ = speed;
}
