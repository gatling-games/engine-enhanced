#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <memory.h>

// Needed for serialize<ResourcePtr> method
#include "ResourceManager.h"

class ISerializedObject;

// Stores a collection of named properties for a serialized object.

// Properties can be written into a property table and read out again,
// allowing objects to be persisted to disk.

// Each property in a PropertyTable is named, allowing properties within
// a serialized object to be moved or deleted without breaking serialization.

// When loading a property, a default value is specified. This allows
// new properties to be loaded seamlessly from the property table if they
// were not present when the table was created.

enum class PropertyTableMode
{
    Reading,
    Writing,
};

class PropertyTable;

struct SerializedProperty
{
    std::string name;

    // If a single value, it is stored here.
    std::string value;

    // If the property is another property table it is here.
    std::shared_ptr<PropertyTable> subTable;
};

class PropertyTable
{
public:
    PropertyTable(PropertyTableMode mode);
    ~PropertyTable();

    // Information about the table
    PropertyTableMode mode() const { return mode_; }
    int propertiesCount() const { return (int)properties_.size(); }
    bool isEmpty() const { return properties_.empty(); }

    // Changes the mode of the property table.
    void setMode(PropertyTableMode newMode);

    // Gets the names of properties in the table.
    // Note - this will not contain values equal to their defaults.
    std::vector<std::string> propertyNames() const;

    // Writes already-serialized data into the property table, ready for reading.
    // Returns true if the data was read successfully.
    bool addPropertyData(const std::string &serializedData);
    bool addPropertyData(std::stringstream &serializedData);

    // Writes property data into the property table
    void addPropertyData(const PropertyTable &existingTable, bool overwriteExistingValues);

    // Removes any property values that are also contained in the propertiesToRemove list.
    // This is the opposite of addPropertyData().
    void deltaCompress(const PropertyTable &propertiesToRemove);

    // Looks for the named property in the table.
    // If it exists, the value is returned, converted to a string.
    // If it does not exist, the default value is returned.
    const std::string getProperty(const std::string &name, const std::string &default) const;

    // Serializes an entire subobject to or from the property table, depending on the current mode.
    void serialize(const std::string &name, ISerializedObject &subobject);

    // Serializes a vector of serializable objects to or from the property table data.
    // The vector must contain pointers to ISerializedObject objects
    template<typename T>
    void serialize(const std::string &name, std::vector<std::shared_ptr<T>>& values)
    {
        if (mode_ == PropertyTableMode::Reading)
        {
            // A vector is serialized by storing multiple properties with the same name.
            // Search the entire property list for matching ones.
            int totalFound = 0;
            for (const SerializedProperty& property : properties_)
            {
                // Skip properties that do not match the specified name
                if (property.name != name)
                {
                    continue;
                }

                // If the list is too short, expand it and create a new element.
                if (values.size() <= totalFound)
                {
                    values.push_back(std::make_shared<T>());
                }
                // If the list is big enough, but the value is null, create a new element
                else if(values[totalFound] == nullptr)
                {
                    values[totalFound] = std::make_shared<T>();
                }

                // Deserialize the property into the object
                values[totalFound]->serialize(*property.subTable);
                totalFound++;
            }

            // If there is left over space at the end, shrink the vector.
            if(totalFound < values.size())
            {
                values.resize(totalFound);
            }
        }
        else
        {
            // Write every value in the vector into a separate property with the same name.
            for(std::shared_ptr<T>& value : values)
            {
                if(value != nullptr)
                {
                    SerializedProperty newProperty;
                    newProperty.name = name;
                    newProperty.value = "";
                    newProperty.subTable = std::make_shared<PropertyTable>(PropertyTableMode::Writing);
                    value->serialize(*newProperty.subTable);
                    properties_.push_back(newProperty);
                }
            }
        }
    }

    // Serializes a property to or from the property table, depending on the current mode.
    // In writing mode, the value is saved as a property with the given name. If
    //          it matches the default value, the property is skipped.
    // In reading mode, the value is set to the value of the property with the given
    //          name. If no matching property can be found, the value is set to the
    //          provided default.
    // This method is templated and works with any data type that can support << and >> with
    // a stringstream instance. Additional data types are supported with overloaded methods 
    // below this one.
    template<typename T>
    void serialize(const std::string &name, T &value, const T default)
    {
        if (mode_ == PropertyTableMode::Reading)
        {
            // Look for the named property.
            const SerializedProperty* property = tryFindProperty(name);
            if (property == nullptr)
            {
                // The property wasnt found. Use the default.
                value = default;
                return;
            }

            // Property exists.
            // Place into a stringStream and extract the value
            std::stringstream stream(property->value);
            stream >> value;
        }
        else
        {
            // Default values are not stored in the property table
            if (value == default)
            {
                tryDeleteProperty(name);
                return;
            }

            // Load the value into a stringstream and use the contents as the property value.
            std::stringstream stream;
            stream << value;
            findOrCreateProperty(name)->value = stream.str();
        }
    }

    // Method for serializing a string value
    // This doesn't work with the default sstream implementation (above), as it breaks down
    // on strings that contain whitespace.
    void serialize(const std::string &name, std::string &value, const std::string default);

    // Method for serializing a resource ptr value.
    // This writes the source path of the resource to the property value.
    template<typename T>
    void serialize(const std::string &name, ResourcePPtr<T> &value, const ResourcePPtr<T> default)
    {
        if (mode_ == PropertyTableMode::Reading)
        {
            // Look for the named property
            const SerializedProperty* property = tryFindProperty(name);
            if (property == nullptr)
            {
                // The property wasnt found. Use the default.
                value = default;
                return;
            }

            // The property exists. It will be "0" if the resource is nullptr and
            // it will be the source path otherwise.
            if (property->value == "0")
            {
                value = nullptr;
            }
            else
            {
                const std::string sourcePath = property->value;
                value = ResourceManager::instance()->load<T>(sourcePath);
            }
        }
        else
        {
            // Default values are not stored in the property table
            if (value == default)
            {
                tryDeleteProperty(name);
                return;
            }

            // If the resource is nullptr use 0 for the value. Otherwise, use the source path.
            findOrCreateProperty(name)->value = (value == nullptr) ? "0" : value->resourcePath();
        }
    }

    // Converts the properties inside the table to the string-based property list format.
    std::string toString(int indentLevel = 1) const;

private:
    PropertyTableMode mode_;
    std::vector<SerializedProperty> properties_;

    // Looks for a property entry with the given name.
    // Creates a new entry if none exists.
    SerializedProperty* findOrCreateProperty(const std::string &name);

    // Looks for a property entry with the given name.
    // Returns nullptr if it does not exist.
    const SerializedProperty* tryFindProperty(const std::string &name) const;

    // Looks for a property with the given name and deletes it.
    void tryDeleteProperty(const std::string &name);
};