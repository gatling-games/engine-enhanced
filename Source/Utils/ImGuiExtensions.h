#pragma once

#include <imgui.h>

#include "ResourceManager.h"
#include "Editor/PropertiesPanel.h"

namespace ImGui
{
    // Makes a large imgui button, filling its entire row
    // This is used for important buttons eg. add component, remove component etc.
    bool BigButton(const std::string &label);
    bool BigButton(const char* label);

    // Shows a modal window when for changing the specified resouce.
    // Returns true if the resource was changed.
    template<typename T>
    bool ResourceSelectModal(const char* modalName, T* &resource)
    {
        bool changed = false;

        if (ImGui::BeginPopupModal(modalName))
        {
            // Draw a button for each resource of the correct type
            for (T* other : ResourceManager::instance()->loadedResourcesOfType<T>())
            {
                if (ImGui::Selectable(other->resourcePath().c_str(), resource == other))
                {
                    resource = other;
                    ImGui::CloseCurrentPopup();
                    changed = true;
                }
            }

            // Display a cancel button at the bottom
            if (ImGui::Button("Cancel", ImVec2(120, 0)))
            {
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
        
        return changed;
    }

    // Shows a resource select field, which opens a modal window when clicked.
    // Returns true if the resource was changed.
    template<typename T>
    bool ResourceSelect(const char* label, const char* modalTitle, T* &resource)
    {
        // Push an extra ID so that multiple ResourceSelect()s work.
        ImGui::PushID(label);

        // Show the resource name in a read-only text box
        const std::string resourceName = (resource == nullptr) ? "none" : resource->resourceName();
        ImGui::InputText("", (char*)resourceName.c_str(), resourceName.length(), ImGuiInputTextFlags_ReadOnly);

        // If the input text was selected, show the resource
        if (resource != nullptr && ImGui::IsItemClicked())
        {
            IEditableObject* ieo = dynamic_cast<IEditableObject*>(resource);
            if (ieo != nullptr)
            {
                PropertiesPanel::instance()->inspect(ieo);
                ImGui::PopID();
                return false;
            }
        }

        // Then draw a button that opens the file selection modal
        ImGui::SameLine();
        if (ImGui::Button("..."))
        {
            ImGui::OpenPopup(modalTitle);
        }

        // Then draw the control label
        ImGui::SameLine();
        ImGui::Text(label);

        // Finally draw the file selection modal (when open).
        bool changed = ResourceSelectModal<T>(modalTitle, resource);

        // We are done with the ID.
        ImGui::PopID();

        return changed;
    }
}
