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

class SHA1Generator {
public:

    struct Hash {
        unsigned int m_hash[5];
    };

    void start();

    Hash end();

    void addString(const std::string & str);

    void addBuffer(const void * buffer, unsigned int length);

    static std::string convertHashToString(const Hash & hash);

private:

    unsigned long long int m_length;

    unsigned int m_hash[HASH_COUNT];

    unsigned int m_actualIndex;

    char m_buffer[HASH_BUFFER_SIZE];

    unsigned int rotateLeft(const unsigned int value,
                            int shift);

    void blockProcess(const char * block);
};

#endif // SHA1_H
