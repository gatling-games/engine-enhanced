#pragma once

#include <string>

class EditorPanel
{
public:
    virtual std::string name() const = 0;
    
    // Called when drawing a window main menu with the specified name
    virtual void drawMenu(const std::string menuName) { }

    // Called when the actual panel is drawn
    virtual void draw() = 0;
};
