#ifndef TEST_INDEX_HPP
#define TEST_INDEX_HPP


#include <map>
#include <sstream>
#include "Testable.hpp"
#include "Config.hpp"
#include "Storage.hpp"
#include "Index.hpp"
#include "Faked.hpp"


namespace ECE141{

    class TestIndex : public Testable {
        public:
        using TestIndexFunc = bool (TestIndex::*)();

        std::map<std::string, TestIndexFunc> testFunctions {
            {"testIndexCtor", &TestIndex::testIndexCtor},
            {"testIndexInsertInt", &TestIndex::testIndexInsertInt},
            {"testIndexInsertString", &TestIndex::testIndexInsertString},
            {"testIndexErase", &TestIndex::testErase},
            {"testIndexEraseString", &TestIndex::testEraseString},
            {"testEncodeDecode", &TestIndex::testEncodeDecode},
            {"testEncodeDecodeString", &TestIndex::testEncodeDecodeString},
            {"testBlockVisitor", &TestIndex::testBlockVisitor},
        };

        bool testIndexCtor(){
            Storage* theTestStorage = getTestStorage();
            Index index(theTestStorage);

            bool success = true;

            success &= index.isChanged() == false;

            IndexKey dummyIndexKey = (uint32_t)0;
            success &= index.valueAt(dummyIndexKey) == std::nullopt;
            success &= index.getSize() == 0;

            delete theTestStorage;
            return success;
        }

        bool testIndexInsertInt(){
            Storage* theTestStorage = getTestStorage();
            Index index(theTestStorage);

            bool success = true;

            // insert index
            for (size_t i = 0; i < 256; i++){
                index[i] = i;
                success &= index.getSize() == i + 1;
            }

            index[0] = 999;
            success &= index.getSize() == 256; // size not change

            // verify index through read function
            for (size_t i = 1; i < 256; i++){
                IntOpt ind = index[i];
                success &= ind != std::nullopt && (ind.value()) == i;
            }

            IntOpt ind = index[0];
            success &= ind != std::nullopt && (ind.value()) == 999;

            success &= index.getSize() == 256; // size not change
            success &= index.isChanged() == true;

            delete theTestStorage;
            return success;
        }

        bool testIndexInsertString(){
            Storage* theTestStorage = getTestStorage();
            Index index(theTestStorage, IndexType::strKey);

            bool success = true;

            // insert index
            for (size_t i = 0; i < 256; i++){
                index[std::to_string(i)] = i;
                success &= index.getSize() == i + 1;
            }

            index[std::string("0")] = 999;
            success &= index.getSize() == 256; // size not change

            // verify index through read function
            for (size_t i = 1; i < 256; i++){
                IntOpt ind = index[std::to_string(i)];
                success &= ind != std::nullopt && (ind.value()) == i;
            }

            IntOpt ind = index[std::string("0")];
            success &= ind != std::nullopt && (ind.value()) == 999;

            success &= index.getSize() == 256; // size not change
            success &= index.isChanged() == true;

            delete theTestStorage;
            return success;
        }

        bool testErase(){
            Storage* theTestStorage = getTestStorage();
            Index index(theTestStorage);

            bool success = true;

            // insert index
            for (size_t i = 0; i < 256; i++){
                index[i] = i;
                success &= index.getSize() == i + 1;
            }

            success &= index.getSize() == 256;

            // --- test success erase ----

            index.setChanged(false);
            success &= index.erase(0); // should be success operation
            success &= index.getSize() == 255; // size should decrease by 1
            success &= index.isChanged(); // database is changed

            for (size_t i = 1; i < 256; i++){
                IntOpt ind = index[i];
                success &= ind != std::nullopt && ind.value() == i; // other index should not change
            }

            IntOpt ind = index[0]; 
            success &= ind == std::nullopt;
            
            // --- test failed erase ----

            index.setChanged(false);
            success &= !index.erase(0); // should be fail operation
            success &= index.getSize() == 255; // size should be same
            success &= !index.isChanged(); // database is NOT changed

            for (size_t i = 1; i < 256; i++){
                IntOpt ind = index[i];
                success &= ind != std::nullopt && ind.value() == i; // other index should not change
            }

            IntOpt ind2 = index[0]; 
            success &= ind2 == std::nullopt;

            // ---- test type mismatch ----
            success &= !index.erase("something");

            delete theTestStorage;
            return success;
        }

