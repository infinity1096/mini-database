//
//  SQLProcessor.cpp
//  PA3
//
//  Created by rick gessner on 4/2/22.
//

#include "SQLProcessor.hpp"
#include "TokenPattern.hpp"
#include "Attribute.hpp"
#include "keywords.hpp"
#include "TableStatements.hpp"
#include "TabularView.hpp"
#include <algorithm>
#include <vector>

namespace ECE141 {

  using StmtFactory = Statement* (*)(SQLProcessor *aProc,
                                     Tokenizer &aTokenize);
  SQLProcessor::SQLProcessor(std::ostream &anOutput, Database* aDataBase)
    :  CmdProcessor(anOutput), database(aDataBase) {
  }

  SQLProcessor::~SQLProcessor() {}
  
  // cookie cutter method shared across different processor, known keywords are different
  bool SQLProcessor::isKnown(Keywords aKeyword) {
    static Keywords theKnown[] =
    {Keywords::create_kw,Keywords::drop_kw,Keywords::describe_kw,Keywords::show_kw,
        Keywords::table_kw, Keywords::tables_kw};
    auto theIt = std::find(std::begin(theKnown),
        std::end(theKnown), aKeyword);
    return theIt != std::end(theKnown);
  }

  // cookie cutter method shared across all processors
  CmdProcessor* SQLProcessor::recognizes(Tokenizer &aTokenizer) {
      if (isKnown(aTokenizer.current().keyword)) {
          return this;
      }
    return nullptr;
  }

  Statement* SQLProcessor::makeStatement(Tokenizer &aTokenizer, StatusResult &aResult) {
    Statement* statement = nullptr;
    Statement* statementTemp;

    switch (aTokenizer.current().keyword){
      case Keywords::create_kw: 
        statementTemp = new CreateTableStatement(database);
        if (statementTemp->parse(aTokenizer) == StatusResult{Errors::noError}){
          statement = statementTemp;
        } 
        break;
      case Keywords::drop_kw:
        statementTemp = new DropTableStatement(database);
        if (statementTemp->parse(aTokenizer) == StatusResult{Errors::noError}){
          statement = statementTemp;
        } 
        break;
      case Keywords::show_kw:
        statementTemp = new ShowTablesStatement(database);
        if (statementTemp->parse(aTokenizer) == StatusResult{Errors::noError}){
          statement = statementTemp;
        } 
        break;
      case Keywords::describe_kw:
        statementTemp = new DescribeTableStatement(database);
        if (statementTemp->parse(aTokenizer) == StatusResult{Errors::noError}){
          statement = statementTemp;
        }
        break;
        case Keywords::insert_kw:
            statementTemp = new InsertTableStatement(database);
            if(statementTemp->parse(aTokenizer) == StatusResult{Errors::noError}){
                statement = statementTemp;
            }
            break;
        case Keywords::select_kw:
             statementTemp = new SelectStatement(database);
            if(statementTemp->parse(aTokenizer) == StatusResult{Errors::noError}){
                statement = statementTemp;
            }
        break;
        case Keywords::update_kw:
             statementTemp = new UpdateStatement(database);
            if(statementTemp->parse(aTokenizer) == StatusResult{Errors::noError}){
                statement = statementTemp;
            }
        break;
        case Keywords::delete_kw:
             statementTemp = new DeleteStatement(database);
            if(statementTemp->parse(aTokenizer) == StatusResult{Errors::noError}){
                statement = statementTemp;
            }
        break;
      default:
        break;
    }

    return statement;
  }

  StatusResult SQLProcessor::showQuery(const SelectQuery& aQuery, Join& aJoin, RowCollection& aCollection) {
     
      const std::string theTableName = aQuery.table_name;
      Entity theEntity = database->getEntity(theTableName);
      std::ostream& theOutput = database->getAppOutput();

      // process filter
      RowCollection filteredCollection;
      
      // process join
      if (aJoin.enabled){
        Entity otherEntity = database->getEntity(aJoin.prototype.other_table_name);

        if (!aJoin.verify(theEntity, otherEntity)){
          return StatusResult{ Errors::invalidArguments };
        }

        // execute join
        SelectQuery selfRowsQuery;
        selfRowsQuery.table_name = theTableName;
        selfRowsQuery.select_all = true;
        
        SelectQuery otherRowsQuery;
        otherRowsQuery.table_name = aJoin.other_table_name;
        otherRowsQuery.select_all = true;
        
        RowCollection selfCollection;
        RowCollection otherCollection;

        database->selectRows(selfRowsQuery, selfCollection);
        database->selectRows(otherRowsQuery, otherCollection);
        
        JoinResult* result = aJoin.executeJoin(theEntity, otherEntity, selfCollection, otherCollection);

        if (!static_cast<StatusResult>(*result)){
          delete result;
          return StatusResult{ Errors::invalidArguments };
        }

        // filter according to filter
        // FIXME: what to do in this case? 

        filteredCollection = std::move(result->collection);
        theEntity = result->entity;

        delete result;
      }else{
        database->selectRows(aQuery, filteredCollection);
      }

      

      // process Order By
      if (aQuery.order_by != std::nullopt){
        size_t orderByIndex = theEntity.getAttributeIndex(aQuery.order_by.value()).value();
        std::sort(filteredCollection.begin(), filteredCollection.end(), 
        [=](const std::unique_ptr<Row>& lhs, const std::unique_ptr<Row>& rhs){
          RowCell lhsCell = (*lhs)[orderByIndex];
          RowCell rhsCell = (*rhs)[orderByIndex];

          return compareToExpression(lhsCell, rhsCell) < 0;
        });
      }

      // process limit
      RowCollection limitedCollection;
      if (aQuery.limit != std::nullopt){
        if (aQuery.limit.value() < filteredCollection.size()){
          filteredCollection.erase(filteredCollection.begin() + aQuery.limit.value(), filteredCollection.end());
        }
      }

      // process fields selected FIXME: not implemented
      if (!aQuery.select_all){
        Entity entityForView = theEntity;
        entityForView.resetAttribute();
        std::vector<size_t> selectedFieldIndex;
        for(auto fieldName : aQuery.selected_fields){
          selectedFieldIndex.push_back(theEntity.getAttributeIndex(fieldName).value()); // aQuery is verified
          entityForView.addAttribute(theEntity.getAttribute(fieldName).value());
        }

        RowCollection collectionForView;
        for (auto& row : filteredCollection){
          Row aRow;

          for (auto fieldIndex : selectedFieldIndex){
            aRow.addContent((*row)[fieldIndex]);
          }

          collectionForView.push_back(std::make_unique<Row>(aRow));
        } 

        TabularView theRowView(theOutput, entityForView, collectionForView);
        theRowView.show(theOutput);

      }else{
        TabularView theRowView(theOutput, theEntity, filteredCollection);
        theRowView.show(theOutput);
      }

      return StatusResult{ Errors::noError };
  }

}

