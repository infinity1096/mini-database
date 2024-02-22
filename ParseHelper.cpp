//
//  ParseHelpers.cpp
//  RGAssignment4
//
//  Created by rick gessner on 4/18/21.
//

#include "ParseHelper.hpp"
#include "Helpers.hpp"
#include "Entity.hpp"
#include "TokenPattern.hpp"
#include "Tokenizer.hpp"

namespace ECE141 {

  // USE: gets properties following the type in an attribute decl...
  StatusResult ParseHelper::parseAttributeOptions(Attribute &anAttribute) {
    while (true){
      // match some of the following sequence:
      // 1. NOT NULL
      // 2. AUTO_INCREMENT
      // 3. PRIMARY KEY
      TokenPattern pattern2(tokenizer); 
      
      if (pattern2.begins().equalsKeyword(Keywords::not_kw).equalsKeyword(Keywords::null_kw)){
        // matched NOT NULL
        anAttribute.setNullable(false);
        continue;
      }

      if (pattern2.begins().equalsKeyword(Keywords::auto_increment_kw)){
        // matched AUTO_INCREMENT
        anAttribute.setAutoIncrement(true);
        continue;
      }
      
      if (pattern2.begins().equalsKeyword(Keywords::primary_kw).equalsKeyword(Keywords::key_kw)){
        // matched PRIMARY KEY
        anAttribute.setPrimaryKey(true);
        continue;
      }

      if (pattern2.begins().equalsPunctuation(',')){
        break;
      }
      
      // when code reaches here, no pattern is matched.
      break;
    }

    return StatusResult{Errors::noError};
  }
    
  //USE : parse an individual attribute (name type [options])
  StatusResult ParseHelper::parseAttribute(Attribute &anAttribute) {
    StatusResult theResult{noError};

    TokenPattern pattern(tokenizer);

    std::string attributeName;
    DataTypes dataType;
    int32_t size;

    if (pattern.begins().containsIdentifier(attributeName).containsDataType(dataType, size)){
      anAttribute.setName(attributeName).setDataType(dataType).setSize(size);
    }else{
      theResult = StatusResult{Errors::invalidArguments};
    }
        
    if(theResult) {
      theResult=parseAttributeOptions(anAttribute);
      if(!anAttribute.isValid()) {
        theResult.error=invalidAttribute;
      }
    }
    return theResult;
  }

  //USE: parse a comma-sep list of (unvalidated) identifiers;
  //     AUTO stop if keyword (or term)
  StatusResult ParseHelper::parseIdentifierList(StringList &aList) {
    StatusResult theResult{noError};
    
    while(theResult && tokenizer.more()) {
      Token &theToken=tokenizer.current();
      if(TokenType::identifier==tokenizer.current().type) {
        aList.push_back(theToken.data);
        tokenizer.next(); //skip identifier...
        tokenizer.skipIf(comma);
      }
      else if(theToken.type==TokenType::keyword) {
        break; //Auto stop if we see a keyword...
      }
      else if(tokenizer.skipIf(right_paren)){
        break;
      }
      else if(semicolon==theToken.data[0]) {
        break;
      }
      else theResult.error=syntaxError;
    }
    return theResult;
  }

  //** USE: get a list of values (identifiers, strings, numbers...)
  StatusResult ParseHelper::parseValueList(StringList &aList) {
    StatusResult theResult{noError};
    
    while(theResult && tokenizer.more()) {
      Token &theToken=tokenizer.current();
      if(TokenType::identifier==theToken.type || TokenType::number==theToken.type) {
        aList.push_back(theToken.data);
        tokenizer.next(); //skip identifier...
        tokenizer.skipIf(comma);
      }
      else if (tokenizer.skipIf(left_paren)) {
        continue;
      }
      else if(tokenizer.skipIf(right_paren)) {
        break;
      }
      else theResult.error=syntaxError;
    }
    return theResult;
  }
    
/*
  //read a comma-sep list of key/value pairs...
  StatusResult ParseHelper::parseKeyValues(KeyValues &aList, Entity &anEntity) {
    StatusResult theResult{noError};    
    while(theResult && tokenizer.more()) {
      TokenSequencer thePhrase(tokenizer);
      std::string theName;
      int theValue{0};
      if(thePhrase.hasId(theName).hasOp(Operators::equal_op).hasNumber(theValue)) {
        tokenizer.next(3);
          //Add code here to finish this...
      }
      theResult.error=syntaxError;
    }
    return theResult;
  }
 */

}
