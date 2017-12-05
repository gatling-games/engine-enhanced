#include "ScenePanel.h"

#include <imgui.h>

#include "EditorManager.h"
#include "Editor/PropertiesPanel.h"

#include "SceneManager.h"
#include "Scene/GameObject.h"

void ScenePanel::draw()
{
    // Get a list of game objects in the scene.
    auto gameObjects = SceneManager::instance()->gameObjects();

    // Draw all of the game objects in the scene with no parent.
    for (unsigned int i = 0; i < gameObjects.size(); ++i)
    {
        drawNode(gameObjects[i]);
    }
}

void ScenePanel::drawNode(GameObject* gameObject)
{
    // If the gameobject has no root nodes, draw a simple button.
    ImGui::Bullet();
    if (ImGui::Selectable(gameObject->name().c_str()))
    {
        gameObjectSelected(gameObject);
    }
}

void ScenePanel::gameObjectSelected(GameObject* gameObject)
{
    // Inspect the object in the properties panel
    PropertiesPanel::instance()->inspect(gameObject);
}