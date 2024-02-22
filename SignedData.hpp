#ifndef SIGNED_DATA_HPP
#define SIGNED_DATA_HPP

#include <vector>
#include <string>
#include <sstream>
#include <memory>

#include "BlockIO.hpp"

namespace ECE141{

  struct SignedData;
  using DividedSignedData = std::vector<SignedData>;
  struct SignedData {

    SignedData();
    SignedData(char* aData, size_t len, uint32_t ownerHash, BlockType type);
    SignedData(char* aData, size_t len, const std::string& owner, BlockType type);
    
    SignedData(const Block& aBlock);
    SignedData(const SignedData& other);
    SignedData(std::istream& aBuffer, uint32_t aHash, BlockType aType);
    SignedData& operator=(const SignedData& other);
    
    ~SignedData();

    // divide whole data into [num_division] parts that contains almost equal
    // portion of the data.
    DividedSignedData subDivide(size_t num_division) const ;
    SignedData& operator+=(const SignedData& other);

    operator Block() const ;
    operator std::stringstream() const ;

    std::unique_ptr<char[]> data;
    uint32_t len = 0;
    uint32_t hash;
    BlockType type;
  };

}

#endif