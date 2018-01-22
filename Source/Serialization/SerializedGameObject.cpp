#include "SerializedGameObject.h"

#include <assert.h>
#include <string>
#include <sstream>

#include "SceneManager.h"

SerializedGameObject::SerializedGameObject(const GameObject* gameObject)
    : name_(gameObject->name()),
    components_()
{
    // Read each component on the gameobject into the components list
    auto components = gameObject->componentList();
    for (unsigned int i = 0; i < components.size(); ++i)
    {
        ComponentInfo ci;
        ci.id = std::to_string(i);
        ci.type = components[i]->name();
        ci.properties = PropertyTable();
        components[i]->serialize(ci.properties);

        components_.push_back(ci);
    }
}

SerializedGameObject::SerializedGameObject(const std::string &serializedData)
{
    // Put the string into a StringStream so we can read it bit by bit.
    std::stringstream stream(serializedData);

    // The full data format is described at 
    // https://gitlab.com/gatling-games/engine/wikis/Technical%20Documentation/Data-Serialization

    // The start of the data should be a header of "SerializedGameObject"
    // Check that is is correct.
    std::string headerMagicValue;
    stream >> headerMagicValue;
    assert(headerMagicValue == "SerializedGameObject");

    // Skip the space after the magic value
    stream.get();

    // The rest of the first line is the gameobject name.
    std::getline(stream, name_);

    // After this, the file contains a series of AddComponent and PropertyTable commands.
    // Parse the entire file, dealing with the commands as we see them.
    while (stream)
    {
        // Read the next command.
        std::string command;
        stream >> command;

        // AddComponent commands add a new component to the gameobject.
        if (command == "AddComponent")
        {
            // There is now a component type name and an id
            ComponentInfo info;
            stream >> info.type >> info.id;

            // Store in the list of components. Its properties will be listed later.
            components_.push_back(info);
        }
        // PropertyTable commands list the properties for a given component
        else if (command == "PropertyTable")
        {
            // After PropertyTable, the command states the component id that
            // the properties are for and the number of properties.
            std::string componentID;
            int propertyCount;
            stream >> componentID >> propertyCount;

            // Read the next section in using a PropertyTable instance
            PropertyTable properties(stream, propertyCount);

            // Find the correct component in the list of components and store
            // the property table.
            for (unsigned int i = 0; i < components_.size(); ++i)
            {
                if (components_[i].id == componentID)
                {
                    components_[i].properties = properties;
                }
            }
        }
    }
}

GameObject* SerializedGameObject::clone() const
{
    // Create the actual gameobject
    GameObject* gameObject = SceneManager::instance()->createGameObject(name_);

    // Attach each component to it in turn
    for (auto componentInfo : components_)
    {
        // Create a component of the specified type
        Component* newComponent = gameObject->createComponent(componentInfo.type);

        // Deserialize the component's properties from the property table.
        PropertyTable& properties = componentInfo.properties;
        newComponent->serialize(properties);
    }

    return gameObject;
}

const std::string SerializedGameObject::toString() const
{
    std::stringstream stream;

    // First write the header:
    // - "SerializedGameObject" magic value
    // - GameObject name
    stream << "SerializedGameObject " << name_ << std::endl;

    // Next write the list of components
    // There is one "AddComponent <type> <id>" line per component
    for (auto componentInfo : components_)
    {
        stream << "AddComponent " << componentInfo.type << " " << componentInfo.id << std::endl;
    }

    // The rest of the stream contains the serialized PropertyTable for each component
    // Each one starts with a "PropertyTable <componentid> <propertycount>" header
    for (auto componentInfo : components_)
    {
        stream << "PropertyTable " << componentInfo.id << " " << componentInfo.properties.propertiesCount() << std::endl;
        stream << componentInfo.properties.toString();
    }

    return stream.str();
}