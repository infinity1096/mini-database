//
//  Storage.cpp
//  RGAssignment2
//
//  Created by rick gessner on 2/27/21.
//


#include <sstream>
#include <cmath>
#include <cstdlib>
#include <optional>
#include <cstring>
#include <vector>
#include <iostream>
#include "Storage.hpp"
#include "Config.hpp"

namespace ECE141 {


  SignedData::SignedData() : data(nullptr), len(0), hash(0), type(BlockType::unknown_block) {}
  
  SignedData::SignedData(char* aData, size_t len, uint32_t ownerHash, BlockType type) : len(len), hash(ownerHash), type(type) {
    data = std::make_unique<char[]>(len);
    memcpy(data.get(), aData, len);
  } 

  SignedData::SignedData(char* aData, size_t len, const std::string& owner, BlockType type) : len(len), hash(Helpers::hashString(owner.c_str())), type(type) {
    data = std::make_unique<char[]>(len);
    memcpy(data.get(), aData, len);
  }
  
  SignedData::SignedData(const SignedData& other){*this = other;}
  SignedData& SignedData::operator=(const SignedData& other){
    len = other.len;
    hash = other.hash;
    type = other.type;

    data = std::make_unique<char[]>(other.len);
    memcpy(data.get(), other.data.get(), len);
    return *this;
  }
    
  SignedData::~SignedData(){}

  SignedData::SignedData(std::istream& aBuffer, uint32_t aHash, BlockType aType){
    aBuffer.seekg(0, std::ios::end);
    uint32_t size = aBuffer.tellg();
    aBuffer.seekg(0);

    data = std::make_unique<char[]>(size);
    aBuffer.read(data.get(), size);

    len = size;
    hash = aHash;
    type = aType;
  }

  SignedData::SignedData(const Block& aBlock) {
    len = aBlock.header.valid_data_length;
    hash = aBlock.header.identity_hash;
    type = static_cast<BlockType>(aBlock.header.type);

    data = std::make_unique<char[]>(len);
    memcpy(data.get(), aBlock.payload, len);
  }

  // divide whole data into [num_division] parts that contains almost equal
  // portion of the data.
  std::vector<SignedData> SignedData::subDivide(size_t aNumberPartition) const {
	  // assumption: data have at least [aNUmberPartition] bytes of data to sub-divide.
	  std::vector<SignedData> result;

	  if (aNumberPartition == 0) {
		  return result;
	  }
	  
	  size_t baseDivision = len / aNumberPartition;
	  size_t residueBytes = len - baseDivision * aNumberPartition;

    size_t readIndex = 0;
	  for (uint32_t i = 0; i < aNumberPartition; i++) {
		  size_t numBytes = (i == 0) ? baseDivision + residueBytes : baseDivision;
		  
      std::unique_ptr<char[]> buffer = std::make_unique<char[]>(numBytes);
      memcpy(buffer.get(), data.get() + readIndex, numBytes);
      readIndex += numBytes;
		  
      SignedData signedData = *this;
		  signedData.data = std::move(buffer);
		  signedData.len = numBytes;
		  result.push_back(signedData);
	  }

	  return result;
  }

  SignedData& SignedData::operator+=(const SignedData& other){
    if (data.get() == nullptr) {
      return (*this = other);
    }

    std::unique_ptr<char[]> theNewData = std::make_unique<char[]>(len + other.len);
    memcpy(theNewData.get(), data.get(), len);
    memcpy(theNewData.get() + len , other.data.get(), other.len);
    
    len = len + other.len;

    // FIXME: verify other's hash and type
    hash = other.hash;
    type = other.type;

    data = std::move(theNewData);

    return *this;
  }
  
  SignedData::operator Block() const {
    Block theBlock;
    if (len > kPayloadSize){
      return theBlock;
    }

    theBlock.header.identity_hash = hash;
    theBlock.header.type = static_cast<char>(type);
    theBlock.header.valid_data_length = len;
    memcpy(theBlock.payload, data.get(), len);

    return theBlock;
  }

