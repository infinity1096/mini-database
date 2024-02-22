#include "RowCells.hpp"

#include <functional>
#include <optional>

namespace ECE141{

    // -------- FIXED LENGTH --------

    MyFixedLengthType::MyFixedLengthType(DataTypes aDataType, size_t aDataLength) : data_type(aDataType), data_length(aDataLength) {}
    
    MyFixedLengthType::MyFixedLengthType(const MyFixedLengthType& other){
        *this = other;
    }

    MyFixedLengthType& MyFixedLengthType::operator=(const MyFixedLengthType& other){
        data_type = other.data_type;
        data_length = other.data_length;
        return *this;
    }

    StatusResult MyFixedLengthType::writeEncoding(std::ostream& anOutput, const char* aBinaryEncoding) {
        anOutput << static_cast<char>(data_type);
        anOutput.write(aBinaryEncoding, data_length);
        return StatusResult{Errors::noError};  
    }

    char* MyFixedLengthType::readEncoding(std::istream& anInput) {
        char theCharIdentifier;
        anInput.read(&theCharIdentifier, sizeof(char));

        if (static_cast<char>(data_type) != theCharIdentifier){
            // should never reach here, wrong object is created to load 
            // from the stream.
            return nullptr;
        }

        char* buffer = new char[data_length];
        anInput.read(buffer, data_length);

        return buffer;
    }

    // -------- BASIC NULL --------

    MyBasicNullType::MyBasicNullType() : MyFixedLengthType(DataTypes::no_type, 0) {}

    MyBasicNullType::MyBasicNullType(const MyBasicNullType& other) : MyFixedLengthType(other) {
        *this = other;
    }

    MyBasicNullType& MyBasicNullType::operator=(const MyBasicNullType& other){
        return *this;
    }

    StatusResult MyBasicNullType::encode(std::ostream& anOutput) {
        return writeEncoding(anOutput, "");
    }

    StatusResult MyBasicNullType::decode(std::istream& anInput) {
        char* data = readEncoding(anInput);
        
        // parse the data

        // free the data
        if (data != nullptr){
            delete data;
            return StatusResult{Errors::noError};
        }

        return StatusResult{Errors::unknownType};
    }

    bool MyBasicNullType::parseFromString(std::string input) {
        return true;
    }

    std::string MyBasicNullType::toString() const {
        return std::string{"NULL"};
    }

    int MyBasicNullType::compareTo(const MyBasicNullType& other) const {
        return 0;
    }

    MyBasicNullType::operator std::string(){return toString();}

    // -------- BASIC BOOL --------
    MyBasicBoolType::MyBasicBoolType(bool aValue) : MyFixedLengthType(DataTypes::bool_type, sizeof(bool)), value(aValue) {}
    
    MyBasicBoolType::MyBasicBoolType(const MyBasicBoolType& other) : MyFixedLengthType(other) {
        *this = other;
    }
    
    MyBasicBoolType& MyBasicBoolType::operator=(const MyBasicBoolType& other){
        value = other.value;
        return *this;
    }

    StatusResult MyBasicBoolType::encode(std::ostream& anOutput) {
        return writeEncoding(anOutput, (char*)&value);
    }

    StatusResult MyBasicBoolType::decode(std::istream& anInput) {
        char* data = readEncoding(anInput);
        
        if (data != nullptr){
            value = *(bool*)(data + 0);

            delete data;
            return StatusResult{Errors::noError};
        }

        return StatusResult{Errors::unknownType};
    }

    bool MyBasicBoolType::parseFromString(std::string input) {
        //std::transform(input.begin(), input.end(), input.begin(), std::tolower);
        
        if (input != "false" && input != "true"){
            return false;
        }
        
        value = (input == "false") ? false : true;
        return true;
    }

    std::string MyBasicBoolType::toString() const {
        return std::string(value ? "TRUE" : "FALSE");
    }

    int MyBasicBoolType::compareTo(const MyBasicBoolType& other) const {
        return static_cast<int>(value) - static_cast<int>(other.value);
    }

    MyBasicBoolType::operator bool(){return value;}

