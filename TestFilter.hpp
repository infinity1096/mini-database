#ifndef TEST_FILTER_HPP
#define TEST_FILTER_HPP


#include <map>
#include "Testable.hpp"

#include "Entity.hpp"
#include "Attribute.hpp"
#include "Row.hpp"
#include "Filter.hpp"

namespace ECE141{

    class TestFilter : public Testable {
        public:
        using TestFilterFunc = bool (TestFilter::*)();

        std::map<std::string, TestFilterFunc> testFunctions {
            {"testCompareToOfBasicTypeBool", &TestFilter::testCompareToOfBasicTypeBool},
            {"testCompareToOfBasicTypeDatetime", &TestFilter::testCompareToOfBasicTypeDatetime},
            {"testRowValueExpression", &TestFilter::testRowValueExpression},
            {"testCompareExpression", &TestFilter::testCompareExpression},
        };

        bool testRowValueExpression(){
            Entity users;
            Attribute name("name", DataTypes::varchar_type, 50);
            Attribute age("age", DataTypes::int_type, 0);
            Attribute height("height", DataTypes::float_type, 0);
            Attribute birth("birth", DataTypes::datetime_type, 0);
            Attribute is_male("is_male", DataTypes::bool_type, 0);
            Attribute null("null", DataTypes::no_type, 0);
            
            users.addAttribute(name);
            users.addAttribute(age);
            users.addAttribute(height);
            users.addAttribute(birth);
            users.addAttribute(is_male);
            users.addAttribute(null);
            
            Row row1;
            MyBasicVarcharType row_name("test person");
            MyBasicIntType row_age(21);
            MyBasicFloatType row_height(1.83);
            MyBasicDateTimeType row_birth({2000,1,1,1,1,1});
            MyBasicBoolType row_is_male(false);
            MyBasicNullType row_null;

            row1.addContent(row_name);
            row1.addContent(row_age);
            row1.addContent(row_height);
            row1.addContent(row_birth);
            row1.addContent(row_is_male);
            row1.addContent(row_null);
            
            ConstantExpression constant("value in constant");
            RowExpression rowExpression1("name");
            RowExpression rowExpression2("age");
            RowExpression rowExpression3("height");
            RowExpression rowExpression4("birth");
            RowExpression rowExpression5("is_male");
            RowExpression rowExpression6("null");
            
            bool success = true;

            success &= constant.eval(row1, users).index() == 0 && std::get<CONSTANT_INDEX>(constant.eval(row1, users)) == std::string("value in constant");

            auto result = std::get<5>(std::get<EXPRESSION_INDEX>(rowExpression1.eval(row1, users))).toString();
            bool a = result == row_name.toString();

            success &= rowExpression1.eval(row1, users).index() == EXPRESSION_INDEX && std::get<5>(std::get<EXPRESSION_INDEX>(rowExpression1.eval(row1, users))).toString() == row_name.toString();
            success &= rowExpression2.eval(row1, users).index() == EXPRESSION_INDEX && std::get<4>(std::get<EXPRESSION_INDEX>(rowExpression2.eval(row1, users))).toString() == row_age.toString();
            success &= rowExpression3.eval(row1, users).index() == EXPRESSION_INDEX && std::get<3>(std::get<EXPRESSION_INDEX>(rowExpression3.eval(row1, users))).toString() == row_height.toString();
            success &= rowExpression4.eval(row1, users).index() == EXPRESSION_INDEX && std::get<2>(std::get<EXPRESSION_INDEX>(rowExpression4.eval(row1, users))).toString() == row_birth.toString();
            success &= rowExpression5.eval(row1, users).index() == EXPRESSION_INDEX && std::get<1>(std::get<EXPRESSION_INDEX>(rowExpression5.eval(row1, users))).toString() == row_is_male.toString();
            success &= rowExpression6.eval(row1, users).index() == EXPRESSION_INDEX && std::get<0>(std::get<EXPRESSION_INDEX>(rowExpression6.eval(row1, users))).toString() == row_null.toString();
            
            return success;
        }

