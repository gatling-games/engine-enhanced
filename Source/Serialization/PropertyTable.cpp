#include "PropertyTable.h"

#include <iostream>

PropertyTable::PropertyTable()
    : mode_(PropertyTableMode::Writing),
    properties_()
{

}

PropertyTable::PropertyTable(const std::string &serializedData)
    : PropertyTable(std::stringstream(serializedData), INT_MAX)
{

}
    
PropertyTable::PropertyTable(std::stringstream &serializedData, int propertyCount)
    : mode_(PropertyTableMode::Reading),
    properties_()
{
    // Read the properties data into the table.
    // Ensure that no more than propertyCount properties are read
    while (serializedData && propertyCount > 0)
    {
        // Each line starts with the property name
        SerializedProperty property;
        serializedData >> property.name;

        // Skip the space after the name
        serializedData.get();

        // The rest of the name is the value
        std::getline(serializedData, property.value);
        if(property.value[property.value.length() - 1] == '\r')
        {
            // Prevent getline from including line endins (\r).
            property.value = property.value.substr(0, property.value.length() - 1);
        }

        // Place each property into the properties list
        if (!serializedData.fail())
            properties_.push_back(property);

        propertyCount--;
    }
}

const std::string PropertyTable::getProperty(const std::string &name, const std::string &default) const
{
    // Look for a matching property
    const SerializedProperty* property = tryFindProperty(name);
    if (property != nullptr)
    {
        return property->value;
    }

    // No property exists. Return the default value.
    return default;
}

void PropertyTable::serialize(const std::string &name, std::string &value, const std::string default)
{
    if (mode_ == PropertyTableMode::Reading)
    {
        // Set the value to the named property, or the default.
        value = getProperty(name, default);
    }
    else
    {
        // Store the property, unless it is the default value.
        if (value == default)
            tryDeleteProperty(name);
        else
            findOrCreateProperty(name)->value = value;
    }
}

const std::string PropertyTable::toString() const
{
    std::stringstream stream;

    // Write each property to the stream in the order they were created.
    // The property list only contains non-default values, so default
    // values will not be written, as expected.
    for (unsigned int i = 0; i < properties_.size(); ++i)
    {
        stream << properties_[i].name << " " << properties_[i].value << std::endl;
    }

    return stream.str();
}

SerializedProperty* PropertyTable::findOrCreateProperty(const std::string &name)
{
    // Look for a property with the given name
    for (unsigned int i = 0; i < properties_.size(); ++i)
    {
        if (properties_[i].name == name)
        {
            return &properties_[i];
        }
    }

    // No property exists. Create a new one.
    SerializedProperty newProperty;
    newProperty.name = name;
    newProperty.value = "";

    // Add the new property to the list and return it.
    properties_.push_back(newProperty);
    return &properties_.back();
}

const SerializedProperty* PropertyTable::tryFindProperty(const std::string &name) const
{
    // Look for a property with the given name
    for (unsigned int i = 0; i < properties_.size(); ++i)
    {
        if (properties_[i].name == name)
        {
            return &properties_[i];
        }
    }

    // None exists. Return nullptr.
    return nullptr;
}

void PropertyTable::tryDeleteProperty(const std::string &name)
{
    // Look for a property with the given name
    for (unsigned int i = 0; i < properties_.size(); ++i)
    {
        if (properties_[i].name == name)
        {
            // Remove the element by moving it to the back of the
            // properties list and popping the back element.
            std::swap(properties_[i], properties_.back());
            properties_.pop_back();
        }
    }
}