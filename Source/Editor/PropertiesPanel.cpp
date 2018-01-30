#include "PropertiesPanel.h"

PropertiesPanel::PropertiesPanel()
    : currentObject_(nullptr)
{

}

void PropertiesPanel::draw()
{
    if(currentObject_ != nullptr)
    {
        currentObject_->drawEditor();
    }
}

void PropertiesPanel::inspect(IEditableObject* object)
{
    currentObject_ = object;
}
