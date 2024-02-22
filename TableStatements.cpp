//
//  TableStatements.cpp
//  Assignment3
//
//  Created by Ivan Salazar on 4/16/22.
//

#include "TableStatements.hpp"
#include "Tokenizer.hpp"
#include "ParseHelper.hpp"
#include "CommandView.hpp"
namespace ECE141 {


//  CreateTableStatement Class defintion
// -----------------------------------------------------------------------------------------------//

    CreateTableStatement::CreateTableStatement(const CreateTableStatement &aCopy) : TableStatement(aCopy.db), parsed_entity(aCopy.parsed_entity){}

    CreateTableStatement::~CreateTableStatement() {
    }

    //USE: -------------------
    StatusResult CreateTableStatement::parse(Tokenizer &aTokenizer) {
        
        TokenPattern pattern(aTokenizer);
        
        // match "CREATE TABLE [identifier] ("
        std::string identifier; 
        if (!pattern.begins().equalsKeyword(Keywords::create_kw).equalsKeyword(Keywords::table_kw).containsIdentifier(identifier).equalsPunctuation('(')){
          return StatusResult{Errors::invalidArguments};
        }

        Entity entity(identifier);
        while (true){
          // parse attribute
          Attribute attribute;

          ParseHelper parseHelper(aTokenizer);
          if (parseHelper.parseAttribute(attribute)){
            entity.addAttribute(attribute);
          }else{
              break;
          }
        }

        if (entity.getAttributes().size() == 0){
          // entity parse fail
          return StatusResult{Errors::invalidAttribute};
        }

        // finally, we match ")" + ";"
        if (!pattern.begins().equalsPunctuation(')').equalsPunctuation(';')){
          return StatusResult{Errors::invalidAttribute};
        }

        parsed_entity = entity;
        return StatusResult{Errors::noError};
      }


    StatusResult CreateTableStatement::run(std::ostream &aStream) {
        
        if (parsed_entity.getName() != ""){ // FIXME: entity valid condition
            db->createTable(&parsed_entity);
        }

        return StatusResult(Errors::noError);
    }


//  DropTableStatement Class defintion
// -----------------------------------------------------------------------------------------------//
    
    DropTableStatement::DropTableStatement(const DropTableStatement &aCopy) : TableStatement(aCopy.db), table_name(aCopy.table_name) {}

    DropTableStatement::~DropTableStatement() {}

    StatusResult DropTableStatement::parse(Tokenizer &aTokenizer) {
        TokenPattern aPattern(aTokenizer);
        
        if (aPattern.begins().equalsKeyword(Keywords::drop_kw).equalsKeyword(Keywords::table_kw).containsIdentifier(table_name).equalsPunctuation(';')){
            return StatusResult{Errors::noError};
        }
        
        return StatusResult{Errors::invalidArguments};
    }

    StatusResult DropTableStatement::run(std::ostream &aStream) {
        return db->dropTable(table_name);
    }

//  ShowTablesStatement Class defintion
// -----------------------------------------------------------------------------------------------//
    
    ShowTablesStatement::ShowTablesStatement(const ShowTablesStatement &aCopy) : TableStatement(aCopy.db) {}

    ShowTablesStatement::~ShowTablesStatement() {}

    //USE: -------------------
    StatusResult ShowTablesStatement::parse(Tokenizer &aTokenizer) {
        TokenPattern aPattern(aTokenizer);
        
        if (aPattern.begins().equalsKeyword(Keywords::show_kw).equalsKeyword(Keywords::tables_kw)){
            aPattern.begins().equalsPunctuation(';');// FIXME: how to handle ; ? 
            return StatusResult{Errors::noError};
        }
        
        return StatusResult{Errors::invalidArguments};
    }

    StatusResult ShowTablesStatement::run(std::ostream &aStream) {
        return db->showTables();
    }
  
//  DescribeTableStatement Class defintion
// -----------------------------------------------------------------------------------------------//