    // -------- BASIC DATETIME --------

    MyBasicDateTimeType::MyBasicDateTimeType() : MyFixedLengthType(DataTypes::datetime_type, sizeof(TimeStamp_t)), time_stamp({}) {}
    MyBasicDateTimeType::MyBasicDateTimeType(TimeStamp_t aTimeStamp) : MyFixedLengthType(DataTypes::datetime_type, sizeof(TimeStamp_t)), time_stamp(aTimeStamp) {}
    
    MyBasicDateTimeType::MyBasicDateTimeType(const MyBasicDateTimeType& other) : MyFixedLengthType(other){
        *this = other;
    }
    
    MyBasicDateTimeType& MyBasicDateTimeType::operator=(const MyBasicDateTimeType& other){
        time_stamp = other.time_stamp;
        return *this;
    }
    StatusResult MyBasicDateTimeType::encode(std::ostream& anOutput) {
        return writeEncoding(anOutput, (char*)&time_stamp);
    }

    StatusResult MyBasicDateTimeType::decode(std::istream& anInput) {
        char* data = readEncoding(anInput);
        
        if (data != nullptr){
            time_stamp.year =   *((int16_t*)(data + 0));
            time_stamp.month =  *((int16_t*)(data + 2));
            time_stamp.day =    *((int16_t*)(data + 4));
            time_stamp.hour =   *((int16_t*)(data + 6));
            time_stamp.minute = *((int16_t*)(data + 8));
            time_stamp.second = *((int16_t*)(data + 10));
            
            delete data;
            return StatusResult{Errors::noError};
        }

        return StatusResult{Errors::unknownType};
    }
    
    bool MyBasicDateTimeType::parseFromString(std::string input) {
        // string must have format yyyy:mm:dd hh:mm:ss
        // which requires it to have:
        // 1. a total length of 19
        // 2. 

        if(19 != input.size()){
            return false;
        }
        
        std::string dateFormat = "YYYY:MM:DD hh:mm:ss";
        bool templateMatch = true;
        for(size_t i = 0; i < 19; i++){
            char theDFChar = dateFormat[0];
            char theChar = input[0];
            if(isalpha(theDFChar)){
                templateMatch &= static_cast<bool>(isdigit(theChar));
            }else if(':' == theDFChar){
                templateMatch &= ':' == theChar;
            }else if(' ' == theDFChar){
                templateMatch &= ' ' == theChar;
            }
        }

        if (!templateMatch){
            return false;
        }

        time_stamp.year = stoi(input.substr(0,4));
        time_stamp.month = stoi(input.substr(5,2));
        time_stamp.day = stoi(input.substr(8,2));
        time_stamp.hour = stoi(input.substr(11,2));
        time_stamp.minute = stoi(input.substr(14,2));
        time_stamp.second = stoi(input.substr(17,2));
                
        return true;
    }


    std::string MyBasicDateTimeType::toString() const {
        std::string str = "";
        str += add_zero_to(std::to_string(time_stamp.year), 4);
        str += ":";
        str += add_zero_to(std::to_string(time_stamp.month), 2);
        str += ":";
        str += add_zero_to(std::to_string(time_stamp.day), 2);
        str += " ";
        str += add_zero_to(std::to_string(time_stamp.hour), 2);
        str += ":";
        str += add_zero_to(std::to_string(time_stamp.minute), 2);
        str += ":";
        str += add_zero_to(std::to_string(time_stamp.second), 2);
        
        return str;
    }

    int MyBasicDateTimeType::compareTo(const MyBasicDateTimeType& other) const {
        if (time_stamp.year != other.time_stamp.year){
            return time_stamp.year - other.time_stamp.year;
        }
        if (time_stamp.month != other.time_stamp.month){
            return time_stamp.month - other.time_stamp.month;
        }
        if (time_stamp.day != other.time_stamp.day){
            return time_stamp.day - other.time_stamp.day;
        }
        if (time_stamp.hour != other.time_stamp.hour){
            return time_stamp.hour - other.time_stamp.hour;
        }
        if (time_stamp.minute != other.time_stamp.minute){
            return time_stamp.minute - other.time_stamp.minute;
        }
        return time_stamp.second - other.time_stamp.second;            
    }
    
