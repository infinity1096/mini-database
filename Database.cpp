//
//  Database.cpp
//  RGAssignment2
//
//  Created by rick gessner on 2/27/21.
//

#include <string>
#include <iostream>
#include <iomanip>
#include <map>
#include "Database.hpp"
#include "Config.hpp"
#include <iostream>
#include "Application.hpp"
#include "Helpers.hpp"
#include "TabularView.hpp"
#include "Filter.hpp"
namespace ECE141 {
  
  Database::Database(std::string aName, const std::string& aPath, Application* anApp)
      : sqlProcessor(anApp->output, this), name(aName), changed(true), app(anApp), storage(aPath, AsExisting{}) {

  }

  Database::~Database() {
    // stream.close();
  }

std::string lineSpace(std::string aLine){
    size_t theMaxLine = 22;
    size_t theLineSize = aLine.size();
    for(size_t i = theLineSize; i < theMaxLine; i++){
        aLine += " ";
    }
    aLine += "|\n";
    return aLine;
}

  // USE: Call this to dump the db for debug purposes...
  StatusResult Database::dump(std::ostream &anOutput) {   
    Timer timer;
    timer.reset(); 
    storage.debugDump();
    app->output <<  storage.size() << " rows in set ( " << timer.elapsed() <<  " secs)\n";
    return StatusResult{noError};
  }

StatusResult Database::createTable(Entity *anEntity){
  Timer timer;
  timer.reset();

  if (anEntity == nullptr){
    return StatusResult{Errors::invalidArguments};
  }

  storage.saveEntity(*anEntity);
  
  // must success given a entity
  app->output << "Query OK, 1 rows affected (" << timer.elapsed() << " secs)\n";

  return StatusResult{Errors::noError};
}

StatusResult  Database::describeTable(const std::string &aName){
  Timer timer;
  timer.reset();
  
  
  auto entityNameList = storage.listEntity();
  if (std::find(entityNameList.begin(), entityNameList.end(), aName) != entityNameList.end()){
    // table exists
    Entity* entity = storage.loadEntity(aName);
    AttributeList attributes = entity->getAttributes();
    delete entity;

    std::vector<size_t> cellLength = {20, 20, 4, 4, 30};
    std::vector<std::string> headers = {"Field", "Type", "Null", "Key", "Extra"};
    
    // print header
    std::string hline = "";
    for (size_t length : cellLength){
      hline += "+";
      hline += std::string(length, '-');
    }
    hline += "+\n";

    std::cout << hline;
    printLine(std::cout, headers, cellLength);
    std::cout << hline;
    
    // print attributes and hline
    for (auto attr : attributes){
      std::vector<std::string> fields;
      // display name
      fields.push_back(attr.getName());
      
      // display datatype
      std::string typeStr = "";
      typeStr += Helpers::dataTypeToString(attr.getType());
      if (attr.getType() == DataTypes::varchar_type){
        typeStr += "(";
        typeStr += std::to_string(attr.getSize());
        typeStr += ")";
      }

      fields.push_back(typeStr);

      // display nullable
      fields.push_back(attr.isNullable() ? "YES" : "NO");

      // display primary key
      fields.push_back(attr.isPrimaryKey() ? "YES" : " ");

      // display extra
      fields.push_back(attr.isAutoIncrement() ? "auto_increment" : " ");

      printLine(std::cout, fields, cellLength);
    }
    std::cout << hline;

    app->output << attributes.size() <<" rows (" << timer.elapsed() << " secs)\n";
    

    StatusResult{Errors::noError};
  }

  return StatusResult{Errors::unknownTable};
}

void Database::printLine(std::ostream& anOutput, std::vector<std::string> strings, std::vector<size_t> fieldWidth){
  anOutput << "|";

  for(size_t i = 0; i < strings.size(); i++){
    anOutput << std::setw(fieldWidth[i]) << std::left << strings[i] << "|";
  }

  anOutput << "\n";
}

StatusResult  Database::dropTable(const std::string &aName){
  Timer timer;
  timer.reset();
  
  auto response = storage.deleteEntity(Helpers::hashString(aName.c_str()));

  size_t count = response.value;

  app->output << "Query OK, " << count << " Rows affected (" << timer.elapsed() << " secs)\n";

  return StatusResult{Errors::noError};
}

StatusResult  Database::showTables(){
  Timer timer;
  timer.reset();

  std::vector<std::string> theEntities = storage.listEntity();

  size_t entSize = theEntities.size();
    std::cout << "+---------------------+\n";
    std::string theName = app->getDatabaseInUse()->name;
    std::string tableStr = "| Tables in " + theName;
    tableStr = lineSpace(tableStr);
    std::cout << tableStr;
    std::cout << "+---------------------+\n";
    for(size_t i = 0; i < entSize; i++){
        std::string theEntity = theEntities[i];
        std::string entString = "| " + theEntity;
        entString = lineSpace(entString);
        std::cout << entString;
    }
    std::cout << "+---------------------+\n";

    app->output << theEntities.size() <<" rows (" << timer.elapsed() << " secs)\n";
    
  return StatusResult{};
}

StatusResult  Database::showIndexes(){
  Attribute table("table", DataTypes::varchar_type, 16);
  Attribute field("field (s)", DataTypes::varchar_type, 16);
  Entity dummy("Indexes");
  dummy.addAttribute(table).addAttribute(field);

  RowCollection dummyCollection;

  std::vector<std::string> entityNames = storage.listEntity();
  for (auto entityName : entityNames){
    auto Indexes = storage.getIndexForEntity(entityName);
    for (auto kv : Indexes){
      Row dummyRow;
      dummyRow.addContent(MyBasicVarcharType(entityName.c_str()));
      dummyRow.addContent(MyBasicVarcharType(kv.first.c_str()));
      dummyCollection.push_back(std::make_unique<Row>(dummyRow));
    }
  }

  TabularView view(app->output, dummy, dummyCollection);
  view.show(app->output);
  return StatusResult{ Errors::noError };
}

StatusResult Database::showIndex(std::string table, std::string field){

  Attribute key("Key", DataTypes::int_type, 0);
  Attribute block("Block #", DataTypes::int_type, 0);
  Entity dummy("Index");
  dummy.addAttribute(key).addAttribute(block);

  RowCollection dummyCollection;
  IndexOpt blockIndex = storage.getIndex(Helpers::hashString(table.c_str()), field);

  if (blockIndex == std::nullopt){
    return StatusResult{unknownTable};
  }
    
  blockIndex.value().eachKV([&](const IndexKey& key, uint32_t index){
    Row aRow;
    aRow.addContent(MyBasicIntType(std::get<0>(key)));
    aRow.addContent(MyBasicIntType(index));
    dummyCollection.push_back(std::make_unique<Row>(aRow));
    return StatusResult{Errors::noError};
  });

  TabularView view(app->output, dummy, dummyCollection);
  view.show(app->output);
  return StatusResult{ Errors::noError };
}
 
StatusResult Database::insertIntoTable(std::string aTableName, const StringList& aParsedFields, const std::vector<StringList>& aParsedValues){
  
  Timer timer;
  timer.reset();

  Entity* theEntity = storage.loadEntity(aTableName);

  if (theEntity == nullptr){
    return StatusResult{Errors::unknownTable};
  }
  
  bool rowParseSuccessful = true;

  std::vector<Row> theRows;
  if(theEntity->verifyFields(aParsedFields)){
    for (auto theCurrentRowValues : aParsedValues){
      Row aRow;
      if (theEntity->createRowFromValues(aRow, aParsedFields, theCurrentRowValues)){
        // row is valid, push row into buffer for save
        theRows.push_back(aRow);
      }else{
        rowParseSuccessful = false;
        break;
      }
    }
  }

  if (!rowParseSuccessful){
    delete theEntity;
    return StatusResult{Errors::invalidArguments};
  }
  
  // save all rows
  for (auto& row : theRows){
    storage.write_row(row, *theEntity);
  }

  // save updated entity (entity updated in createRowFromValues by issue primary ID)
  storage.saveEntity(*theEntity);

  storage.syncIndex(Helpers::hashString(aTableName.c_str()));

  app->output << "Query OK, " << theRows.size() << " rows affacted (" << timer.elapsed() << " secs) \n";
  
  delete theEntity;
  return StatusResult{Errors::noError};
}
   
StatusResult  Database::selectRows(const SelectQuery &aQuery, RowCollection &aRows){
    // if the only field is the * (all)
    SelectQuery theQuery(aQuery);
    Entity theEntity = getEntity(aQuery.table_name);
    
    storage.visitRow(aQuery.table_name, [&](const Row& aRow, size_t index) -> bool {
      if (theQuery.filter.eval(aRow, theEntity)){
        aRows.push_back(std::make_unique<Row>(aRow));
      }
      return false;
    });
    
    return StatusResult{Errors::noError};
}

size_t Database::updateRows(const SelectQuery &aQuery, RowCell updateCell, size_t indexInRow){
    SelectQuery theQuery(aQuery);
    Entity theEntity = getEntity(aQuery.table_name);
    
    size_t updateCount = 0;

    storage.visitRow(aQuery.table_name, [&](const Row& aRow, size_t index) -> bool {
      if (theQuery.filter.eval(aRow, theEntity)){
        Row newRow = aRow;
        newRow.setCell(indexInRow, updateCell);

        storage.updateRow(aQuery.table_name, index, newRow);

        updateCount++;
      }
      return false;
      });
    
    return updateCount;
}

size_t Database::deleteRows(const SelectQuery &aQuery){
    SelectQuery theQuery(aQuery);
    Entity theEntity = getEntity(aQuery.table_name);
    
    size_t deleteCount = 0;

    storage.visitRow(aQuery.table_name, [&](const Row& aRow, size_t blockIndex) -> bool {
      if (theQuery.filter.eval(aRow, theEntity)){
      
        storage.deleteRow(aQuery.table_name, blockIndex);

        deleteCount++;
        return true;
      }
      return false;
      });

    return deleteCount;
}

Entity Database::getEntity(const std::string &aName){
    Entity* entity = storage.loadEntity(aName);
    Entity copy = *entity;
    delete entity;
    return copy;
}

std::ostream& Database::getAppOutput() {
    return app->output;
}


bool  Database::containsTable(std::string aTableName){
  auto entityList = storage.listEntity();
  return std::find(entityList.begin(), entityList.end(), aTableName) != entityList.end();
}

}
