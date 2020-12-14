#ifndef ExpandableHashMap_h
#define ExpandableHashMap_h

#include <list>
#include <vector>
#include <iostream>
#include <utility>
#include <algorithm>

template<typename KeyType, typename ValueType>
class ExpandableHashMap
{
public:
    ExpandableHashMap(double maximumLoadFactor = 0.5);
    ~ExpandableHashMap();
    void reset();
    int size() const;
    int numbucks() const;
    void associate(const KeyType& key, const ValueType& value);

      // for a map that can't be modified, return a pointer to const ValueType
    const ValueType* find(const KeyType& key) const;

      // for a modifiable map, return a pointer to modifiable ValueType
    ValueType* find(const KeyType& key)
    {
        return const_cast<ValueType*>(const_cast<const ExpandableHashMap*>(this)->find(key));
    }

      // C++11 syntax for preventing copying and assignment
    ExpandableHashMap(const ExpandableHashMap&) = delete;
    ExpandableHashMap& operator=(const ExpandableHashMap&) = delete;

private:
    std::vector<std::list<std::pair<KeyType, ValueType>>> m_hashtable;
    int num_buckets;
    int m_size;
    double maxload;
    
    //hash function
    unsigned int getBucketNumber(const KeyType& key) const
    {
        unsigned int hasher(const KeyType& k); // prototype
        unsigned int h = hasher(key);
        return h % num_buckets;
    }
};

//returns number of buckets, not associations
template<typename KeyType, typename ValueType>
int ExpandableHashMap<KeyType, ValueType>::numbucks() const {
    return num_buckets;
}

//set load factor and stuff
template<typename KeyType, typename ValueType>
ExpandableHashMap<KeyType, ValueType>::ExpandableHashMap(double maximumLoadFactor)
:maxload(maximumLoadFactor), m_hashtable(8)
{
    m_size = 0;
    num_buckets = 8;
    
}

template<typename KeyType, typename ValueType>
ExpandableHashMap<KeyType, ValueType>::~ExpandableHashMap()
{
    m_hashtable.clear();
}

//reset all variables, resize table

template<typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType, ValueType>::reset()
{
    m_hashtable.clear();
    
    m_size = 0;
    num_buckets = 8;
    
    for (int i = 0; i <num_buckets; i++) {
        m_hashtable.resize(8);
    }
    
}

template<typename KeyType, typename ValueType>
int ExpandableHashMap<KeyType, ValueType>::size() const
{
    return m_size;
}

template<typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType, ValueType>::associate(const KeyType& key, const ValueType& value)
{
    //check if key in there, if there update
    unsigned h = getBucketNumber(key);
    typename std::list<std::pair<KeyType, ValueType>>::iterator it = m_hashtable[h].begin();
    //update value if already has same key value in there
    if (it!=m_hashtable[h].end()) {
        while(it != m_hashtable[h].end()) {
            if(it->first == key) {
                it->second = value;
                return;
            }
            it++;
        }
    }
    
        //else add value into linked list at the hashed value
        m_hashtable[h].emplace_back(key, value);
        m_size++;
        
    //rehash by splicing all the lists together, then rehash each item into the list based on the new number of buckets
        if (((double)m_size/(double)num_buckets) > maxload) {
            std::list<std::pair<KeyType, ValueType>> temp;
            typename std::list<std::pair<KeyType, ValueType>>::iterator ij = temp.begin();
            for (int i = 0; i< numbucks(); i++) {
                temp.splice(ij, m_hashtable[i]);
            }
            m_hashtable.clear();
            m_hashtable.resize(num_buckets*2);
            num_buckets = num_buckets*2;
            
            for(ij = temp.begin(); ij != temp.end(); ij++) {
                unsigned c = getBucketNumber(ij->first);
                m_hashtable[c].emplace_back(ij->first, ij->second);
            }
        }
    
    
}

//hash the passed in key, and then check that bucket for the value, if there return pointer to it, otherwise return nullptr
template<typename KeyType, typename ValueType>
const ValueType* ExpandableHashMap<KeyType, ValueType>::find(const KeyType& key) const
{
    unsigned h = getBucketNumber(key);
    typename std::list<std::pair<KeyType, ValueType>>::const_iterator it = m_hashtable[h].begin();
    if (!m_hashtable[h].empty()) {
        while(it != m_hashtable[h].end()) {
            if(it->first == key) {
                return &(it->second);
            }
            it++;
        }
    }
    
    return nullptr;
}
#endif /* Header_h */