    DescribeTableStatement::DescribeTableStatement(const DescribeTableStatement &aCopy) : TableStatement(aCopy.db), table_name(aCopy.table_name) {}

    DescribeTableStatement::~DescribeTableStatement() {}

    //USE: -------------------
    StatusResult DescribeTableStatement::parse(Tokenizer &aTokenizer) {
        TokenPattern aPattern(aTokenizer);
        
        if (aPattern.begins().equalsKeyword(Keywords::describe_kw).containsIdentifier(table_name).equalsPunctuation(';')){
            return StatusResult{Errors::noError};
        }
        
        return StatusResult{Errors::invalidArguments};
    }

    StatusResult DescribeTableStatement::run(std::ostream &aStream) {
        return db->describeTable(table_name);
    }

//  InsertTableStatement Class defintion
// -----------------------------------------------------------------------------------------------//

    InsertTableStatement::InsertTableStatement(const InsertTableStatement &aCopy) : TableStatement(aCopy.db), table_name(aCopy.table_name) {}

    InsertTableStatement::~InsertTableStatement() {}

    //USE: -------------------
    StatusResult InsertTableStatement::parse(Tokenizer &aTokenizer) {
        
        TokenPattern thePattern(aTokenizer);
        std::string theTableName;
        if(!thePattern.begins().nextIs({Keywords::insert_kw, Keywords::into_kw})
           .containsIdentifier(theTableName)                                      // identifier grabs the Table name
           .equalsPunctuation('(')){
               return StatusResult{Errors::invalidArguments};
        }
        
        
        // validate table is in active database
        if (!db->containsTable(theTableName)){
            return StatusResult{Errors::unknownTable};
        }
        
        table_name = theTableName;
        
        // parse fields
        ParseHelper parseHelper(aTokenizer);
        parseHelper.parseIdentifierList(row_fields);

        // match VALUES
        if(!thePattern.begins().equalsKeyword(Keywords::values_kw)){
            return StatusResult{Errors::invalidArguments};
        }

        while (true){
            // match "(", [string OR number], ..., [string OR number], ")"
            StringList stringList;
            if (!parseHelper.parseValueList(stringList)){
                break;
            }
            aTokenizer.skipIf(comma);
            row_values.push_back(stringList);
        }

        if (!thePattern.begins().equalsPunctuation(';')){
            return StatusResult{Errors::syntaxError};
        }

        // I will move the validation into insert statements. Here is a statement, 
        // which should be decopuled from parsing the type.



        return StatusResult{Errors::noError};
    }

    StatusResult InsertTableStatement::run(std::ostream &aStream) {
        return db->insertIntoTable(table_name, row_fields, row_values);
    }

//  SelectStatement Class defintion
// -----------------------------------------------------------------------------------------------//

    SelectStatement::SelectStatement(const SelectStatement &aCopy) : TableStatement(aCopy.db){}

    SelectStatement::~SelectStatement() {}

    StatusResult SelectStatement::parseWhereClause(Tokenizer& aTokenizer) {
        TokenPattern thePattern(aTokenizer);
        
        while (true){
            // Match [identifier / number] [comp_operator] [identifier / number]
            ExpressionPrototype prototype;
            if (thePattern.begins().equalsKeyword(Keywords::not_kw)){
                prototype.isInverted = true;
            }

            if (!thePattern.begins().containsNumberIdentifier(prototype.lhs).containsCompOperator(prototype.compOp).containsNumberIdentifier(prototype.rhs)){
                break;
            }

            if (!thePattern.begins().containsLogicalOperator(prototype.logicalOp)){        
                prototype.logicalOp = Logical::no_op;
            }

            select_query.addExpressionPrototype(prototype);

            if (thePattern.begins().equalsPunctuation(',')){continue;}
        }

        return StatusResult{ Errors::noError };
    }

