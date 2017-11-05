#pragma once

#include "Editor/EditorPanel.h"

class ResourcesPanel : public EditorPanel
{
public:
    // EditorPanel overrides
    virtual std::string name() const { return "Resources Panel"; }
    virtual void draw();

};