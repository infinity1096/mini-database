#pragma once
//
//  TestSelectRow.hpp
//  Assignment4
//
//  Created by Ivan Salazar on 4/23/22.
//

#ifndef TestParseExpression_hpp
#define TestParseExpression_hpp

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

namespace ECE141 {

    class TestParseExpression : public Testable {

    public:
        using TestParseExpressionFunc = bool (TestParseExpression::*)();

        std::map<std::string, TestParseExpressionFunc> testFunctions{
            {"testExpression", &TestParseExpression::testParseExpression}
        };

        bool testParseExpression() {
            bool theResult = true;
            std::stringstream theStream;

            theStream << "select * from users where zipcode > 92122;";

            Tokenizer theTokenizer(theStream);

            theTokenizer.tokenize();



            Application theApp = Application(std::cout);
            Database theDB = Database("test", Config::getDBPath("test"), &theApp);
            SelectStatement theStatement(&theDB);
            theResult &= theStatement.parse(theTokenizer);
            

            return theResult;

        }







        // testable infrastructure
        TestParseExpression() {
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
#pragma once
