#pragma once

// An object that can be displayed in the UI and edited.
class IEditableObject
{
public:
    virtual void drawEditor() = 0;

    // Called when the object is double-clicked in the editor
    virtual void onOpenAction() { }
};
