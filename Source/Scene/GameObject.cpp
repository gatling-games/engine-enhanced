#include "GameObject.h"

#include <imgui.h>

#include "SceneManager.h"

#include "Scene/Component.h"
#include "Scene/Camera.h"
#include "Scene/StaticMesh.h"
#include "Scene/Helicopter.h"
#include "Scene/Transform.h"
#include "Scene/Freecam.h"
#include "Scene/Terrain.h"

GameObject::GameObject(const std::string &name)
    : name_(name)
{
    // Give every GameObject instance a transform component
    // This ensures that gameobject can be parented inside each other.
    createComponent<Transform>();
}

void GameObject::drawEditor()
{
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

void GameObject::serialize(BitWriter&) const
{
    
}

void GameObject::deserialize(BitReader&)
{
    
}

void GameObject::update(float deltaTime)
{
    for(unsigned int i = 0; i < components_.size(); ++i)
    {
        components_[i]->update(deltaTime);
    }
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

    const std::string errorMessage = "CreateComponent() type " + typeName + " not defined";
    throw std::exception(errorMessage.c_str());
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
