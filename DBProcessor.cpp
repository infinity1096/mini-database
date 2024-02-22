//
//  DBProcessor.cpp
//  RGAssignment2
//
//  Created by rick gessner on 2/27/21.
//

#include "DBProcessor.hpp"

#include <map>
#include <fstream>
#include "Config.hpp"
#include "DBProcessor.hpp"
#include "FolderView.hpp"
#include "FolderReader.hpp"
#include "Database.hpp"
#include "Helpers.hpp"
#include "CommandView.hpp"
#include "TokenPattern.hpp"
#include "DBStatements.hpp"

namespace ECE141 {

  //like a typedef!!!
  using StmtFactory = Statement* (*)(DBProcessor *aProc,
                                   Tokenizer &aTokenize);

  DBProcessor::DBProcessor(std::ostream &anOutput, Application* anApp)
    : CmdProcessor(anOutput), app(anApp){}
  
  DBProcessor::~DBProcessor() {
  }

  bool DBProcessor::isKnown(Keywords aKeyword) {
      static Keywords theKnown[] =
      { Keywords::database_kw,Keywords::databases_kw, Keywords::create_kw, Keywords::use_kw,
        Keywords::drop_kw,Keywords::show_kw, Keywords::dump_kw};
      auto theIt = std::find(std::begin(theKnown),
          std::end(theKnown), aKeyword);
      return theIt != std::end(theKnown);
  }
 
  
  //CmdProcessor interface ...
  CmdProcessor* DBProcessor::recognizes(Tokenizer &aTokenizer) {
      if (isKnown(aTokenizer.current().keyword)) {
          return this;
      }
    return nullptr;
  }

  // USE: retrieve a statement based on given text input...
  Statement* DBProcessor::makeStatement(Tokenizer &aTokenizer,
                                        StatusResult &aResult) {
            
      TokenPattern aPattern(aTokenizer); // set the token array to be the source of pattern recognizer
      std::string identifier;
    
      DBStatement* statement = nullptr;
      DBStatement* statementTemp = nullptr;

      if (aTokenizer.more()){
          switch (aTokenizer.current().keyword){
            case Keywords::create_kw:
                statementTemp = new CreateDBStatement(app);
                if (statementTemp->parse(aTokenizer)){
                    statement = statementTemp;
                }
                break;
            case Keywords::drop_kw:
                statementTemp = new DropDBStatement(app);
                if (statementTemp->parse(aTokenizer)){
                    statement = statementTemp;
                }
                break;
            case Keywords::show_kw:
                statementTemp = new ShowDBStatement(app);
                if (statementTemp->parse(aTokenizer)){
                    statement = statementTemp;
                    break;
                }
                delete statementTemp;
                statementTemp = new ShowIndexesStatement(app);
                if (statementTemp->parse(aTokenizer)){
                    statement = statementTemp;
                    break;
                }
                delete statementTemp;
                statementTemp = new ShowIndexFromStatement(app);
                if (statementTemp->parse(aTokenizer)){
                    statement = statementTemp;
                    break;
                }
                break;
            case Keywords::use_kw:
                statementTemp = new UseDBStatement(app);
                if (statementTemp->parse(aTokenizer)){
                    statement = statementTemp;
                }
                break;
            case Keywords::dump_kw:
                statementTemp = new DumpDBStatement(app);
                if (statementTemp->parse(aTokenizer)){
                    statement = statementTemp;
                }
                break;
            default: break;
          }

          return statement;
      }else{
          if (statementTemp != nullptr) { delete statementTemp; }
          return nullptr;
      }
  }
}

