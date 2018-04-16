#pragma once

#include "Scene/GameObject.h"
#include "Serialization/SerializedObject.h"
#include "Serialization/PropertyTable.h"

// The base class for every component that is attached to a GameObject.
// It contains a series of callbacks that are triggered
class Component : public ISerializedObject
{
public:
    explicit Component(GameObject* gameObject);
    virtual ~Component();

    // The name of the component type.
    const std::string name() const;

    // The GameObject that the component is attached to
    GameObject* gameObject() const { return gameObject_; }

    // True if the update() method is currently called each frame
    bool updateEnabled() const { return updateEnabled_; }
    void setUpdateEnabled(bool enabled) { updateEnabled_ = enabled; }

    // Called each frame.
	virtual void update(float deltaTime);

    // Used to draw the imgui properties section
	virtual void drawProperties();

    // Serialization and deserialization of the component.
    // Triggered when loading, saving, or sending over the network.
	virtual void serialize(PropertyTable &table) override;

private:
    GameObject* gameObject_;
    bool updateEnabled_;
};