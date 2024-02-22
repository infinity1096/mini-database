//
//  Index.hpp
//  RGAssignment3
//
//  Created by rick gessner on 4/2/21.
//

#ifndef Index_hpp
#define Index_hpp

#define INDEX_SAVE_HEADER "BlockIndex v1"

#include <stdio.h>
#include <map>
#include <map>
#include <functional>
#include <variant>
#include <optional>

#include "Storable.hpp"
#include "BasicTypes.hpp"
#include "Errors.hpp"
#include "DataStructures.hpp"
#include "SignedData.hpp"


namespace ECE141 {

  enum class IndexType {intKey=0, strKey};
  using IndexKey = std::variant<uint32_t, std::string>;
  using IntOpt = std::optional<uint32_t>;
  
  using IndexVisitor =
    std::function<bool(const IndexKey&, uint32_t)>;
  
  class Storage;
  struct Index : public Storable, public BlockIterator {

    Index(Storage* aStorage = nullptr, // defaulted for std::map operator[]
          IndexType aType=IndexType::intKey)
        :  storage(aStorage), type(aType) {
          changed=false;
          entityId=0;
        }

    Index(const Index& other);
    Index& operator=(const Index& other);
    
    class ValueProxy {
    public:
      Index     &index;
      IndexKey  key;
      IndexType type;
      
      ValueProxy(Index &anIndex, uint32_t aKey)
        : index(anIndex), key(aKey), type(IndexType::intKey) {}
      
      ValueProxy(Index &anIndex, const std::string &aKey)
        : index(anIndex), key(aKey), type(IndexType::strKey) {}
      
      ValueProxy& operator= (uint32_t aValue) {
        index.setKeyValue(key,aValue);
        return *this;
      }
      
      operator IntOpt() {return index.valueAt(key);}
    }; //value proxy
    
    ValueProxy operator[](const std::string &aKey);

    ValueProxy operator[](uint32_t aKey);
      
    bool    isChanged();
    Index&  setChanged(bool aChanged);
    Index& setName(std::string aName) { name = aName; return *this; }
                    
    IntOpt valueAt(IndexKey &aKey);

    bool setKeyValue(IndexKey &aKey, uint32_t aValue);
        
    StatusResult erase(const std::string &aKey) ;
    StatusResult erase(uint32_t aKey) ;

    size_t getSize();
    std::string getName() const {return name;}
    
    bool exists(IndexKey aKey);
      
    StatusResult encode(std::ostream &anOutput) override ;
    StatusResult decode(std::istream &anInput) override ;
    
    //visit blocks associated with index
    StatusResult each(BlockVisitor aVisitor) override ;

    // visit headers associated with index
    StatusResult each(BlockHeaderVisitor aVisitor) override ;

    //visit index values (key, value)...
    bool eachKV(IndexVisitor aCall) ;

    operator SignedData();
    
  protected:
    
    Storage*                      storage;
    std::map<IndexKey, uint32_t>  data;
    IndexType                     type;
    std::string                   name; // TODO: not used
    bool                          changed;
    uint32_t                      entityId; // TODO: not used
  }; //index

  //using IndexMap = std::map<std::string, std::unique_ptr<Index> >;

}


#endif /* Index_hpp */