        bool testCompareExpression(){
            Entity users;
            Attribute name("name", DataTypes::varchar_type, 50);
            Attribute age("age", DataTypes::int_type, 0);
            Attribute height("height", DataTypes::float_type, 0);
            Attribute height2("height2", DataTypes::float_type, 0);
            Attribute birth("birth", DataTypes::datetime_type, 0);
            Attribute is_male("is_male", DataTypes::bool_type, 0);
            Attribute null("null", DataTypes::no_type, 0);
            
            users.addAttribute(name);
            users.addAttribute(age);
            users.addAttribute(height);
            users.addAttribute(height2);
            users.addAttribute(birth);
            users.addAttribute(is_male);
            users.addAttribute(null);
            
            Row row1;
            MyBasicVarcharType row_name("test person");
            MyBasicIntType row_age(21);
            MyBasicFloatType row_height(1.83);
            MyBasicFloatType row_height2(1.65);
            MyBasicDateTimeType row_birth({2000,1,1,1,1,1});
            MyBasicBoolType row_is_male(false);
            MyBasicNullType row_null;

            row1.addContent(row_name);
            row1.addContent(row_age);
            row1.addContent(row_height);
            row1.addContent(row_height2);
            row1.addContent(row_birth);
            row1.addContent(row_is_male);
            row1.addContent(row_null);

            RowExpression rowExpression1("name");       // eval to "test person"
            RowExpression rowExpression2("age");        // eval to 21
            RowExpression rowExpression3("height");     // eval to 1.83
            RowExpression rowExpression3_2("height2");  // eval to 1.65
            RowExpression rowExpression4("birth");      // eval to 2000:1:1 1:1:1
            RowExpression rowExpression5("is_male");    // eval to false
            RowExpression rowExpression6("null");       // eval to null

            ConstantExpression constant1("aaa");
            ConstantExpression constant2("24");
            ConstantExpression constant3("1.0");
            ConstantExpression constant3_2("-100");
            ConstantExpression constant4("2022:05:11 00:00:00");
            ConstantExpression constant5("false");
            ConstantExpression constant6("NULL");
            
            ComparisionExpression compExpr1(rowExpression1, constant1, Operators::gt_op); // true
            ComparisionExpression compExpr2(rowExpression2, constant2, Operators::gt_op); // false
            ComparisionExpression compExpr3(rowExpression3, constant3, Operators::gt_op); // true
            ComparisionExpression compExpr4(rowExpression4, constant4, Operators::lt_op); // true
            ComparisionExpression compExpr5(rowExpression5, constant5, Operators::equal_op); // true
            ComparisionExpression compExpr6(rowExpression6, constant6, Operators::equal_op); // true
            ComparisionExpression compExpr7(rowExpression3, rowExpression3_2, Operators::gt_op); // true
            
            bool success = true;

            success &= compExpr1.eval(row1, users) == true;
            success &= compExpr2.eval(row1, users) == false;
            success &= compExpr3.eval(row1, users) == true;
            success &= compExpr4.eval(row1, users) == true;
            success &= compExpr5.eval(row1, users) == true;
            success &= compExpr6.eval(row1, users) == true;
            success &= compExpr7.eval(row1, users) == true;

            Filter f1;
            f1.addExpression(compExpr1, Logical::and_op);
            f1.addExpression(compExpr2, Logical::no_op);

            Filter f2;
            f2.addExpression(compExpr1, Logical::or_op);
            f2.addExpression(compExpr2, Logical::no_op);
            
            success &= f1.eval(row1, users) == false;
            success &= f2.eval(row1, users) == true;
            
            return success;
        }

        bool testCompareToOfBasicTypeBool(){
            MyBasicBoolType b1(false);
            MyBasicBoolType b2(true);

            bool success = true;
            success &= b1.compareTo(b2) < 0;
            success &= b2.compareTo(b1) > 0;
            success &= b1.compareTo(b1) == 0;
            success &= b2.compareTo(b2) == 0;
            
            return success;        
        }

        bool testCompareToOfBasicTypeDatetime(){
            MyBasicDateTimeType d1({1998,1,1,1,1,1});
            MyBasicDateTimeType d2({1999,1,1,1,1,1});
            MyBasicDateTimeType d3({1999,2,1,1,1,1});
            MyBasicDateTimeType d4({1999,2,2,1,1,1});
            MyBasicDateTimeType d5({1999,2,2,2,1,1});
            MyBasicDateTimeType d6({1999,2,2,2,2,1});
            MyBasicDateTimeType d7({1999,2,2,2,2,2});

            bool success = true;
            success &= d1.compareTo(d2) < 0;
            success &= d2.compareTo(d3) < 0;
            success &= d3.compareTo(d4) < 0;
            success &= d4.compareTo(d5) < 0;
            success &= d5.compareTo(d6) < 0;
            success &= d6.compareTo(d7) < 0;

            success &= d2.compareTo(d1) > 0;
            success &= d3.compareTo(d2) > 0;
            success &= d4.compareTo(d3) > 0;
            success &= d5.compareTo(d4) > 0;
            success &= d6.compareTo(d5) > 0;
            success &= d7.compareTo(d6) > 0;
            
            success &= d1.compareTo(d1) == 0;
            success &= d2.compareTo(d2) == 0;
            success &= d3.compareTo(d3) == 0;
            success &= d4.compareTo(d4) == 0;
            success &= d5.compareTo(d5) == 0;
            success &= d6.compareTo(d6) == 0;

            return success;        
        }

        

        // testable infrastructure
        TestFilter(){
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
