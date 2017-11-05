#pragma once

#include <string>

class EditorPanel
{
public:
    virtual std::string name() const = 0;
    virtual void draw() = 0;
};