  SignedData::operator std::stringstream() const {
    std::stringstream buffer;
    buffer.write(data.get(), len);
    buffer.seekg(0);

    return buffer;
  }

  // FIXME: is (*this) ready for entity_index_map ? 
  Storage::Storage(std::string aPath, AsNew tag) : BlockIO(aPath, true), entity_index_map(this, IndexType::intKey), block_index_map(this, IndexType::intKey) {
    if (Config::useCache(CacheType::row)){
      row_cache = RowCache(Config::getCacheSize(CacheType::row));
    }
    initialize_state().infer_secondary_state();
  }

  Storage::Storage(std::string aPath, AsExisting tag) : BlockIO(aPath, false), entity_index_map(this, IndexType::intKey), block_index_map(this, IndexType::intKey) {
    if (Config::useCache(CacheType::row)){
      row_cache = RowCache(Config::getCacheSize(CacheType::row));
    }
    restore_state();
  }

  uint32_t Storage::saveData(const SignedData& signedData){
      
      uint32_t blockSavedIndex;

      if (signedData.len <= kPayloadSize){
        // fits within one block
        blockSavedIndex = consumeNextEmptyBlock();
        (*this)[blockSavedIndex] = signedData;
      }else{
        // need to sub-divide the block
        DividedSignedData division = signedData.subDivide(kBlockDivision);
        
        // creates a index map
        IndexMap indexMap;

        uint32_t thePosition = 0;
        for(auto theSignedData : division){
          // push stored block index into index block
          indexMap[thePosition++] = saveData(theSignedData);
        }

        // store index block
        blockSavedIndex = consumeNextEmptyBlock();
        (*this)[blockSavedIndex] = indexMap;
      }
      
      // return index of index block
      return blockSavedIndex;
    }
    
    SignedData Storage::loadData(uint32_t anIndex){
      Block theBlock = (*this)[anIndex];
      if (static_cast<char>(BlockType::linker_block) != theBlock.header.type){
        // return the info in block
        return theBlock; // conversion constructor of SignedData Called here
      }

      // recursively load data according to index map in linker
      IndexMap index_map(theBlock);

      SignedData result;

      for(auto kv : index_map){
          result += loadData(kv.second);
      }

      return result;
    }

    StatusResult Storage::deleteData(uint32_t anIndex){
      if (!is_index_within_bound(anIndex)){
        return StatusResult{Errors::unknownIndex};
      }

      Block toDeleteBlock = (*this)[anIndex]; // to utilize block cache
      BlockHeader toDelete = toDeleteBlock.header;
      size_t count = 0;

      if (toDelete.type == static_cast<char>(BlockType::linker_block)){
        // recursively delete linked data        
        IndexMap blockIndex = static_cast<Block>((*this)[anIndex]);
        for (auto kv : blockIndex){
          count += deleteData(kv.second).value;
        }
      }

      free(anIndex);
      count += 1;

      StatusResult theResult{Errors::noError};
      theResult.value = count;

      return theResult;
    }

    // FIXME: cache entity names somewhere? 
    StringList Storage::listEntity(){
      StringList list;

      entity_index_map.eachKV([&](const IndexKey& index, uint32_t blockIdx){
        Entity* entity = loadEntity(std::get<0>(index));
        if (entity != nullptr){  
          list.push_back(entity->getName());
          delete entity;
        }else{
          std::cout << "Entity decode fail in listEntity \n";
          return false;
        }
        return true;
      });

      return list;
    }

