/**
 * memoryman.cpp
 */

#include <utils/memoryman.h>
#include <cstdio>
#include <initpanopt.h>
#include <cstring>
#include <macros.h>
#include <utils/architecture.h>

#ifndef CLASSIC_NEW_DELETE

thread_local ChunkStack * MemoryManager::sm_smallStacks = nullptr;

thread_local ChunkStack * MemoryManager::sm_largeStacks = nullptr;

thread_local Buffer * Pool::sm_head = nullptr;

ChunkStack * MemoryManager::sm_sequentialSmallStacks = nullptr;

ChunkStack * MemoryManager::sm_sequentialLargeStacks = nullptr;

Buffer * Pool::sm_sequentialHead = nullptr;

bool MemoryManager::sm_initialized = false;

#endif

void release(void * ptr) {

#ifndef CACHE_LINE_ALIGNMENT
    delete [] ptr;
    return;
#endif

    if (ptr == nullptr) {
        return;
    }
    //delete [] ptr;
    //return;

    char * ptr2 = (char*)ptr;
    ptr2 -= sizeof(void*);
#ifdef CLASSIC_NEW_DELETE
    delete [] *((char**)ptr2);
#else
    MemoryManager::release(*((char**)ptr2));
#endif
}

void panOptMemcpy(void * dest,
                  const void * src,
                  size_t size) {
    //memcpy(dest, src, size);
    //return;
    if (likely(size < Architecture::getLargestCacheSize() )) {
        Architecture::getMemCpyCache()(dest, src, size);
    } else {
        Architecture::getMemCpyNoCache()(dest, src, size);
    }
}


void panOptMemset(void *dest, int value, size_t size)
{
    memset(dest, value, size);
}
