#ifndef TEST_PARSE_HELPER_HPP
#define TEST_PARSE_HELPER_HPP


#include <map>
#include <sstream>
#include "Testable.hpp"

#include "Entity.hpp"
#include "Attribute.hpp"
#include "Tokenizer.hpp"
#include "ParseHelper.hpp"

namespace ECE141{

    class TestParseHelper : public Testable {
        public:
        using TestParseHelperFunc = bool (TestParseHelper::*)();

        std::map<std::string, TestParseHelperFunc> testFunctions {
            {"testParseAttributeCase1", &TestParseHelper::testParseAttributeCase1}
        };

        bool testParseAttributeCase1(){
            std::stringstream buffer;
            buffer << "name varchar(50) not null";

            Tokenizer tokenizer(buffer);
            tokenizer.tokenize();

            Attribute a1;
            
            ParseHelper parseHelper(tokenizer);
            parseHelper.parseAttribute(a1);

            bool success = true;

            success &= a1.getName() == "name";
            success &= a1.getSize() == 50;
            success &= a1.getType() == DataTypes::varchar_type;
            success &= a1.isAutoIncrement() == false;
            success &= a1.isNullable() == false;
            success &= a1.isPrimaryKey() == false;
            
            return success;
        }

        

        // testable infrastructure
        TestParseHelper(){
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
