#include "Index.hpp"

#include <optional>
#include <string>

#include "Storage.hpp"

namespace ECE141{

    Index::Index(const Index& other) {
        *this = other;
    }

    Index& Index::operator=(const Index& other) {
        storage = other.storage;
        data = other.data;
        type = other.type;
        name = other.name;
        changed = other.changed;
        entityId = other.entityId;
        return *this;
    }

    Index::ValueProxy Index::operator[](const std::string &aKey) {
      return ValueProxy(*this,aKey);
    }

    Index::ValueProxy Index::operator[](uint32_t aKey) {
      return ValueProxy(*this,aKey);
    }
      
    bool   Index::isChanged() {return changed;}

    Index&  Index::setChanged(bool aChanged) {
      changed=aChanged; return *this;      
    }
                    
    IntOpt Index::valueAt(IndexKey &aKey) {
      return exists(aKey) ? data[aKey] : (IntOpt)(std::nullopt);
    }

    bool Index::setKeyValue(IndexKey &aKey, uint32_t aValue) {
      if (data[aKey] == aValue){
        return false;
      }

      data[aKey]=aValue;

      return changed=true; //side-effect indended!
    }
        
    StatusResult Index::erase(const std::string &aKey) {
        //check input type
        if (type == IndexType::intKey) {
            return StatusResult{ Errors::invalidArguments };
        }

        IndexKey theKey = aKey;

        // check key exists
        if (exists(theKey)) {
            data.erase(theKey);
            changed = true;
            return StatusResult{ Errors::noError };
        }
        else {
            return StatusResult{ Errors::invalidArguments };
        }
    }

    StatusResult Index::erase(uint32_t aKey) {
      //check input type
      if (type == IndexType::strKey){
        return StatusResult{Errors::invalidArguments};
      }

      IndexKey theKey = aKey;

      // check key exists
      if (exists(theKey)){
        data.erase(theKey);
        changed = true;
        return StatusResult{ Errors::noError };
      }else{
        return StatusResult{Errors::invalidArguments}; 
      }
    }

    size_t Index::getSize() {return data.size();}
    
    bool Index::exists(IndexKey aKey) {
      return data.count(aKey);
    }
      
    StatusResult Index::encode(std::ostream &anOutput) {
      
      // write header
      String header = std::string(INDEX_SAVE_HEADER);
      header.encode(anOutput);

      // write type
      String saveType = (type == IndexType::intKey ? std::string("I") : std::string("S"));
      saveType.encode(anOutput);
      
      // write size
      Size mapSize = getSize();
      mapSize.encode(anOutput);

      // write entity Id
      Size entityIdSave = entityId;
      entityIdSave.encode(anOutput);

      // write Name
      String nameSave = name;
      nameSave.encode(anOutput);
      
      // write content
      if (type == IndexType::intKey){
        for (auto kv : data){
          if (kv.first.index() != 0){
            return StatusResult{Errors::invalidAttribute};
          }

          Size key = std::get<0>(kv.first);
          Size idx = kv.second;

          key.encode(anOutput);
          idx.encode(anOutput);
        }
      }else{
        for (auto kv : data){
          if (kv.first.index() != 1){
            return StatusResult{Errors::invalidAttribute};
          }

          String key = std::get<1>(kv.first);
          Size idx = kv.second;

          key.encode(anOutput);
          idx.encode(anOutput);
        }
      }

      return StatusResult{Errors::noError};
    }
    
    StatusResult Index::decode(std::istream &anInput) {
      // decode header
      String header = std::string("");
      header.decode(anInput);

      if (std::string(INDEX_SAVE_HEADER) != static_cast<std::string>(header)) {
        return StatusResult{Errors::readError};
      }

      // decode type
      String saveType("");
      saveType.decode(anInput);

      if (static_cast<std::string>(saveType) == "I"){
        type = IndexType::intKey;
      }else if (static_cast<std::string>(saveType) == "S"){
        type = IndexType::strKey;
      }else{
        return StatusResult{Errors::readError};
      }
      
      // decode size
      Size mapSize;
      mapSize.decode(anInput);

      // decode entity Id
      Size entityIdSave;
      entityIdSave.decode(anInput);
      entityId = entityIdSave;

      // decode Name
      String nameSave("");
      nameSave.decode(anInput);
      name = nameSave;
      
      // decode content
      for (size_t i = 0; i < mapSize; i++){
        IndexKey key;
        if (type == IndexType::intKey){
          Size intKey;
          intKey.decode(anInput);
          key = intKey;
        }else{
          String strKey("");
          strKey.decode(anInput);
          key = strKey;
        }

        Size idx;
        idx.decode(anInput);

        data[key] = idx; 
      }

      return StatusResult{Errors::noError};
    }
    
    //visit blocks associated with index
    StatusResult Index::each(BlockVisitor aVisitor) {
      Block theBlock;
      for(auto thePair : data) {
        theBlock = (*storage)[thePair.second];
        if(!aVisitor(theBlock,thePair.second)) {return StatusResult{Errors::unknownError};}  
      }
      return StatusResult{Errors::noError};
    }

    // visit headers associated with index
    StatusResult Index::each(BlockHeaderVisitor aVisitor) {
      BlockHeader theHeader;
      for(auto thePair : data) {
        theHeader = (*storage)[thePair.second];
        if(!aVisitor(theHeader,thePair.second)) {return StatusResult{Errors::unknownError};}  
      }
      return StatusResult{Errors::noError};
    }

    //visit index values (key, value)...
    bool Index::eachKV(IndexVisitor aCall) {
      for(auto thePair : data) {
        if(!aCall(thePair.first,thePair.second)) {
          return false;
        }
      }
      return true;
    }

    Index::operator SignedData(){
      SignedData data;
      
      std::stringstream ss;
      encode(ss);

      ss.seekg(0,std::ios::end);
      size_t encodingLength = ss.tellg();

      data.data = std::make_unique<char[]>(encodingLength);
      ss.read(data.data.get(), encodingLength);

      data.len = encodingLength;
      data.hash = entityId;

      return data;
    } 

}