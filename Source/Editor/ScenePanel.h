#pragma once

#include "Editor/EditorPanel.h"

class ScenePanel : public EditorPanel
{
public:
    // EditorPanel overrides
    virtual std::string name() const { return "Scene Panel"; }
    virtual void draw();

};