        bool testEraseString(){
            Storage* theTestStorage = getTestStorage();
            Index index(theTestStorage, IndexType::strKey);

            bool success = true;

            // insert index
            for (size_t i = 0; i < 256; i++){
                index[std::to_string(i)] = i;
                success &= index.getSize() == i + 1;
            }

            success &= index.getSize() == 256;

            // --- test success erase ----

            index.setChanged(false);
            success &= index.erase(std::to_string(0)); // should be success operation
            success &= index.getSize() == 255; // size should decrease by 1
            success &= index.isChanged(); // database is changed

            for (size_t i = 1; i < 256; i++){
                IntOpt ind = index[std::to_string(i)];
                success &= ind != std::nullopt && ind.value() == i; // other index should not change
            }

            IntOpt ind = index[std::to_string(0)]; 
            success &= ind == std::nullopt;
            
            // --- test failed erase ----

            index.setChanged(false);
            success &= !index.erase(std::to_string(0)); // should be fail operation
            success &= index.getSize() == 255; // size should be same
            success &= !index.isChanged(); // database is NOT changed

            for (size_t i = 1; i < 256; i++){
                IntOpt ind = index[std::to_string(i)];
                success &= ind != std::nullopt && ind.value() == i; // other index should not change
            }

            IntOpt ind2 = index[std::to_string(0)]; 
            success &= ind2 == std::nullopt;

            // ---- test type mismatch ----
            success &= !index.erase(1);

            delete theTestStorage;
            return success;
        }

        bool testEncodeDecode(){
            std::stringstream buffer;

            Storage* theTestStorage = getTestStorage();
            Index index(theTestStorage, IndexType::intKey);

            bool success = true;

            // insert index
            for (size_t i = 0; i < 256; i++){
                index[i] = i;
                success &= index.getSize() == i + 1;
            }

            index.encode(buffer);

            buffer.seekg(0);

            Index index2(theTestStorage, IndexType::intKey);
            index2.decode(buffer);

            for (size_t i = 0; i < 256; i++){
                IntOpt ind = index2[i];
                success &= ind != std::nullopt && ind.value() == i;
            }

            success &= index.getSize() == index2.getSize();

            delete theTestStorage;
            return success;
        }

        bool testEncodeDecodeString(){
            std::stringstream buffer;

            Storage* theTestStorage = getTestStorage();
            Index index(theTestStorage, IndexType::strKey);

            bool success = true;

            // insert index
            for (size_t i = 0; i < 256; i++){
                index[std::to_string(i)] = i;
                success &= index.getSize() == i + 1;
            }

            index.encode(buffer);

            buffer.seekg(0);

            Index index2(theTestStorage, IndexType::intKey);
            index2.decode(buffer);

            for (size_t i = 0; i < 256; i++){
                IntOpt ind = index2[std::to_string(i)];
                success &= ind != std::nullopt && ind.value() == i;
            }

            success &= index.getSize() == index2.getSize();
            
            delete theTestStorage;
            return success;
        }

        bool testBlockVisitor(){
            Storage* theTestStorage = getTestStorage();
            Index index(theTestStorage);

            // insert some dummy data
            std::vector<std::string> dummyData;
            std::vector<size_t> savedBlockId;
            for (size_t i = 0; i < 256; i++){
                dummyData.push_back(Fake::People::full_name());
                String str = dummyData[i];

                std::stringstream ss;
                str.encode(ss);

                Block aBlock;
                ss.seekg(0, std::ios::end);
                aBlock.header.valid_data_length = ss.tellg();

                ss.seekg(0);
                ss.read(aBlock.payload, aBlock.header.valid_data_length);

                size_t savedBlock = theTestStorage->consumeNextEmptyBlock();
                savedBlockId.push_back(savedBlock);
                (*theTestStorage)[savedBlock] = aBlock;
            }

            // generate a accurate Index
            for (size_t i = 0; i < 256; i++){
                index[i] = savedBlockId[i];
            }

            std::vector<std::string> recovered;
            index.each([&](const Block& aBlock, uint32_t index){

                String str("");

                std::stringstream ss;
                ss.write(aBlock.payload, aBlock.header.valid_data_length);

                ss.seekg(0);

                str.decode(ss);

                recovered.push_back(str);
                return StatusResult{Errors::noError};
            });

            bool success = true;

            std::sort(dummyData.begin(), dummyData.end());
            std::sort(recovered.begin(), recovered.end());
            
            for (size_t i = 0; i < 256; i++) {
                success &= strcmp(dummyData[i].c_str(),recovered[i].c_str()) == 0;
            }

            delete theTestStorage;
            return success;
        }

        Storage* getTestStorage(){
            Storage* theTestStorage = new Storage(Config::getDBPath("IndexTest"), AsNew{});
            return theTestStorage;
        }

        // testable infrastructure
        TestIndex(){
            count = getNumTests();
        }

        size_t getNumTests(){
            return testFunctions.size();
        }
    
        std::optional<std::string> getTestName(size_t index) const {
            size_t current_index = 0;

            for (auto kv : testFunctions){
                if (index == current_index++){
                    return kv.first;
                }
            }

            return std::nullopt;
        }

        bool operator() (const std::string& testName){
            if (testFunctions.count(testName) > 0){
                return (this->*testFunctions[testName])();
            }else{
                return false;
            }
        }
    };

}

#endif