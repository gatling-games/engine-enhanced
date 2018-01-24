#pragma once

#include <imgui.h>

#include "ResourceManager.h"

namespace ImGui
{
    template<typename T>
    void ResourceSelectModal(const char* modalName, T* &resource)
    {
        // Get the source path of the currently selected resource
        const std::string resourcePath = (resource == nullptr) ? "none" : resource->resourcePath();

        if (ImGui::BeginPopupModal(modalName))
        {
            // Draw a button for each source file in the resource manager
            const std::vector<std::string>* sourceFiles = ResourceManager::instance()->allSourceFiles();
            for (unsigned int i = 0; i < sourceFiles->size(); ++i)
            {
                const std::string sourceFilePath = sourceFiles->at(i);
                if (ImGui::Selectable(sourceFilePath.c_str(), sourceFilePath == resourcePath))
                {
                    resource = ResourceManager::instance()->load<T>(sourceFilePath);
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
