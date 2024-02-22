//
//  Entity.cpp
//  PA3
//
//  Created by rick gessner on 3/2/22.
//

#include <stdio.h>
#include <vector>
#include <optional>
#include <memory>
#include <string>
#include "Entity.hpp"

#include "BlockIO.hpp"

namespace ECE141 {

  using StringList = std::vector<std::string>;

 //STUDENT: Implement this class...
  Entity::Entity() : primID(1), entity_name(""), auto_increment(0) {}
  Entity::Entity(const std::string aName) : primID(1), entity_name(aName), auto_increment(0) {}

  Entity::Entity(const Entity &aCopy) {
    attributes = aCopy.attributes;
    primary_key_name = aCopy.primary_key_name;
    entity_name = aCopy.entity_name;
    auto_increment = aCopy.auto_increment;
    primID  = aCopy.primID;
  }

  Entity::~Entity() {}
 
  Entity& Entity::addAttribute(const Attribute &anAttribute) {
    if (getAttribute(anAttribute.getName()) == std::nullopt){
      attributes.push_back(anAttribute);

      if (anAttribute.isPrimaryKey()) {
          primary_key_name = anAttribute.getName();
      }
    }

    return *this;
  }

  AttributeOpt Entity::getAttribute(const std::string &aName) const {
    
    auto pos = std::find_if(attributes.begin(), attributes.end(), [=](const Attribute& attribute){
      return aName == attribute.getName();});
    
    if (pos != attributes.end()){
      return *pos;
    }

    return std::nullopt;
  }

  std::optional<size_t> Entity::getAttributeIndex(const std::string &aName) const {
    AttributeList attrList = getAttributes();
    std::optional<size_t> index = std::nullopt;

    for (size_t i = 0; i < attrList.size(); i++){
      if (attrList[i].getName() == aName){
        index = i;
      }
    }

    return index;
  }
  
  AttributeOpt Entity::getPrimaryKey() const {   
    if (primary_key_name == std::nullopt){
      return std::nullopt;
    }

    return getAttribute(primary_key_name.value());
  }

  void Entity::resetAttribute(){
    attributes.clear();
  }

  StatusResult Entity::encode(std::ostream& anOutput) {
    // encode entity header
    anOutput.write(ENTITY_HEADER, sizeof(ENTITY_HEADER));
    
    // encode entity name
    uint32_t nameLength = entity_name.size();
    anOutput.write((char*)(&nameLength), sizeof(uint32_t));
    anOutput.write(entity_name.c_str(), nameLength + 1);

    // encode attributes
    uint32_t attributeLength = attributes.size();
    anOutput.write((char*)(&attributeLength), sizeof(uint32_t));

    for (auto attribute : attributes){
      attribute.encode(anOutput);
    }
    
    // encode primID
    anOutput.write((char*)&primID, sizeof(int32_t));

    return StatusResult{ Errors::noError };
  }

  StatusResult Entity::decode(std::istream& anInput) {
    // decode and verify header
    char header[sizeof(ENTITY_HEADER)]; 
    anInput.read(header, sizeof(ENTITY_HEADER));

    if (strncmp(header, ENTITY_HEADER, sizeof(ENTITY_HEADER)) != 0){
      return StatusResult{Errors::invalidArguments};
    }

    // decode name
    uint32_t nameLength;
    anInput.read((char*)(&nameLength), sizeof(uint32_t));

    char* nameStr = new char[nameLength + 1];
    anInput.read(nameStr, nameLength + 1);
    entity_name = std::string(nameStr);
    delete [] nameStr;

    uint32_t attributeSize;
    anInput.read((char*)(&attributeSize), sizeof(uint32_t));

    for (size_t i = 0; i < attributeSize; i++){
      Attribute attribute;
      attribute.decode(anInput);
      attributes.push_back(attribute);
    }
      
    int32_t thePrimID;
    anInput.read((char*)(&thePrimID), sizeof(int32_t));
    primID = thePrimID;

    recover_state_from_attributes();
    return StatusResult{ Errors::noError };
  }

  Entity::operator Block() {
    Block entityBlock;

    // encode entity to a buffer
    std::stringstream entityEncoding;
    encode(entityEncoding);

    // check the size of the buffer is less then kPayloadSize
    entityEncoding.seekg(0, std::ios::end);
    uint32_t entityEncodingSize = entityEncoding.tellg();

    if (entityEncodingSize > kPayloadSize){
      std::cout << "Error in entity encoding : encoding size too large \n";
      return entityBlock;
    }

    entityBlock.header.type = static_cast<char>(BlockType::entity_block);
    entityBlock.header.valid_data_length = entityEncodingSize;

    entityEncoding.seekg(0);
    entityEncoding.read(entityBlock.payload, entityEncodingSize);

    return entityBlock;
  }

