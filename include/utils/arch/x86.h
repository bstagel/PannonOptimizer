/**
 * @file x86.h This file contains the API of the ArchitectureX86 class.
 * @author smidla
 */

#ifndef X86_H
#define X86_H

#include <globals.h>
#include <utils/architecture.h>

/**
 * This class implements detection of properties of x86 architectures.
 * It is able to execute architecture-specific functions for high-performance operations.
 */
class ArchitectureX86: public Architecture {
public:

    /**
     * Default constructor of the ArchitectureX86 class.
     *
     * @constructor
     */
    ArchitectureX86();

    /**
     * Detects the properties of the x86 architecture.
     * Gathers information about the CPU, cache and computing features of the system.
     */
    void detect();

    /**
     * Custom X86 architecture specific custom function for effective dense vector-dense vector addition.
     * This function also returns the nonzero count of the result vector.
     *
     * @return The result vector and its nonzero count in a specific variable type.
     * For details, see Architecture::AddVecDenseToDense_NonzCount.
     */
    AddVecDenseToDense_NonzCount getAddVecDenseToDense_NonzCount() const;

    /**
     * Custom X86 architecture specific custom function for effective dense vector-dense vector addition.
     *
     * @return The result vector in a specific variable type.
     * For details, see Architecture::AddVecDenseToDense.
     */
    AddVecDenseToDense getAddVecDenseToDense() const;

protected:

    /**
     * Struct for storing the addresses of registers.
     */
    struct Registers {
        unsigned int m_eax;
        unsigned int m_ebx;
        unsigned int m_ecx;
        unsigned int m_edx;
    };

    /**
     * @brief getBits
     * @param pattern
     * @param start
     * @param end
     * @return
     */
    unsigned int getBits(unsigned int pattern,
                         unsigned int start,
                         unsigned int end);

    /**
     * Queries a cpuid command from the processor.
     *
     * @param args The register addresses the return of the cpuid command to be extracted to.
     * @return The register addresses containing the input of the cpuid command.
     */
    Registers cpuid(Registers args) const;

    /**
     * Queries and returns the cache info of a cache with specific ID.
     * The cache info is stored in the cache list of the architecture class.
     *
     * @see Architecture::m_caches
     * @param cacheId The ID of the cache to be queried.
     */
    void loadCacheInfo(int cacheId);

    /**
     * Returns the number of caches in the current computer.
     *
     * @return The number of caches.
     */
    unsigned int getCacheCount() const;

    /**
     * Gets the name of the CPU from the processor.
     * This gets stored in the m_cpuName private variable.
     *
     * @see Architecture::m_cpuName
     */
    void setCPUData();

    /**
     * Checks if the feature with the given bit exists, and adds it to the feature list if it is so.
     *
     * @param reg The register address with the given feature.
     * @param bit The bit id of the feature.
     * @param name The name of the feature.
     */
    void loadFeature(unsigned int reg, unsigned int bit,
                     const std::string & name);

    /**
     * Queries which features do the processor supports.
     */
    void setFeatureList();

    /**
     * Queries the cache count and the cache data from each cache from the processor.
     */
    void setCacheData();

    /**
     * Queries the memory size from the system.
     */
    void setMemoryData();
};

#endif // X86_H