    StatusResult SelectStatement::parseJoinClause(Tokenizer& aTokenizer){
        TokenPattern thePattern(aTokenizer);

        std::string table1,fieldName1;
        std::string table2,fieldName2;

        if (thePattern.containsIdentifier(join.prototype.other_field_name)
            .equalsKeyword(Keywords::on_kw)
            .containsIdentifier(table1)
            .equalsOperator(Operators::dot_op)
            .containsIdentifier(fieldName1)
            .equalsOperator(Operators::equal_op)
            .containsIdentifier(table2)
            .equalsOperator(Operators::dot_op)
            .containsIdentifier(fieldName2)
        ){
            if (table1 != select_query.table_name && table2 != select_query.table_name){
                return StatusResult{Errors::unknownTable};
            }

            if (table1 == select_query.table_name){
                join.prototype.table_name = table1;
                join.prototype.field_name = fieldName1;
                join.prototype.other_table_name = table2;
                join.prototype.other_field_name = fieldName2;
            }else{
                join.prototype.table_name = table2;
                join.prototype.field_name = fieldName2;
                join.prototype.other_table_name = table1;
                join.prototype.other_field_name = fieldName1;
            }

            join.enable();

            return StatusResult{Errors::noError};
        }

        return StatusResult{Errors::syntaxError};
    }

    StatusResult SelectStatement::parse(Tokenizer &aTokenizer){
        
        TokenPattern thePattern(aTokenizer);
        std::string theTableName;
        StringList theRowFields;
        int theLimit;
        std::string theOrderBy;
        StringList theExpression;

        // Initialize the select_query object
        select_query = SelectQuery();

        // 1. Match [SELECT]
        if(!thePattern.begins().equalsKeyword(Keywords::select_kw)){
            return StatusResult{Errors::invalidArguments};
        }

        // 2. Match [field_1], [field_2], ... OR [*]
        if (thePattern.begins().equalsOperator(Operators::multiply_op)){
            
            select_query.select_all = true; // TODO: change to setter
        }else{
            ParseHelper theParseHelper(aTokenizer);
            theParseHelper.parseIdentifierList(theRowFields);
            
            select_query.selected_fields = theRowFields; // TODO: change to setter
        }

        // 3. Match [FROM] [table_name]
        if (thePattern.begins().equalsKeyword(Keywords::from_kw).containsIdentifier(theTableName)){
            if (!db->containsTable(theTableName)) {
                return StatusResult{ Errors::unknownTable };
            }

            select_query.table_name = theTableName; // TODO: change to setter
        }

        // Parse the clauses
        while (true) {

            // Match [;]
            if (thePattern.begins().equalsPunctuation(';')) {
                return StatusResult{ Errors::noError };
            }

            // Match [LIMIT] [num_limit]
            if (thePattern.begins()
                .equalsKeyword(Keywords::limit_kw)
                .containsNumber(theLimit)) {
                select_query.setLimit(theLimit);
                continue;
            }

            // Match [ORDER] [BY] [field_name]
            if (thePattern.begins()
                .equalsKeyword(Keywords::order_kw)
                .equalsKeyword(Keywords::by_kw)
                .containsIdentifier(theOrderBy)) {
                select_query.setOrderBy(theOrderBy);
                continue;
            }

            // Match [WHERE] [expression_1], [expression_2], ...
            if (thePattern.begins().equalsKeyword(Keywords::where_kw)){
                parseWhereClause(aTokenizer);
                continue;
            }

            if (thePattern.begins().containsLeftRight(join.prototype.joinType).equalsKeyword(Keywords::join_kw)){
                parseJoinClause(aTokenizer);
                continue;
            }

            return StatusResult{ Errors::noError};
         }
    }

    StatusResult SelectStatement::run(std::ostream& anOutput){
        RowCollection theRows;
        Entity entity = db->getEntity(select_query.table_name);
        select_query.parseExpression(entity);

        if (!select_query.verify(entity)){
            return StatusResult{Errors::invalidArguments};
        }

        db->sqlProcessor.showQuery(select_query, join, theRows);
     
        return StatusResult{Errors::noError};
    }
    
