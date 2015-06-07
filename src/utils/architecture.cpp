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
Architecture::AddDenseToDenseAbsRel Architecture::sm_addDenseToDenseAbsRelCachePtr;
Architecture::AddDenseToDenseAbsRel Architecture::sm_addDenseToDenseAbsRelNoCachePtr;
Architecture::AddDenseToDenseAbs Architecture::sm_addDenseToDenseAbsCachePtr;
Architecture::AddDenseToDenseAbs Architecture::sm_addDenseToDenseAbsNoCachePtr;
Architecture::AddDenseToDense Architecture::sm_addDenseToDenseCachePtr;
Architecture::AddDenseToDense Architecture::sm_addDenseToDenseNoCachePtr;

size_t Architecture::sm_largestCacheSize;

Architecture::Architecture():
    m_cpuCount(0),
    m_coreCount(0),
    m_totalMemory(0)
{
    sm_memCpyCachePtr = memcpy;
    sm_memCpyNoCachePtr = memcpy;

    sm_denseToDenseDotProductUnstablePtr = ::denseToDenseDotProductUnstable;
    sm_denseToSparseDotProductUnstablePtr = ::denseToSparseDotProductUnstable;
    sm_denseToDenseDotProductStablePtr = ::denseToDenseDotProductStable;
    sm_denseToSparseDotProductStablePtr = ::denseToSparseDotProductStable;
    sm_indexedDenseToIndexedDenseDotProductUnstablePtr = ::indexedDenseToIndexedDenseDotProductUnstable;
    sm_indexedDenseToIndexedDenseDotProductStablePtr = ::indexedDenseToIndexedDenseDotProductStable;

    sm_addDenseToDenseAbsRelCachePtr = ::denseToDenseAddAbsRel;
    sm_addDenseToDenseAbsRelNoCachePtr = ::denseToDenseAddAbsRel;
    sm_addDenseToDenseAbsCachePtr = ::denseToDenseAddAbs;
    sm_addDenseToDenseAbsNoCachePtr = ::denseToDenseAddAbs;
    sm_addDenseToDenseCachePtr = ::denseToDenseAdd;
    sm_addDenseToDenseNoCachePtr = ::denseToDenseAdd;
}

Architecture::~Architecture()
{

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

ArchitectureInterface::AddDenseToDenseAbsRel Architecture::getAddDenseToDenseAbsRelCache()
{
    return sm_addDenseToDenseAbsRelCachePtr;
}

ArchitectureInterface::AddDenseToDenseAbsRel Architecture::getAddDenseToDenseAbsRelNoCache()
{
    return sm_addDenseToDenseAbsRelNoCachePtr;
}

ArchitectureInterface::AddDenseToDenseAbs Architecture::getAddDenseToDenseAbsCache()
{
    return sm_addDenseToDenseAbsCachePtr;
}

ArchitectureInterface::AddDenseToDenseAbs Architecture::getAddDenseToDenseAbsNoCache()
{
    return sm_addDenseToDenseAbsNoCachePtr;
}

ArchitectureInterface::AddDenseToDense Architecture::getAddDenseToDenseCache()
{
    return sm_addDenseToDenseCachePtr;
}

ArchitectureInterface::AddDenseToDense Architecture::getAddDenseToDenseNoCache()
{
    return sm_addDenseToDenseNoCachePtr;
}

size_t Architecture::getLargestCacheSize() {
    return sm_largestCacheSize;
}
