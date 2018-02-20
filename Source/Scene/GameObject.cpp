#include "GameObject.h"

#include <imgui.h>

#include "SceneManager.h"
#include "InputManager.h"

#include "Scene/Component.h"
#include "Scene/Camera.h"
#include "Scene/StaticMesh.h"
#include "Scene/Helicopter.h"
#include "Scene/Transform.h"
#include "Scene/Freecam.h"
#include "Scene/Terrain.h"

#include "Serialization/Prefab.h"

#include "Utils/ImGuiExtensions.h"
#include "EditorManager.h"

GameObject::GameObject()
    : GameObject("Blank GameObject")
{

}

GameObject::GameObject(const std::string &name)
    : name_(name),
    prefab_(nullptr)
{
    // Give every GameObject instance a transform component
    // This ensures that gameobject can be parented inside each other.
    createComponent<Transform>();
}

GameObject::GameObject(Prefab* prefab)
    : name_(""),
    prefab_(prefab)
{
    // Read the prefab's properties into this gameobject.
    PropertyTable prefabProperties = prefab_->serializedProperties();
    serialize(prefabProperties);
}

GameObject::~GameObject()
{
    for (Component* component : components_)
    {
        delete component;
    }

    // Ensure the scene manager knows the object has been deleted.
    SceneManager::instance()->gameObjectDeleted(this);

    // Ensure the properties panel isnt still showing the object
    if (PropertiesPanel::instance()->current() == this)
    {
        PropertiesPanel::instance()->inspect(nullptr);
    }
}

void GameObject::drawEditor()
{
    // Check if this gameobject is a prefab.
    // This is done by checking if the currently viewed object is a prefab object.
    const bool isPrefab = dynamic_cast<Prefab*>(PropertiesPanel::instance()->current()) != nullptr;

    // If we have a prefab, display a prefab info section.
    if (!isPrefab && prefab_ != nullptr)
    {
        drawPrefabInfoSection();
    }

    // Draw the gameobject header
    if (ImGui::CollapsingHeader("GameObject", 0, false, true))
    {
        std::vector<char> nameBuffer(1024);
        memcpy(&nameBuffer.front(), name_.c_str(), std::min(1024, (int)name_.length()));
        ImGui::InputText("Name", &nameBuffer.front(), 1024);
        name_ = &nameBuffer.front();
    }

    // Draw each component's section in turn
    drawComponentsSection();

    // Place an add component button under the components list
    drawAddComponentSection();

    // If we dont have a prefab, display a save as prefab button
    if (!isPrefab && prefab_ == nullptr)
    {
        drawSaveAsPrefabSection();
    }
}

void GameObject::drawComponentsSection()
{
    // Draw each component's section in turn
    for (Component* component : components_)
    {
        // Place a space before the component
        ImGui::Spacing();

        // Put the component controls inside a drop down
        if (ImGui::CollapsingHeader(component->name().c_str(), ImGuiTreeNodeFlags_DefaultOpen))
        {
            component->drawProperties();
        }
    }
}

void GameObject::drawAddComponentSection()
{
    // Display a big add component button
    ImGui::Spacing();
    if (ImGui::Button("Add Component", ImVec2(ImGui::GetContentRegionAvailWidth(), 40.0f)))
    {
        ImGui::OpenPopup("Add Component");
    }

    // Draw the add component popup, when selected
    if (ImGui::BeginPopupContextItem("Add Component"))
    {
        if (ImGui::Selectable("Camera")) createComponent<Camera>();
        if (ImGui::Selectable("Static Mesh")) createComponent<StaticMesh>();
        if (ImGui::Selectable("Helicopter")) createComponent<Helicopter>();

        ImGui::EndPopup();
    }
}

void GameObject::drawSaveAsPrefabSection()
{
    ImGui::Spacing();
    if (ImGui::Button("Save As Prefab", ImVec2(ImGui::GetContentRegionAvailWidth(), 40.0f)))
    {
        // Display a save dialog
        const std::string savePath = EditorManager::instance()->showSaveDialog("Save As Prefab", name_, "prefab");
        if (!savePath.empty())
        {
            // Create a new prefab at the save path.
            // If a prefab already exists at the path, the existing prefab is returned.
            Prefab* prefab = ResourceManager::instance()->createResource<Prefab>(savePath);
            prefab->cloneGameObject(this);
        }
    }
}

