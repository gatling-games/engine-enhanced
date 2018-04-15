#include "Windmill.h"

#include "imgui.h"

#include "Scene/Transform.h"

Windmill::Windmill(GameObject* gameObject)
    : Component(gameObject),
    rotationSpeed_(0.0f),
    axis_(0)
{

}

void Windmill::drawProperties()
{
    ImGui::Text("Rotation Axis");
    ImGui::RadioButton("X", &axis_, 0); ImGui::SameLine();
    ImGui::RadioButton("Y", &axis_, 1); ImGui::SameLine();
    ImGui::RadioButton("Z", &axis_, 2);
    ImGui::SliderFloat("Speed", &rotationSpeed_, -2000.0f, 2000.0f);
}

void Windmill::serialize(PropertyTable& table)
{
    table.serialize("rotation_speed", rotationSpeed_, 0.0f);
    table.serialize("rotation_axis", axis_, 0);
}

void Windmill::update(float deltaTime)
{
    switch (axis_)
    {
        case 0:
            gameObject()->transform()->rotateLocal(rotationSpeed_ * deltaTime, Vector3(1.0f, 0.0f, 0.0f));
            break;
        case 1:
            gameObject()->transform()->rotateLocal(rotationSpeed_ * deltaTime, Vector3(0.0f, 1.0f, 0.0f));
            break;
        case 2:
            gameObject()->transform()->rotateLocal(rotationSpeed_ * deltaTime, Vector3(0.0f, 0.0f, 1.0f));
            break;
    }
    
}
