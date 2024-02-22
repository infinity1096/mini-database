#ifndef TEST_ROW_HPP
#define TEST_ROW_HPP


#include <map>
#include <sstream>
#include "Testable.hpp"

#include "Row.hpp"

namespace ECE141{

    class TestRow : public Testable {
        public:
        using TestRowFunc = bool (TestRow::*)();

        std::map<std::string, TestRowFunc> testFunctions {
            {"testEncodeDecode", &TestRow::testRowEncodeDecode}
        };

        bool testRowEncodeDecode(){
            MyBasicNullType mynull;
            MyBasicIntType myInt(42);
            MyBasicFloatType myFloat(3.14);
            MyBasicDateTimeType myTime({2022,4,16,14,39,0});
            MyBasicBoolType myBool(false);
            MyBasicVarcharType myString("Test string");

            Row row;
            row.addContent(mynull);
            row.addContent(myInt);
            row.addContent(myFloat);
            row.addContent(myTime);
            row.addContent(myBool);
            row.addContent(myString);

            std::stringstream buffer;
            row.encode(buffer);

            std::cout << buffer.str();

            Row row2;
            buffer.seekg(0);
            row2.decode(buffer);

            auto varchar = row2[5];

            return true;
        }

        // testable infrastructure
        TestRow(){
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