#pragma once

#include "Scene/GameObject.h"
#include "Scene/Component.h"

#include "Serialization/PropertyTable.h"

class SerializedGameObject
{
private:
    struct ComponentInfo
    {
        std::string type;
        std::string id;
        PropertyTable properties;

        ComponentInfo(PropertyTableMode mode)
            : properties(mode)
        {
            
        }
    };

public:
    SerializedGameObject(const GameObject* gameObject);
    SerializedGameObject(const std::string &serializedData);

    // The name of the game object
    const std::string name() const { return name_; }

    // Creates a gameobject based on the serialized data
    GameObject* clone() const;

    // Converts the game object to a string
    const std::string toString() const;

private:
    std::string name_;
    std::vector<ComponentInfo> components_;
};