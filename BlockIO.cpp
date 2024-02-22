//
//  BlockIO.cpp
//  RGAssignment2
//
//  Created by rick gessner on 2/27/21.
//

#include "BlockIO.hpp"

#include <cstring>
#include <iomanip>
#include <iostream>

#include "Entity.hpp"
#include "Row.hpp"

namespace ECE141 {
  BlockHeader::BlockHeader(BlockType aType)
    : type(static_cast<char>(aType)) {
      valid_data_length = 0;
  }

  BlockHeader::BlockHeader(const BlockHeader &aCopy) {
    *this=aCopy;
  }
      
  void BlockHeader::empty() {
    type=static_cast<char>(BlockType::free_block);
    valid_data_length = 0;
  }
  
  BlockHeader& BlockHeader::operator=(const BlockHeader &aCopy) {
    type=aCopy.type;
    valid_data_length = aCopy.valid_data_length;
    identity_hash = aCopy.identity_hash;
    return *this;
  }

  Block::Block(BlockType aType) {
    memset(payload, 0, kPayloadSize);
  }

  Block::Block(const Block &aCopy) {
    *this=aCopy;
  }

  Block& Block::operator=(const Block &aCopy) {
    std::memcpy(payload, aCopy.payload, kPayloadSize);
    header=aCopy.header;
    return *this;
  }

  BlockIO::BlockIO(std::string aPath, bool asNew) : block_cache(std::nullopt) {

    if (asNew) {
        stream.open(aPath, std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc);
    }
    else {
        stream.open(aPath, std::ios::in | std::ios::out | std::ios::binary);
    }

    initialize_block_count().scan_empty_block_list();

    if (Config::useCache(CacheType::block)){
      block_cache = LRUCache<size_t, Block>(Config::getCacheSize(CacheType::block));
    }
  }

  BlockIO::~BlockIO(){stream.close();}

  BlockView BlockIO::operator[](size_t index){
      return BlockView(this, index);
  }

  StatusResult BlockIO::free(uint32_t index){
    if(index < 0 || index >= size()){
      return StatusResult{Errors::unknownIndex};
    }

    BlockHeader freeHeader(BlockType::free_block);
    (*this)[index] = freeHeader;
    empty_block_index_list.push_back(index);
    //block_count--; 

    if (block_cache != std::nullopt){
      block_cache->remove(index);
    }

    return StatusResult{Errors::noError};
  }

  StatusResult  BlockIO::each(BlockHeaderVisitor visitor){
    for(uint32_t index = 0; index < block_count; index++){
      visitor((*this)[index], index);
    }
    return StatusResult{Errors::noError};
  }

  StatusResult  BlockIO::each(BlockVisitor visitor){
    for(uint32_t index = 0; index < block_count; index++){
      visitor((*this)[index], index);
    }
    return StatusResult{Errors::noError};
  }

  uint32_t  BlockIO::consumeNextEmptyBlock(){
      uint32_t nextEmptyBlock;
      if (empty_block_index_list.size() > 0){
        nextEmptyBlock = empty_block_index_list.back();
        empty_block_index_list.pop_back();
      }else{
        nextEmptyBlock = block_count;
        block_count++;
      }

      return nextEmptyBlock;
  }

  BlockIO& BlockIO::initialize_block_count(){
    stream.seekg(0, std::ios::end); // go to end of file
    block_count = stream.tellg() / kBlockSize; // calculate number of blocks
    return *this;
  }

  // private functions
  BlockIO& BlockIO::scan_empty_block_list(){
    each([&](const BlockHeader& header, uint32_t index){
      if (header.type == static_cast<char>(BlockType::free_block)){
        empty_block_index_list.push_back(index);
      }
      return StatusResult{ Errors::noError };
    });

    return *this;
  }

  uint32_t BlockIO::size() const {
    return block_count; 
  }

  void BlockIO::flush(){
    stream.flush();
  }

  void BlockIO::debugDump(){
    std::cout << "BlockIO debug dump: \n";
    std::cout << "ID|    HASH    | len |\n";
    each([](const BlockHeader& aHeader, uint32_t index){
      std::cout << std::setw(2) << std::left << aHeader.type << "|";
      std::cout << std::setw(12) << std::left << aHeader.identity_hash << "|";
      std::cout << std::setw(5) << std::left << aHeader.valid_data_length << "|";
      std::cout << "\n";
      return StatusResult{ Errors::noError };
    });
  }

  // my own interface for all functionalities 
  BlockIO& BlockIO::goto_block(size_t index){
    stream.seekg(index * kBlockSize);
    return *this;
  }

  BlockIO& BlockIO::write_block(size_t index, const Block& block){
    goto_block(index);
    stream.write((char*)(&block), kBlockSize);

    if (block_cache != std::nullopt){
      block_cache->put(index, block);
    }

    return *this;
  }

  BlockIO& BlockIO::read_block(size_t index, Block& block){
    if (block_cache != std::nullopt && block_cache->contains(index)){
      block_cache->get(index, block);
    }else{
      goto_block(index);
      stream.read((char*)(&block), kBlockSize);
    }

    return *this;
  }

  // FIXME: block cache not enabled for header
  BlockIO& BlockIO::write_header(size_t index, const BlockHeader& header){
    goto_block(index);
    stream.write((char*)(&header), sizeof(BlockHeader));
    stream.flush();
    return *this;
  }

  BlockIO& BlockIO::read_header(size_t index, BlockHeader& header){
    goto_block(index);
    stream.read((char*)(&header), sizeof(BlockHeader));
    return *this;
  }

  BlockView::operator Block(){
    Block theReadBlock;
    base->read_block(index, theReadBlock);
    return theReadBlock;
  }

  BlockView::operator BlockHeader(){
    BlockHeader theReadHeader;
    base->read_header(index, theReadHeader);
    return theReadHeader;
  }

  void BlockView::operator=(const Block& aBlock){
    base->write_block(index, aBlock);
  }

  void BlockView::operator=(const BlockHeader& aHeader){
    base->write_header(index, aHeader);
  }
  
}
