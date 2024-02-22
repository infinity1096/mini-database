#ifndef TEST_CACHE_HPP
#define TEST_CACHE_HPP


#include <map>
#include <sstream>
#include "Testable.hpp"
#include "Config.hpp"
#include "Storage.hpp"
#include "Cache.hpp"
#include "Faked.hpp"


namespace ECE141{

    class TestCache : public Testable {
        public:
        using TestCacheFunc = bool (TestCache::*)();

        std::map<std::string, TestCacheFunc> testFunctions {
            {"testCacheCtor", &TestCache::testCacheCtor},
            {"testCapacityLimit", &TestCache::testCapacityLimit},
            {"testMostRecent", &TestCache::testMostRecent},
            {"testValidity", &TestCache::testValidity},
            {"testUpdate", &TestCache::testUpdate},
            {"testBlockCache", &TestCache::testBlockCache},
        };

        bool testCacheCtor(){
            LRUCache<std::string, std::string> cache(5);

            bool success = true;

            success &= cache.size() == 0;
            success &= cache.contains("aaa") == false;
            
            return success;
        }

        bool testCapacityLimit(){
            LRUCache<std::string, std::string> cache(10);

            bool success = true;

            for (size_t i = 0; i < 100; i++){
                cache.put(std::to_string(i), std::to_string(i));

                success &= cache.size() == (i < 10 ? i + 1 : 10);
            }

            return success;
        }

        bool testMostRecent(){
            LRUCache<std::string, std::string> cache(5);

            bool success = true;

            // fill in 5 elements, key 0 is at last now.
            for (size_t i = 0; i < 5; i++){
                cache.put(std::to_string(i), std::to_string(i));
            }

            // access key 0
            std::string empty;
            cache.get(std::string("0"), empty);

            // fill in another 4 elements
            for (size_t i = 0; i < 4; i++){
                cache.put(std::to_string(i + 5), std::to_string(i + 5));
            }

            // shoule still able to access key 0
            success &= cache.contains(std::string("0"));
            success &= !cache.contains(std::string("1"));
            success &= !cache.contains(std::string("2"));
            success &= !cache.contains(std::string("3"));
            success &= !cache.contains(std::string("4"));
            
            return success;
        }

        bool testValidity(){
            LRUCache<std::string, std::string> cache(100);

            bool success = true;

            for (size_t i = 0; i < 100; i++){
                cache.put(std::to_string(i), std::to_string(i));
            }

            for (size_t i = 0; i < 100; i++){
                std::string output;
                cache.get(std::to_string(i), output);
                success &=  output == std::to_string(i);
            }


            return success;
        }

        bool testUpdate(){
            LRUCache<std::string, std::string> cache(100);

            bool success = true;

            for (size_t i = 0; i < 100; i++){
                cache.put(std::to_string(i), std::to_string(i));
            }

            for (size_t i = 0; i < 100; i++){
                cache.put(std::to_string(i), std::to_string(i + 10));
            }
            

            for (size_t i = 0; i < 100; i++){
                std::string output;
                cache.get(std::to_string(i), output);
                success &=  output == std::to_string(i + 10);
            }


            return success;
        }

        bool testBlockCache(){

            size_t testSize = 1000;
            size_t readRepeat = 10;

            double time_nocache_write, time_nocache_read;
            Config::setCacheSize(CacheType::block, 0);
            BlockIO no_cache(Config::getDBPath("blockCacheTest"), true);

            Timer timer1;
            std::vector<size_t> index_no_cache;
            // write to 
            for(size_t i = 0; i < testSize; i++){
                Block newBlock;
                size_t idx = no_cache.consumeNextEmptyBlock();
                index_no_cache.push_back(idx);
                no_cache[idx] = newBlock;
            }

            time_nocache_write = timer1.elapsed();
            timer1.reset();

            Block b1;
            for(size_t j = 0; j < readRepeat; j++){
                for(size_t i = 0; i < testSize; i++){
                    no_cache.read_block(index_no_cache[i], b1);
                }
            }

            time_nocache_read = timer1.elapsed();

            double time_cache_write, time_cache_read;

            Config::setCacheSize(CacheType::block, testSize);

            BlockIO cache(Config::getDBPath("blockCacheTest2"), true);

            Timer timer2;
            std::vector<size_t> index_cache;
            // write to 
            for(size_t i = 0; i < testSize; i++){
                Block newBlock;
                size_t idx = cache.consumeNextEmptyBlock();
                index_cache.push_back(idx);
                cache[idx] = newBlock;
            }
            time_cache_write = timer2.elapsed();
            timer2.reset();

            Block b2;

            for(size_t j = 0; j < readRepeat; j++){
                for(size_t i = 0; i < testSize; i++){
                    cache.read_block(index_no_cache[i], b2);
                }
            }

            time_cache_read = timer2.elapsed();

            std::cout << "Cache ON: \n";
            std::cout << "Read: " << time_cache_read << "Write: " << time_cache_write;
            std::cout << "Cache OFF: \n";
            std::cout << "Read: " << time_nocache_read << "Write: " << time_nocache_write;
            
             
            return (time_cache_read + time_cache_write) < (time_nocache_read + time_nocache_write);
        }

        // testable infrastructure
        TestCache(){
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