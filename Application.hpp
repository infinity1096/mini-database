//
//  AppProcessor.hpp
//  Database1
//
//  Created by rick gessner on 4/4/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#ifndef Application_hpp
#define Application_hpp

#include <stdio.h>
#include <stack>
#include "CmdProcessor.hpp"
#include "DBProcessor.hpp"

namespace ECE141 {

  class Application : public CmdProcessor {
  public:
    
    Application(std::ostream &anOutput);
    Application(const Application& other);
    Application& operator=(const Application& other);
    virtual ~Application();

      //app api...
            std::string   getVersion() {return "0.9";}
    
    virtual StatusResult  handleInput(std::istream &anInput);
            CmdProcessor* recognizes(Tokenizer &aTokenizer) override;
            Statement*    makeStatement(Tokenizer &aTokenizer,
                                        StatusResult &aResult) override;
            StatusResult  run(Statement *aStmt) override;
      

          StatusResult  createDatabase(const std::string &aName);
          StatusResult  dumpDatabase(const std::string &aName);
          StatusResult  dropDatabase(const std::string &aName);
          StatusResult  showDatabases() const;
          StatusResult  useDatabase(const std::string &aName);
          bool          dbExists(const std::string &aDBName);
          Database*     getDatabaseInUse();
      
          DBProcessor dbProcessor;
          FolderReader dbReader;

          void          pushCommandToStack(std::string command);
          std::ostream& output;


          private: 
          Database *db = nullptr;
          std::stack<std::string> unfinished_commands;
  };
  
}

#endif /* Application_hpp */
