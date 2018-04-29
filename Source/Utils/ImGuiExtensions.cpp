#include "ImGuiExtensions.h"

namespace ImGui
{
    bool BigButton(const std::string& label)
    {
        return BigButton(label.c_str());
    }

    bool BigButton(const char* label)
    {
        return ImGui::Button(label, ImVec2(ImGui::GetContentRegionAvailWidth(), 40.0f));
    }
}
