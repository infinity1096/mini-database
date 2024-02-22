//
//  Attribute.cpp
//  Assignment4
//
//  Created by rick gessner on 4/18/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#include "Attribute.hpp"

#include <iostream>
#include <cstring>

namespace ECE141 {



  Attribute::Attribute(DataTypes aType)
    : type(aType),size(0),autoIncrement(0),primary(0),nullable(1) {}
 
  Attribute::Attribute(std::string aName, DataTypes aType, uint32_t aSize)  {
    name=aName;
    type=aType;
    size=aSize;
    autoIncrement=0;
    primary=0;
    nullable=1;
  }
 
  Attribute::Attribute(const Attribute &aCopy)  {
    name=aCopy.name;
    type=aCopy.type;
    size=aCopy.size;
    autoIncrement=aCopy.autoIncrement;
    primary=aCopy.primary;
    nullable=aCopy.nullable;
  }
 
  Attribute::~Attribute()  {
  }
 
  Attribute& Attribute::setName(std::string aName)  {
    name = aName;
    return *this;
  }
 
  Attribute& Attribute::setDataType(DataTypes aType) {
    type=aType;
    return *this;
  }

  Attribute& Attribute::setSize(int aSize) {
    size=aSize; return *this;
  }

  Attribute& Attribute::setAutoIncrement(bool anAuto) {
    autoIncrement=anAuto; return *this;
  }

  Attribute& Attribute::setPrimaryKey(bool aPrimary) {
    primary=aPrimary; return *this;
  }

  Attribute& Attribute::setNullable(bool aNullable) {
    nullable=aNullable; return *this;
  }

  bool Attribute::isValid() {
    return true;
  }
   
  AttributeValue Attribute::toValue(const std::string &aValue) const {
    //might need to do type-casting here...    
    return AttributeValue{aValue};
  }

  StatusResult Attribute::encode(std::ostream& anOutput) {
    // encode header
    anOutput.write(ATTRIBUTE_HEADER, sizeof(ATTRIBUTE_HEADER));

    // encode name
    uint32_t nameLength = name.size();
    anOutput.write((char*)(&nameLength), sizeof(uint32_t));
    anOutput.write(name.c_str(), nameLength + 1);

    // encode attributes
    char typeChar = static_cast<char>(type);
    anOutput.write(&typeChar, sizeof(char));
    anOutput.write((char*)(&size), sizeof(uint16_t));
    anOutput.write((char*)(&autoIncrement), sizeof(uint16_t));
    anOutput.write((char*)(&primary), sizeof(uint16_t));
    anOutput.write((char*)(&nullable), sizeof(uint16_t));

    return StatusResult{Errors::noError};
  }

  StatusResult Attribute::decode(std::istream& anInput) {
    // decode and verify header
    char header[sizeof(ATTRIBUTE_HEADER)]; 
    anInput.read(header, sizeof(ATTRIBUTE_HEADER));

    if (strncmp(header, ATTRIBUTE_HEADER, sizeof(ATTRIBUTE_HEADER)) != 0){
      return StatusResult{Errors::invalidArguments};
    }

    // decode name
    uint32_t nameLength;
    anInput.read((char*)(&nameLength), sizeof(uint32_t));

    char* nameStr = new char[nameLength + 1];
    anInput.read(nameStr, nameLength + 1);
    name = std::string(nameStr);
    delete[] nameStr;

    //decode attributes
    char typeChar;
    anInput.read(&typeChar, sizeof(char));
    type = static_cast<DataTypes>(typeChar);

    anInput.read((char*)&size, sizeof(uint16_t));
    anInput.read((char*)&autoIncrement, sizeof(uint16_t));
    anInput.read((char*)&primary, sizeof(uint16_t));
    anInput.read((char*)&nullable, sizeof(uint16_t));

    return StatusResult{Errors::noError};
  }

  bool Attribute::verify() const {
    // only type int can be auto-incremented
    return (autoIncrement != 1) && (type == DataTypes::int_type);
  }

}
