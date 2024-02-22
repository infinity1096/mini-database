//
//  Entity.hpp
//  Assignment3
//
//  Created by rick gessner on 3/18/22.
//  Copyright © 2022 rick gessner. All rights reserved.
//

#ifndef Entity_hpp
#define Entity_hpp

#include <stdio.h>
#include <vector>
#include <optional>
#include <memory>
#include <string>
#include <map>

#include "Attribute.hpp"
#include "Errors.hpp"
#include "BasicTypes.hpp"
#include "Tokenizer.hpp"
#include "Storable.hpp"
#include "Verifiable.hpp"
#include "Row.hpp"

#define ENTITY_HEADER "Entity v1:"

namespace ECE141 {
  class Block;
  using AttributeOpt = std::optional<Attribute>;
  using AttributeList = std::vector<Attribute>;
  using AttributeMap = std::map<std::string, Attribute>;
  using StringOpt = std::optional<std::string>;

  class Entity;
  using RowCellStringCreator = std::optional<RowCell> (Entity::*)(std::string);

  //------------------------------------------------

  class Entity : public Storable, public IVerifiable{    
  public:
                          Entity();
                          Entity(const std::string aName);
                          Entity(const Entity &aCopy);
    
    virtual               ~Entity();
    
    const std::string&    getName() const {return entity_name;}
    void                  setName(std::string aName) {entity_name = aName;}
    AttributeList         getAttributes() const {return attributes;}
    Entity&               addAttribute(const Attribute &anAttribute);
    AttributeOpt          getAttribute(const std::string &aName) const;
    std::optional<size_t> getAttributeIndex(const std::string &aName) const;
    AttributeOpt          getPrimaryKey() const;
    void                  resetAttribute();

    StatusResult encode(std::ostream& anOutput) override;
    StatusResult decode(std::istream& anInput) override;
      
    StatusResult verifyFields(std::vector<std::string>  aParsedFields);
    //StatusResult verifyValues(std::vector<std::string>  aParsedFields, std::vector<std::vector<RowCell>> aParsedValues);

    
    using StringList = std::vector<std::string>;
    StatusResult createRowFromValues(Row& aRow, const StringList& parsedFields, const StringList& parsedValue);

    std::vector<size_t> computeFieldSizes();
    static std::map<DataTypes, size_t> type_field_len_map;

    // conversion operator to encode itself as a block
    operator Block();
    bool operator==(const Entity& other) const ;

    bool verify() const override; 
           
  protected:
    AttributeList   attributes;
    int32_t         primID;

    // state defined by attributes
    StringOpt       primary_key_name;
    std::string     entity_name;
    uint32_t        auto_increment;

  private:
    void recover_state_from_attributes(){
      primary_key_name = std::nullopt;
      auto_increment = 0;

      for (auto attribute : attributes){
        if(attribute.isPrimaryKey()){
          primary_key_name = attribute.getName();
          auto_increment = attribute.isAutoIncrement() ? 1 : 0;
        }
      }
    }

  };
  
}
#endif /* Entity_hpp */
