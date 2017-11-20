#pragma once

#include <string>
#include "Math/Rect.h"

class EditorPanel
{
public:
    virtual std::string name() const = 0;
    virtual void draw() = 0;

    Rect size() const;
    void setSize(const Rect &rect);

private:
    Rect size_;
};
