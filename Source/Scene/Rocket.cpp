#include "Rocket.h"

#include "Scene/Transform.h"
#include "Physics/Rigidbody.h"
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

void Rocket::update(float deltaTime)
{
    if (initialised_ == false)
    {
        Rigidbody* collider;
        if (collider = gameObject()->findComponent<Rigidbody>())
        {
            Vector3 velocity = transform_->forwards() * speed_;
            collider->setVelocity(velocity);
            std::cout << "Forwards is " << transform_->forwards().x << ", " << transform_->forwards().y << ", " << transform_->forwards().z << std::endl;
            std::cout << "Velocity is " << velocity.x << ", " << velocity.y << ", " << velocity.z << std::endl;
            initialised_ = true;
        }
    }
}

