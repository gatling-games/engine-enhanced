#include "PropertiesPanel.h"

#include <imgui.h>

#include "Math/Vector3.h"

void PropertiesPanel::draw()
{
    if (ImGui::CollapsingHeader("GameObject", 0, false, true))
    {
        static char nameBuf[32] = "Building_Tall_01";
        ImGui::InputText("Name", nameBuf, 32);

        static Vector3 position;
        ImGui::InputFloat3("Position", &position.x);

        static Vector3 rotation;
        ImGui::InputFloat3("Rotation", &rotation.x);

        static Vector3 scale = Vector3::one();
        ImGui::InputFloat3("Scale", &scale.x);
    }

    ImGui::Spacing();
    if (ImGui::CollapsingHeader("Box Collider", 0, false, true))
    {
        static bool enabled = true;
        ImGui::Checkbox("Enabled", &enabled);
    }

    ImGui::Spacing();
    if (ImGui::CollapsingHeader("Destructible", 0, false, true))
    {
        static bool destructibleEnabled = true;
        ImGui::Checkbox("Enabled", &destructibleEnabled);

        static float startingHealth = 150.0f;
        ImGui::InputFloat("Starting Health", &startingHealth);
    }
}