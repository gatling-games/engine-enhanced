#pragma once

#include <vector>

#include "Editor/EditorPanel.h"
#include "Application.h"

class DebugPanel : public EditorPanel
{
public:
    DebugPanel();

    // EditorPanel overrides
    virtual std::string name() const { return "Debug Panel"; }
    virtual void draw();

    // Adds a module's debug menu to the panel.
    void addModule(ApplicationModule* module);

private:
    std::vector<ApplicationModule*> modules_;
};