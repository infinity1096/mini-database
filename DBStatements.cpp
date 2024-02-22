//
//  DBStatements.cpp
//  Assignment3
//
//  Created by Ivan Salazar on 4/16/22.
//

#include "DBStatements.hpp"
#include "Tokenizer.hpp"
#include "TokenPattern.hpp"
namespace ECE141 {


//  CreateDBStatment Class defintion
// -----------------------------------------------------------------------------------------------//
    StatusResult  CreateDBStatement::parse(Tokenizer& aTokenizer){
        TokenPattern thePattern(aTokenizer);
        if (thePattern.begins()
            .equalsKeyword(Keywords::create_kw)
            .equalsKeyword(Keywords::database_kw)
            .containsIdentifier(db_name)
            .equalsPunctuation(';')
        ){
            return StatusResult{Errors::noError};
        }
        return StatusResult{Errors::invalidArguments};
    }
    
    StatusResult CreateDBStatement::run(std::ostream &aStream) {

        return dbInterpreter->createDatabase(db_name);
    }


//  DropDBStatement Class defintion
// -----------------------------------------------------------------------------------------------//
    StatusResult  DropDBStatement::parse(Tokenizer& aTokenizer){
        TokenPattern thePattern(aTokenizer);
        if (thePattern.begins()
            .equalsKeyword(Keywords::drop_kw)
            .equalsKeyword(Keywords::database_kw)
            .containsIdentifier(db_name)
            .equalsPunctuation(';')
        ){
            return StatusResult{Errors::noError};
        }
        return StatusResult{Errors::invalidArguments};
    }
    StatusResult DropDBStatement::run(std::ostream &aStream) {
    //return commandable.runStatement(*this);
        return dbInterpreter->dropDatabase(db_name);
    }

//  ShowDBStatement Class defintion
// -----------------------------------------------------------------------------------------------//
    StatusResult  ShowDBStatement::parse(Tokenizer& aTokenizer){
        TokenPattern thePattern(aTokenizer);
        if (thePattern.begins()
            .equalsKeyword(Keywords::show_kw)
            .equalsKeyword(Keywords::databases_kw)
            .equalsPunctuation(';')
        ){
            return StatusResult{Errors::noError};
        }
        return StatusResult{Errors::invalidArguments};
    }
    StatusResult ShowDBStatement::run(std::ostream &aStream) {
    //return commandable.runStatement(*this);
        return dbInterpreter->showDatabases();
    }
  
//  DumpDBStatement Class defintion
// -----------------------------------------------------------------------------------------------//
    StatusResult  DumpDBStatement::parse(Tokenizer& aTokenizer){
        TokenPattern thePattern(aTokenizer);
        if (thePattern.begins()
            .equalsKeyword(Keywords::dump_kw)
            .equalsKeyword(Keywords::database_kw)
            .containsIdentifier(db_name)
            .equalsPunctuation(';')
        ){
            return StatusResult{Errors::noError};
        }
        return StatusResult{Errors::invalidArguments};
    }

    StatusResult DumpDBStatement::run(std::ostream &aStream) {
    //return commandable.runStatement(*this);
        return dbInterpreter->dumpDatabase(db_name);
    }


    StatusResult  UseDBStatement::parse(Tokenizer& aTokenizer){
        TokenPattern thePattern(aTokenizer);
        if (thePattern.begins()
            .equalsKeyword(Keywords::use_kw)
            .containsIdentifier(db_name)
            .equalsPunctuation(';')
        ){
            return StatusResult{Errors::noError};
        }
        return StatusResult{Errors::invalidArguments};
    }

    StatusResult UseDBStatement::run(std::ostream &aStream) {
    //return commandable.runStatement(*this);
        return dbInterpreter->useDatabase(db_name);

    }

    // -----------------------------------------------------

    StatusResult ShowIndexesStatement::parse(Tokenizer& aTokenizer){
        TokenPattern thePattern(aTokenizer);
        if (thePattern.begins()
            .equalsKeyword(Keywords::show_kw)
            .equalsKeyword(Keywords::indexes_kw)
            .equalsPunctuation(';')
        ){
            return StatusResult{Errors::noError};
        }
        return StatusResult{Errors::invalidArguments};
    }

    StatusResult ShowIndexesStatement::run(std::ostream& aStream){
        return dbInterpreter->getDatabaseInUse()->showIndexes();
    }

    // ------------------------------------------------------

    StatusResult ShowIndexFromStatement::parse(Tokenizer& aTokenizer){
        TokenPattern thePattern(aTokenizer);
        if (thePattern.begins()
            .equalsKeyword(Keywords::show_kw)
            .equalsKeyword(Keywords::index_kw)
            .containsIdentifier(field)
            .equalsKeyword(Keywords::from_kw)
            .containsIdentifier(table_name)
            .equalsPunctuation(';')
        ){
            return StatusResult{Errors::noError};
        }
        return StatusResult{Errors::invalidArguments};
    }

    StatusResult ShowIndexFromStatement::run(std::ostream& aStream){
        return dbInterpreter->getDatabaseInUse()->showIndex(table_name, field);
    }

}
