//
//  DBProcessor.hpp
//  RGAssignment2
//
//  Created by rick gessner on 2/27/21.
//

#ifndef DBProcessor_hpp
#define DBProcessor_hpp

#include <stdio.h>
#include <vector>
#include <fstream>
#include "CmdProcessor.hpp"
#include "Tokenizer.hpp"
#include "Database.hpp"
#include "FolderReader.hpp"
#include "FolderView.hpp"

namespace ECE141 {

  class Application;
  class DBProcessor : public CmdProcessor {
  public:
    
    DBProcessor(std::ostream &anOutput, Application* anApp);
    ~DBProcessor();
    
      //cmd processor interface...
    CmdProcessor* recognizes(Tokenizer &aTokenizer) override;
    Statement*    makeStatement(Tokenizer &aTokenizer,
                                StatusResult &aResult) override;
    StatusResult  run(Statement *aStmt) override {
      return aStmt->run(output);
    }

    bool          isKnown(Keywords aKeyword);

  protected:
      Application*      app;
  };
}
#endif /* DBProcessor_hpp */
