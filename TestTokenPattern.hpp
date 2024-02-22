#ifndef TEST_TOKEN_PATTERN_HPP
#define TEST_TOKEN_PATTERN_HPP


#include <map>
#include <sstream>
#include "Testable.hpp"

#include "Entity.hpp"
#include "Attribute.hpp"
#include "Tokenizer.hpp"
#include "TokenPattern.hpp"

namespace ECE141{

    class TestTokenPattern : public Testable {
        public:
        using TestTestTokenPatternFunc = bool (TestTokenPattern::*)();

        std::map<std::string, TestTestTokenPatternFunc> testFunctions {
            {"testIsValidAttribute", &TestTokenPattern::testIsValidAttribute},
            {"testEqualsConstraint", &TestTokenPattern::testEqualsConstraint},
            {"testEqualsDataType", &TestTokenPattern::testEqualsDataType}
        };

        bool testIsValidAttribute(){
            std::vector<Attribute> theAttrs;
            std::stringstream buffer;
            buffer << "id int boolean,";
            buffer.seekg(0);
            auto theTokenizer = Tokenizer(buffer);
            theTokenizer.tokenize();
                        
            TokenPattern thePattern(theTokenizer);
            return thePattern.begins().isValidAttribute(theAttrs);
        }

        bool testEqualsConstraint(){
            std::stringstream buffer;
            buffer << "Primary Key Auto_increment Not Null";
            buffer.seekg(0);
            
            auto theTokenizer = Tokenizer(buffer);
            theTokenizer.tokenize();
            
            std::vector<Token> tokens;
            
            TokenPattern thePattern(theTokenizer);
            
            if(thePattern.begins().equalsConstraint()){
                return true;
            }
            else{return false;}
            
        }

        Tokenizer& prepareToken(std::string string) {
            
            std::stringstream buffer;
            buffer << string;
            buffer.seekg(0);

            auto theTokenizer = new Tokenizer(buffer);
            theTokenizer->tokenize();

            return *theTokenizer;
        }
        
        bool testEqualsDataType(){
            
            
            TokenPattern varchar1(prepareToken("varchar(100)"));
            TokenPattern varchar2(prepareToken("varchar(100;"));
            TokenPattern varchar3(prepareToken("varchar(-123)"));
            TokenPattern varchar4(prepareToken("varchar"));

            TokenPattern bool1(prepareToken("boolean"));
            TokenPattern int1(prepareToken("int"));
            TokenPattern float1(prepareToken("float"));
            TokenPattern datetime1(prepareToken("datetime"));
            
            DataTypes type;
            int32_t length;

            bool success = true;

            if(varchar1.begins().containsDataType(type, length)){
                success &= type==DataTypes::varchar_type && length == 100;
            }
            else {
                success = false;
            }

            success &= !varchar2.begins().containsDataType(type, length);
            success &= !varchar3.begins().containsDataType(type, length);
            success &= !varchar4.begins().containsDataType(type, length);

            success &= bool1.begins().containsDataType(type, length);
            success &= type == DataTypes::bool_type;

            success &= int1.begins().containsDataType(type, length);
            success &= type == DataTypes::int_type;

            success &= float1.begins().containsDataType(type, length);
            success &= type == DataTypes::float_type;

            success &= datetime1.begins().containsDataType(type, length);
            success &= type == DataTypes::datetime_type;

            return success;
        }

        // testable infrastructure
        TestTokenPattern(){
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
