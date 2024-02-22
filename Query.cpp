//
//  DBQuery.cpp
//  Assignment5
//
//  Created by Ivan Salazar on 5/1/22.
//

#include "Query.hpp"

namespace ECE141{
    
    SelectQuery::SelectQuery() : selected_fields({}) {}

    SelectQuery::SelectQuery(std::string aTableName, StringList& aFieldsList) : table_name(aTableName), selected_fields(aFieldsList), select_all(false), limit(std::nullopt), order_by(std::nullopt) {}
 
    SelectQuery::SelectQuery(const SelectQuery& aCopy) {
        *this = aCopy;
    }

    SelectQuery& SelectQuery::operator=(const SelectQuery& aSelectQuery){
        table_name = aSelectQuery.table_name;
        selected_fields = aSelectQuery.selected_fields;
        select_all = aSelectQuery.select_all;
        filter = aSelectQuery.filter;
        limit = aSelectQuery.limit;
        order_by = aSelectQuery.order_by;
        return *this;
    }

    SelectQuery::~SelectQuery(){}

    void SelectQuery::setLimit(size_t aLimit) {
        limit = aLimit;
    }

    void SelectQuery::setOrderBy(std::string anOrderBy) {
        order_by = anOrderBy;
    }

    void SelectQuery::addExpressionPrototype(const ExpressionPrototype exprPrototype){
        expr_prototypes.push_back(exprPrototype);
    }

    StatusResult SelectQuery::parseExpression(const Entity& aEntity){

        AttributeList attributes = aEntity.getAttributes();
        std::map<std::string, size_t> attributeNames;
        
        for(auto attribute : attributes){
            attributeNames[attribute.getName()] = 0;
        }

        for (auto prototype : expr_prototypes){
            ValueExpression* lhs = attributeNames.count(prototype.lhs) ? new ValueExpression(RowExpression(prototype.lhs)) : new ValueExpression(ConstantExpression(prototype.lhs));
            ValueExpression* rhs = attributeNames.count(prototype.rhs) ? new ValueExpression(RowExpression(prototype.rhs)) : new ValueExpression(ConstantExpression(prototype.rhs));

            ComparisionExpression compExpression(*lhs, *rhs, prototype.compOp);

            if (prototype.isInverted){
                compExpression.setInverted();
            }

            filter.addExpression(compExpression, prototype.logicalOp);

            delete lhs;
            delete rhs;
        }

        return StatusResult{ Errors::noError };
    }

    bool SelectQuery::verify(const Entity& aEntity){
        bool result = true;
        // FIXME: handle join with verify 
        /*
        // verify selected fields
        for (auto field : selected_fields){
            result &= aEntity.getAttribute(field) != std::nullopt;
        }

        // verify filter
        result &= filter.verify(aEntity);

        //verify order_by
        result &= order_by == std::nullopt || aEntity.getAttribute(order_by.value()) != std::nullopt;
        */

        return result;
    }
}
