//
//  SQLProcessor.hpp
//  PA3
//
//  Created by rick gessner on 4/2/22.
//

#ifndef SQLProcessor_hpp
#define SQLProcessor_hpp

#include <stdio.h>
#include <iostream>

#include "CmdProcessor.hpp"
#include "Tokenizer.hpp"
#include "Query.hpp"
#include "Row.hpp"
#include "Join.hpp"

namespace ECE141 {

  class Statement;
  class DBProcessor; //define this later...
  class Entity;
  class Database;
  //Should this processor have it's own Statement types?

  class SQLProcessor : public CmdProcessor {
  public:
    
    SQLProcessor(std::ostream &anOutput, Database* aDataBase);
    virtual ~SQLProcessor();
      
    bool          isKnown(Keywords aKeyword);
    
    CmdProcessor* recognizes(Tokenizer &aTokenizer) override;
    Statement*    makeStatement(Tokenizer &aTokenizer,
                                StatusResult &aResult) override;

    StatusResult showQuery(const SelectQuery& aQuery, Join& ajoin, RowCollection& aCollection);
    
    StatusResult  run(Statement *aStmt) override {
      return aStmt->run(output); // FIXME: output
    };
    
    //We don't DO the work here, but we can help route cmds...

  protected:
    Database* database;
  };

}
#endif /* SQLProcessor_hpp */
