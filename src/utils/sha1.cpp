#include <utils/sha1.h>
#include <cstring>
#include <sstream>
#include <iostream>

void SHA1Generator::start() {
    m_hash[0] = START_SHA1_0;
    m_hash[1] = START_SHA1_1;
    m_hash[2] = START_SHA1_2;
    m_hash[3] = START_SHA1_3;
    m_hash[4] = START_SHA1_4;

    m_length = 0;
    m_actualIndex = 0;
}

SHA1Generator::Hash SHA1Generator::end() {

    //unsigned long long int appendLength = 448 - (m_length + 1) % 512;
    // a vegere bemasoljuk amit be kell masolni
    unsigned int remaining = HASH_BUFFER_SIZE - m_actualIndex;
    memset(m_buffer + m_actualIndex, 0, remaining);
    m_buffer[m_actualIndex] |= 0x80;
    m_length *= 8ULL;
    //m_length = 0x158ULL;
    memcpy(m_buffer + HASH_BUFFER_SIZE - sizeof(m_length),
           &m_length, sizeof(m_length));
    unsigned int index;
    for (index = 0; index < sizeof(m_length) / 2; index++) {
        char temp = m_buffer[HASH_BUFFER_SIZE - index - 1];
        m_buffer[HASH_BUFFER_SIZE - index - 1] =
                m_buffer[HASH_BUFFER_SIZE - sizeof(m_length) + index];
        m_buffer[HASH_BUFFER_SIZE - sizeof(m_length) + index] = temp;
    }

    blockProcess(m_buffer);

    Hash hash;
    for (index = 0; index < 5; index++) {
        hash.m_hash[index] = m_hash[index];
    }
    return hash;
}

unsigned int SHA1Generator::rotateLeft(const unsigned int value, int shift) {
    if ((shift &= sizeof (value)*8 - 1) == 0)
        return value;
    return (value << shift) | (value >> (sizeof (value)*8 - shift));
}

void SHA1Generator::blockProcess(const char * block) {
    unsigned int words[80];
    unsigned int index;
    for (index = 0; index < 16; index++) {
        union _aux {
            unsigned int word;
            char ch[4];
        } aux;
        aux.ch[0] = block[index * 4 + 3];
        aux.ch[1] = block[index * 4 + 2];
        aux.ch[2] = block[index * 4 + 1];
        aux.ch[3] = block[index * 4];
        words[index] = aux.word;
    }

    for (index = 16; index < 80; index++) {
        words[index] = rotateLeft((words[index - 3] ^ words[index - 8] ^ words[index - 14] ^ words[index - 16]), 1);
    }

    unsigned int a = m_hash[0];
    unsigned int b = m_hash[1];
    unsigned int c = m_hash[2];
    unsigned int d = m_hash[3];
    unsigned int e = m_hash[4];
    for (index = 0; index < 80; index++) {
        /*std::cout << std::hex;
        std::cout.width(8);
        std::cout.fill('0');
        std::cout << words[index] << std::endl;*/
        unsigned int k;
        unsigned int f;
        if (index < 20) {
            f = d ^ (b & (c^d));
            k = 0x5A827999; //0x5A827999;
        } else if (index < 40) {
            f = b ^ c ^ d;
            k = 0x6ED9EBA1; //0x6ED9EBA1;
        } else if (index < 60) {
            f = (b & c) | (b & d) | (c & d);
            k = 0x8F1BBCDC; //0x8F1BBCDC;
        } else {
            f = b ^ c ^ d;
            k = 0xCA62C1D6; //0xCA62C1D6;
        }
        unsigned int temp;
        temp = rotateLeft(a, 5) + f + e + k + words[index];
        e = d;
        d = c;
        c = rotateLeft(b, 30);
        b = a;
        a = temp;
    }
    m_hash[0] += a;
    m_hash[1] += b;
    m_hash[2] += c;
    m_hash[3] += d;
    m_hash[4] += e;
}

void SHA1Generator::addBuffer(const void * buffer, unsigned int length) {
    m_length += length;
    unsigned int remaining = HASH_BUFFER_SIZE - m_actualIndex;
    if (remaining > length) {
        memcpy(m_buffer + m_actualIndex, buffer, length);
        m_actualIndex += length;
    } else {
        unsigned int index;

        // az elejet feldolgozzuk
        memcpy(m_buffer + m_actualIndex, buffer, remaining);
        blockProcess(m_buffer);

        // aztan a maradek 64 bajtos egysegeket
        length -= remaining;
        for (index = 0; index < length / HASH_BUFFER_SIZE; index++) {
            blockProcess(static_cast<const char*>(buffer) + index * HASH_BUFFER_SIZE + remaining);
        }

        // vegul ha marad, azt elhelyezzuk a bufferben
        memcpy(m_buffer, static_cast<const char*>(buffer) + index * HASH_BUFFER_SIZE  + remaining,
               length % HASH_BUFFER_SIZE);
        m_actualIndex = length % HASH_BUFFER_SIZE;
    }
}

void SHA1Generator::addString(const std::string &str) {
    addBuffer( str.c_str(), str.length() );
}

std::string SHA1Generator::convertHashToString(const Hash & hash) {
    char localHash[ sizeof(Hash)];
    char resultHash[ sizeof(Hash)];
    unsigned int hashIndex;
    unsigned int byteIndex;
    memcpy(localHash, &hash, sizeof(Hash));
    for (hashIndex = 0; hashIndex < HASH_COUNT; hashIndex++) {
        for (byteIndex = 0; byteIndex < sizeof(unsigned int); byteIndex++) {
            resultHash[hashIndex * sizeof(unsigned int) + byteIndex] =
                    localHash[hashIndex * sizeof(unsigned int) +
                    (sizeof(unsigned int) - byteIndex - 1) ];
        }
    }
    std::ostringstream stream;
    std::cout << std::hex;
    stream << std::hex;
    for (byteIndex = 0; byteIndex < sizeof(Hash); byteIndex++) {
        stream.width(2);
        stream.fill('0');

        //std::cout.width(2);
        //std::cout.fill('0');
        int num = resultHash[byteIndex] & 0xFF;
        //std::cout << num << " | ";

        stream << num;
        if ((byteIndex + 1) % 4 == 0 && byteIndex < sizeof(Hash) - 1) {
            stream << " ";
        }
    }
    std::cout << std::endl;
    std::string result = stream.str();
    for (byteIndex = 0; byteIndex < result.length(); byteIndex++) {
        if (result[byteIndex] >= 'a' && result[byteIndex] <= 'f') {
            result[byteIndex] = 'A' + (result[byteIndex] - 'a');
        }
    }
    return result;

}

