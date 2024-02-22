#ifndef ROW_HPP
#define ROW_HPP

#include <map>
#include <memory>
#include <string>
#include <variant>
#include <vector>
#include <cstring>
#include <algorithm>
#include <string>

#include "Storable.hpp"
#include "BasicTypes.hpp"
#include "RowCells.hpp"

#define ROW_PREFIX "@Row: v1"
#define ROW_PREFIX_SIZE 8

namespace ECE141{

    
    static std::map<size_t, DataTypes> indexToType = {
        {0, DataTypes::no_type},
        {1, DataTypes::bool_type},
        {2, DataTypes::datetime_type},
        {3, DataTypes::float_type},
        {4, DataTypes::int_type}, 
        {5, DataTypes::varchar_type}
    };


    class Row;
    class Block;
    using RowCellCreator = RowCell (Row::*)(std::istream&);
    class Row : public Storable {
        public:
        Row() {}
        Row(std::istream& anInput){
            decode(anInput);
        }

        StatusResult encode(std::ostream& anOutput) override;
        StatusResult decode(std::istream& anInput) override;

        operator Block() ;

        template<typename T>
        void addContent(const T& rawCell){
            RowCell cell = rawCell;
            content.push_back(cell);
        }

        RowCell operator[](uint32_t index) {
            return content[index];
        }

        StatusResult setCell(size_t index, RowCell cell) {
            content[index] = cell;
            return StatusResult{ Errors::noError };
        }

        StatusResult removeCell(size_t index) {
            content.erase(content.begin() + index);
            return StatusResult{ Errors::noError };
        }

        StatusResult append(const Row& other) {
            content.insert(content.end(), other.content.begin(), other.content.end());
            return StatusResult{ Errors::noError };
        }
        
        std::string toString() const {
            std::string str;
            for (auto cell : content){
                std::visit([&](auto& element){str += element.toString();}, cell);
                str += "|";
            }
            return str;
        }

        std::vector<std::string> toStringVec() const {
            std::vector<std::string> output;
            for (auto cell : content) {
                std::visit([&](auto& element) {output.push_back(element.toString()); }, cell);
            }

            return output;
        }

        private:
        std::vector<RowCell> content;

        // encode different types
        static std::map<char, RowCellCreator> rowCellCreators;

        RowCell constructNullCell(std::istream& input);
        RowCell constructBoolCell(std::istream& input);
        RowCell constructDatetimeCell(std::istream& input);
        RowCell constructFloatCell(std::istream& input);
        RowCell constructIntCell(std::istream& input);
        RowCell constructVarcharCell(std::istream& input);
    };

    using RowCollection = std::vector<std::unique_ptr<Row>>;
}


#endif
