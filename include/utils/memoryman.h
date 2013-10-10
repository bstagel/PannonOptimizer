/**
 * @file memoryman.h
 */

#ifndef MEMORYMAN_H
#define MEMORYMAN_H

#define CLASSIC_NEW_DELETE

#ifndef CLASSIC_NEW_DELETE

#include <new>
#include <cstdlib>
#include <iostream>
#include <cstdio>


struct ChunkStack;

struct Chunk {
    Chunk * m_previous;
    ChunkStack * m_stack;
};

struct Buffer {
    Buffer * m_next;
    unsigned int m_size;
    char * m_bufferStart;
    char * m_bufferEnd;
    char * m_actual;
};

class Pool {
    friend class MemoryManagerInitializer;
public:
    static void init() {
        sm_head = static_cast<Buffer*>(malloc(sizeof(Buffer)));
        sm_head->m_next = 0;
        sm_head->m_bufferStart = static_cast<char*>(malloc( sm_poolSize ));
        sm_head->m_actual = sm_head->m_bufferStart;
        sm_head->m_size = sm_poolSize;
        sm_head->m_bufferEnd = sm_head->m_bufferStart + sm_poolSize;
    }

    static void release() {
        //        sm_head = 0;
        //        return;
        Buffer * actual = sm_head;
        while (actual != 0) {
            Buffer * next = actual->m_next;
            free(actual->m_bufferStart);
            free(actual);
            actual = next;
        }
        sm_head = 0;
    }

    inline static Chunk * getChunk(size_t size) {
        Buffer * actual = sm_head;
        size += sizeof(Chunk);
        while ( actual != 0 && ((actual->m_bufferEnd - actual->m_actual) < (long int)size) ) {
            actual = actual->m_next;
        }
        if (actual == 0) {
            Buffer * newBuffer = static_cast<Buffer*>(malloc(sizeof(Buffer)));
            newBuffer->m_next = sm_head;
            sm_head = newBuffer;
            newBuffer->m_bufferStart = static_cast<char*>(malloc( sm_poolSize ));
            newBuffer->m_actual = newBuffer->m_bufferStart;
            newBuffer->m_size = sm_poolSize;
            newBuffer->m_bufferEnd = newBuffer->m_bufferStart + sm_poolSize;
            actual = newBuffer;
        }
        Chunk * result = reinterpret_cast<Chunk*>(actual->m_actual);
        actual->m_actual += size;
        return result;
    }

    static Buffer * sm_head;

private:
    static const int sm_poolSize = 10485760*3;

};

class ChunkStack {
public:
    void init() {
        m_head = 0;
    }

    void release() {

    }

    inline Chunk * getChunk(size_t size) {
        Chunk * result;
        if (m_head != 0) {
            result = m_head;
            m_head = m_head->m_previous;
        } else {
            result = Pool::getChunk(size);
            result->m_stack = this;
            result->m_previous = 0;
        }
        return result;
    }

    inline void addChunk(Chunk * chunk) {
        chunk->m_previous = m_head;
        m_head = chunk;
    }

private:
    Chunk * m_head;
};

class MemoryManager {
    friend class MemoryManagerInitializer;
public:
    MemoryManager() {
        //std::cout << "Initialize memory manager" << std::endl;
        //init();
    }

    ~MemoryManager() {
        release();
    }

    inline static void * allocate(size_t size) {
        Chunk * chunk;
        if (size <= (size_t)sm_maxSmallStackSize) {
            chunk = sm_smallStacks[size >> 2].getChunk(size);
        } else if (size <= (size_t) sm_maxLargeStackSize) {
            chunk = sm_largeStacks[size >> 10].getChunk(size);
        } else {
            chunk = static_cast<Chunk*>(malloc( sizeof(Chunk) + size ));
            chunk->m_stack = 0;
        }
        return chunk + 1;
        //return chunk->m_start;
    }

    inline static void release(void * pointer) {
        Chunk * chunk = reinterpret_cast<Chunk*>(reinterpret_cast<char*>(pointer) - sizeof(Chunk));
        if (chunk->m_stack != 0) {
            chunk->m_stack->addChunk(chunk);
        } else {
            free(chunk);
        }
    }

private:

    static const int sm_smallSteps = 4;
    static const int sm_maxSmallStackSize = 1024;

    static const int sm_largeSteps = 1024;
    static const int sm_maxLargeStackSize = 1024 * 1024;

    static ChunkStack * sm_smallStacks;
    static ChunkStack * sm_largeStacks;

    static void init() {
        Pool::init();
        unsigned int count = sm_maxSmallStackSize / sm_smallSteps + 1;
        sm_smallStacks = static_cast<ChunkStack*>(malloc( count * sizeof(ChunkStack) ));
        unsigned int index;
        for (index = 0; index < count; index++) {
            sm_smallStacks[index].init();
        }

        count = sm_maxLargeStackSize / sm_largeSteps + 1;
        sm_largeStacks = static_cast<ChunkStack*>(malloc( count * sizeof(ChunkStack) ));
        for (index = 0; index < count; index++) {
            sm_largeStacks[index].init();
        }
        //printf("Memory manager initialized\n");

    }

    static void release() {
        unsigned int index;
        unsigned int count = sm_maxSmallStackSize / sm_smallSteps;
        for (index = 0; index < count; index++) {
            sm_smallStacks[index].release();
        }
        free(sm_smallStacks);
        sm_smallStacks = 0;
        count = sm_maxLargeStackSize / sm_largeSteps;
        for (index = 0; index < count; index++) {
            sm_largeStacks[index].release();
        }
        free(sm_largeStacks);
        sm_largeStacks = 0;
        Pool::release();
        printf("Memory manager released\n");
    }
};

inline void * operator new (size_t size) {
    //if ( Pool::sm_head != 0 ) {
        return MemoryManager::allocate(size);
    //}
    //return malloc(size);
}

inline void operator delete(void * p) {
    if (p != 0) {
        //if ( Pool::sm_head != 0 ) {
            MemoryManager::release(p);
        //} else {
            // free(p);
        //}
    }
}

class MemoryManagerInitializer {
public:
    MemoryManagerInitializer() {
        static MemoryManager manager;
        static bool first = true;
        if (first == true) {
            manager.init();
            first = false;
        }
    }
};

static MemoryManagerInitializer memoryInitializer;

#endif

#endif // MEMORYMAN_H