  bool Entity::operator==(const Entity& other) const {
    bool same = true;
    same &= entity_name == other.entity_name;
    if (attributes.size() == other.attributes.size()){
      for(size_t i = 0; i < attributes.size(); i++){
        same &= attributes[i] == other.attributes[i];
      }
    }else{
      same = false;
    }
    return same;
  }

  bool Entity::verify() const {
    bool result = true;
    
    // let each attribute verify
    for (auto attribute : attributes){
      result &= attribute.verify();
    }

    // verify there are at most 1 primary key
    size_t numPrimaryKey = 0;
    for (auto attribute : attributes){
      numPrimaryKey += attribute.isPrimaryKey() ? 1 : 0;
    }

    result &= numPrimaryKey <= 1;

    // verify auto_increment can be only on primary key
    for (auto attribute : attributes){
      result &= (!attribute.isAutoIncrement()) || attribute.isPrimaryKey();  
    }

    return result;
  }

    StatusResult Entity::verifyFields(std::vector<std::string>  aParsedFields){
        std::vector<Attribute> requiredAttributes;
        std::vector<Attribute> autoIncrementAttributes;
        Attribute primaryKey;

        for(const Attribute& theAttr: attributes){
            
            if(!theAttr.isNullable() && !theAttr.isAutoIncrement()){
              requiredAttributes.push_back(theAttr);
            }

            if(theAttr.isPrimaryKey()){
              primaryKey = theAttr;
            }
            
            if(theAttr.isAutoIncrement()){
              autoIncrementAttributes.push_back(theAttr);
            }
        }

        // verify that parsed fields are valid fields and not auto incremented
        for(const std::string& theField : aParsedFields){
          if (getAttribute(theField) == std::nullopt || getAttribute(theField).value().isAutoIncrement()){
            return StatusResult{Errors::invalidAttribute};
          }   
        }

        // verify that all required fields are present in parsed fields
        for(const auto& theRequired : requiredAttributes){
          if (std::find(aParsedFields.begin(), aParsedFields.end(), theRequired.getName()) == aParsedFields.end()){
            return StatusResult{Errors::invalidAttribute};
          }   
        }
        
        return StatusResult{Errors::noError};
    }

    StatusResult Entity::createRowFromValues(Row& aRow, const StringList& parsedFields, const StringList& parsedValue){
      for (auto& attribute : attributes){
        // FIXME: assumption made here that primary keys are always auto increment
        // which is not true in general. 
        if (attribute.isPrimaryKey() && attribute.isAutoIncrement()){
          // process primary key
          if (attribute.getType() == DataTypes::int_type){
            MyBasicIntType primaryKeyCell(primID++);
            aRow.addContent(primaryKeyCell);
          }else{
            return StatusResult{Errors::notImplemented};
          }
        }else{
          // process normal field

          auto theAttributeIt = std::find(parsedFields.begin(), parsedFields.end(), attribute.getName());

          

          if (theAttributeIt == parsedFields.end()){
            // attribute not found in inputs. insert null cell.
              aRow.addContent(MyBasicNullType());
          }else{
            // attribute found in input. insert accordingly.

            // as the user input fields may be in different order than what is stored in
            // the entity, we need to find out what the index in parsedFields is each 
            // attribute stored in index. 
            size_t theAttributeIndex = std::distance(parsedFields.begin(), theAttributeIt);
            
            std::string value = parsedValue[theAttributeIndex]; 

            // use type information to create row cell from input value accordingly
            std::optional<RowCell> insertedRowCell = createRowCellFromInput(attribute.getType(), value);

            if (insertedRowCell == std::nullopt) {
                return StatusResult{ Errors::invalidArguments };
            }

            // save the created RowCell to row
            aRow.addContent(*insertedRowCell);
          }

          
        }
      }

      return StatusResult{ Errors::noError };
    }

    std::vector<size_t> Entity::computeFieldSizes(){
      std::vector<size_t> spaces;
      for(auto attr : attributes){
        spaces.push_back(type_field_len_map[attr.getType()]);
      }
      return spaces;
    }

    std::map<DataTypes, size_t> Entity::type_field_len_map = {
      {DataTypes::no_type,  8},
      {DataTypes::bool_type,  8},
      {DataTypes::int_type,  8},
      {DataTypes::float_type,  8},
      {DataTypes::datetime_type,  18},
      {DataTypes::varchar_type,  30},
    };

}
