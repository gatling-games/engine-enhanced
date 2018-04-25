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
#include "Scene/StaticTurret.h"
#include "Scene/Rocket.h"

#include "Physics/SphereCollider.h"
#include "Physics/BoxCollider.h"

#include "Serialization/Prefab.h"
#include "EditorManager.h"
#include "Windmill.h"
#include "HelicopterView.h"
#include "Physics/Rigidbody.h"
#include "Physics/TerrainCollider.h"
#include "Utils/ImGuiExtensions.h"

GameObject::GameObject()
    : GameObject("Blank GameObject")
{

}

GameObject::GameObject(const std::string &name)
    : GameObject(name, nullptr)
{

}

GameObject::GameObject(const std::string &name, Prefab* prefab)
    : name_(name),
    flags_(0),
    prefab_(prefab)
{
    // Give every GameObject instance a transform component
    // This ensures that gameobject can be parented inside each other.
    createComponent<Transform>();

    if (prefab != nullptr)
    {
        // Read the prefab's properties into this gameobject.
        PropertyTable prefabProperties = prefab_->serializedProperties();
        serialize(prefabProperties);
    }

    // Register this gameobject with the scene manager.
    SceneManager::instance()->gameObjectCreated(this);
}

GameObject::~GameObject()
{
    // Use a while loop, not a for loop, as the components list
    //   is modified whenever a component gets deleted.
    while(!components_.empty())
    {
        delete components_.back();
    }

    // Ensure the scene manager knows the object has been deleted.
    SceneManager::instance()->gameObjectDeleted(this);

    // Ensure the properties panel isnt still showing the object
    if (PropertiesPanel::instance()->current() == this)
    {
        PropertiesPanel::instance()->inspect(nullptr);
    }
}

bool GameObject::hasFlag(GameObjectFlag flag) const
{
    return ((int)flags_ & (int)flag) != 0;
}

void GameObject::setFlag(GameObjectFlag flag, bool state)
{
    if(state)
    {
        flags_ |= (uint32_t)flag;
    }
    else
    {
        flags_ &= ~(uint32_t)flag;
    }
}

void GameObject::setFlags(GameObjectFlagList flags)
{
    flags_ = flags;
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

    // Delete the gameobject if the delete key is pressed
    if (ImGui::IsKeyDown(GLFW_KEY_DELETE))
    {
        delete this;
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
			// Display a remove component button
			// This must not be shown for transform components, they cannot be removed.
			if (static_cast<Transform*>(component) != nullptr && ImGui::BigButton("Remove Component"))
			{
				delete component;
			}
			else
			{
				component->drawProperties();
			}
        }
    }
}

void GameObject::drawAddComponentSection()
{
    // Display a big add component button
    ImGui::Spacing();
    if (ImGui::BigButton("Add Component"))
    {
        ImGui::OpenPopup("Add Component");
    }

    // Draw the add component popup, when selected
    if (ImGui::BeginPopupContextItem("Add Component"))
    {
        if (ImGui::Selectable("Camera")) createComponent<Camera>();
        if (ImGui::Selectable("Static Mesh")) createComponent<StaticMesh>();
        if (ImGui::Selectable("Helicopter")) createComponent<Helicopter>();
        if (ImGui::Selectable("Helicopter View")) createComponent<HelicopterView>();
        if (ImGui::Selectable("Shield")) createComponent<Shield>();
        if (ImGui::Selectable("Windmill")) createComponent<Windmill>();
        if (ImGui::Selectable("Sphere Collider")) createComponent<SphereCollider>();
        if (ImGui::Selectable("Box Collider")) createComponent<BoxCollider>();
        if (ImGui::Selectable("Rigidbody")) createComponent<Rigidbody>();
        if (ImGui::Selectable("Terrain Collider")) createComponent<TerrainCollider>();
        if (ImGui::Selectable("Static Turret")) createComponent<StaticTurret>();
        if (ImGui::Selectable("Rocket")) createComponent<Rocket>();

        ImGui::EndPopup();
    }
}

void GameObject::drawSaveAsPrefabSection()
{
    ImGui::Spacing();
    if (ImGui::BigButton("Save As Prefab"))
    {
        // Display a save dialog
        const std::string savePath = EditorManager::instance()->showSaveDialog("Save As Prefab", name_, "prefab");
        if (!savePath.empty())
        {
            // Create a new prefab at the save path.
            // If a prefab already exists at the path, the existing prefab is returned.
            Prefab* prefab = ResourceManager::instance()->createResource<Prefab>(savePath);

            // Break any existing prefab link, and then clone into the new prefab
            prefab_ = nullptr;
            prefab->cloneGameObject(this);
            prefab_ = prefab;

            ResourceManager::instance()->saveAllSourceFiles();
        }
    }
}

void GameObject::drawPrefabInfoSection()
{
    // Display a break link button
    if (ImGui::BigButton("Break Link With " + prefab_->resourceName()))
    {
        prefab_ = nullptr;

        // We no longer have a prefab, so drawin the rest of this ui will
        // cause a crash.
        return;
    }

    // Display an apply changes button
    if (ImGui::BigButton("Apply Changes to " + prefab_->resourceName()))
    {
        prefab_->cloneGameObject(this);
    }

    // Display a revert changes button
    if (ImGui::BigButton("Revert Changes from " + prefab_->resourceName()))
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
        table.serialize("prefab", prefab_);
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
        table.serialize("prefab", prefab_);
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

        // Build a list of child gameobjects and write them too.
        std::vector<GameObject*> children;
        for(Transform* child : transform()->children())
        {
            children.push_back(child->gameObject());
        }
        table.serialize("children", children);
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

        // Build a list of child gameobjects and get the propertytable to modify it as needed.
        std::vector<GameObject*> children;
        for (Transform* child : transform()->children())
        {
            children.push_back(child->gameObject());
        }
        table.serialize("children", children);

        // Make sure each child references this go as its parent
        for (GameObject* childGameObject : children)
        {
            childGameObject->transform()->setParentTransform(transform());
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
        if(components_[i]->updateEnabled() == false)
        {
            continue;
        }

        components_[i]->update(deltaTime);
    }
}

void GameObject::handleInput(const InputCmd& inputs)
{
    for (Component* component : components_)
    {
        component->handleInput(inputs);
    }
}

void GameObject::handleCollision(Collider* collider)
{
	for(Component* component : components_)
	{
		component->handleCollision(collider);
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

    if (typeName == "HelicopterView")
        return createComponent<HelicopterView>();

    if (typeName == "StaticTurret")
        return createComponent<StaticTurret>();

    if (typeName == "Terrain")
        return createComponent<Terrain>();

    if (typeName == "Shield")
        return createComponent<Shield>();

    if (typeName == "Windmill")
        return createComponent<Windmill>();

    if (typeName == "SphereCollider")
        return createComponent<SphereCollider>();

    if (typeName == "BoxCollider")
        return createComponent<BoxCollider>();

    if (typeName == "Rigidbody")
        return createComponent<Rigidbody>();

	if (typeName == "TerrainCollider")
		return createComponent<TerrainCollider>();

    if (typeName == "Rocket")
        return createComponent<Rocket>();

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

void GameObject::removeComponent(Component* discard)
{
	auto it = std::find(components_.begin(), components_.end(), discard);

	if (it != components_.end())
	{
		components_.erase(it);
	}
}
