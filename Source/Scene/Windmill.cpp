#include "Windmill.h"

#include "imgui.h"

#include "Scene/Transform.h"

Windmill::Windmill(GameObject* gameObject)
    : Component(gameObject),
    rotationSpeed_(20.0f)
{

}

void Windmill::drawProperties()
{
    ImGui::SliderFloat("Speed", &rotationSpeed_, 0.0f, 200.0f);
}

void Windmill::serialize(PropertyTable& table)
{
    table.serialize("rotation_speed", rotationSpeed_, 0.0f);
}

void Windmill::update(float deltaTime)
{
    gameObject()->transform()->children()[0]->rotateLocal(rotationSpeed_ * deltaTime, Vector3(-1.0f, 0.0f, 0.0f));
}
