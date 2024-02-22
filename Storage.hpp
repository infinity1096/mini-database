//
//  Storage.hpp
//  RGAssignment2
//
//  Created by rick gessner on 2/27/21.
//

#ifndef Storage_hpp
#define Storage_hpp

#include <vector>
#include <fstream>
#include <optional>
#include <utility>
#include <string>

#include "Helpers.hpp"
#include "BlockIO.hpp"
#include "Row.hpp"
#include "Entity.hpp"
#include "DataStructures.hpp"
#include "Storable.hpp"
#include "Index.hpp"
#include "SignedData.hpp"
#include "Cache.hpp"
#include "Config.hpp"

namespace ECE141 {

  class IStorage {
    virtual uint32_t SaveData(const SignedData& signedData)=0;
    virtual SignedData readData(const uint32_t& ownerHash)=0;

    SignedData readData(const std::string& owner){
      return readData(Helpers::hashString(owner.c_str()));
    };
  };

  const size_t kBlockDivision = 100;
  
  struct AsNew {};
  struct AsExisting {};
  using StringList = std::vector<std::string>;
  using RowVisitor = std::function<bool(const Row& aRow, size_t anIndex)>; // return true if row deleted
  using IndexOpt = std::optional<Index>;
  using RowCache = LRUCache<std::string, Row>;
  using RowCacheOpt = std::optional<RowCache>;

  //FIXME: using std::string of entityName + ID as key, should use pair.
  // however, pairs have some hashing issues. 

  class Storage : public Storable, public BlockIO{
    public:
    Storage(std::string aPath, AsNew tag);
    Storage(std::string aPath, AsExisting tag);
    virtual ~Storage() {};

    uint32_t      saveData(const SignedData& signedData);
    SignedData    loadData(uint32_t anIndex);
    StatusResult  deleteData(uint32_t anIndex);

    StringList    listEntity();
    StatusResult  saveEntity(Entity aEntity);
    Entity*       loadEntity(uint32_t aEntityHash);
    Entity*       loadEntity(std::string aEntityName);
    StatusResult  deleteEntity(uint32_t aEntityHash);

    // external visitor should call this function to access rows    
    StatusResult  read_row(const std::string& entityName, size_t index, Row& outputRow);
    StatusResult  write_row(Row aRow, Entity aEntity);
    StatusResult  deleteRow(std::string EntityName, size_t blockIndex);
    StatusResult  updateRow(std::string EntityName, size_t blockIndex, Row& newRow);

    IndexOpt      getIndex(uint32_t entityHash, std::string field);
    std::map<std::string, Index> getIndexForEntity(std::string entity);


    StatusResult encode(std::ostream& anOutput) override ;
    StatusResult decode(std::istream& anInput) override ;

    StatusResult visitRow(std::string EntityName, RowVisitor visitor);



    void         syncIndex(uint32_t entityHash);

    private:
    // saved state
    Index      entity_index_map; // [Entity Hash] -> [Block containing Entity]
    Index      block_index_map;  // [Entity Hash] ->  [Block containing Index]

    RowCacheOpt row_cache;
    // recovered state
    std::optional<std::map<uint32_t, Entity>> id_entity_map = std::nullopt;
    std::optional<std::map<uint32_t, std::map<std::string, Index>>> entity_field_index_map = std::nullopt;
    
    bool is_index_within_bound(uint32_t index);

    Storage& initialize_state();
    Storage& restore_state();
    Storage& update_state();
    Storage& update_index_of(uint32_t entityHash);
    Storage& infer_secondary_state();
  };
}


#endif /* Storage_hpp */
