#pragma once

#include <string>

class EditorPanel
{
public:
    virtual std::string name() const = 0;
    
    // Panel callbacks
    // Triggered at certain stages of the editor ui rendering
    virtual void draw() = 0;
};
