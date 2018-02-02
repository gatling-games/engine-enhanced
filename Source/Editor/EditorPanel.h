#pragma once

#include <string>

class EditorPanel
{
public:
    virtual std::string name() const = 0;
    
    // Called when the actual panel is drawn
    virtual void draw() = 0;
};
