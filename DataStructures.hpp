#ifndef DATA_STRUCTURES_HPP
#define DATA_STRUCTURES_HPP

#include <map>
#include "BlockIO.hpp"
#include "Storable.hpp"
#include "Errors.hpp"

namespace ECE141{
    
    
    #if 0
    template<typename T>
    class Vector : public Storable {
        public:
        Vector(const std::vector<T>& anOtherVector) {contained_vector = anOtherVector;}
        operator std::vector<T>() {return contained_vector;}

        StatusResult encode(std::ostream& anOutput) override {
            Size size = contained_vector.size();
            size.encode(anOutput);
            for(uint32_t i = 0; i < size; i++){
                contained_vector[i].encode(anOutput);
            }
        }

        StatusResult decode(std::istream& anInput) override {
            Size size;
            size.decode(anInput);
            contained_vector.clear();
            for(uint32_t i = 0; i < size; i++){
                T element;
                element.decode(anInput);
                contained_vector.push_back(element);
            }
        }

        private:
        std::vector<T> contained_vector;
    };
    #endif
    
    class String : public Storable {
        public:
        String(const std::string& other) {contained_string = other;}
        operator std::string() {return contained_string;}

        StatusResult encode(std::ostream& anOutput) override;
        StatusResult decode(std::istream& anInput) override;

        private:
        std::string contained_string;
    };

    class Size : public Storable {
        public:
        Size() {contained_integer = 0;}
        Size(const uint32_t& other) {contained_integer = other;}
        operator uint32_t() const {return contained_integer;}

        bool operator<(const Size& other) const {
            return contained_integer < other.contained_integer;
        }

        StatusResult encode(std::ostream& anOutput) override;
        StatusResult decode(std::istream& anInput) override;

        private:
        uint32_t contained_integer; 
    };

    using Hash = Size;
    using BlockIndex = Size;

    class IndexMap : public Storable, public std::map<Size, Size> {
    public:
        IndexMap() {}
        IndexMap(const Block& aBlock){
            std::stringstream ss;
            ss.write(aBlock.payload, aBlock.header.valid_data_length);
            ss.seekg(0);
            decode(ss);
        }

        operator Block() {
            Block block;
            std::stringstream ss;
            encode(ss);

            // FIXME: check encoding size
            block.header.type = static_cast<char>(BlockType::linker_block);
            block.header.identity_hash = 0;
            ss.seekg(0, std::ios::end);
            block.header.valid_data_length = ss.tellg();

            ss.seekg(0);
            ss.read(block.payload, block.header.valid_data_length);

            return block;
        }

        StatusResult encode(std::ostream& anOutput) override {
            Size theMapSize = this->size();
            theMapSize.encode(anOutput);
            for (auto kv : (*this)) {
                Size key = kv.first;
                Size value = kv.second;
                key.encode(anOutput);
                value.encode(anOutput);
            }
            return StatusResult{ Errors::noError };
        }

        StatusResult decode(std::istream& anInput) override {
            Size theMapSize;
            theMapSize.decode(anInput);
            for (uint32_t i = 0; i < theMapSize; i++) {
                Size key;
                Size value;
                key.decode(anInput);
                value.decode(anInput);

                (*this)[key] = value;
            }
            return StatusResult{ Errors::noError };
        }

    };

}

#endif