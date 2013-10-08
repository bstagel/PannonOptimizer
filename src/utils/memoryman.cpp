/**
 * memoryman.cpp
 */

#include <utils/memoryman.h>

#ifndef CLASSIC_NEW_DELETE

ChunkStack * MemoryManager::sm_smallStacks = 0;

ChunkStack * MemoryManager::sm_largeStacks = 0;

Buffer * Pool::sm_head = 0;

#endif