    StatusResult Storage::saveEntity(Entity aEntity){
      
      uint32_t entityHash = Helpers::hashString(aEntity.getName().c_str());

      std::stringstream buffer;
      aEntity.encode(buffer);
      SignedData entityData = SignedData(buffer, entityHash, BlockType::entity_block);
      
      if (entity_index_map.exists((IndexKey)entityHash)){
        // entity already exists, need to delete existing
        IntOpt index = entity_index_map[entityHash];

        if (index == std::nullopt){
          return StatusResult{Errors::invalidArguments};
        }

        deleteData(index.value());
      }

      uint32_t entitySavedBlock = saveData(entityData);


      if (!entity_index_map.exists((IndexKey)entityHash)){
        // initialize index upon first save
        Index index(this, IndexType::intKey);
        index.setName(aEntity.getPrimaryKey().value().getName());
        (*entity_field_index_map)[entityHash][aEntity.getPrimaryKey().value().getName()] = index;
        
        update_index_of(entityHash);
      }


      entity_index_map[entityHash] = entitySavedBlock;
      id_entity_map.value()[entityHash] = aEntity;

      update_state();// TODO: not update every time? 

      return StatusResult{ Errors::noError };
    }


    Entity* Storage::loadEntity(uint32_t aEntityHash){
      if (id_entity_map == std::nullopt){
        IntOpt index = entity_index_map[aEntityHash];

        if (index == std::nullopt){
          return nullptr; // entity not found
        }

        SignedData entityData = loadData(index.value());
        std::stringstream buffer = entityData;

        Entity* entity = new Entity("empty");
        if (entity->decode(buffer)){
          return entity;
        }

        std::cout << "Entity decode fail \n";
        return nullptr;
      }else{
        if(id_entity_map.value().count(aEntityHash) == 0){
          return nullptr;
        }

        return new Entity(id_entity_map.value()[aEntityHash]);
      }
    }

    Entity* Storage::loadEntity(std::string aEntityName){
      return loadEntity(Helpers::hashString(aEntityName.c_str()));
    }

    StatusResult Storage::deleteEntity(uint32_t aEntityHash){
      
      IntOpt index = entity_index_map[aEntityHash];

      if (index == std::nullopt){
        return StatusResult{Errors::unknownEntity};
      }

      size_t count = 0;
      
      // delete all rows associated with entity
      each([&](const Block& aBlock, uint32_t index){
        if (aBlock.header.identity_hash == aEntityHash && aBlock.header.type == static_cast<char>(BlockType::data_block)){
          free(index);
          count++;
        }  
        return StatusResult{ Errors::noError };
      });
      
      // delete entity from storage
      StatusResult entityDeleteResult = deleteData(index.value());
      
      // delete index of entity from storage
      IntOpt idx = block_index_map[aEntityHash];
      deleteData(idx.value());

      // delete entity from map
      entity_index_map.erase(aEntityHash);
      id_entity_map.value().erase(aEntityHash);

      block_index_map.erase(aEntityHash);
      entity_field_index_map.value().erase(aEntityHash);

      update_state();

      StatusResult theResult =  StatusResult{Errors::noError};
      theResult.value = count + entityDeleteResult.value;

      return theResult;
    }

    StatusResult Storage::read_row(const std::string& entityName, size_t index, Row& outputRow){
      auto key = entityName + std::to_string(index);
      if (row_cache != std::nullopt && row_cache->contains(key)){
        row_cache->get(key, outputRow);
        return StatusResult{Errors::noError};
      }
      
      Block aBlock = (*this)[index];
      std::stringstream ss;
      ss.write(aBlock.payload, aBlock.header.valid_data_length);
      ss.seekg(0);
      Row row(ss);
      outputRow = row;

      // write to cache in case we didn't have it in the cache
      if (row_cache != std::nullopt){
        auto key = entityName + std::to_string(index);
        row_cache->put(key, row);
      }

      return StatusResult{Errors::noError};
    }

