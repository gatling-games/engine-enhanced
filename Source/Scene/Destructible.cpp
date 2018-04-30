#include "Destructible.h"

#include "imgui.h"

Destructible::Destructible(GameObject* gameObject)
    : Component(gameObject),
    health_(50.0f)
{

}

void Destructible::drawProperties()
{
    ImGui::DragFloat("Health", &health_, 1.0f);
}

void Destructible::serialize(PropertyTable &table)
{
    table.serialize("health", health_, 50.0f);
}

void Destructible::takeDamage(float damage)
{
    health_ -= damage;

    if (health_ <= 0.0f)
    {
        delete gameObject();
    }
}