#pragma once
//
//  TestSelectRow.hpp
//  Assignment4
//
//  Created by Ivan Salazar on 4/23/22.
//

#ifndef TestSelectRow_h
#define TestSelectRow_h

#include <map>
#include <sstream>
#include "Testable.hpp"
#include "Config.hpp"
#include "Entity.hpp"
#include "Attribute.hpp"
#include "Tokenizer.hpp"
#include "TokenPattern.hpp"
#include "TableStatements.hpp"
#include "BasicTypes.hpp"
#include "Row.hpp"
#include "Query.hpp"
#include "Application.hpp"
#include "Filter.hpp"
#include "TabularView.hpp"
namespace ECE141 {

    class TestSelectRow : public Testable {

    public:
        using TestSelectRowFunc = bool (TestSelectRow::*)();

        std::map<std::string, TestSelectRowFunc> testFunctions{
            {"testSelectRow", &TestSelectRow::testSelectRow}
        };

        bool testSelectRow() {
            
            Application theApp = Application(std::cout);
            Database theDB = Database("test", Config::getDBPath("test"), &theApp);
            StringList theSL = StringList({ "*" });
            SelectQuery theSQ = SelectQuery("users", theSL);
            RowExpression theRowExpression("zipcode");
            ConstantExpression theConstantExpression("92122");
            Operators theOp(Operators::gt_op);
            Logical theLogic(Logical::no_op);
            ComparisionExpression theExpression(theRowExpression, theConstantExpression, theOp);
            theSQ.filter.addExpression(theExpression, theLogic);
            RowCollection theRows;

            theDB.selectRows(theSQ, theRows);
            Entity theEntity(theDB.getEntity("users"));


            TabularView theView(std::cout, theEntity, theRows, theSQ);
            theView.show(std::cout);
            
            return true;
            
        }

            
        
     



        // testable infrastructure
        TestSelectRow() {
            count = getNumTests();
        }

        size_t getNumTests() {
            return testFunctions.size();
        }

        std::optional<std::string> getTestName(size_t index) const {
            size_t current_index = 0;

            for (auto kv : testFunctions) {
                if (index == current_index++) {
                    return kv.first;
                }
            }

            return std::nullopt;
        }

        bool operator() (const std::string& testName) {
            if (testFunctions.count(testName) > 0) {
                return (this->*testFunctions[testName])();
            }
            else {
                return false;
            }
        }
    };

}


#endif /* TestEntityVerify_h */
