#ifndef JOIN_HPP
#define JOIN_HPP

#include <string>
#include <algorithm>

#include "Entity.hpp"
#include "keywords.hpp"
#include "Filter.hpp"

namespace ECE141{
    
    struct JoinResult{
        RowCollection collection;
        Entity entity;
        StatusResult result;

        operator StatusResult() {return result;}
    };

    struct JoinPrototype{
        std::string table_name;
        std::string field_name;
        std::string other_table_name;
        std::string other_field_name;

        Keywords joinType;
    };

    class Join{
        public:

        bool verify(const Entity& leftEntity, const Entity& rightEntity) {
            join_type = prototype.joinType;
            other_table_name = prototype.other_table_name;
            self_field = prototype.field_name;
            other_field = prototype.other_field_name;

            return true; // FIXME: hack
        }

        void enable() {
            enabled = true;
        }

        JoinResult* executeJoin(const Entity& selfEntity, const Entity& otherEntity, RowCollection& selfRows, RowCollection& otherRows) const {
            if (join_type == Keywords::left_kw){
                return leftJoin(selfRows, otherRows, selfEntity, otherEntity, self_field, other_field);
            }
            return nullptr; // FIXME: not implemented
        }

        JoinResult* leftJoin(RowCollection& leftRows, RowCollection& rightRows, const Entity& leftEntity, const Entity& rightEntity, std::string leftField, std::string rightField) const {
            
            JoinResult* joinResult = new JoinResult();

            // join entities
            joinResult->entity.setName(leftEntity.getName() + " JOIN " + rightEntity.getName());
            
            AttributeList leftAttributes = leftEntity.getAttributes();
            AttributeList rightAttributes = rightEntity.getAttributes();
            size_t leftTableJoinFieldIndex = leftEntity.getAttributeIndex(leftField).value();
            size_t rightTableJoinFieldIndex = rightEntity.getAttributeIndex(rightField).value();
            
            for (auto attribute : leftAttributes){
                if (attribute.getName() != leftField) {
                    joinResult->entity.addAttribute(attribute);
                }
            }

            for (auto attribute : rightAttributes){
                if (attribute.getName() != rightField){
                    joinResult->entity.addAttribute(attribute);
                }
            }

            RowExpression leftExpression(leftField);
            RowExpression rightExpression(rightField); 

            // FIXME: O(n^2) solution. Use sorting to reduce to nlog(n)
            for (auto& leftRow : leftRows){
                bool matchedRow = false;
                for (auto& rightRow : rightRows){
                    RowCell lhs = std::get<EXPRESSION_INDEX>(leftExpression.eval(*leftRow, leftEntity));
                    RowCell rhs = std::get<EXPRESSION_INDEX>(rightExpression.eval(*rightRow, rightEntity));
                    if (0 == compareToExpression(lhs, rhs)){
                        Row newRow(*leftRow);
                        Row rightRowCopy(*rightRow);

                        newRow.removeCell(leftTableJoinFieldIndex); // remove joined field from rhs
                        rightRowCopy.removeCell(rightTableJoinFieldIndex); // remove joined field from rhs
                        
                        newRow.append(rightRowCopy);
                        joinResult->collection.push_back(std::make_unique<Row>(newRow));
                        matchedRow = true;
                    }
                }

                if (!matchedRow){
                    int rightSize = rightRows[0]->toStringVec().size(); // FIXME: hack
                    Row newRow(*leftRow);
                    newRow.removeCell(leftTableJoinFieldIndex); // remove joined field from rhs
                        
                    for(int i = 0; i < rightSize - 1; i++){
                        newRow.addContent(MyBasicNullType());
                    }
                    joinResult->collection.push_back(std::make_unique<Row>(newRow));
                }
            }
            
            joinResult->result = StatusResult{Errors::noError}; // FIXME: check return value!

            return joinResult;
        }

        bool enabled = false;

        std::string other_table_name;
        std::string self_field;
        std::string other_field;

        Keywords join_type;

        JoinPrototype prototype;
    };

}

#endif