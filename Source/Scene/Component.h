#pragma once

#include "Scene/GameObject.h"

// The base class for every component that is attached to a GameObject.
// It contains a series of callbacks that are triggered
class Component
{
public:
    explicit Component(GameObject* gameObject);
    virtual ~Component();

    // The GameObject that the component is attached to
    GameObject* gameObject() const { return gameObject_; }

    // Called each frame.
    virtual void update(float deltaTime) { }

    // Serialization and deserialization of the component.
    // Triggered when loading, saving, or sending over the network.
    virtual void serialize(BitWriter &writer) { }
    virtual void deserialize(BitReader &reader) { }

private:
    GameObject* gameObject_;
};