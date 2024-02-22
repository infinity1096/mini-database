//
//  Query.hpp
//  Assignment5
//
//  Created by Ivan Salazar on 5/1/22.
//


#ifndef Query_hpp
#define Query_hpp

#include <vector>
#include <string>
#include <optional>

#include "Filter.hpp"
#include "Verifiable.hpp"

namespace ECE141 {
    class Filter;
    using StringList = std::vector<std::string>;

    struct ExpressionPrototype{
        std::string lhs;
        Operators compOp;
        std::string rhs;
        bool isInverted = false;
        Logical logicalOp = Logical::no_op;
    };

    class SelectQuery {
    public:
        SelectQuery();
        SelectQuery(std::string aTableName, StringList& aFieldsList);
        SelectQuery(const SelectQuery& aCopy);
        SelectQuery& operator=(const SelectQuery& aSelectQuery);
        ~SelectQuery();

        void setLimit(size_t aLimit);
        void setOrderBy(std::string anOrderBy);

        void addExpressionPrototype(const ExpressionPrototype exprPrototype);
        StatusResult parseExpression(const Entity& aEntity);

        bool verify(const Entity& aEntity);

        std::string             table_name;
        
        StringList              selected_fields;
        bool                    select_all = false;
        
        Filter                  filter;
        std::optional<size_t>   limit;
        StringOpt               order_by;

        //private:
        std::vector<ExpressionPrototype> expr_prototypes; 
    };
}


#endif /* Query_h */
