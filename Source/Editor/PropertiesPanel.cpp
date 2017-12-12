#include "PropertiesPanel.h"

#include <imgui.h>

#include "InputManager.h"

#include "Scene/Component.h"
#include "Scene/Camera.h"
#include "Scene/StaticMesh.h"
#include "Scene/Helicopter.h"

PropertiesPanel::PropertiesPanel()
    : currentGameObject_(nullptr)
{

}

void PropertiesPanel::draw()
{
    // Draw the gameobject header
    drawGameObjectSection();

    // Draw each component's section in turn
    drawComponentsSection();

    // Place an add component button under the components list
    drawAddComponentSection();
}

void PropertiesPanel::inspect(GameObject* gameObject)
{
    currentGameObject_ = gameObject;
}

void PropertiesPanel::drawGameObjectSection()
{
    if (currentGameObject_ == nullptr)
    {
        return;
    }

    // Draw the GameObject header
    if (ImGui::CollapsingHeader("GameObject", 0, false, true))
    {
        ImGui::InputText("Name", (char*)currentGameObject_->name().c_str(), 32);
    }
}

void PropertiesPanel::drawComponentsSection()
{
    if (currentGameObject_ == nullptr)
    {
        return;
    }

    // Draw each component's section in turn
    auto components = currentGameObject_->componentList();
    for (unsigned int i = 0; i < components.size(); ++i)
    {
        // Place a space before the component
        ImGui::Spacing();

        // Put the component controls inside a drop down
        if (ImGui::CollapsingHeader(components[i]->name().c_str(), ImGuiTreeNodeFlags_DefaultOpen))
        {
            components[i]->drawProperties();
        }
    }
}

void PropertiesPanel::drawAddComponentSection()
{
    if (currentGameObject_ == nullptr)
    {
        return;
    }

    // Display a big add component button
    ImGui::Spacing();
    if (ImGui::Button("Add Component", ImVec2(ImGui::GetContentRegionAvailWidth(), 40.0f)))
    {
        ImGui::OpenPopup("Add Component");
    }

    // Draw the add component popup, when selected
    if (ImGui::BeginPopupContextItem("Add Component"))
    {
        if (ImGui::Selectable("Camera")) currentGameObject_->createComponent<Camera>();
        if (ImGui::Selectable("Static Mesh")) currentGameObject_->createComponent<StaticMesh>();
        if (ImGui::Selectable("Helicopter")) currentGameObject_->createComponent<Helicopter>();

        ImGui::EndPopup();
    }
}
