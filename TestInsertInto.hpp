#ifndef TEST_INSERT_INTO_HPP
#define TEST_INSERT_INTO_HPP


#include <map>
#include <sstream>
#include "Testable.hpp"

#include "Entity.hpp"
#include "Attribute.hpp"
#include "Tokenizer.hpp"
#include "TokenPattern.hpp"
#include "TableStatements.hpp"


namespace ECE141{

    class TestInsertInto : public Testable {
        /*
         id int NOT NULL auto_increment primary key
         first_name varchar NOT NULL,
         last_name varchar,
         zipcode int;
         */
        
        std::vector<std::string> testFields = {"first_name", "last_name", "zipcode"};
        std::vector<std::vector <RowCell>> theTestValues;
        std::vector<RowCell> testValue;
        public:
        using TestInsertIntoFunc = bool (TestInsertInto::*)();

        std::map<std::string, TestInsertIntoFunc> testFunctions {
            {"testParseStatement", &TestInsertInto::testParseStatement}
        };

        bool testParseStatement(){
            bool result = true;
            MyBasicVarcharType fname_1 ("terry");
            MyBasicVarcharType lname_1("pratchett");
            MyBasicIntType zip_1(92124);
            
            MyBasicVarcharType fname_2 ("ivan");
            MyBasicVarcharType lname_2("salazar");
            MyBasicIntType zip_2(92123);
            
            MyBasicVarcharType fname_3 ("hannah");
            MyBasicVarcharType lname_3("harman");
            MyBasicIntType zip_3(93536);
        
            
            testValue.push_back(fname_1);
            testValue.push_back(lname_1);
            testValue.push_back(zip_1);
            theTestValues.push_back(testValue);
            testValue.clear();
            
            testValue.push_back(fname_2);
            testValue.push_back(lname_2);
            testValue.push_back(zip_2);
            theTestValues.push_back(testValue);
            testValue.clear();
            
            testValue.push_back(fname_3);
            testValue.push_back(lname_3);
            testValue.push_back(zip_3);
            theTestValues.push_back(testValue);
            
            
            
            InsertTableStatement tryInsertInto(nullptr);
            
            std::stringstream buffer;
            buffer << "INSERT INTO Users (first_name, last_name, zipcode) VALUES (\"terry\" ,\"pratchett\" , 92124), (\"ivan\" ,\"salazar\" , 92123),"
            << "(\"hannah\" ,\"harman\" , 93536);";

            Tokenizer aTokenizer(buffer);
            aTokenizer.tokenize();

            tryInsertInto.parse(aTokenizer);
            
            // FIXME: temporarily disabled
            /*
            result &= tryInsertInto.parsedFields == testFields;
            size_t vecLimit = theTestValues.size();
            size_t limit = testFields.size();
            for(size_t j = 0; j < vecLimit; j++){
                for(size_t i = 0; i < limit; i++){
                    result &= tryInsertInto.parsedValues[j][i].index() == theTestValues[j][i].index();
                }
            }
            */

            return false; // FIXME: temporarily disabled

        }
        
        

        // testable infrastructure
        TestInsertInto(){
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