    MyBasicDateTimeType::operator std::string() {return toString();}

    std::string MyBasicDateTimeType::add_zero_to(std::string aStr, size_t targetLength) const {
            
        if (aStr.size() < targetLength){
            std::string theLeadingZeros(targetLength - aStr.size(),'0');
            aStr = theLeadingZeros + aStr;
        }

        return aStr;
    }

    // -------- BASIC FLOAT --------
    MyBasicFloatType::MyBasicFloatType(float aValue) : MyFixedLengthType(DataTypes::float_type, sizeof(float)), value(aValue) {}
    
    MyBasicFloatType::MyBasicFloatType(const MyBasicFloatType& other) : MyFixedLengthType(other) {
        *this = other;   
    }

    MyBasicFloatType& MyBasicFloatType::operator=(const MyBasicFloatType& other){
        value = other.value;
        return *this;
    }

    StatusResult MyBasicFloatType::encode(std::ostream& anOutput) {
        return writeEncoding(anOutput, (char*)&value);
    }

    StatusResult MyBasicFloatType::decode(std::istream& anInput) {
        char* data = readEncoding(anInput);
        
        if (data != nullptr){
            value = *(float*)(data + 0);

            delete data;
            return StatusResult{Errors::noError};
        }

        return StatusResult{Errors::unknownType};
    }

    bool MyBasicFloatType::parseFromString(std::string input) {
        value = stof(input);
        return true;
    }

    std::string MyBasicFloatType::toString() const {
        return std::to_string(value);
    }

    int MyBasicFloatType::compareTo(const MyBasicFloatType& other) const {
        if (value < other.value){
            return -1;
        }else if (value > other.value){
            return 1;
        }
        return 0;
    }

    MyBasicFloatType::operator float(){return value;}

    // -------- BASIC INTEGER --------


    MyBasicIntType::MyBasicIntType(int32_t aValue) : MyFixedLengthType(DataTypes::int_type, sizeof(int32_t)), value(aValue) {}
    
    MyBasicIntType::MyBasicIntType(const MyBasicIntType& other) : MyFixedLengthType(other) {
        *this = other;
    }
    
    MyBasicIntType& MyBasicIntType::operator=(const MyBasicIntType& other){
        value = other.value;
        return *this;
    }

    StatusResult MyBasicIntType::encode(std::ostream& anOutput) {
        return writeEncoding(anOutput, (char*)&value);
    }

    StatusResult MyBasicIntType::decode(std::istream& anInput) {
        char* data = readEncoding(anInput);
        
        if (data != nullptr){
            value = *(int32_t*)(data + 0);

            delete data;
            return StatusResult{Errors::noError};
        }

        return StatusResult{Errors::unknownType};
    }

    bool MyBasicIntType::parseFromString(std::string input) {
        value = stoi(input);
        return true;
    }

    std::string MyBasicIntType::toString() const {
        return std::to_string(value);
    }

    int MyBasicIntType::compareTo(const MyBasicIntType& other) const {
        return value - other.value;
    }

    MyBasicIntType::operator int32_t() {return value;}

    // -------- BASIC VARCHAR --------
    MyBasicVarcharType::MyBasicVarcharType() : string_ptr(nullptr), string_length(0) {
        string_ptr = new char[1];
    }

    MyBasicVarcharType::MyBasicVarcharType(const char* aCString) : string_ptr(nullptr), string_length(0) {
        *this = aCString;
    }
    

    MyBasicVarcharType::MyBasicVarcharType(const MyBasicVarcharType& anOther){
        *this = anOther;
    }

    MyBasicVarcharType& MyBasicVarcharType::operator=(const MyBasicVarcharType& anOther){
        if (string_ptr != nullptr){
            delete[] string_ptr;
            string_ptr = nullptr;
        }

        string_ptr = new char[anOther.string_length + 1];
        memcpy(string_ptr, anOther.string_ptr, anOther.string_length + 1);
        string_length = anOther.string_length;
        return *this;
    }

