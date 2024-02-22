#pragma once
//
//  TestExpression.hpp
//  Assignment6
//
//  Created by Ivan Salazar on 4/23/22.
//

#ifndef TestExpression_h
#define TestExpression_h

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

    class TestExpression : public Testable {

    public:
        using TestExpressionFunc = bool (TestExpression::*)();

        std::map<std::string, TestExpressionFunc> testFunctions{
            {"testParseExpression", &TestExpression::testParseExpression}
        };

        bool testParseExpression() {
            bool theResult = true;

            std::stringstream theStream;
            StringList theList;
            theStream << "age > 21";

            Tokenizer theTokenizer = Tokenizer(theStream);

            theTokenizer.tokenize();

            ParseHelper theHelper = ParseHelper(theTokenizer);

            theResult &= theHelper.parseExpression(theList);

            theResult &= theList[0] == "age";
            theResult &= theList[1] == ">";
            theResult &= theList[2] == "21";


            

            
            return true;

        }







        // testable infrastructure
        TestExpression() {
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
#pragma once
