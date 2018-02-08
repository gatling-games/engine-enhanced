#pragma once

#include "Scene/GameObject.h"

#include "Serialization/SerializedObject.h"

#include "ResourceManager.h"

class Prefab : public Resource, public IEditableObject, public ISerializedObject
{
public:
    Prefab(ResourceID resourceID);

    // Implements a custom editor
    void drawEditor() override;

    // Reads or writes the prefab properties.
    void serialize(PropertyTable &table) override;

private:
    // The template gameobject.
    // This is not part of the actual scene, and will not be initialised.
    GameObject gameObject_;
};