    StatusResult Storage::write_row(Row aRow, Entity aEntity){
      uint32_t entityHash = Helpers::hashString(aEntity.getName().c_str());
      
      Block rowBlock = aRow;
      rowBlock.header.identity_hash = entityHash;
      uint32_t rowSavedBlock = consumeNextEmptyBlock();
      (*this)[rowSavedBlock] = rowBlock;

      // write to cache
      if (row_cache != std::nullopt){
        auto key = aEntity.getName() + std::to_string(rowSavedBlock);
        row_cache->put(key, aRow);
      }

      uint32_t primaryIdIndex = *aEntity.getAttributeIndex(aEntity.getPrimaryKey().value().getName());

      uint32_t id = std::get<4>(aRow[primaryIdIndex]);

      (*entity_field_index_map)[entityHash][aEntity.getPrimaryKey().value().getName()][id] = rowSavedBlock;

      return StatusResult{ Errors::noError };
    }

    StatusResult Storage::deleteRow(std::string EntityName, size_t blockIndex){
      
      free(blockIndex);
      if (row_cache != std::nullopt){
        auto key = EntityName + std::to_string(blockIndex);
        row_cache->remove(key);
      }

      return StatusResult{ Errors::noError };
    }
  
    StatusResult Storage::updateRow(std::string EntityName, size_t blockIndex, Row& newRow){
      
      Block rowBlock = newRow;
      rowBlock.header.identity_hash = Helpers::hashString(EntityName.c_str());
      (*this)[blockIndex] = rowBlock;

      // write to cache
      if (row_cache != std::nullopt){
        auto key = EntityName + std::to_string(blockIndex);
        row_cache->put(key, newRow);
      }

      return StatusResult{Errors::noError};
    }

    IndexOpt Storage::getIndex(uint32_t entityHash, std::string field){
      if ((*entity_field_index_map).count(entityHash)){
        if ((*entity_field_index_map)[entityHash].count(field)){
          return (*entity_field_index_map)[entityHash][field];
        }
      }
      return std::nullopt;
    }

    StatusResult Storage::encode(std::ostream& anOutput) {
      entity_index_map.encode(anOutput);
      block_index_map.encode(anOutput);
      return StatusResult{ Errors::noError };
    }

    StatusResult Storage::decode(std::istream& anInput) {
      entity_index_map.decode(anInput);
      block_index_map.decode(anInput);
      return StatusResult{ Errors::noError };
    }

    StatusResult Storage::visitRow(std::string entityName, RowVisitor visitor){
      uint32_t entityHash = Helpers::hashString(entityName.c_str());
      
      IntOpt index = entity_index_map[entityHash];

      if (index == std::nullopt){
        return StatusResult{Errors::unknownEntity};
      }

      Entity* theEntity = loadEntity(entityName);

      // FIXME: index for sepecific where clause not implemented
      IndexOpt blockIndex = getIndex(Helpers::hashString(entityName.c_str()), theEntity->getPrimaryKey().value().getName());

      if (blockIndex == std::nullopt){
        // FIXME: can visit header only
        each([&](const Block& aBlock, uint32_t index){
          Block copyBlock(aBlock);
          if (aBlock.header.identity_hash == entityHash && aBlock.header.type == static_cast<char>(BlockType::data_block)){
            std::stringstream ss;
            ss.write(copyBlock.payload, aBlock.header.valid_data_length);
            ss.seekg(0);
            Row row(ss);
            visitor(row, index);
          }
          return StatusResult{ Errors::noError };
        });
      }else{
        // use iterator of index
          blockIndex.value().eachKV([&](const IndexKey& indexKey, uint32_t blockIndex) {
            uint32_t indexKeyNum = std::get<0>(indexKey);
            
            Row row;
            read_row(entityName, blockIndex, row);

            bool indexDeleted = visitor(row, blockIndex);

            if (indexDeleted) {
                (*entity_field_index_map)[entityHash][theEntity->getPrimaryKey()->getName()].erase(indexKeyNum);
            }

            return StatusResult{ Errors::noError };
          });
      }

      delete theEntity;

      return StatusResult{ Errors::noError };
    }

    void Storage::syncIndex(uint32_t entityHash){
      update_index_of(entityHash).update_state();
    }
    
    std::map<std::string, Index> Storage::getIndexForEntity(std::string entity){
      // TODO: hack, add verify
      return (*entity_field_index_map)[Helpers::hashString(entity.c_str())];
    }

