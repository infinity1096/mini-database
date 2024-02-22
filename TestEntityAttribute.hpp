#ifndef TEST_ENTITY_ATTRIBUTE_HPP
#define TEST_ENTITY_ATTRIBUTE_HPP


#include <map>
#include <sstream>
#include "Testable.hpp"

#include "Entity.hpp"
#include "Attribute.hpp"

namespace ECE141{

    class TestEntityAttribute : public Testable {
        public:
        using TestTestEntityAttributeFunc = bool (TestEntityAttribute::*)();

        std::map<std::string, TestTestEntityAttributeFunc> testFunctions {
            {"testAttributeEncoding", &TestEntityAttribute::testAttributeEncoding},
            {"testEntityEncoding", &TestEntityAttribute::testEntityEncoding}
        };

        bool testAttributeEncoding(){
            std::stringstream buffer;

            Attribute a1;
            a1.setName(std::string("Attribute1"));
            a1.setDataType(DataTypes::int_type);
            a1.setSize(0);
            a1.setNullable(1);
            a1.setPrimaryKey(1);
            a1.setAutoIncrement(1);

            a1.encode(buffer);

            buffer.seekg(0);

            Attribute a2;
            a2.decode(buffer);

            bool success = true;

            success &= a2.getName() == a1.getName();
            success &= a2.getType() == a1.getType();
            success &= a2.getSize() == a1.getSize();
            success &= a2.isAutoIncrement() == a1.isAutoIncrement();
            success &= a2.isNullable() == a1.isNullable();
            success &= a2.isPrimaryKey() == a1.isPrimaryKey();

            return success;
        }

        bool testEntityEncoding(){
            std::stringstream buffer;

            Attribute a1(std::string("Primary"), DataTypes::int_type);
            Attribute a2(std::string("birthday"), DataTypes::datetime_type);
            Attribute a3(std::string("height"), DataTypes::float_type);
            
            Entity e(std::string("Entity1"));
            e.addAttribute(a1);
            e.addAttribute(a2);
            e.addAttribute(a3);

            e.encode(buffer);

            Entity e2("empty");

            buffer.seekg(0);
            e2.decode(buffer); 

            bool success = true;
            success &= e2.getAttribute("Primary").value() == a1;
            success &= e2.getAttribute("birthday").value() == a2;
            success &= e2.getAttribute("height").value() == a3;

            success &= e2.getName() == e.getName();

            return success;
        }

        // testable infrastructure
        TestEntityAttribute(){
            count = getNumTests();
        }

        size_t getNumTests(){
            return testFunctions.size();
        }
    
        std::optional<std::string> getTestName(size_t index) const {
            size_t current_index = 0;

            for (auto kv : testFunctions){
                if (index == current_index++){
                    return kv.first;
                }
            }

            return std::nullopt;
        }

        bool operator() (const std::string& testName){
            if (testFunctions.count(testName) > 0){
                return (this->*testFunctions[testName])();
            }else{
                return false;
            }
        }
    };

}

#endif