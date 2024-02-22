#ifndef TEST_TABLE_STATEMENTS_HPP
#define TEST_TABLE_STATEMENTS_HPP


#include <map>
#include <sstream>
#include "Testable.hpp"

#include "Entity.hpp"
#include "Attribute.hpp"
#include "Tokenizer.hpp"
#include "TokenPattern.hpp"
#include "TableStatements.hpp"


namespace ECE141{

    class TestTableStatements : public Testable {
        public:
        using TestTableStatementsFunc = bool (TestTableStatements::*)();

        std::map<std::string, TestTableStatementsFunc> testFunctions {
            {"testParseEntity", &TestTableStatements::testParseEntity}
        };

        bool testParseEntity(){
            CreateTableStatement tryCreateTable(nullptr);
            
            std::stringstream buffer;
            buffer << "create table Users (";
            buffer << " id int NOT NULL auto_increment primary key,";
            buffer << " first_name varchar(50) NOT NULL,";
            buffer << " last_name varchar(50),";
            buffer << " zipcode int);\n";

            Tokenizer aTokenizer(buffer);
            aTokenizer.tokenize();

            tryCreateTable.parse(aTokenizer);
            
            Entity entity = tryCreateTable.getEntity();

            bool success = true;

            success &= entity.getName() == "Users";
            success &= entity.getAttributes().size() == 4;
            
            // attribute 1
            success &= entity.getAttribute("id").value().getType() == DataTypes::int_type;
            success &= entity.getAttribute("id").value().getSize() == 0;
            success &= entity.getAttribute("id").value().isNullable() == 0;
            success &= entity.getAttribute("id").value().isAutoIncrement() == 1;
            success &= entity.getAttribute("id").value().isPrimaryKey() == 1;

            // attribute 2
            success &= entity.getAttribute("first_name").value().getType() == DataTypes::varchar_type;
            success &= entity.getAttribute("first_name").value().getSize() == 50;
            success &= entity.getAttribute("first_name").value().isAutoIncrement() == 0;
            success &= entity.getAttribute("first_name").value().isPrimaryKey() == 0;
            success &= entity.getAttribute("first_name").value().isNullable() == 0;

            // attribute 3
            success &= entity.getAttribute("last_name").value().getType() == DataTypes::varchar_type;
            success &= entity.getAttribute("last_name").value().getSize() == 50;
            success &= entity.getAttribute("last_name").value().isAutoIncrement() == 0;
            success &= entity.getAttribute("last_name").value().isPrimaryKey() == 0;
            success &= entity.getAttribute("last_name").value().isNullable() == 1;

            // attribute 4
            success &= entity.getAttribute("zipcode").value().getType() == DataTypes::int_type;
            success &= entity.getAttribute("zipcode").value().getSize() == 0;
            success &= entity.getAttribute("zipcode").value().isAutoIncrement() == 0;
            success &= entity.getAttribute("zipcode").value().isPrimaryKey() == 0;
            success &= entity.getAttribute("zipcode").value().isNullable() == 1;

            return success;
        }

        // testable infrastructure
        TestTableStatements(){
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
