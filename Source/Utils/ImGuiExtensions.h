#pragma once

#include <imgui.h>

#include "ResourceManager.h"

namespace ImGui
{
    template<typename T>
    void ResourceSelectModal(const char* modalName, T* &resource)
    {
        if (ImGui::BeginPopupModal(modalName))
        {
            // Draw a button for each resource of the correct type
            for(T* other : ResourceManager::instance()->loadedResourcesOfType<T>())
            {
                if (ImGui::Selectable(other->resourcePath().c_str(), resource == other))
                {
                    resource = other;
                    ImGui::CloseCurrentPopup();
                }
            }

            // Display a cancel button at the bottom
            if (ImGui::Button("Cancel", ImVec2(120, 0)))
            {
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
    }

    template<typename T>
    void ResourceSelect(const char* label, const char* modalTitle, T* &resource)
    {
        // Push an extra ID so that multiple ResourceSelect()s work.
        ImGui::PushID(label);

        // Show the resource name in a read-only text box
        const std::string resourceName = (resource == nullptr) ? "none" : resource->resourceName();
        ImGui::InputText("", (char*)resourceName.c_str(), resourceName.length(), ImGuiInputTextFlags_ReadOnly);

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
        ResourceSelectModal<T>(modalTitle, resource);

        // We are done with the ID.
        ImGui::PopID();
    }
}
