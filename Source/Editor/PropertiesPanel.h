#pragma once

#include "Editor/EditableObject.h"
#include "Editor/EditorPanel.h"
#include "Utils/Singleton.h"

class PropertiesPanel : public EditorPanel, public Singleton<PropertiesPanel>
{
public:
    PropertiesPanel();

    // EditorPanel overrides
    virtual std::string name() const { return "Properties Panel"; }
    virtual void draw();

    // The object being inspected
    IEditableObject* current() const { return currentObject_; }

    // Changes the object being inspected
    void inspect(IEditableObject* object);

private:
    IEditableObject* currentObject_;
};
