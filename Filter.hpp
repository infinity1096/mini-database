#ifndef FILTER_HPP
#define FILTER_HPP

#include <string>
#include <algorithm>

#include "Row.hpp"
#include "Entity.hpp"

namespace ECE141{

#define CONSTANT_INDEX 0
#define EXPRESSION_INDEX 1

using RowCellValue = std::variant<std::string, RowCell>;

class CastAndCompare{
    public:
    CastAndCompare(std::string lhs) : raw_string(lhs) {}

    int operator()(MyBasicNullType anElement){
        MyBasicNullType casted;
        if (!casted.parseFromString(raw_string)){
            return -1;
        }else{
            return casted.compareTo(anElement);
        }
    }

    int operator()(MyBasicBoolType anElement){
        MyBasicBoolType casted;
        if (!casted.parseFromString(raw_string)){
            return -1;
        }else{
            return casted.compareTo(anElement);
        }
    }
    int operator()(MyBasicDateTimeType anElement){
        MyBasicDateTimeType casted;
        if (!casted.parseFromString(raw_string)){
            return -1;
        }else{
            return casted.compareTo(anElement);
        }
    }
    int operator()(MyBasicFloatType anElement){
        MyBasicFloatType casted;
        if (!casted.parseFromString(raw_string)){
            return -1;
        }else{
            return casted.compareTo(anElement);
        }
    }
    int operator()(MyBasicIntType anElement){
        MyBasicIntType casted;
        if (!casted.parseFromString(raw_string)){
            return -1;
        }else{
            return casted.compareTo(anElement);
        }
    }
    int operator()(MyBasicVarcharType anElement){
        MyBasicVarcharType casted;
        if (!casted.parseFromString(raw_string)){
            return -1;
        }else{
            return casted.compareTo(anElement);
        }
    }

    private:
    std::string raw_string;
};

// cast rhs into lhs then compare

int compareToCast(std::string lhs, RowCell rhs);
int compareToExpression(RowCell lhs, RowCell rhs);

// implement a comparator for RowCellValue
int compareTo(RowCellValue lhs, RowCellValue rhs);
  
class IEvaluate{
    public:
    virtual RowCellValue eval(Row aRow, const Entity& aEntity) const =0;
};

class RowExpression : public IEvaluate {
    public:
    RowExpression(std::string fieldName) : field_name(fieldName) {}

    RowCellValue eval(Row aRow, const Entity& aEntity) const override {
        std::optional<size_t> attributeIndex = aEntity.getAttributeIndex(field_name);
        if (attributeIndex == std::nullopt){
            // Error: attribute not found. 
            return std::string("");
        }

        RowCell value = aRow[attributeIndex.value()];
        
        return value;
    }

    bool verify(const Entity& aEntity) const {
        return aEntity.getAttributeIndex(field_name) != std::nullopt;
    }

    public:
    std::string field_name;
};

class ConstantExpression : public IEvaluate {
    public:
    ConstantExpression(std::string value) : value(value) {}

    RowCellValue eval(Row aRow, const Entity& aEntity) const override {
        return value; 
    }

    bool verify(const Entity& aEntity) const {
        return true;
    }

    private:
    std::string value;
};

using ValueExpression = std::variant<RowExpression, ConstantExpression>;
using CompareOperator = char;

class ComparisionExpression{
    public:
    ComparisionExpression(ValueExpression lhs, ValueExpression rhs, Operators op) : lhs(lhs), rhs(rhs), compare_operator(op) {}

    bool eval(Row aRow, const Entity& aEntity){
        

        RowCellValue lhsValue;
        RowCellValue rhsValue;
        
        std::visit([&](const auto& type){lhsValue = type.eval(aRow, aEntity);}, lhs);
        std::visit([&](const auto& type){rhsValue = type.eval(aRow, aEntity);}, rhs);

        int compareResult = compareTo(lhsValue, rhsValue);
        bool logicalResult = false;

        switch (compare_operator){
            case Operators::equal_op       : logicalResult =  compareResult == 0;  break;
            case Operators::notequal_op    : logicalResult =  compareResult != 0;  break;
            case Operators::lt_op          : logicalResult =  compareResult <  0;  break;
            case Operators::lte_op         : logicalResult =  compareResult <= 0;  break;
            case Operators::gt_op          : logicalResult =  compareResult >  0;  break;
            case Operators::gte_op         : logicalResult =  compareResult >= 0;  break;
            default                        : logicalResult =  false; //should never reach here 
        }

        return logical_inverted ? !logicalResult : logicalResult;
    }

    bool verify(const Entity& aEntity){
        bool result = true;
        std::visit([&](const auto& type){result &= type.verify(aEntity);}, lhs);
        std::visit([&](const auto& type){result &= type.verify(aEntity);}, rhs);
        return result;
    }

    void setInverted(){
        logical_inverted = true;
    }

    private:
    ValueExpression lhs;
    ValueExpression rhs;
    Operators       compare_operator;
    bool            logical_inverted = false;
};

class Filter{
    public:
    bool eval(Row aRow, const Entity& aEntity){
        if (expressions.size() != logical_operators.size()){
            std::cout << "Error in filter logical expression evaluation \n";
            return false;
        }

        for (size_t i = 0; i < expressions.size(); i++){
            bool expressionResult = expressions[i].eval(aRow, aEntity);
            
            switch (logical_operators[i]){
                case Logical::no_op  :  if (!expressionResult) {return false;} break; 
                case Logical::or_op  :  if (expressionResult) {i += 1;} break;
                case Logical::and_op :  if (!expressionResult) {return false;} break;
                default              :  std::cout << "error in comp expression eval \n"; return false;
            }
        }

        return true;
    }

    void addExpression(ComparisionExpression compExpression, Logical logicalOperator){
        expressions.push_back(compExpression);
        logical_operators.push_back(logicalOperator);
    }

    bool verify(const Entity& anEntity){
        bool result = true;
        for(auto expression : expressions){
            result &= expression.verify(anEntity);
        }
        return result;
    }

    private:
    std::vector<ComparisionExpression> expressions;
    std::vector<Logical> logical_operators;
};




}

#endif