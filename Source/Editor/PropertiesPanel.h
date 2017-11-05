#pragma once

#include "Editor/EditorPanel.h"

class PropertiesPanel : public EditorPanel
{
public:
    // EditorPanel overrides
    virtual std::string name() const { return "Properties Panel"; }
    virtual void draw();

};