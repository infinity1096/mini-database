//
//  CommandProcessor.cpp
//
//  Created by rick gessner on 3/30/20
//  Copyright © 2018 rick gessner. All rights reserved.
//

#include <iostream>
#include "Application.hpp"
#include "Tokenizer.hpp"
#include "CommandView.hpp"
#include <memory>
#include <algorithm>
#include <vector>
#include "Config.hpp"
#include "CommandView.hpp"
#include "ScriptStatement.hpp"

#include "Storage.hpp"

namespace ECE141 {
  
  size_t Config::cacheSize[]={0,0,0};

  Application::Application(std::ostream &anOutput) 
    : CmdProcessor(anOutput), dbProcessor(anOutput, this),
    dbReader(Config::getStoragePath()), output(anOutput){
  }


  Application::Application(const Application& other) : CmdProcessor(other.output), dbProcessor(other.output, this), dbReader(Config::getStoragePath()),output(other.output) {
      *this = other;
  }

  Application& Application::operator=(const Application& other) {
      db = other.db;
      unfinished_commands = other.unfinished_commands;
      return *this;
  }
  
  Application::~Application() {
      if (db != nullptr) {
          delete db;
          db = nullptr;
      }
  }
  
  // USE: -----------------------------------------------------
  
  bool isKnown(Keywords aKeyword) {
    static Keywords theKnown[]=
      {Keywords::quit_kw,Keywords::version_kw, Keywords::help_kw, Keywords::script_kw};
    auto theIt = std::find(std::begin(theKnown),
                           std::end(theKnown), aKeyword);
    return theIt!=std::end(theKnown);
  }

  CmdProcessor* Application::recognizes(Tokenizer &aTokenizer) {
    if(isKnown(aTokenizer.current().keyword)) {
      return this;
    }
    return nullptr;
  }

  StatusResult Application::run(Statement* aStatement) {
    switch(aStatement->getType()) {
      case Keywords::quit_kw:
        output << "DB::141 is shutting down\n";
        return StatusResult(ECE141::userTerminated);
        break;
      case Keywords::version_kw:
        output << "Version " << getVersion() << "\n"; break;
      case Keywords::help_kw:
        output << "Help system available\n"; break;
      case Keywords::script_kw:
        aStatement->run(output);
        break;
      default:
        aStatement->run(output);
        break;

    }
    
    return StatusResult{Errors::noError};
  }
  
  // USE: retrieve a statement based on given text input...
  Statement* Application::makeStatement(Tokenizer &aTokenizer,
                                        StatusResult &aResult) {
    Token theToken=aTokenizer.current();
    if (isKnown(theToken.keyword)) {
      if (theToken.keyword != Keywords::script_kw){
        aTokenizer.next(); //skip ahead...
        return new Statement(theToken.keyword);
      }else{
        ScriptStatement* statement = new ScriptStatement(this);
        if (statement->parse(aTokenizer)){
          return statement;
        }
        delete statement;
        return nullptr;
      }
    }

    return nullptr;
  }
    Database* Application::getDatabaseInUse() {
        return db;
    }

    bool Application::dbExists(const std::string &aDBName) {
        bool theDBExists = false;
        dbReader.each(Config::getDBExtension(), [&](const std::string& aName)-> bool {      // closure
            if (aDBName == aName) { theDBExists = true; return true; }
            else { return false; }
            });
        return theDBExists;
    }


    StatusResult Application::showDatabases() const {
        FolderView theView(Config::getStoragePath(), Config::getDBExtension());
        theView.show(output);
        return StatusResult{ Errors::noError };
    }

    // USE: DB dump all storage blocks
    StatusResult Application::dumpDatabase(const std::string &aName)  {
        if (dbExists(aName)) {
            CommandView theCmdView;
            
            std::string dbPath = Config::getDBPath(aName);

            if (db != nullptr && db->getName() == aName) {
                theCmdView.showDump(db, output);
            }
            else {
                Database* tempDB = new Database(aName, dbPath, this);
                theCmdView.showDump(tempDB, output);
                delete tempDB;
            }

            return StatusResult{ Errors::noError };
        }
        else {
            output << "This Database does not exist" << "\n";
        }
      return StatusResult{ECE141::unknownDatabase};
    }

    // USE: call DB object to be loaded into memory...
    StatusResult Application::useDatabase(const std::string &aName) {
      if (dbExists(aName)) {
          std::string dbPath = Config::getDBPath(aName);

          if (db != nullptr){
            delete db;
            db = nullptr;
          }

          db = new Database(aName, dbPath, this);

          output << "Database Changed\n";
          return StatusResult{ Errors::noError };
      }
      else {
          output << "This Database does not exist" << "\n";
      }

      return StatusResult{ECE141::unknownDatabase};
    }

    

  //build a tokenizer, tokenize input, ask processors to handle...
  StatusResult Application::handleInput(std::istream &anInput){

    std::unique_ptr<Tokenizer> theTokenizer;
    std::stringstream ss;

    if (unfinished_commands.size() > 0){
      ss << unfinished_commands.top();
      unfinished_commands.pop();
      theTokenizer = std::make_unique<Tokenizer>(ss);
    }else{
      theTokenizer = std::make_unique<Tokenizer>(anInput);
    }

    StatusResult theResult=theTokenizer->tokenize();
    
    while (theResult && theTokenizer->more()) {
      Config::getTimer().reset();
      if(auto *theProc=recognizes(*theTokenizer)) {
        if(auto *theCmd=theProc->makeStatement(*theTokenizer,theResult)) {
          theResult=theProc->run(theCmd);
          if(theResult) theTokenizer->skipIf(';');
          delete theCmd;
          continue;
        }
      }
      
      if (auto *theProc = dbProcessor.recognizes(*theTokenizer)) {
        Statement* theCommand = nullptr;
          if ((theCommand = theProc->makeStatement(*theTokenizer, theResult)) != nullptr) {
              theResult = run(theCommand);
             
              delete theCommand;
              continue;
          }

      }
      
      if(getDatabaseInUse() != nullptr){
          if (auto* theCmd = getDatabaseInUse()->sqlProcessor.makeStatement(*theTokenizer, theResult)) {
              theResult = run(theCmd);
              delete theCmd;
              continue;
          }
      }
  
      theResult=unknownCommand;
      theTokenizer->reset();
          
    }
    return theResult;
  }

    StatusResult Application::createDatabase(const std::string &aName) {
        if (!dbExists(aName)) {
            CommandView theCmdView;
            Storage temp(Config::getDBPath(aName), AsNew{});
            theCmdView.showCreate(output);
            return StatusResult(Errors::noError);
        }
        else { output << "This Database already exists" << "\n"; }
      return StatusResult{Errors::databaseExists};
    }

    // USE: call this to perform the dropping of a database (remove the file)...
    StatusResult Application::dropDatabase(const std::string &aName) {
        if (dbExists(aName)) {
            CommandView theCmdView;
            std::string dbPath = Config::getDBPath(aName);

            std::cout << "Dropped database " << aName << "\n";

            // if the database is in use, then we need to close it
            if (db != nullptr && db->getName() == aName){
              delete db;
              db = nullptr;
            }

            std::filesystem::remove(dbPath.c_str());
            theCmdView.showDrop(output);
            return StatusResult(Errors::noError);
        }
        else {
            output << "This Database does not exist" << "\n";
        }
      return StatusResult{unknownDatabase};
    }

    void Application::pushCommandToStack(std::string command){
      unfinished_commands.push(command);
    }

}
