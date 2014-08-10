#ifndef ARCHITECTUREINTERFACE_H
#define ARCHITECTUREINTERFACE_H

#include <cstddef>

class ArchitectureInterface {
public:


    typedef void (*MemSetNoCache)(const void * dest,
                                  unsigned char value,
                                  unsigned int size);


    typedef void (*MemSetCache)(const void * dest,
                                unsigned char value,
                                unsigned int size);


    /******************************************
     * Copies size bytes from src to dest area.
     *       - 32 or 64 bit
     *       - Unix or Windows arguments
     *       - SSE2 (copy without cache)
     *****************************************/

    typedef void* (*MemCpyNoCache)(void * dest,
                                   const void * src,
                                   size_t size);

    /******************************************
     * Copies size bytes from src to dest area.
     *       - 32 or 64 bit
     *       - Unix or Windows arguments
     *       - Classic copy, with cache, SSE2
     *****************************************/

    typedef void* (*MemCpyCache)(void * dest,
                                 const void * src,
                                 size_t size);

    /******************************************
     * Dot product dense to dense, unstable:
     *       - 32 or 64 bit
     *       - Unix or Windows arguments
     *       - SSE2 or AVX
     *****************************************/

    typedef double (*DenseToDenseDotProductUnstable)(const double * vec1,
                                                     const double * vec2,
                                                     size_t count);

    /******************************************
     * Dot product dense to sparse, unstable:
     *       - 32 or 64 bit
     *       - Unix or Windows arguments
     *       - SSE2 or AVX
     *****************************************/

    typedef double (*DenseToSparseDotProductUnstable)(const double * dense,
                                                      const double * sparse,
                                                      const unsigned int * indices,
                                                      size_t count);

    /******************************************
     * Dot product dense to dense, stable
     *       - 32 or 64 bit
     *       - Unix or Windows arguments
     *       - SSE2 or AVX
     *****************************************/

    typedef double (*DenseToDenseDotProductStable)(const double * vec1,
                                                   const double * vec2,
                                                   size_t count,
                                                   double * negPtr);

    /******************************************
     * Dot product dense to sparse, stable:
     *       - 32 or 64 bit
     *       - Unix or Windows arguments
     *       - SSE2 or AVX
     *****************************************/

    typedef double (*DenseToSparseDotProductStable)(const double * dense,
                                                    const double * sparse,
                                                    const unsigned int * indices,
                                                    size_t count,
                                                    double * negPtr);


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
    virtual bool featureExists(const char * feature) const = 0;

    /**
     * Returns the number of CPUs in the system.
     *
     * @return The number of CPUs in the system.
     */
    virtual unsigned int getCPUCount() const = 0;

    /**
     * Returns the number of processor cores in the system.
     *
     * @return The number of processor cores in the system.
     */
    virtual unsigned int getCoreCount() const = 0;

    /**
     * Returns the name of the CPU.
     *
     * @return The name of the CPU.
     */
    virtual const char * getCPUName() const = 0;

    /**
     * Returns the number of cache levels in the system.
     *
     * @return The number of cache levels in the system.
     */
    virtual unsigned int getCacheLevelCount() const = 0;

    /**
     * Returns a cache with the given index.
     *
     * @param index The index of the cache to be returned.
     * @return The cache with the given index.
     */
    virtual const Cache & getCache(unsigned int index) const = 0;

    /**
     * Returns the name of the architecture.
     *
     * @return The name of the architecture.
     */
    virtual const char * getArchitectureName() const = 0;

    /**
     * Returns the name of the CPU architecture.
     *
     * @return The name of the CPU architecture.
     */
    virtual const char * getCPUArchitectureName() const = 0;

    /**
     * Returns the size of the memory in the system.
     *
     * @return The size of the memory in the system.
     */
    virtual unsigned long long int getTotalMemorySize() const = 0;

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

};

#endif // ARCHITECTUREINTERFACE_H