void GameObject::drawPrefabInfoSection()
{
    // Display a break link button
    if (ImGui::Button(("Break Link With " + prefab_->resourceName()).c_str(), ImVec2(ImGui::GetContentRegionAvailWidth(), 40.0f)))
    {
        prefab_ = nullptr;

        // We no longer have a prefab, so drawin the rest of this ui will
        // cause a crash.
        return;
    }

    // Display an apply changes button
    if (ImGui::Button(("Apply Changes to " + prefab_->resourceName()).c_str(), ImVec2(ImGui::GetContentRegionAvailWidth(), 40.0f)))
    {
        prefab_->cloneGameObject(this);
    }

    // Display a revert changes button
    if (ImGui::Button(("Revert Changes from " + prefab_->resourceName()).c_str(), ImVec2(ImGui::GetContentRegionAvailWidth(), 40.0f)))
    {
        // Write all prefab properties into a property table
        PropertyTable table(PropertyTableMode::Writing);
        prefab_->serialize(table);

        // Read the properties onto the gameobject
        table.setMode(PropertyTableMode::Reading);
        serialize(table);
    }

    ImGui::Spacing();
}

void GameObject::serialize(PropertyTable &table)
{
    // If we dont yet have a prefab, check if we are meant to have one
    if (prefab_ == nullptr && table.mode() == PropertyTableMode::Reading)
    {
        table.serialize("prefab", prefab_, (ResourcePPtr<Prefab>)nullptr);
    }

    // If we are reading and have a prefab, add the prefab data
    if (table.mode() == PropertyTableMode::Reading && prefab_ != nullptr)
    {
        table.addPropertyData(prefab_->serializedProperties(), false);
    }

    // First, serialize game object settings
    table.serialize("name", name_, "Unnamed GameObject");

    // If we have a prefab, we need to write it out.
    // This MUST NOT be called when when reading, as it may replace the prefab will nullptr
    if (table.mode() == PropertyTableMode::Writing)
    {
        table.serialize("prefab", prefab_, (ResourcePPtr<Prefab>)nullptr);
    }

    // Now we need to serialize each component.
    // This is complex, so handle reading and writing separately.
    if (table.mode() == PropertyTableMode::Writing)
    {
        // Write each component to the property table.
        // The name of each subtable is the type name of the component.
        for (Component* component : components_)
        {
            table.serialize(component->name(), *component);
        }
    }
    else // aka if table.mode() == PropertyTableMode::Reading
    {
        // The name of each property in the table is the name of a component.
        // Loop through all of the properties and attempt to spawn each one.
        const std::vector<std::string> propertyNames = table.propertyNames();

        // First, delete any components that are on the gameobject but should not be.
        for (unsigned int i = 0; i < components_.size(); ++i)
        {
            // The component should not be on the gameobject if its name
            // cannot be found in the list of property names.
            if (std::find(propertyNames.begin(), propertyNames.end(), components_[i]->name()) == propertyNames.end())
            {
                // Delete the component.
                std::swap(components_[i], components_.back());
                components_.pop_back();
                i--;
            }
        }

        // First, read data for each component that *should* be on the GameObject.
        for (const std::string& property : propertyNames)
        {
            // Check if a component already exists.
            Component* component = findComponent(property);

            // If it doesnt exist, try to spawn a matching component.
            if (component == nullptr)
            {
                component = createComponent(property);
            }

            // Now, if the component exists, deserialize its data.
            if (component != nullptr)
            {
                table.serialize(component->name(), *component);
            }
        }
    }

    // If we are writing and have a prefab, strip out unchanged properties
    if (table.mode() == PropertyTableMode::Writing && prefab_ != nullptr)
    {
        table.deltaCompress(prefab_->serializedProperties());
    }
}

void GameObject::update(float deltaTime)
{
    for (unsigned int i = 0; i < components_.size(); ++i)
    {
        components_[i]->update(deltaTime);
    }
}

Component* GameObject::findComponent(const std::string &typeName)
{
    for (Component* component : components_)
    {
        if (component->name() == typeName)
        {
            return component;
        }
    }

    return nullptr;
}

Component* GameObject::createComponent(const std::string &typeName)
{
    if (typeName == "Transform")
        return createComponent<Transform>();

    if (typeName == "Camera")
        return createComponent<Camera>();

    if (typeName == "StaticMesh")
        return createComponent<StaticMesh>();

    if (typeName == "Freecam")
        return createComponent<Freecam>();

    if (typeName == "Helicopter")
        return createComponent<Helicopter>();

    if (typeName == "Terrain")
        return createComponent<Terrain>();

    std::cerr << "CreateComponent() type " + typeName + " not defined" << std::endl;
    return nullptr;
}

Transform* GameObject::transform() const
{
    return findComponent<Transform>();
}

Camera* GameObject::camera() const
{
    return findComponent<Camera>();
}

StaticMesh* GameObject::staticMesh() const
{
    return findComponent<StaticMesh>();
}

Terrain* GameObject::terrain() const
{
    return findComponent<Terrain>();
}
