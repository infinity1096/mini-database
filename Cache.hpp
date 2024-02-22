#ifndef CACHE_HPP
#define CACHE_HPP

#include <list>
#include <unordered_map>

namespace ECE141{

    template<typename T>
    class ICache{
        public:
        ICache(T* base) : base_ptr(base) {}

        T& get(){
            if (!is_loaded){
                base_ptr->load();
                is_loaded = true;
            }
            
            return *base_ptr;
        }

        private:
        bool is_loaded = false;
        T* base_ptr;
    };

    template<typename KeyT, typename ValueT>
    class LRUCache {
    public:
        LRUCache(size_t maxSize) : max_size(maxSize) {}
        LRUCache(const LRUCache& other) {*this = other;}
        ~LRUCache() {}

        // FIXME: Warning that this means LRU cache always copy by reference! 
        LRUCache& operator=(const LRUCache& other){
            key_list_map = other.key_list_map;
            key_list = other.key_list;
            map = other.map;
            max_size = other.max_size;
            return *this;
        }
        
        void put(const KeyT &key, const ValueT& value){
            if (contains(key)){
                update_element(key, value).move_to_front(key);
            }else{
                if (size() == max_size){
                    evict_last().insert_front(key, value);
                }else{
                    insert_front(key, value);
                }
            }
        }

        void get(const KeyT& key, ValueT& outputValue){
            if (contains(key)){
                access_element(key, outputValue).move_to_front(key);   
            }
        }

        void remove(const KeyT& key){
            if (contains(key)) {
                map.erase(key);
                key_list.erase(key_list_map.at(key));
                key_list_map.erase(key);
            }
        }

        bool contains(const KeyT& key) const {
            return map.count(key) != 0;
        }

        size_t  size() const {
            return map.size();
        }

    protected:

        inline LRUCache& evict_last(){
            map.erase(key_list.back());
            key_list_map.erase(key_list.back());
            key_list.pop_back();

            return *this;
        }

        inline LRUCache& move_to_front(const KeyT& key){
            key_list.erase(key_list_map.at(key));
            key_list.push_front(key);
            key_list_map.at(key) = key_list.begin();

            return *this;
        }

        inline LRUCache& insert_front(const KeyT& key, const ValueT& value){
            key_list.push_front(key);
            key_list_map[key] = key_list.begin();

            map[key] = value;
            return *this;
        }

        inline LRUCache& update_element(const KeyT& key, const ValueT& value){
            map.at(key) = value;
            return *this;
        }
        
        inline LRUCache& access_element(const KeyT& key, ValueT& output){
            output = map.at(key);
            return *this;
        }

        size_t max_size; //prevent cache from growing past this size...

        // store map to address of key for O(1) access
        std::unordered_map<KeyT, typename std::list<KeyT>::iterator> key_list_map;
        std::list<KeyT>   key_list;
        std::unordered_map<KeyT, ValueT> map;
    };

}

#endif