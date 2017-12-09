#pragma once

#include <string>
#include <vector>

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

struct SerializedProperty
{
    std::string name;
    std::string value;
};

class PropertyTable
{
public:
    PropertyTable();
    PropertyTable(const std::string &serializedData);

    // Information about the table
    PropertyTableMode mode() const { return mode_; }
    int propertiesCount() const { return (int)properties_.size(); }
    bool isEmpty() const { return properties_.empty(); }

    // Looks for the named property in the table.
    // If it exists, the value is returned, converted to a string.
    // If it does not exist, the default value is returned.
    const std::string getProperty(const std::string &name, const std::string &default) const;

    // Methods for storing and loading properties.
    void serialize(const std::string &name, std::string &value, const std::string default);
    void serialize(const std::string &name, bool &value, const bool default);
    void serialize(const std::string &name, int &value, const int default);

    // Converts the properties inside the table to the string-based property list format.
    const std::string toString() const;

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