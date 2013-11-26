#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <globals.h>
#include <vector>

extern const unsigned int hashTableSizes[];
extern const unsigned int hashTableSizesSize;

template<class TYPE, class HASH_FUNCTION>
class HashTable {
public:
    HashTable() {
        m_count = 0;
    }

    virtual ~HashTable() {

    }

    void setSize(unsigned int size) {
        size *= 1.1;
        unsigned int start = 0;
        unsigned int end = hashTableSizesSize - 1;
        unsigned int index = (start + end) >> 1;
        while (start <= end && hashTableSizes[index] != size) {
            if (hashTableSizes[index] < size) {
                start = index + 1;
            } else if (hashTableSizes[index] > size) {
                end = index - 1;
            }
            index = (start + end) >> 1;
        }
        size = hashTableSizes[index + 1];
        m_table.clear();
        m_table.resize(size);
    }

    void add(const TYPE & value) {
        unsigned int hash = HASH_FUNCTION::getHash(value) % m_table.size();
        m_table[hash].push_back(value);
        m_count++;
    }

    bool addUnique(const TYPE & value) {
        unsigned int hash = HASH_FUNCTION::getHash(value) % m_table.size();
        typename std::vector<TYPE>::const_iterator iter = m_table[hash].begin();
        typename std::vector<TYPE>::const_iterator iterEnd = m_table[hash].end();
        while (iter < iterEnd && *iter != value) {
            iter++;
        }
        if (iter >= iterEnd) {
            m_table[hash].push_back(value);
            m_count++;
            return true;
        }
        return false;
    }

    const TYPE * get(const TYPE & value) const {
        unsigned int hash = HASH_FUNCTION::getHash(value) % m_table.size();
        typename std::vector<TYPE>::const_iterator iter = m_table[hash].begin();
        typename std::vector<TYPE>::const_iterator iterEnd = m_table[hash].end();
        while (iter < iterEnd && *iter != value) {
            iter++;
        }
        if (iter < iterEnd) {
            return &(*iter);
        }
        return 0;
    }

    unsigned int getCount() const {
        return m_count;
    }

private:
    std::vector< std::vector<TYPE> > m_table;

    unsigned int m_count;
};

#endif // HASHTABLE_H
