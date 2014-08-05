/**
 * @file sha1.h This file contains the API of the SHA1Generator class.
 * @author Jozsef Smidla
 */

#ifndef SHA1_H
#define SHA1_H

#include <globals.h>
#include <string>

#define START_SHA1_0 0x67452301
#define START_SHA1_1 0xefcdab89
#define START_SHA1_2 0x98badcfe
#define START_SHA1_3 0x10325476
#define START_SHA1_4 0xc3d2e1f0

#define HASH_COUNT 5
#define HASH_BUFFER_SIZE 64


/**
 * This class is able to generate SHA1 hashes.
 *
 * @class SHA1Generator
 */
class SHA1Generator {
public:

    /**
     * This struct represents a SHA1 hash.
     */
    struct Hash {
        unsigned int m_hash[5];
    };

    /**
     * Initializes the hash generator to the SHA1 starting values.
     */
    void start();

    /**
     * Finishes the hashing process and returns with the hash value.
     *
     * @see Hash
     * @return The hash.
     */
    Hash end();

    /**
     * Adds a specific string to the buffer.
     *
     * @see m_buffer
     * @param str The string to be added.
     */
    void addString(const std::string & str);

    /**
     * Adds data to the hashing buffer.
     *
     * @see m_buffer
     * @param buffer Pointer to the data to be added.
     * @param length The size of the data of be added.
     */
    void addBuffer(const void * buffer, unsigned int length);

    /**
     * Converts the given Hash to string.
     *
     * @param hash The hash to be converted.
     * @return The hash converted to string.
     */
    static std::string convertHashToString(const Hash & hash);

private:

    /**
     * The total size of the hashed data.
     */
    unsigned long long int m_length;

    /**
     * The current hash of the processed data.
     */
    unsigned int m_hash[HASH_COUNT];

    /**
     * The actual size of the data in the buffer.
     */
    unsigned int m_actualIndex;

    /**
     * The buffer of the data to be hashed.
     * If the buffer fills up, its block can be hashed.
     */
    char m_buffer[HASH_BUFFER_SIZE];

    /**
     * @brief rotateLeft
     * @param value
     * @param shift
     * @return
     */
    unsigned int rotateLeft(const unsigned int value,
                            int shift);

    /**
     * Hashes a 64-byte long data block.
     *
     * @param block The block to be hashed.
     */
    void blockProcess(const char * block);
};

#endif // SHA1_H
