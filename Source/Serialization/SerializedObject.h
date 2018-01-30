#pragma once

#include "PropertyTable.h"

class ISerializedObject
{
public:
    // Serialization and deserialization of the component.
    // Triggered when loading, saving, or sending over the network.
    virtual void serialize(PropertyTable &table) = 0;
};