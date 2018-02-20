#include "PropertyTable.h"

#include <iostream>

#include "SerializedObject.h"

PropertyTable::PropertyTable(PropertyTableMode mode)
    : mode_(mode)
{

}

PropertyTable::~PropertyTable()
{

}

void PropertyTable::clear()
{
    properties_.clear();
}

void PropertyTable::setMode(PropertyTableMode newMode)
{
    mode_ = newMode;

    // Recursively set the mode on subtables
    for(SerializedProperty& prop : properties_)
    {
        if(prop.subTable != nullptr)
        {
            prop.subTable->setMode(newMode);
        }
    }
}

std::vector<std::string> PropertyTable::propertyNames() const
{
    std::vector<std::string> names;
    for (const SerializedProperty& property : properties_)
    {
        names.push_back(property.name);
    }

    return names;
}

bool PropertyTable::addPropertyData(const std::string& serializedData)
{
    assert(mode_ == PropertyTableMode::Reading);
    assert(properties_.empty());

    std::stringstream stream(serializedData);
    return addPropertyData(stream);
}

bool PropertyTable::addPropertyData(std::stringstream& serializedData)
{
    assert(mode_ == PropertyTableMode::Reading);
    assert(properties_.empty());

    // The propertytable must begin with a {.
    if (serializedData.get() != '{')
    {
        std::cerr << "ERROR - Property stream: Stream started without a { " << std::endl;
        return false;
    }

    while (serializedData)
    {
        // Each line starts with the property name
        SerializedProperty property;
        serializedData >> property.name;

        // If a } was encountered, instead of a property name, we are done.
        if (property.name.compare("}") != std::string::npos)
        {
            // Leave the } for the parent propertytable to check
            serializedData.unget();
            return true;
        }

        // Skip the space after the name
        serializedData.get();

        // The next symbol is either = or {
        std::string propertyType;
        serializedData >> propertyType;

        // If it is an =, the line contains a single value.
        if (propertyType == "=")
        {
            // Skip the space before the value
            serializedData.get();

            // The rest of the name is the value
            std::getline(serializedData, property.value);
            if (!property.value.empty() && property.value[property.value.length() - 1] == '\r')
            {
                // Prevent getline from including line endings (\r).
                property.value = property.value.substr(0, property.value.length() - 1);
            }
        }
        // Otherwise, the { indicates the value is itsself a property table.
        else if (propertyType == "{")
        {
            // Leave the { in the stream for the child propertytable to check.
            serializedData.unget();

            // Create and read the property table.
            // This wil advance the string stream to after the subtable's data.
            property.subTable.reset(new PropertyTable(PropertyTableMode::Reading));
            if (!property.subTable->addPropertyData(serializedData))
            {
                std::cerr << "ERROR - Property stream: Failed to read subtable. " << std::endl;
                return false;
            }

            // The subtable should be followed by a }.
            std::string nextValue;
            serializedData >> nextValue;
            if (nextValue != "}")
            {
                std::cerr << "ERROR - Property stream: Read " << nextValue << " when } expected. " << std::endl;
                return false;
            }
        }
        // If anythign else is found, it is an error.
        else
        {
            std::cerr << "ERROR - Property stream: Read " << propertyType << " when = or { expected. " << std::endl;
            return false;
        }

        // Place the property into the properties list
        if (!serializedData.fail())
        {
            properties_.push_back(property);
        }
    }

    // Upon encountering a }, the loop above returns true.
    // If we reach here, the stream is finished, but we never found a }.
    std::cerr << "ERROR - Property stream: Reached end of stream without }. " << std::endl;
    return false;
}

void PropertyTable::addPropertyData(const PropertyTable& existingTable, bool overwriteExistingValues)
{
    assert(mode_ == PropertyTableMode::Reading);

    for(const SerializedProperty& property : existingTable.properties_)
    {
        // We need to handle single-value and subtable properties separately.

        // If this is a subtable, perform this action recursively.
        if(property.subTable != nullptr)
        {
            // Check a subtable exists
            SerializedProperty* prop = findOrCreateProperty(property.name);
            if(prop->subTable == nullptr)
            {
                prop->subTable.reset(new PropertyTable(PropertyTableMode::Reading));
            }

            // Addpropertydata to the subtable
            prop->subTable->addPropertyData(*property.subTable, overwriteExistingValues);
        }
        // Otherwise, add/overwrite a single value
        else if(tryFindProperty(property.name) == nullptr || overwriteExistingValues)
        {
            findOrCreateProperty(property.name)->value = property.value;
        }
    }
}

void PropertyTable::deltaCompress(const PropertyTable& propertiesToRemove)
{
    assert(mode_ == PropertyTableMode::Writing);

    for(unsigned int i = 0; i < properties_.size(); ++i)
    {
        SerializedProperty& property = properties_[i];
        
        // First check if the property is in the list of ones to remove
        const SerializedProperty* removeListProp = propertiesToRemove.tryFindProperty(property.name);
        if(removeListProp == nullptr)
        {
            continue;
        }

        // If the property is a subtable, delta compress the subtable
        if(property.subTable != nullptr && removeListProp->subTable != nullptr)
        {
            property.subTable->deltaCompress(*removeListProp->subTable);

            // The property needs to stay in place if the subtable still contains data
            if(property.subTable->properties_.empty() == false)
            {
                continue;
            }
        }

        // The property needs to stay in place if the values dont match
        if(property.value != removeListProp->value)
        {
            continue;
        }

        // Otherwise, the property is redundant and can be removed.
        properties_.erase(properties_.begin() + i);
        --i;
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

void PropertyTable::serialize(const std::string& name, ISerializedObject& subobject)
{
    if (mode_ == PropertyTableMode::Reading)
    {
        // Look for a property table with the correct name.
        const SerializedProperty* property = tryFindProperty(name);
        if (property != nullptr && property->subTable.get() != nullptr)
        {
            property->subTable->setMode(PropertyTableMode::Reading);
            subobject.serialize(*property->subTable);
        }
        else
        {
            // No property table found.
            // Make an empty pt and the subobject will use defaults for everything
            PropertyTable emptyPT(PropertyTableMode::Reading);
            subobject.serialize(emptyPT);
        }
    }
    else
    {
        // Create a new property table and write to it.
        std::shared_ptr<PropertyTable> subtable(new PropertyTable(PropertyTableMode::Writing));
        subobject.serialize(*subtable);

        // Save the property table with the correct name
        findOrCreateProperty(name)->subTable.swap(subtable);
    }
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

std::string PropertyTable::toString(int indentLevel) const
{
    std::stringstream stream;

    // The stream must begin with a {.
    stream << "{";

    // Each line in the table is indented by the specified amount
    std::string indent = "";
    for (int i = 0; i < indentLevel; ++i)
    {
        indent += "    "; // 4 spaces 
    }

    // Write each property to the stream in the order they were created.
    // The property list only contains non-default values, so default
    // values will not be written, as expected.
    for (const SerializedProperty& property : properties_)
    {
        // Each property has its own line, starting with the name
        stream << "\n" << indent << property.name << " ";

        // Each property is either a single value, or another property table.
        if (property.subTable.get() != nullptr)
        {
            // Serialize the entire subtable
            stream << property.subTable->toString(indentLevel + 1);
        }
        else
        {
            // Write an =, and then the value
            stream << "= " << property.value;
        }
    }

    // The stream ends with a }.
    // Indent it by indentLevel - 1 levels
    stream << "\n";
    for (int i = 0; i < indentLevel - 1; ++i) stream << "    "; // 4 spaces
    stream << "}";

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