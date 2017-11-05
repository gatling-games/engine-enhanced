#include "ScenePanel.h"

#include <imgui.h>

void ScenePanel::draw()
{
    if (ImGui::TreeNode("Terrain"))
    {
        ImGui::Text("base_renderer");
        ImGui::Text("vegetation_renderer");
        ImGui::TreePop();
    }

    for (int i = 0; i < 5; ++i)
    {
        if (ImGui::TreeNode((const void*)i, "enemy_tank"))
        {
            ImGui::Text("collider");

            if (ImGui::TreeNode("turret"))
            {
                ImGui::Text("light");
                ImGui::Text("fire_effects");

                ImGui::TreePop();
            }

            ImGui::TreePop();
        }
    }

    if (ImGui::TreeNode("player_helicopter"))
    {
        ImGui::Text("guns_left");
        ImGui::Text("guns_right");
        ImGui::Text("rotor_blades");

        if (ImGui::TreeNode("player"))
        {
            ImGui::Text("head");
            ImGui::Text("hand_left");
            ImGui::Text("hand_right");
            ImGui::TreePop();
        }

        ImGui::TreePop();
    }
}