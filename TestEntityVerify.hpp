//
//  TestEntityVerify.hpp
//  Assignment4
//
//  Created by Ivan Salazar on 4/23/22.
//

#ifndef TestEntityVerify_h
#define TestEntityVerify_h

#include <map>
#include <sstream>
#include "Testable.hpp"

#include "Entity.hpp"
#include "Attribute.hpp"
#include "Tokenizer.hpp"
#include "TokenPattern.hpp"
#include "TableStatements.hpp"
#include "BasicTypes.hpp"
#include "Row.hpp"

namespace ECE141{

    class TestEntityVerify : public Testable {
        
        public:
        using TestEntityVerifyFunc = bool (TestEntityVerify::*)();

        std::map<std::string, TestEntityVerifyFunc> testFunctions {
            {"TestFieldsVerify", &TestEntityVerify::TestFieldsVerify},
            {"TestValuesVerify", &TestEntityVerify::TestValuesVerify}
            
        };

        bool TestFieldsVerify(){
            std::vector<std::string> theFields = {"id", "first_name", "last_name", "zipcode"};
            bool result = true;
            Entity theEntity("Users");
            Attribute a1("id", DataTypes::int_type);
            Attribute a2("first_name", DataTypes::varchar_type);
            Attribute a3("last_name", DataTypes::varchar_type);
            Attribute a4("zipcode", DataTypes::int_type);
            
            theEntity.addAttribute(a1);
            theEntity.addAttribute(a2);
            theEntity.addAttribute(a3);
            theEntity.addAttribute(a4);
            
            StatusResult theResult = theEntity.verifyFields(theFields);
            
            if(theResult != StatusResult{Errors::noError}){
                result = false;
            }
            
            return result;

        }
        bool TestValuesVerify(){
            
            std::vector<std::string> theFields = {"id", "first_name", "last_name", "zipcode"};
            std::vector<std::vector<RowCell>> testValues;
            std::vector<RowCell> testValue;
            bool result = true;
            Entity theEntity("Users");
            Attribute a1("id", DataTypes::int_type);
            Attribute a2("first_name", DataTypes::varchar_type);
            Attribute a3("last_name", DataTypes::varchar_type);
            Attribute a4("zipcode", DataTypes::int_type);
            
            theEntity.addAttribute(a1);
            theEntity.addAttribute(a2);
            theEntity.addAttribute(a3);
            theEntity.addAttribute(a4);
            
            MyBasicIntType id(1);
            MyBasicVarcharType fname ("terry");
            MyBasicVarcharType lname("pratchett");
            MyBasicIntType zip(92124);
            
            testValue.push_back(id);
            testValue.push_back(fname);
            testValue.push_back(lname);
            testValue.push_back(zip);
            
            testValues.push_back(testValue);
            
            /* FIXME: temporarily disabled
            StatusResult theResult = theEntity.verifyValues(theFields, testValues);
                 
            if(theResult != StatusResult{Errors::noError}){
                result = false;
            }
            */
            
            return false; // FIXME: temporarily disabled 
        }
        
        

        // testable infrastructure
        TestEntityVerify(){
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


#endif /* TestEntityVerify_h */
