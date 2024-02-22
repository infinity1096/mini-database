#ifndef TEST_STORAGE_HPP
#define TEST_STORAGE_HPP


#include <map>
#include <sstream>
#include <optional>
#include "Testable.hpp"
#include "Storage.hpp"
#include "Config.hpp"

namespace ECE141{

    class TestStorage : public Testable {
        public:
        using TestTestStorageFunc = bool (TestStorage::*)();

        std::map<std::string, TestTestStorageFunc> testFunctions {
            {"testInitialize", &TestStorage::testInitialize},    
            {"testDataSave", &TestStorage::testDataSave},
            {"testEntitySave", &TestStorage::testEntitySave},
        };

        bool testInitialize(){
            Storage* storage = new Storage(Config::getDBPath("testStorage"), AsNew{});
            storage->debugDump();
            
            bool success = (storage->listEntity().size() == 0);

            delete storage;
            return success;
        }

        bool testDataSave(){
            Storage* storage = new Storage(Config::getDBPath("testStorage"), AsNew{});

            size_t verifySize = 100000; // This can be at least 100MB

            char* data = prepareRandomData(verifySize);
            SignedData signedData(data, verifySize, "Me", BlockType::data_block);

            size_t savedIndex = storage->saveData(signedData);

            delete storage;
            Storage storage2(Config::getDBPath("testStorage"), AsExisting{});
            
            auto recovered = storage2.loadData(savedIndex);

            bool success = true;
            success &= !memcmp(data, recovered.data.get(), verifySize);

            delete data;
            storage2.debugDump();

            return success;
        } 

        bool testEntitySave(){
            Storage* storage = new Storage(Config::getDBPath("testStorage"), AsNew{});
            
            Attribute a1(std::string("Primary"), DataTypes::int_type);
            Attribute a2(std::string("birthday"), DataTypes::datetime_type);
            Attribute a3(std::string("height"), DataTypes::float_type);
            
            Entity entity1(std::string("Entity1"));
            entity1.addAttribute(a1);
            entity1.addAttribute(a2);
            entity1.addAttribute(a3);

            Entity entity2(std::string("Entity2"));
            entity2.addAttribute(a1);
            entity2.addAttribute(a2);

            bool success = true;
            success &= storage->listEntity().size() == 0;
            success &= storage->size() == 2;

            storage->saveEntity(entity1);

            success &= storage->size() == 3;
            success &= storage->listEntity().size() == 1;

            storage->saveEntity(entity2);
            success &= storage->listEntity().size() == 2;
            
            delete storage;
            Storage* storage2 = new Storage(Config::getDBPath("testStorage"), AsExisting{});

            success &= storage->listEntity().size() == 2;
            Entity* entity1Recovered = storage2->loadEntity(Helpers::hashString("Entity1"));
            Entity* entity2Recovered = storage2->loadEntity(Helpers::hashString("Entity2"));
            
            success &= entity1Recovered != nullptr;
            success &= entity2Recovered != nullptr;
            
            if (success){
                success &= (*entity1Recovered) == entity1;
                success &= (*entity2Recovered) == entity2;
            }

            success &= storage2->size() == 4; // block0 + meta + 2x entity
            
            return success;
        }

        char* prepareRandomData(size_t length) {
            char* buffer = new char[length];
            char value = 0;
            for (size_t i = 0; i < length; i++) {
                value += 1;
                value *= 3;
                buffer[i] = value;
            }

            return buffer;
        }

        // testable infrastructure
        TestStorage(){
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