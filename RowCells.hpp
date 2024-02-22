#ifndef ROW_CELLS_HPP
#define ROW_CELLS_HPP

#include <string>
#include <variant>
#include <optional>

#include "BlockIO.hpp"
#include "BasicTypes.hpp"
#include "Storable.hpp"

namespace ECE141{

// Each of the following classes supports one datastructure described by
    // Basic_types.hpp and implements common interfaces.
    // The required types are:
    // no_type, bool_type, datetime_type, float_type, int_type, varchar_type
    // 
    // I currently have two views for varchar type:
    //
    // 1. It is a template class varchar<MAX_SIZE> that is parameterized by the 
    //    number of bytes it can hold.
    //
    // 2. All length of varchar type are of the same type, just that the max size
    //    is stored as a attribute of varchar and checked during assignment operations. 
    // 
    // I have decided to take path 2 for its ease.

    class MyBasicNullType;
    class MyBasicBoolType;
    class MyBasicDateTimeType;
    class MyBasicFloatType;
    class MyBasicIntType;
    class MyBasicVarcharType;

    // They all implement a common interface:
    class IMyBasicType : public Storable {
        virtual bool        parseFromString(std::string input)=0;
        virtual std::string toString() const =0;
    };

    // helper class that can handle all fixed length data format.
    class MyFixedLengthType : public IMyBasicType {
        public:
        MyFixedLengthType(DataTypes aDataType, size_t aDataLength);
        MyFixedLengthType(const MyFixedLengthType& other);
        MyFixedLengthType& operator=(const MyFixedLengthType& other);

        StatusResult writeEncoding(std::ostream& anOutput, const char* aBinaryEncoding);
        char* readEncoding(std::istream& anInput) ;

        private:
        DataTypes data_type;
        size_t    data_length;
    };

    class MyBasicNullType : public MyFixedLengthType {
        public:
        MyBasicNullType();
        MyBasicNullType(const MyBasicNullType& other);
        MyBasicNullType& operator=(const MyBasicNullType& other);

        StatusResult encode(std::ostream& anOutput) override ;
        StatusResult decode(std::istream& anInput) override ;
        
        bool parseFromString(std::string input) override ;
        std::string toString() const override ;

        int compareTo(const MyBasicNullType& other) const ;
        operator std::string();
    };

    class MyBasicBoolType : public MyFixedLengthType {
        public:
        MyBasicBoolType(bool aValue = false);
        MyBasicBoolType(const MyBasicBoolType& other);
        MyBasicBoolType& operator=(const MyBasicBoolType& other);

        StatusResult encode(std::ostream& anOutput) override ;
        StatusResult decode(std::istream& anInput) override ;

        bool parseFromString(std::string input) override ;
        std::string toString() const override ;
        
        int compareTo(const MyBasicBoolType& other) const ;

        operator bool();

        private:
        bool value;
    };

    typedef struct{
        int16_t year;
        int16_t month;
        int16_t day;
        int16_t hour;
        int16_t minute;
        int16_t second;
    } TimeStamp_t;

    class MyBasicDateTimeType : public MyFixedLengthType {
        public:
        MyBasicDateTimeType();
        MyBasicDateTimeType(TimeStamp_t aTimeStamp);
        MyBasicDateTimeType(const MyBasicDateTimeType& other);
        MyBasicDateTimeType& operator=(const MyBasicDateTimeType& other);

        StatusResult encode(std::ostream& anOutput) override ;
        StatusResult decode(std::istream& anInput) override ;
        
        bool parseFromString(std::string input) override ;
        std::string toString() const override ;

        int compareTo(const MyBasicDateTimeType& other) const ;
        
        operator std::string();

        private:
        TimeStamp_t time_stamp;
    
        std::string add_zero_to(std::string aStr, size_t targetLength) const ;
    };

    class MyBasicFloatType : public MyFixedLengthType {
        public:
        MyBasicFloatType(float aValue = 0.0f);
        MyBasicFloatType(const MyBasicFloatType& other);
        MyBasicFloatType& operator=(const MyBasicFloatType& other);

        StatusResult encode(std::ostream& anOutput) override ;
        StatusResult decode(std::istream& anInput) override ;

        bool parseFromString(std::string input) override ;
        std::string toString() const override ;

        int compareTo(const MyBasicFloatType& other) const ;

        operator float();

        private:
        float value;
    };

    class MyBasicIntType : public MyFixedLengthType {
        public:
        MyBasicIntType(int32_t aValue = 0);
        MyBasicIntType(const MyBasicIntType& other);
        MyBasicIntType& operator=(const MyBasicIntType& other);

        StatusResult encode(std::ostream& anOutput) override ;
        StatusResult decode(std::istream& anInput) override ;

        bool parseFromString(std::string input) override ;
        std::string toString() const override ;

        int compareTo(const MyBasicIntType& other) const ;

        operator int32_t();

        private:
        int32_t value;
    };

    class MyBasicVarcharType : public IMyBasicType{
        public:
        MyBasicVarcharType();
        MyBasicVarcharType(const char* aCString);
        MyBasicVarcharType(const MyBasicVarcharType& anOther);
        MyBasicVarcharType& operator=(const MyBasicVarcharType& anOther);
        ~MyBasicVarcharType();

        MyBasicVarcharType& operator=(const char* aCStr);

        StatusResult encode(std::ostream& anOutput) override ;
        StatusResult decode(std::istream& anInput) override ;

        bool parseFromString(std::string input) override ;
        std::string toString() const override ;

        size_t size() const ;

        int compareTo(const MyBasicVarcharType& other) const ;

        operator std::string();

        private:
        char*   string_ptr = nullptr;
        uint32_t  string_length = 0;
    };

    using RowCell = std::variant<
        MyBasicNullType, 
        MyBasicBoolType, 
        MyBasicDateTimeType, 
        MyBasicFloatType, 
        MyBasicIntType, 
        MyBasicVarcharType
    >;

    // free function used to call corresponding creator given type and 
    // input to parse.
    std::optional<RowCell> createRowCellFromInput(DataTypes datatype, std::string input);

}

#endif