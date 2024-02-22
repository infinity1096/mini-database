//
//  BlockIO.hpp
//  RGAssignment2
//
//  Created by rick gessner on 2/27/21.
//

#ifndef BlockIO_hpp
#define BlockIO_hpp

#include <functional>
#include <sstream>
#include <cstring>
#include <vector>
#include <fstream>
#include <optional>

#include "Errors.hpp"
#include "Cache.hpp"
#include "Config.hpp"

#define NO_NEXT_BLOCK -1
#define METABLOCK_HEADER "META BLOCK: "

#define BLOCK_NULLPTR -1
namespace ECE141 {
  
  enum class BlockType {
    meta_block = 'M',
    data_block='D',
    free_block='F',
    entity_block='E',
    linker_block='L',
    index_block='I',
    unknown_block='U',
  };

  //a small header that describes the block...
  struct BlockHeader {
    BlockHeader(BlockType aType=BlockType::data_block);
    BlockHeader(const BlockHeader &aCopy);
    void empty();
    BlockHeader& operator=(const BlockHeader &aCopy);
   
    char          type;               //char version of block type
    uint32_t      valid_data_length;
    uint32_t      identity_hash;
  };

  const size_t kBlockSize = 1024;
  const size_t kPayloadSize = kBlockSize - sizeof(BlockHeader);
  
  class Block {
  public:
    Block(BlockType aType=BlockType::data_block);
    Block(const Block &aCopy);
    
    Block& operator=(const Block &aCopy);
  
    BlockHeader   header;
    char          payload[kPayloadSize];
  };

  class BlockIO; 

  // this class acts as polymorphic return type that supports
  // reading and writing to blockIO in a readable way
  // block = block_io[index];
  // block_io[index] = block2;
  class BlockView {
    public:
    BlockView(BlockIO* aBase, size_t anIndex) : base(aBase), index(anIndex) {}

    operator Block();
    operator BlockHeader();

    void operator=(const Block& aBlock);
    void operator=(const BlockHeader& aHeader);
    
    private:
    BlockIO* base;
    size_t index;
  };

  
  // class that handles block read/write with a filestream
  using BlockHeaderVisitor = std::function<StatusResult(const BlockHeader& header, uint32_t index)>;
  using BlockVisitor = std::function<StatusResult(const Block& block, uint32_t index)>;
  class BlockIterator {
    public:
    virtual StatusResult each(BlockVisitor visitor)=0;
    virtual StatusResult each(BlockHeaderVisitor visitor)=0;
  };
  
  class BlockIO : public BlockIterator{
    public:
    BlockIO(std::string aPath, bool asNew = false);
    ~BlockIO();

    BlockView     operator[](size_t index); // main interface of reading & writing blocks
    StatusResult  free(uint32_t index);     // free a block at index

    StatusResult  each(BlockHeaderVisitor visitor) override;
    StatusResult  each(BlockVisitor visitor) override;
    uint32_t      consumeNextEmptyBlock();
    uint32_t      size() const;
    void          flush();
    void          debugDump();
    
    friend class BlockView;
    
    BlockIO& read_block    (size_t index, Block& block);
    BlockIO& write_block   (size_t index, const Block& block);

    protected:
    std::fstream stream;
    uint32_t block_count;
  
    private:
    std::vector<uint32_t> empty_block_index_list;

    std::optional<LRUCache<size_t, Block>> block_cache;

    // private set of interface for this class
    BlockIO& initialize_block_count();
    BlockIO& scan_empty_block_list();

    BlockIO& goto_block   (size_t index);

    BlockIO& read_header   (size_t index, BlockHeader& header);
    BlockIO& write_header  (size_t index, const BlockHeader& header);
  };
}

#endif /* BlockIO_hpp */
