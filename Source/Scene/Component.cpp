#include "Component.h"

#include <imgui.h>

Component::Component(GameObject* gameObject)
    : gameObject_(gameObject),
    updateEnabled_(true)
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

void Component::update(float)
{
	// Do nothing
}

void Component::handleInput(const InputCmd&)
{
    // Do nothing
}

void Component::handleCollision(Collider*)
{
	// Do nothing
}

void Component::drawProperties()
{
	ImGui::Text("%s", "This component has no properties.");
}

void Component::serialize(PropertyTable&)
{
	// Do nothing
}
