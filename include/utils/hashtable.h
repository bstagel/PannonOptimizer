/**
 * @file hashtable.h
 * @author smidla
 */

#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <globals.h>
#include <vector>

#include <map>

#include <iostream>

/**
 * This array contains the sizes for a hash table hashing specified number of entries.
 */
extern const unsigned int hashTableSizes[];

/**
 * The number of sizes in hashTableSizes.
 *
 * @see hashTableSizes
 */
extern const unsigned int hashTableSizesSize;

/**
 * This class describes a hash table, able to store and get entries.
 * The hashing algorithm and the hashed type in template.
 *
 * @class HashTable
 */
template<class TYPE, class HASH_FUNCTION>
class HashTable {
public:

    /**
     * Constructor of the HashTable class.
     *
     * @constructor
     */
    HashTable() {
        m_count = 0;
    }

    /**
     * Destructor of the HashTable class.
     *
     * @destructor
     */
    virtual ~HashTable() {

    }

    /**
     * Sets the hash table size to be able to contain a given number of entries.
     *
     * @param size The number of entries allowed in the hash table.
     */
    void setSize(unsigned int size) {
        size *= 1.1;
        unsigned int start = 0;
        unsigned int end = hashTableSizesSize - 1;
        unsigned int index = (start + end) >> 1;
        while (start < end && hashTableSizes[index] != size) {
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

    /**
     * Adds an entry to the hash table.
     * The hashing function is determined by the HASH_FUNCTION template.
     *
     * @param value The entry to be added.
     */
    void add(const TYPE & value) {
        unsigned int hash = HASH_FUNCTION::getHash(value) % m_table.size();
        m_table[hash].push_back(value);
        m_count++;
    }

    /**
     * Adds an entry to the hash table, if its hash is unique.
     *
     * @param value The entry to be added.
     * @return True if the hash of the entry is unique.
     */
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

    /**
     * Gets an entry from the hash table by its hash.
     *
     * @param value The entry value to be searched for.
     * @return Pointer to the entry with the same hash with value. Returns 0 if there is no entry such that.
     */
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

    /**
     * Returns the number of entries in the hash table.
     *
     * @return The number of entries in the hash table.
     */
    unsigned int getCount() const {
        return m_count;
    }

    /**
     * Prints the maximum number of entries with unique hash to the output.
     */
    void showCounters() {
        typename std::vector< std::vector<TYPE> >::const_iterator iter = m_table.begin();
        typename std::vector< std::vector<TYPE> >::const_iterator iterEnd = m_table.end();
        unsigned int max = 0;
        for (; iter != iterEnd; iter++) {
            if (iter->size() > max) {
                max = iter->size();
            }
            //std::cout << iter->size() << std::endl;
            //std::cin.get();
        }
        std::cout << "MAX = " << max << std::endl;
    }

private:

    /**
     * The table storing the entries in the hashed position.
     */
    std::vector< std::vector<TYPE> > m_table;

    /**
     * The number of entries in the hash table.
     */
    unsigned int m_count;
};

#endif // HASHTABLE_H
