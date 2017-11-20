#pragma once

#include <string>
#include "Math/Rect.h"

class EditorPanel
{
public:
    virtual std::string name() const = 0;
    
    // Panel callbacks
    // Triggered at certain stages of the editor ui rendering
    virtual void draw() = 0;
    virtual void onResize(int width, int height) { }

    Rect size() const;
    void setSize(const Rect &rect);

private:
    Rect size_;
};
