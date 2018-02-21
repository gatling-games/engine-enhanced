#pragma once

#include "Scene/GameObject.h"

#include "Serialization/SerializedObject.h"

#include "ResourceManager.h"

class Prefab : public Resource, public IEditableObject, public ISerializedObject
{
public:
    Prefab(ResourceID resourceID);

    // Gets the serialized prefab property table.
    // This method deliberately returns *A COPY* of the table, as it is modified
    // by per-instance properties from the instantiated gameobject
    PropertyTable serializedProperties() const { return properties_; };

    // Implements a custom editor
    void drawEditor() override;

    // Reads or writes the prefab properties.
    void serialize(PropertyTable &table) override;

    // Causes the prefab to copy the specified gameobject.
    // The existing prefab data will be completely overwritten.
    void cloneGameObject(GameObject* original);

private:
    // Keep the prefab in a serialized form until instantiated.
    PropertyTable properties_;
};