#pragma once

#include "Editor/EditorPanel.h"

class OutputPanel : public EditorPanel
{
public:
    // EditorPanel overrides
    virtual std::string name() const { return "Output Panel"; }
    virtual void draw();

};