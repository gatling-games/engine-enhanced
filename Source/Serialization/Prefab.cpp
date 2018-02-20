#include "Prefab.h"

#include <imgui.h>

#include "SceneManager.h"
#include "Editor/PropertiesPanel.h"

Prefab::Prefab(ResourceID resourceID)
    : Resource(resourceID),
    gameObject_("Prefab")
{

}

PropertyTable Prefab::serializedProperties()
{
    PropertyTable properties(PropertyTableMode::Writing);
    gameObject_.serialize(properties);
    properties.setMode(PropertyTableMode::Reading);
    return properties;
}

void Prefab::drawEditor()
{
    gameObject_.drawEditor();

    if(ImGui::Button("Create Instance in Scene", ImVec2(ImGui::GetContentRegionAvailWidth(), 40.0f)))
    {
        PropertiesPanel::instance()->inspect(SceneManager::instance()->createGameObject(this));
    }
}

void Prefab::serialize(PropertyTable &table)
{
    gameObject_.serialize(table);
}

void Prefab::cloneGameObject(GameObject* original)
{
    // We are creating a prefab from an existing GameObject.
    // Serialize the source gameobject, and then deserialise its contents
    // onto the gameobject inside the prefab.
    PropertyTable properties(PropertyTableMode::Writing);
    original->serialize(properties);
    properties.setMode(PropertyTableMode::Reading);
    gameObject_.serialize(properties);
}
