/**
 * @file memoryman.h
 */

#ifndef MEMORYMAN_H
#define MEMORYMAN_H

#define CLASSIC_NEW_DELETE

#ifndef CLASSIC_NEW_DELETE

#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)

#include <new>
#include <cstdlib>
#include <iostream>
#include <cstdio>

// TODO: cache hatarokra valo igazitast meg kell oldani

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
        if (unlikely(actual == 0)) {
            //printf("Uj buffer kell\n");
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
    static const int sm_poolSize = 10485760;

};

class ChunkStack {
public:
    ChunkStack() {
        m_head = 0;
    }

    void init() {
        m_head = 0;
    }

    void release() {

    }

    inline Chunk * getChunk(size_t size) {
        Chunk * result;
        //int b;
        if (m_head != 0) {
            //b = 0;
            result = m_head;
            m_head = m_head->m_previous;
        } else {
            //b = 1;
            //result = reinterpret_cast<Chunk*>(malloc(sizeof(Chunk) + size + 100));
            result = Pool::getChunk(size);

            result->m_stack = this;
            result->m_previous = 0;
        }
        //result->m_stack = this;


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

    static bool sm_initialized;

    MemoryManager() {
        //std::cout << "Initialize memory manager" << std::endl;
        //init();
    }

    ~MemoryManager() {
        release();
    }

    inline static void * allocate(size_t size) {
        Chunk * chunk;
        if (likely(size <= (size_t)sm_maxSmallStackSize)) {
            chunk = sm_smallStacks[size >> 2].getChunk(size);
        } else if (size <= (size_t) sm_maxLargeStackSize) {
            chunk = sm_largeStacks[size >> 10].getChunk(size);
        } else {
            chunk = static_cast<Chunk*>(malloc( sizeof(Chunk) + size ));
            chunk->m_stack = 0;
        }

        chunk++;
//        printf("address: %p\n", chunk);
//        getchar();
        return static_cast<void*>(chunk);
    }

    inline static void release(void * pointer) {
        Chunk * chunk = reinterpret_cast<Chunk*>(pointer);  //reinterpret_cast<Chunk*>(reinterpret_cast<char*>(pointer) - sizeof(Chunk));
        chunk--;
        if (likely(chunk->m_stack != 0)) {
            chunk->m_stack->addChunk(chunk);
        } else {
            free(chunk);
        }
        //free(pointer);
    }

private:

    static const int sm_smallSteps = 4;
    static const int sm_maxSmallStackSize = 1024;

    static const int sm_largeSteps = 1024;
    static const int sm_maxLargeStackSize = 1024 * 1024;

    static ChunkStack * sm_smallStacks;
    static ChunkStack * sm_largeStacks;
public:
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
        sm_initialized = true;
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
        sm_initialized = false;
    }
};

inline void * operator new (size_t size) {
    //return malloc(size);
    return MemoryManager::allocate(size);
}

inline void operator delete(void * p) {
//    free(p);
    if (p != 0) {
        MemoryManager::release(p);
    }
}

inline void * operator new [](size_t size) {
    //return malloc(size);
    return MemoryManager::allocate(size);
}

inline void operator delete [](void * p) {
    //free(p);
    if (p != 0) {
        MemoryManager::release(p);
    }
}


/*class MemoryManagerInitializer {
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

static MemoryManagerInitializer memoryInitializer;*/

__attribute__((constructor))
static void initMemory() {
    if (MemoryManager::sm_initialized == false) {
        MemoryManager::init();
        printf("Memory manager initialized\n");
    }
}

__attribute__((destructor))
static void releaseMemory() {
    static bool released = false;
    if (MemoryManager::sm_initialized == true && released == false) {
        MemoryManager::release();
        printf("Memory manager released %p %p\n", &MemoryManager::sm_initialized,
               &released);
        released = true;
    }
}

#undef likely
#undef unlikely

#endif

union Pointer {
    void * ptr;
    unsigned long int bits;
};

// TODO: ezeket attenni egy meta.h fileba

template <unsigned n>
struct Logarithm
{
    enum { value = 1 + Logarithm<n / 2>::value };
};

template <>
struct Logarithm<1>
{
  enum { value = 0 };
};

template <class T, unsigned alignment>
T * alloc(int size) {
    size *= sizeof(T);
    char * originalPtr = new char[size + alignment + sizeof(void*)];
    char * ptr2 = originalPtr + alignment + sizeof(void*);
    Pointer ptr;
    ptr.ptr = ptr2;
    ptr.bits >>= Logarithm<alignment>::value;
    ptr.bits <<= Logarithm<alignment>::value;
    ptr2 = static_cast<char*>(ptr.ptr);
    ptr2 -= sizeof(void*);
    *((char**)ptr2) = originalPtr;
    ptr2 = ptr2 + sizeof(void*);
    return reinterpret_cast<T*>(ptr2);
}

void release(void * ptr);


#endif // MEMORYMAN_H
