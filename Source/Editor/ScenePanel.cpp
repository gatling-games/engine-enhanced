#include "ScenePanel.h"

#include <imgui.h>

#include "EditorManager.h"
#include "Editor/PropertiesPanel.h"

#include "SceneManager.h"
#include "Scene/GameObject.h"
#include "Scene/Transform.h"

void ScenePanel::draw()
{
    // Draw all of the game objects in the scene with no parent.
    for (GameObject* gameObject : SceneManager::instance()->sceneObjects())
    {
        if (gameObject->transform()->parentTransform() == nullptr)
        {
            drawNode(gameObject);
        }
    }
}

void ScenePanel::drawNode(GameObject* gameObject)
{
    // Determine the tree flags, based on selection state.
    ImGuiTreeNodeFlags nodeFlags = 0;

    // Set flag when gameObject selected
    if (PropertiesPanel::instance()->current() == gameObject)
    {
        nodeFlags |= ImGuiTreeNodeFlags_Selected;
    }

    // Set as tree leaf when gameObject has no children
    if (gameObject->transform()->children().empty())
    {
        nodeFlags |= ImGuiTreeNodeFlags_Leaf;
    }

    // Show a different colour for gameobjects with a prefab.
    if (gameObject->prefab() != nullptr)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_Button]);
    }

    // Draw the actual tree node
    bool node_open = ImGui::TreeNodeEx(gameObject->name().c_str(), nodeFlags);
    if (ImGui::IsItemClicked())
    {
        gameObjectSelected(gameObject);
    }

    if (node_open)
    {
        // Draw child nodes when parent node opened
        for (Transform* child : gameObject->transform()->children())
        {
            drawNode(child->gameObject());
        }

        ImGui::TreePop();
    }

    // If we changed the colour, change it back
    if (gameObject->prefab() != nullptr)
    {
        ImGui::PopStyleColor();
    }
}

void ScenePanel::gameObjectSelected(GameObject* gameObject)
{
    // Inspect the object in the properties panel
    PropertiesPanel::instance()->inspect(gameObject);
}