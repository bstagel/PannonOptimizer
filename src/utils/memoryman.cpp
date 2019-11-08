//=================================================================================================
/*!
//  This file is part of the Pannon Optimizer library. 
//  This library is free software; you can redistribute it and/or modify it under the 
//  terms of the GNU Lesser General Public License as published by the Free Software 
//  Foundation; either version 3.0, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
//  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//  See the GNU General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License; see the file 
//  COPYING. If not, see http://www.gnu.org/licenses/.
*/
//=================================================================================================

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
    ptr = nullptr;
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
