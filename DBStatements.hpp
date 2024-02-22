//
//  databaseStatement.hpp
//  Assignment3
//
//  Created by Ivan Salazar on 4/16/22.
//

#ifndef DBStatements_hpp
#define DBStatements_hpp

#include <stdio.h>
#include "Statement.hpp"
#include "Application.hpp"
#include "DBProcessor.hpp"

namespace ECE141 {

    class DBStatement : public Statement {
    public:
        DBStatement(Application* anApp, std::string aName = "") : dbInterpreter(anApp) {}

        Application* dbInterpreter;
    };

    // -----------------------------------------------------------------------------------------------//
    class CreateDBStatement : public DBStatement {
    public:
        CreateDBStatement(Application* anApp = nullptr, std::string aName = "") : DBStatement(anApp, aName) {};

        virtual   StatusResult  parse(Tokenizer& aTokenizer) override;
        virtual   StatusResult  run(std::ostream& aStream) override;

        std::string db_name;
    };

    // -----------------------------------------------------------------------------------------------//

    class DropDBStatement : public DBStatement {
    public:
        DropDBStatement(Application* anApp = nullptr, std::string aName = "") : DBStatement(anApp, aName) {};

        virtual   StatusResult  parse(Tokenizer& aTokenizer) override;
        virtual   StatusResult  run(std::ostream& aStream) override;

        std::string db_name;
    };

    // -----------------------------------------------------------------------------------------------//

    class ShowDBStatement : public DBStatement {
    public:
        ShowDBStatement(Application* anApp = nullptr)
            : DBStatement(anApp) {}

        virtual   StatusResult  parse(Tokenizer& aTokenizer) override;
        virtual   StatusResult  run(std::ostream& aStream) override;

    };

    class UseDBStatement : public DBStatement {
    public:
        UseDBStatement(Application* anApp = nullptr, std::string aName = "") : DBStatement(anApp, aName) {};

        virtual   StatusResult  parse(Tokenizer& aTokenizer) override;
        virtual   StatusResult  run(std::ostream& aStream) override;

        std::string db_name;
    };

    // -----------------------------------------------------------------------------------------------//
    class DumpDBStatement : public DBStatement {
    public:
        DumpDBStatement(Application* anApp = nullptr, std::string aName = "") : DBStatement(anApp, aName) {};

        virtual   StatusResult  parse(Tokenizer& aTokenizer) override;
        virtual   StatusResult  run(std::ostream& aStream) override;

        std::string db_name;
    };

    // ---------------------------------------------------------------------------------

    class ShowIndexesStatement : public DBStatement {
    public:
        ShowIndexesStatement(Application* anApp = nullptr, std::string aName = "") : DBStatement(anApp, aName) {};

        virtual   StatusResult  parse(Tokenizer& aTokenizer) override;
        virtual   StatusResult  run(std::ostream& aStream) override;
    };

    // ---------------------------------------------------------------------------------------

    class ShowIndexFromStatement : public DBStatement {
    public:
        ShowIndexFromStatement(Application* anApp = nullptr, std::string aName = "") : DBStatement(anApp, aName) {};

        virtual   StatusResult  parse(Tokenizer& aTokenizer) override;
        virtual   StatusResult  run(std::ostream& aStream) override;

        std::string field;
        std::string table_name;
    };

}

#endif