    StatusResult  UpdateStatement::parse(Tokenizer &aTokenizer){
        TokenPattern thePattern(aTokenizer);

        // 1. Match [UPDATE] [table_name] [SET]
        if(!thePattern.begins()
            .equalsKeyword(Keywords::update_kw)
            .containsIdentifier(select_query.table_name)
            .equalsKeyword(Keywords::set_kw)){
            return StatusResult{Errors::invalidArguments};
        }

        // verify table exists
        if (!db->containsTable(select_query.table_name)){
            return StatusResult{Errors::unknownTable};
        }

        // 2. Match [field_name] [=] [value]
        if (!thePattern.begins()
            .containsIdentifier(field_name)
            .equalsOperator(Operators::equal_op)
            .containsNumberIdentifier(value)){
            return StatusResult{Errors::syntaxError};
        }

        // 3. Match [WHERE] + Clauses
        if (thePattern.begins().equalsKeyword(Keywords::where_kw)){
            parseWhereClause(aTokenizer);
        }else{
            return StatusResult{Errors::syntaxError};
        }

        // 4. Match [;]
        if (!thePattern.begins().equalsPunctuation(';')){
            return StatusResult{Errors::syntaxError};
        }

        return StatusResult{Errors::noError};
    }

    StatusResult  UpdateStatement::run(std::ostream &aStream){
        
        Entity entity = db->getEntity(select_query.table_name);
        select_query.parseExpression(entity);

        // verify field_name exists in entity
        Attribute theAttribute;
        size_t attributeIndex = 0;

        if(entity.getAttribute(field_name) == std::nullopt){
            return StatusResult{Errors::unknownAttribute};
        }else{
            theAttribute = entity.getAttribute(field_name).value();
            attributeIndex = entity.getAttributeIndex(theAttribute.getName()).value();
        }

        // cast value from string into RowCell according to entity
        std::optional<RowCell> modifiedCell = createRowCellFromInput(theAttribute.getType(), value);

        if (modifiedCell == std::nullopt){
            return StatusResult{Errors::invalidArguments};
        }

        RowCell toInsert = modifiedCell.value();

        // go through rows using filters and update  
        size_t updateCount = db->updateRows(select_query, toInsert, attributeIndex);

        CommandView theview;
        theview.showAffect(db->getAppOutput(), updateCount);

        return StatusResult{Errors::noError};
    }

    StatusResult  DeleteStatement::parse(Tokenizer &aTokenizer) {
        TokenPattern thePattern(aTokenizer);

        // 1. Match [DELETE] [FROM] [table_name]
        if(!thePattern.begins()
            .equalsKeyword(Keywords::delete_kw)
            .equalsKeyword(Keywords::from_kw)
            .containsIdentifier(select_query.table_name)){
            return StatusResult{Errors::invalidArguments};
        }

        // verify table exists
        if (!db->containsTable(select_query.table_name)){
            return StatusResult{Errors::unknownTable};
        }

        // 2. Match [WHERE] + Clause
        if (thePattern.begins().equalsKeyword(Keywords::where_kw)){
            parseWhereClause(aTokenizer);
        }else{
            return StatusResult{Errors::syntaxError};
        }

        // 3. Match [;]
        if (!thePattern.begins().equalsPunctuation(';')){
            return StatusResult{Errors::syntaxError};
        }

        return StatusResult{Errors::noError};
    }

    StatusResult  DeleteStatement::run(std::ostream &aStream) {
        Entity entity = db->getEntity(select_query.table_name);
        select_query.parseExpression(entity);

        size_t deleteCount = db->deleteRows(select_query);
        
        CommandView theView;
        theView.showAffect(db->getAppOutput(), deleteCount);

        return StatusResult{ Errors::noError };
    }
        
}
