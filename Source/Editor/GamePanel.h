#pragma once

#include "Editor/EditorPanel.h"

class GamePanel : public EditorPanel
{
public:
    // EditorPanel overrides
    virtual std::string name() const { return "Game Panel"; }
    virtual void draw();

};