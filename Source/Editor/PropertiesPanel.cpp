#include "PropertiesPanel.h"

#include <imgui.h>

#include "Scene/Component.h"

PropertiesPanel::PropertiesPanel()
    : currentGameObject_(nullptr)
{
    
}

void PropertiesPanel::draw()
{
    // Exit if we are not looking at an object
    if(currentGameObject_ == nullptr)
    {
        return;
    }

    // Draw the GameObject header
    if (ImGui::CollapsingHeader("GameObject", 0, false, true))
    {
        ImGui::InputText("Name", (char*)currentGameObject_->name().c_str(), 32);
    }

    // Draw each component's section in turn
    auto components = currentGameObject_->componentList();
    for(unsigned int i = 0; i < components.size(); ++i)
    {
        // Place a space before the component
        ImGui::Spacing();

        // Get the component name from typeid
        // All of the names begin with "Class ", so skip the first 6 chars
        const char* componentName = typeid(*components[i]).name() + 6;

        // Put the component controls inside a drop down
        if (ImGui::CollapsingHeader(componentName, ImGuiTreeNodeFlags_DefaultOpen))
        {
            components[i]->drawProperties();
        }
    }
}

void PropertiesPanel::inspect(GameObject* gameObject)
{
    currentGameObject_ = gameObject;
}
