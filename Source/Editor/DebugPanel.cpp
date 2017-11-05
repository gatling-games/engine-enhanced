#include "DebugPanel.h"

#include <imgui.h>

DebugPanel::DebugPanel()
    : modules_()
{

}

void DebugPanel::draw()
{
    // Draw the menu for each module
    for (unsigned int i = 0; i < modules_.size(); ++i)
    {
        if (ImGui::CollapsingHeader(modules_[i]->name().c_str()))
        {
            modules_[i]->drawDebugMenu();
        }
    }
}

void DebugPanel::addModule(ApplicationModule* module)
{
    modules_.push_back(module);
}