#include "Row.hpp"

#include "BlockIO.hpp"

namespace ECE141{

    StatusResult Row::encode(std::ostream& anOutput) {
        anOutput << ROW_PREFIX;
        uint32_t contentSize = static_cast<uint32_t>(content.size());
        
        anOutput.write((char*)(&contentSize), sizeof(uint32_t));

        for (auto& row : content){
            std::visit([&](auto& saveable){saveable.encode(anOutput);}, row);              
        }

        return StatusResult{ Errors::noError };
    }

    std::map<char, RowCellCreator> Row::rowCellCreators = {
        {static_cast<char>(DataTypes::no_type), &Row::constructNullCell},
        {static_cast<char>(DataTypes::bool_type), &Row::constructBoolCell},
        {static_cast<char>(DataTypes::datetime_type), &Row::constructDatetimeCell},
        {static_cast<char>(DataTypes::float_type), &Row::constructFloatCell},
        {static_cast<char>(DataTypes::int_type), &Row::constructIntCell},
        {static_cast<char>(DataTypes::varchar_type), &Row::constructVarcharCell}
    };

    StatusResult Row::decode(std::istream& anInput) {
            char temp[ROW_PREFIX_SIZE];
            anInput.read(temp, ROW_PREFIX_SIZE);

            if (strncmp(temp, ROW_PREFIX, ROW_PREFIX_SIZE) != 0){
                return StatusResult{Errors::unknownType};
            }

            uint32_t contentSize;
            anInput.read((char*)(&contentSize), sizeof(uint32_t));

            for (size_t i = 0; i < contentSize; i++) {
                char identifierChar = anInput.peek();
                
                if (rowCellCreators.count(identifierChar) != 0){
                    auto cell = (this->*rowCellCreators[identifierChar])(anInput);
                    content.push_back(cell);
                }else{
                    StatusResult{ Errors::unexpectedValue };
                }
            }

            return StatusResult{ Errors::noError };
    }

    RowCell Row::constructNullCell(std::istream& input){
        std::unique_ptr ptr = std::make_unique<MyBasicNullType>();
        ptr->decode(input);
        RowCell cell = *ptr;
        return cell;
    }

    RowCell Row::constructBoolCell(std::istream& input){
        std::unique_ptr ptr = std::make_unique<MyBasicBoolType>();
        ptr->decode(input);
        RowCell cell = *ptr;
        return cell;
    }
    
    RowCell Row::constructDatetimeCell(std::istream& input){
        std::unique_ptr<MyBasicDateTimeType> ptr = std::make_unique<MyBasicDateTimeType>();
        ptr->decode(input);
        RowCell cell = *ptr;
        return cell;
    }
    
    RowCell Row::constructFloatCell(std::istream& input){
        std::unique_ptr ptr = std::make_unique<MyBasicFloatType>();
        ptr->decode(input);
        RowCell cell = *ptr;
        return cell;
    }
    
    RowCell Row::constructIntCell(std::istream& input){
        std::unique_ptr ptr = std::make_unique<MyBasicIntType>();
        ptr->decode(input);
        RowCell cell = *ptr;
        return cell;
    }
    
    RowCell Row::constructVarcharCell(std::istream& input){
        std::unique_ptr ptr = std::make_unique<MyBasicVarcharType>();
        ptr->decode(input);
        RowCell cell = *ptr;
        return cell;
    }

    Row::operator Block() {
        Block rowBlock;

        // encode entity to a buffer
        std::stringstream rowEncoding;
        encode(rowEncoding);

        // check the size of the buffer is less then kPayloadSize
        rowEncoding.seekg(0, std::ios::end);
        uint32_t rowEncodingSize = rowEncoding.tellg();

        if (rowEncodingSize > kPayloadSize){
            std::cout << "Error in entity encoding : encoding size too large \n";
            return rowBlock;
        }

        rowBlock.header.type = static_cast<char>(BlockType::data_block);
        rowBlock.header.valid_data_length = rowEncodingSize;

        rowEncoding.seekg(0);
        rowEncoding.read(rowBlock.payload, rowEncodingSize);

        return rowBlock;
    }

}