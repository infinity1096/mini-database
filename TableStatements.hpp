//
//  TableStatement.hpp
//  Assignment3
//
//  Created by Ivan Salazar on 4/16/22.
//

#ifndef TableStatements_hpp
#define TableStatements_hpp

#include <stdio.h>
#include "Statement.hpp"
#include "Entity.hpp"
#include "Database.hpp"
#include "TokenPattern.hpp"
#include "Row.hpp"
#include "Query.hpp"
#include "Join.hpp"

namespace ECE141 {
    
    class TableStatement : public Statement{
    public:
        TableStatement(Database* aDB) : db(aDB) {}
                
        Database* db;
    };


// -----------------------------------------------------------------------------------------------//
    class CreateTableStatement : public TableStatement{
    public:      
      CreateTableStatement(Database* aDB) : TableStatement(aDB) {};
      CreateTableStatement(const CreateTableStatement &aCopy);
      
      virtual                 ~CreateTableStatement();
      
      virtual   StatusResult  parse(Tokenizer& aTokenizer) override;
      
      virtual   const char*   getStatementName() const override {return "create_table_statement";}
      virtual   StatusResult  run(std::ostream &aStream) override;
      
      Entity                  getEntity() { return parsed_entity; }

    protected:
      Entity parsed_entity;
      std::string tableName;
    };

// -----------------------------------------------------------------------------------------------//

    class DropTableStatement : public TableStatement{
    public:
       
        DropTableStatement(Database* aDb) : TableStatement(aDb) {};
        DropTableStatement(const DropTableStatement &aCopy);
      
      virtual                 ~DropTableStatement();
      
      virtual   StatusResult  parse(Tokenizer &aTokenizer) override;
      
      
      virtual   const char*   getStatementName() const override {return "drop_table_statement";}
      virtual   StatusResult  run(std::ostream &aStream) override;
      
    protected:
      std::string table_name;
    };

// -----------------------------------------------------------------------------------------------//

    class ShowTablesStatement : public TableStatement{
    public:
        
        ShowTablesStatement(Database* aDb) : TableStatement(aDb) {};
        ShowTablesStatement(const ShowTablesStatement &aCopy);
      
      virtual                 ~ShowTablesStatement();
      
      virtual   StatusResult  parse(Tokenizer &aTokenizer) override;
      
      
      virtual   const char*   getStatementName() const override{return "show_tables_statement";}
      virtual   StatusResult  run(std::ostream &aStream) override;
      
    protected:
      
    };
// -----------------------------------------------------------------------------------------------//
    class DescribeTableStatement : public TableStatement{
    public:
        
        DescribeTableStatement(Database* aDb) : TableStatement(aDb) {};
        DescribeTableStatement(const DescribeTableStatement &aCopy);
      
      virtual                 ~DescribeTableStatement();
      
      virtual   StatusResult  parse(Tokenizer &aTokenizer) override;
      
      virtual   const char*   getStatementName() const override{return "describe_table_statement";}
      virtual   StatusResult  run(std::ostream &aStream) override;
      
    protected:
      std::string table_name;
    };

    class InsertTableStatement : public TableStatement{
    public:
        
        InsertTableStatement(Database* aDb) : TableStatement(aDb) {};
        InsertTableStatement(const InsertTableStatement &aCopy);
      
      virtual                 ~InsertTableStatement();
      
      virtual   StatusResult  parse(Tokenizer &aTokenizer) override;
      
      virtual   const char*   getStatementName() const override{return "insert_table_statement";}
      virtual   StatusResult  run(std::ostream &aStream) override;
      
      using StringList = std::vector<std::string>;

        StringList                  row_fields;
        std::vector<StringList>     row_values;
        std::string                 table_name;
    
      
     
    };

    class SelectStatement : public TableStatement{
    public:
        
        SelectStatement(Database* aDB) : TableStatement(aDB) , select_query(){};
        SelectStatement(const SelectStatement &aCopy);
        
        
        virtual                 ~SelectStatement();
        
        virtual StatusResult parse(Tokenizer &aTokenizer) override;
        StatusResult parseWhereClause(Tokenizer& aTokenizer);
        StatusResult parseJoinClause(Tokenizer& aTokenizer); 
        
        virtual const char* getStatementName() const override {return "select_statement";}
        virtual StatusResult run(std::ostream &aStream) override;

        using StringList = std::vector<std::string>;
        
        
        // using the SelectQuery Object to store all state info from the Select Command
        SelectQuery         select_query;
        Join                join;
    };

    class UpdateStatement : public SelectStatement{
    public:
        
        UpdateStatement(Database* aDB) : SelectStatement(aDB) {};
        virtual ~UpdateStatement() override {};
                  
        virtual StatusResult  parse(Tokenizer &aTokenizer) override;
  
        virtual const char*   getStatementName() const override {return "update_statement";}
        virtual StatusResult  run(std::ostream &aStream) override;
    
    private:
        std::string field_name;
        std::string value;
    };

    class DeleteStatement : public SelectStatement{
    public:
        
        DeleteStatement(Database* aDB) : SelectStatement(aDB) {};
        virtual ~DeleteStatement() override {};
                  
        virtual StatusResult  parse(Tokenizer &aTokenizer) override;
  
        virtual const char*   getStatementName() const override {return "update_statement";}
        virtual StatusResult  run(std::ostream &aStream) override;
    
    private:
    };
    
}

#endif




