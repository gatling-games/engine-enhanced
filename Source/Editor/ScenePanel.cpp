#include "ScenePanel.h"

#include <imgui.h>

#include "EditorManager.h"
#include "Editor/PropertiesPanel.h"

#include "SceneManager.h"
#include "Scene/GameObject.h"

void ScenePanel::draw()
{
    // Draw all of the game objects in the scene with no parent.
    auto gameObjects = SceneManager::instance()->gameObjects();
    for (unsigned int i = 0; i < gameObjects.size(); ++i)
    {
        drawNode(gameObjects[i]);
    }
}

void ScenePanel::drawNode(GameObject* gameObject)
{
    // Determine the tree flags, based on selection state.
    ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Leaf;
    if (PropertiesPanel::instance()->current() == gameObject)
    {
        nodeFlags |= ImGuiTreeNodeFlags_Selected;
    }

    // Draw the actual tree node
    ImGui::TreeNodeEx(gameObject->name().c_str(), nodeFlags);
    if (ImGui::IsItemClicked())
    {
        gameObjectSelected(gameObject);
    }
}

void ScenePanel::gameObjectSelected(GameObject* gameObject)
{
    // Inspect the object in the properties panel
    PropertiesPanel::instance()->inspect(gameObject);
}