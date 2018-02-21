#include "Prefab.h"

#include <imgui.h>

#include "SceneManager.h"
#include "Editor/PropertiesPanel.h"

Prefab::Prefab(ResourceID resourceID)
    : Resource(resourceID),
    properties_(PropertyTableMode::Reading)
{

}

void Prefab::drawEditor()
{
    if(ImGui::Button("Create Instance in Scene", ImVec2(ImGui::GetContentRegionAvailWidth(), 40.0f)))
    {
        PropertiesPanel::instance()->inspect(SceneManager::instance()->createGameObject(this));
    }
}

void Prefab::serialize(PropertyTable &table)
{
    // The prefab is saved in an already-serialized form,
    // so we just need to write the entire thing to or from the table.
    if (table.mode() == PropertyTableMode::Writing)
    {
        table.addPropertyData(properties_, true);
    }
    else
    {
        properties_ = table;
        properties_.setMode(PropertyTableMode::Reading);
    }
}

void Prefab::cloneGameObject(GameObject* original)
{
    // We are creating a prefab from an existing GameObject.
    // Serialize the source gameobject into the prefab property table
    
    // First, delete the existing data
    properties_.clear();

    // The gameobject delta compresses its property list based on its prefab (aka this)
    // We need to write to a different property table and copy into the prefab after we finish.
    PropertyTable table(PropertyTableMode::Writing);
    original->serialize(table);
    
    // Store the properties in reading mode, ready for use later.
    properties_ = table;
    properties_.setMode(PropertyTableMode::Reading);
}
