
#include "Filter.hpp"

namespace ECE141 {

    int compareToCast(std::string lhs, RowCell rhs) {
        return std::visit(CastAndCompare(lhs), rhs);
    }

    int compareToExpression(RowCell lhs, RowCell rhs) {
        std::string lhsString = std::visit([](const auto& element) {return element.toString(); }, lhs);
        return std::visit(CastAndCompare(lhsString), rhs);
    }

    // implement a comparator for RowCellValue

    int compareTo(RowCellValue lhs, RowCellValue rhs) {
        if (lhs.index() == CONSTANT_INDEX && rhs.index() == CONSTANT_INDEX) {
            // both are undetermined strings. use compare operators for std::string
            return std::get<CONSTANT_INDEX>(lhs).compare(std::get<CONSTANT_INDEX>(rhs));
        }

        if (lhs.index() == CONSTANT_INDEX && rhs.index() == EXPRESSION_INDEX) {
            // cast lhs to type of rhs and compare
            return compareToCast(std::get<CONSTANT_INDEX>(lhs), std::get<EXPRESSION_INDEX>(rhs));
        }

        if (lhs.index() == EXPRESSION_INDEX && rhs.index() == CONSTANT_INDEX) {
            // cast rhs to type of lhs and compare. 
            // Notice: we feed in rhs and lhs in reverse to the below function. 
            // therefore its result is inverted.
            return -compareToCast(std::get<CONSTANT_INDEX>(rhs), std::get<EXPRESSION_INDEX>(lhs));
        }

        // Now, both sides are expression. 
        return compareToExpression(std::get<EXPRESSION_INDEX>(lhs), std::get<EXPRESSION_INDEX>(rhs));
    }

}