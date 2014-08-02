#include <globals.h>
#include <utils/architecture.h>

Architecture::MemCpyCache Architecture::sm_memCpyCachePtr;
Architecture::MemCpyNoCache Architecture::sm_memCpyNoCachePtr;
Architecture::DenseToDenseDotProductUnstable Architecture::sm_denseToDenseDotProductUnstablePtr;

size_t Architecture::sm_largestCacheSize;
