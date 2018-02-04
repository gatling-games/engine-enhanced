#include "CppUnitTest.h"

#include "Serialization/PropertyTable.h"
#include "Serialization/BitWriter.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace EngineTests
{
    struct TestStruct
    {
        int intValue1;
        int intValue2;
        std::string stringValue;

        void serialize(PropertyTable &table)
        {
            table.serialize("IntValue1", intValue1, 578);
            table.serialize("IntValue2", intValue2, 578);
            table.serialize("StringValue", stringValue, "Hello World");
        }
    };

    TEST_CLASS(PropertyTableTests)
    {
    public:

        TEST_METHOD(BlankConstructor)
        {
            // Make a property table with no properties
            PropertyTable table(PropertyTableMode::Writing);

            // Check that no properties are loaded.
            Assert::IsTrue(PropertyTableMode::Writing == table.mode());
            Assert::AreEqual(0, table.propertiesCount());
            Assert::IsTrue(table.isEmpty());
        }

        TEST_METHOD(DataConstructor)
        {
            // Manually make a property table with 2 existing properties:
            // IntVal = 3
            // StringVal = Hello
            const std::string existingProperties = "{\nIntVal = 3\nStringVal = Hello\n}";
            PropertyTable table(PropertyTableMode::Reading);
            Assert::IsTrue(table.addPropertyData(existingProperties));

            // Check that the 2 properties are loaded
            Assert::IsTrue(PropertyTableMode::Reading == table.mode());
            Assert::AreEqual(2, table.propertiesCount());
            Assert::IsFalse(table.isEmpty());
        }

        TEST_METHOD(TestSerialize)
        {
            // Create a test struct
            // NOTE - Ensure that no values match the defaults
            TestStruct testStruct;
            testStruct.intValue1 = 109123126;
            testStruct.intValue2 = -231238;
            testStruct.stringValue = "A long serialized string";

            // Serialize it into a new property table
            PropertyTable table(PropertyTableMode::Writing);
            Assert::IsTrue(PropertyTableMode::Writing == table.mode());
            testStruct.serialize(table);

            // Extract the serialized data as a property list.
            const std::string serialized = table.toString();

            // Check the data is correct.
            const std::string expected = "{\nIntValue1 = 109123126\nIntValue2 = -231238\nStringValue = A long serialized string\n}";
            Assert::AreEqual(3, table.propertiesCount());
            Assert::AreEqual(expected, serialized);
        }

        TEST_METHOD(TestSerializeSkipsDefaults)
        {
            // Create a test struct, with some default values
            TestStruct testStruct;
            testStruct.intValue1 = 578; // default
            testStruct.intValue2 = -231238; // != default
            testStruct.stringValue = "Hello World"; // default

            // Serialize it into a new property table
            PropertyTable table(PropertyTableMode::Writing);
            Assert::IsTrue(PropertyTableMode::Writing == table.mode());
            testStruct.serialize(table);

            // Extract the serialized data as a property list.
            const std::string serialized = table.toString();

            // Check the data is correct.
            // The serialized data should only contain non-default values
            const std::string expected = "{\nIntValue2 = -231238\n}";
            Assert::AreEqual(1, table.propertiesCount());
            Assert::AreEqual(expected, serialized);
        }

        TEST_METHOD(TestDeserialize)
        {
            // Use test data with 3 properties
            const std::string existingProperties = "{IntValue1 = 109123126\nIntValue2 = -231238\nStringValue = A long serialized string\n}";

            // Create a property table with the values already in it
            PropertyTable table(PropertyTableMode::Reading);
            Assert::IsTrue(table.addPropertyData(existingProperties));
            Assert::IsTrue(PropertyTableMode::Reading == table.mode());
            Assert::AreEqual(3, table.propertiesCount());

            // Create a new test struct and deserialize from the table.
            TestStruct testStruct;
            testStruct.serialize(table);

            // Check that the test struct values match what was in the properties table
            Assert::AreEqual(109123126, testStruct.intValue1);
            Assert::AreEqual(-231238, testStruct.intValue2);
            Assert::AreEqual(std::string("A long serialized string"), testStruct.stringValue);
        }

        TEST_METHOD(TestDeserializeUsesDefaults)
        {
            // Use test data with 1 property
            const std::string existingProperties = "{IntValue2 = -231238\n}";

            // Create a property table with the values already in it
            // Skip some values, so they will use the default values.
            PropertyTable table(PropertyTableMode::Reading);
            Assert::IsTrue(table.addPropertyData(existingProperties));
            Assert::IsTrue(PropertyTableMode::Reading == table.mode());
            Assert::AreEqual(1, table.propertiesCount());

            // Create a new test struct and deserialize from the table.
            TestStruct testStruct;
            testStruct.serialize(table);

            // Check that the test struct values match what was in the properties table
            // or the defaults for values that are not in the table.
            Assert::AreEqual(578, testStruct.intValue1); // default
            Assert::AreEqual(-231238, testStruct.intValue2); // != default
            Assert::AreEqual(std::string("Hello World"), testStruct.stringValue); // default
        }
    };
}