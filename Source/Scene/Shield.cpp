#include "Shield.h"
#include "imgui.h"

Shield::Shield(GameObject* gameObject)
    : Component(gameObject),
    radius_(70.0f)
{

}

void Shield::drawProperties()
{
    ImGui::DragFloat("Radius", &radius_, 1.0f, 5.0f, 200.0f);
}

void Shield::serialize(PropertyTable& table)
{
    table.serialize("radius", radius_, 70.0f);
}
