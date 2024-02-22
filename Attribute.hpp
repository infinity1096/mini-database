//
//  Attribute.hpp
//  Assignment4
//
//  Created by rick gessner on 4/18/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#ifndef Attribute_hpp
#define Attribute_hpp

#include <stdio.h>
#include <string>
#include <vector>
#include <optional>
#include <functional>
#include <variant>
#include "keywords.hpp"
#include "BasicTypes.hpp"

#include "Storable.hpp"
#include "Verifiable.hpp"

#define ATTRIBUTE_HEADER "Attr v1:"

namespace ECE141 {
 
  using AttributeValue = std::optional<std::string>;
  class Attribute : public Storable , public IVerifiable {
  protected:
    std::string   name;
    DataTypes     type;
    uint16_t      size = 10; //max=1000
    uint16_t      autoIncrement = 1;
    uint16_t      primary = 1;
    uint16_t      nullable = 1;
    //Others?

  public:        
    Attribute(DataTypes aType=DataTypes::no_type);
    Attribute(std::string aName, DataTypes aType, uint32_t aSize=0);
    Attribute(const Attribute &aCopy);
    ~Attribute();
    
    Attribute&          setName(std::string aName);
    Attribute&          setDataType(DataTypes aType);
    Attribute&          setSize(int aSize);
    Attribute&          setAutoIncrement(bool anAuto);
    Attribute&          setPrimaryKey(bool anAuto);
    Attribute&          setNullable(bool aNullable);
    
    bool                isValid(); //is this Entity valid?
    
    const std::string&  getName() const {return name;}
    DataTypes           getType() const {return type;}
    size_t              getSize() const {return size;}
    AttributeValue      toValue(const std::string &aValue) const;

    bool                isPrimaryKey() const {return primary;}
    bool                isNullable() const {return nullable;}
    bool                isAutoIncrement() const {return autoIncrement;}    

    bool operator==(const Attribute& other) const {
      bool cmp = true;
      cmp &= name == other.name;
      cmp &= type == other.type;
      cmp &= size == other.size;
      cmp &= autoIncrement == other.autoIncrement;
      cmp &= primary == other.primary;
      cmp &= nullable == other.nullable;
      
      return cmp;
    }

    StatusResult encode(std::ostream& anOutput) override ;
    StatusResult decode(std::istream& anInput) override ;

    bool verify() const override ;
  };
  
  using AttributeOpt = std::optional<Attribute>;
  using AttributeList = std::vector<Attribute>;
  

}


#endif /* Attribute_hpp */
