//
//  Database.hpp
//  RGAssignment2
//
//  Created by rick gessner on 2/27/21.
//

#ifndef Database_hpp
#define Database_hpp

#include <stdio.h>
#include <fstream>
#include <map>
#include <memory>
#include "Storage.hpp"
#include "Storable.hpp"
#include "Entity.hpp"
#include "SQLProcessor.hpp"
#include "BlockIO.hpp"
#include "TableStatements.hpp"
#include "ParseHelper.hpp"
#include "Query.hpp"

namespace ECE141 {
class Application;
using StringOpt = std::optional<std::string>;
using EntityMap = std::map<std::string, std::unique_ptr<Entity> >;

enum createMode {shouldOpen, shouldCreate};

class Database : public Storable {
public:

    Database(const std::string aName, const std::string& aPath, Application* anApp);

    virtual ~Database();

    StatusResult    dump(std::ostream& anOutput);

    Entity getEntity(const std::string& aName);
    std::ostream& getAppOutput();
    StringOpt getEntityByName(uint32_t hashID);

    StatusResult encode(std::ostream& anOutput) override {
        return StatusResult{ Errors::notImplemented };
    }
    StatusResult decode(std::istream& anInput) override {
        return StatusResult{ Errors::notImplemented };
    }

    bool         containsTable(std::string aTableName);

    StatusResult  createTable(Entity* anEntity);
    StatusResult  describeTable(const std::string& aName);
    StatusResult  dropTable(const std::string& aName);
    StatusResult  showTables();
    StatusResult  showIndexes();
    StatusResult  showIndex(std::string table, std::string field);
    StatusResult  insertIntoTable(std::string aTableName, const StringList& aParsedFields, const std::vector<StringList>& aParsedValues);
    StatusResult  selectRows(const SelectQuery &aQuery, RowCollection &aRows);
    size_t        updateRows(const SelectQuery &aQuery, RowCell updateCell, size_t indexInRow);
    size_t        deleteRows(const SelectQuery &aQuery);
    // FIXME: change size_t to StatueResult

    SQLProcessor sqlProcessor;
    std::string getName() { return name; }
  protected:
    
    StatusResult saveDBHeader(uint32_t aPos=0);
    std::string     name;
    //std::fstream    stream;   //low level stream used by storage...
    bool            changed;  //might be helpful, or ignore if you prefer.
      
    Application* app;
    Storage storage;
  
  private:
    void printLine(std::ostream& anOutput, std::vector<std::string> strings, std::vector<size_t> fieldWidth);

  };

}
#endif /* Database_hpp */
