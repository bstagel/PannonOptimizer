#include <globals.h>
#include <utils/architecture.h>
#include <utils/primitives.h>
#include <cstring>

Architecture::MemCpyCache Architecture::sm_memCpyCachePtr;
Architecture::MemCpyNoCache Architecture::sm_memCpyNoCachePtr;
Architecture::DenseToDenseDotProductUnstable Architecture::sm_denseToDenseDotProductUnstablePtr;
Architecture::DenseToDenseDotProductStable Architecture::sm_denseToDenseDotProductStablePtr;
Architecture::DenseToSparseDotProductUnstable Architecture::sm_denseToSparseDotProductUnstablePtr;
Architecture::DenseToSparseDotProductStable Architecture::sm_denseToSparseDotProductStablePtr;
Architecture::IndexedDenseToIndexedDenseDotProductUnstable Architecture::sm_indexedDenseToIndexedDenseDotProductUnstablePtr;
Architecture::IndexedDenseToIndexedDenseDotProductStable Architecture::sm_indexedDenseToIndexedDenseDotProductStablePtr;

size_t Architecture::sm_largestCacheSize;

Architecture::Architecture():
    m_cpuCount(0),
    m_coreCount(0),
    m_totalMemory(0)
{
    sm_memCpyCachePtr = memcpy;
    sm_memCpyNoCachePtr = memcpy;

    sm_denseToDenseDotProductUnstablePtr = denseToDenseDotProductUnstable;
    sm_denseToSparseDotProductUnstablePtr = denseToSparseDotProductUnstable;
    sm_denseToDenseDotProductStablePtr = denseToDenseDotProductStable;
    sm_denseToSparseDotProductStablePtr = denseToSparseDotProductStable;
    sm_indexedDenseToIndexedDenseDotProductUnstablePtr = indexedDenseToIndexedDenseDotProductUnstable;
    sm_indexedDenseToIndexedDenseDotProductStablePtr = indexedDenseToIndexedDenseDotProductStable;
}

bool Architecture::featureExists(const char *feature) const {
    return m_features.find(std::string(feature)) != m_features.end();
}

unsigned int Architecture::getCPUCount() const {
    return m_cpuCount;
}

unsigned int Architecture::getCoreCount() const {
    return m_coreCount;
}

const char *Architecture::getCPUName() const {
    return m_cpuName.c_str();
}

unsigned int Architecture::getCacheLevelCount() const {
    return m_caches.size();
}

const Architecture::Cache & Architecture::getCache(unsigned int index) const {
    return m_caches[index];
}

const char *Architecture::getArchitectureName() const {
    return m_architectureName.c_str();
}

const char *Architecture::getCPUArchitectureName() const {
    return m_cpuArchitectureName.c_str();
}

unsigned long long int Architecture::getTotalMemorySize() const {
    return m_totalMemory;
}

Architecture::MemCpyCache Architecture::getMemCpyCache() {
    return sm_memCpyCachePtr;
}

Architecture::MemCpyNoCache Architecture::getMemCpyNoCache() {
    return sm_memCpyNoCachePtr;
}

Architecture::DenseToDenseDotProductUnstable Architecture::getDenseToDenseDotProductUnstable() {
    return sm_denseToDenseDotProductUnstablePtr;
}

ArchitectureInterface::DenseToSparseDotProductUnstable Architecture::getDenseToSparseDotProductUnstable() {
    return sm_denseToSparseDotProductUnstablePtr;
}

ArchitectureInterface::DenseToDenseDotProductStable Architecture::getDenseToDenseDotProductStable() {
    return sm_denseToDenseDotProductStablePtr;
}

ArchitectureInterface::DenseToSparseDotProductStable Architecture::getDenseToSparseDotProductStable() {
    return sm_denseToSparseDotProductStablePtr;
}

Architecture::IndexedDenseToIndexedDenseDotProductUnstable Architecture::getIndexedDenseToIndexedDenseDotProductUnstable()
{
    return sm_indexedDenseToIndexedDenseDotProductUnstablePtr;
}

ArchitectureInterface::IndexedDenseToIndexedDenseDotProductStable Architecture::getIndexedDenseToIndexedDenseDotProductStable()
{
    return sm_indexedDenseToIndexedDenseDotProductStablePtr;
}

size_t Architecture::getLargestCacheSize() {
    return sm_largestCacheSize;
}