    MyBasicVarcharType::~MyBasicVarcharType(){
        if (string_ptr != nullptr){
            delete[] string_ptr;
            string_ptr = nullptr;
        }
    }

    MyBasicVarcharType& MyBasicVarcharType::operator=(const char* aCStr){
        if (string_ptr != nullptr){
            delete[] string_ptr;
            string_ptr = nullptr;
        }

        uint32_t theStringSize = strlen(aCStr);
        string_ptr = new char[strlen(aCStr) + 1];
        memcpy(string_ptr, aCStr, theStringSize + 1);
        string_length = theStringSize;
        return *this;
    }

    StatusResult MyBasicVarcharType::encode(std::ostream& anOutput) {
        anOutput << static_cast<char>(DataTypes::varchar_type);     // encode type
        anOutput.write((char*)(&string_length), sizeof(uint32_t));  // encode string length. Note: this does not include '\0' at end
        anOutput.write(string_ptr, string_length + 1);

        return StatusResult{Errors::noError};
    }

    StatusResult MyBasicVarcharType::decode(std::istream& anInput) {
        if (string_ptr != nullptr){
            delete[] string_ptr;
            string_ptr = nullptr;
        }

        char theCharIdentifier;
        anInput.read(&theCharIdentifier, sizeof(char));

        if (theCharIdentifier != static_cast<char>(DataTypes::varchar_type)){
            return StatusResult{Errors::unknownType};
        }

        anInput.read((char*)&string_length, sizeof(uint32_t));
        
        string_ptr = new char[string_length + 1];
        anInput.read(string_ptr, string_length + 1);

        return StatusResult{Errors::noError};
    }

    bool MyBasicVarcharType::parseFromString(std::string input) {
        *this = input.c_str();
        return true;
    }

    std::string MyBasicVarcharType::toString() const {
        return std::string(string_ptr);
    }

    size_t MyBasicVarcharType::size() const {
        return string_length;
    }

    int MyBasicVarcharType::compareTo(const MyBasicVarcharType& other) const {
        return toString().compare(other.toString());
    }

    MyBasicVarcharType::operator std::string(){return toString();}

    using RowCellStringCreator = std::function<std::optional<RowCell>(std::string)>;

    std::optional<RowCell> nullCellCreator(std::string value){
        MyBasicNullType cell;
        if (cell.parseFromString(value) == true){
        return cell;
        }
        return std::nullopt;
    }

    std::optional<RowCell> boolCellCreator(std::string value){
        MyBasicBoolType cell;
        if (cell.parseFromString(value) == true){
        return cell;
        }
        return std::nullopt;
    }

    std::optional<RowCell> datetimeCellCreator(std::string value){
        MyBasicDateTimeType cell;
        if (cell.parseFromString(value) == true){
        return cell;
        }
        return std::nullopt;
    }

    std::optional<RowCell> floatCellCreator(std::string value){
        MyBasicFloatType cell;
        if (cell.parseFromString(value) == true){
        return cell;
        }
        return std::nullopt;
    }

    std::optional<RowCell> intCellCreator(std::string value){
        MyBasicIntType cell;
        if (cell.parseFromString(value) == true){
        return cell;
        }
        return std::nullopt;
    }

    std::optional<RowCell> varcharCellCreator(std::string value){
        MyBasicVarcharType cell;
        if (cell.parseFromString(value) == true){
        return cell;
        }
        return std::nullopt;
    }

    std::map<DataTypes, RowCellStringCreator> rowCellCreators = {
      {DataTypes::no_type, &nullCellCreator},
      {DataTypes::bool_type, &boolCellCreator},
      {DataTypes::datetime_type, &datetimeCellCreator},
      {DataTypes::float_type, &floatCellCreator},
      {DataTypes::int_type, &intCellCreator},
      {DataTypes::varchar_type, &varcharCellCreator}
    };

    std::optional<RowCell> createRowCellFromInput(DataTypes datatype, std::string input){
        return (rowCellCreators[datatype])(input);
    }

}