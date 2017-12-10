#include "Component.h"

Component::Component(GameObject* gameObject)
    : gameObject_(gameObject)
{

}

Component::~Component()
{

}

const std::string Component::name() const
{
    // Get the component name from typeid
    const char* componentName = typeid(*this).name();

    // The name begins with "Class " on msvc
    // Skip the first 6 chars (5 in Class plus the space)
    return std::string(componentName + 6);
}