    bool Storage::is_index_within_bound(uint32_t index){
      return 0 <= index && index < size();
    }

    Storage& Storage::initialize_state(){
      IndexMap block_zero_map;

      uint32_t block_zero_index = consumeNextEmptyBlock();
      if (0 != block_zero_index){
        std::cout << "Error in storage initialization: invalid first block creation \n";
      }

      Block blockZero = block_zero_map;
      blockZero.header.type = static_cast<char>(BlockType::meta_block);
      (*this)[block_zero_index] = blockZero; // occupy block 0

      // convert state to signed data
      std::stringstream buffer;
      encode(buffer);

      uint32_t meta_index = saveData(SignedData(buffer, Helpers::hashString("meta"), BlockType::meta_block));

      block_zero_map[Helpers::hashString("meta")] = meta_index;
      
      blockZero = block_zero_map;
      blockZero.header.type = static_cast<char>(BlockType::meta_block);
      (*this)[block_zero_index] = blockZero; // occupy block 0

      return *this;
    }

    Storage& Storage::restore_state(){
      IndexMap block_zero_map = static_cast<Block>((*this)[0]); // load index map from block 0

      if (block_zero_map.count(Helpers::hashString("meta"))){
        SignedData metaData = loadData(block_zero_map[Helpers::hashString("meta")]);
        std::stringstream buffer;
        buffer.write(metaData.data.get(), metaData.len);
        buffer.seekg(0);
        decode(buffer);
        infer_secondary_state();
      }else{
        std::cout << "Error in meta initialization: index not found in block 0\n";
      }

      return *this;
    }

    Storage& Storage::update_state(){
      IndexMap block_zero_map = static_cast<Block>(loadData(0)); // load index map from block 0

      for (auto kv : block_zero_map){
        deleteData(kv.second);
      }

      deleteData(0);

      initialize_state();

      return *this;
    }

    Storage& Storage::update_index_of(uint32_t entityHash){
      // delete data stored in block_index_map
      IntOpt idx = block_index_map[entityHash];
      
      if (idx != std::nullopt) {
          deleteData(idx.value());
      }

      // re-encode index of entity [entityHash] from secondary state: entity_field_index_map
      std::stringstream ss;
      Size size = (*entity_field_index_map)[entityHash].size();

      size.encode(ss);

      for (auto kv : (*entity_field_index_map)[entityHash]){
        kv.second.encode(ss);
      }

      uint32_t savedLocation = saveData(SignedData(ss, entityHash, BlockType::index_block));

      block_index_map[entityHash] = savedLocation;
      return *this;
    }

    Storage& Storage::infer_secondary_state(){
      // reconstruct id_entity_map
      std::map<uint32_t, Entity> tempIdEntityMap;
      
      entity_index_map.eachKV([&](const IndexKey& index, uint32_t blockIdx){
        uint32_t hash = std::get<0>(index);
        Entity* entity = loadEntity(hash);
        
        if (entity == nullptr){
          return StatusResult{Errors::unknownEntity};
        }

        tempIdEntityMap[hash] = *entity;
        delete entity;

        return StatusResult{Errors::noError};
      });

      id_entity_map = tempIdEntityMap;

      std::map<uint32_t, std::map<std::string, Index>> tempEntityFieldIndexMap;

      block_index_map.eachKV([&](const IndexKey& entityHash, uint32_t blockIdx){
        std::stringstream ss = loadData(blockIdx);

        Size numIndex;
        numIndex.decode(ss);

        std::map<std::string, Index> innerMap;

        for (size_t i = 0; i < numIndex; i++){
          Index index(this, IndexType::intKey);
          index.decode(ss);

          innerMap[index.getName()] = index;
        }

        tempEntityFieldIndexMap[std::get<0>(entityHash)] = innerMap;

        return StatusResult{Errors::noError};
      });

      entity_field_index_map = tempEntityFieldIndexMap;

      return *this;
    }

}

