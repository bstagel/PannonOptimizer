/**
 * @file architecture.h This file contains the API of the Architecture class.
 * @author Jozsef Smidla
 */

#ifndef ARCHITECTURE_H
#define ARCHITECTURE_H

#include <string>
#include <set>
#include <vector>

#include <utils/architectureinterface.h>

/**
 * This pure virtual class is used to detect and use specific architectures.
 * Certain operations are faster with using architecture-specific CPU commands.
 */
class Architecture: public ArchitectureInterface {
public:

    /**
     * Default constructor of the Architecture class.
     *
     * @constructor
     */
    Architecture();

    virtual ~Architecture();

    /**
     * Detects the properties of specific architectures.
     * Gathers information about the CPU, cache and computing features of the system.
     */
    virtual void detect() = 0;

    /**
     * Searches the supported feature list for the given feature and returns true if it is supported.
     *
     * @param feature The feature name to be searched for.
     * @return True if the given feature is supported by the processor.
     */
    bool featureExists(const char * feature) const;

    /**
     * Returns the number of CPUs in the system.
     *
     * @return The number of CPUs in the system.
     */
    unsigned int getCPUCount() const;

    /**
     * Returns the number of processor cores in the system.
     *
     * @return The number of processor cores in the system.
     */
    unsigned int getCoreCount() const;

    /**
     * Returns the name of the CPU.
     *
     * @return The name of the CPU.
     */
    const char * getCPUName() const;

    /**
     * Returns the number of cache levels in the system.
     *
     * @return The number of cache levels in the system.
     */
    unsigned int getCacheLevelCount() const;

    /**
     * Returns a cache with the given index.
     *
     * @param index The index of the cache to be returned.
     * @return The cache with the given index.
     */
    const Cache & getCache(unsigned int index) const;

    /**
     * Returns the name of the architecture.
     *
     * @return The name of the architecture.
     */
    const char * getArchitectureName() const;

    /**
     * Returns the name of the CPU architecture.
     *
     * @return The name of the CPU architecture.
     */
    const char * getCPUArchitectureName() const;

    /**
     * Returns the size of the memory in the system.
     *
     * @return The size of the memory in the system.
     */
    unsigned long long int getTotalMemorySize() const;

    /**
     * Custom architecture specific custom function for effective dense vector-dense vector addition.
     * This function also returns the nonzero count of the result vector.
     *
     * @return The result vector and its nonzero count in a specific variable type.
     * For details, see AddVecDenseToDense_NonzCount.
     */
    virtual AddVecDenseToDense_NonzCount getAddVecDenseToDense_NonzCount() const = 0;

    /**
     * Custom X86 architecture specific custom function for effective dense vector-dense vector addition.
     *
     * @return The result vector in a specific variable type.
     * For details, see Architecture::AddVecDenseToDense.
     */
    virtual AddVecDenseToDense getAddVecDenseToDense() const = 0;

    static MemCpyCache getMemCpyCache();

    static MemCpyNoCache getMemCpyNoCache();

    static DenseToDenseDotProductUnstable getDenseToDenseDotProductUnstable();

    static DenseToSparseDotProductUnstable getDenseToSparseDotProductUnstable();

    static DenseToDenseDotProductStable getDenseToDenseDotProductStable();

    static DenseToSparseDotProductStable getDenseToSparseDotProductStable();

    static IndexedDenseToIndexedDenseDotProductUnstable getIndexedDenseToIndexedDenseDotProductUnstable();

    static IndexedDenseToIndexedDenseDotProductStable getIndexedDenseToIndexedDenseDotProductStable();

    static size_t getLargestCacheSize();

protected:

    static MemCpyCache sm_memCpyCachePtr;

    static MemCpyNoCache sm_memCpyNoCachePtr;

    static DenseToDenseDotProductUnstable sm_denseToDenseDotProductUnstablePtr;

    static DenseToSparseDotProductUnstable sm_denseToSparseDotProductUnstablePtr;

    static DenseToDenseDotProductStable sm_denseToDenseDotProductStablePtr;

    static DenseToSparseDotProductStable sm_denseToSparseDotProductStablePtr;

    static IndexedDenseToIndexedDenseDotProductUnstable sm_indexedDenseToIndexedDenseDotProductUnstablePtr;

    static IndexedDenseToIndexedDenseDotProductStable sm_indexedDenseToIndexedDenseDotProductStablePtr;

    static size_t sm_largestCacheSize;

    /**
     * The set of features the system supports.
     */
    std::set<std::string> m_features;

    /**
     * The number of CPUs in the system.
     */
    unsigned int m_cpuCount;

    /**
     * The number of processor cores in the system.
     */
    unsigned int m_coreCount;

    /**
     * The name of the CPU.
     */
    std::string m_cpuName;

    /**
     * The list of caches in the system.
     */
    std::vector<Cache> m_caches;

    /**
     * The name of the architecture.
     */
    std::string m_architectureName;

    /**
     * The name of the CPU architecture.
     */
    std::string m_cpuArchitectureName;

    /**
     * The size of the system memory.
     */
    unsigned int m_totalMemory;
};

#endif // ARCHITECTURE_H
