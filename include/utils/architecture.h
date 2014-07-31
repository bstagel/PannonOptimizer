/**
 * @file architecture.h This file contains the API of the Architecture class.
 * @author Jozsef Smidla
 */

#ifndef ARCHITECTURE_H
#define ARCHITECTURE_H

#include <globals.h>
#include <string>
#include <set>
#include <vector>

/**
 * This pure virtual class is used to detect and use specific architectures.
 * Certain operations are faster with using architecture-specific CPU commands.
 */
class Architecture {
public:

    /******************************************
     * Add dense to dense:
     *      - count nonzeros in result?
     *      - using relative tolerance?
     *      - using absolute tolerance?
     *      - using both tolerances?
     *      - 32 or 64 bit
     *      - Unix or Windows arguments
     *      - vectors are sparse or not
     *      - SSE2 or AVX
     *****************************************/


    typedef unsigned int (*AddVecDenseToDense_NonzCount)(const double * vec1,
                                                         const double * vec2,
                                                         double * result,
                                                         double lambda,
                                                         unsigned int count,
                                                         unsigned int nonzeros1,
                                                         unsigned int nonzeros2,
                                                         unsigned int tolerances);

    typedef void (*AddVecDenseToDense)(const double * vec1,
                                       const double * vec2,
                                       double * result,
                                       double lambda,
                                       unsigned int count,
                                       unsigned int nonzeros1,
                                       unsigned int nonzeros2,
                                       unsigned int tolerances);


    /**
     * This type describes the type of a system cache.
     */
    enum CACHE_TYPE {
        DATA_CACHE = 1,
        INSTRUCTION_CACHE = 2,
        UNIFIED_CACHE = 3
    };

    /**
     * This struct describes a system cache.
     */
    struct Cache {
        unsigned int m_level;
        unsigned int m_size;
        unsigned int m_lineSize;
        bool m_fullAssociative;
        unsigned int m_associativity;
        unsigned int m_linePartitions;
        int m_sets;
        enum CACHE_TYPE m_type;
        unsigned int m_threads;
    };

    /**
     * Default constructor of the Architecture class.
     *
     * @constructor
     */
    Architecture():
        m_cpuCount(0),
        m_coreCount(0),
        m_totalMemory(0)
    {

    }

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
    bool featureExists(const std::string & feature) const {
        return m_features.find(feature) != m_features.end();
    }

    /**
     * Returns the number of CPUs in the system.
     *
     * @return The number of CPUs in the system.
     */
    unsigned int getCPUCount() const {
        return m_cpuCount;
    }

    /**
     * Returns the number of processor cores in the system.
     *
     * @return The number of processor cores in the system.
     */
    unsigned int getCoreCount() const {
        return m_coreCount;
    }

    /**
     * Returns the name of the CPU.
     *
     * @return The name of the CPU.
     */
    const std::string & getCPUName() const {
        return m_cpuName;
    }

    /**
     * Returns the number of cache levels in the system.
     *
     * @return The number of cache levels in the system.
     */
    unsigned int getCacheLevelCount() const {
        return m_caches.size();
    }

    /**
     * Returns a cache with the given index.
     *
     * @param index The index of the cache to be returned.
     * @return The cache with the given index.
     */
    const Cache & getCache(unsigned int index) const {
        return m_caches[index];
    }

    /**
     * Returns the name of the architecture.
     *
     * @return The name of the architecture.
     */
    const std::string & getArchitectureName() const {
        return m_architectureName;
    }

    /**
     * Returns the name of the CPU architecture.
     *
     * @return The name of the CPU architecture.
     */
    const std::string & getCPUArchitectureName() const {
        return m_cpuArchitectureName;
    }

    /**
     * Returns the size of the memory in the system.
     *
     * @return The size of the memory in the system.
     */
    unsigned long long int getTotalMemorySize() const {
        return m_totalMemory;
    }

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

protected:

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
