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

#ifndef ARCHITECTUREINTERFACE_H
#define ARCHITECTUREINTERFACE_H

#include <cstddef>
#include <utils/numerical.h>

class ArchitectureInterface {
public:

    ArchitectureInterface() {}

    virtual ~ArchitectureInterface() {}

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

    typedef Numerical::Double (*DenseToDenseDotProductUnstable)(const Numerical::Double * vec1,
                                                                const Numerical::Double * vec2,
                                                                size_t count);

    /******************************************
     * Dot product dense to sparse, unstable:
     *       - 32 or 64 bit
     *       - Unix or Windows arguments
     *       - SSE2 or AVX
     *****************************************/

    typedef Numerical::Double (*DenseToSparseDotProductUnstable)(const Numerical::Double * dense,
                                                                 const Numerical::Double * sparse,
                                                                 const unsigned int * indices,
                                                                 size_t count);

    /******************************************
     * Dot product dense to dense, stable
     *       - 32 or 64 bit
     *       - Unix or Windows arguments
     *       - SSE2 or AVX
     *****************************************/

    typedef Numerical::Double (*DenseToDenseDotProductStable)(const Numerical::Double * vec1,
                                                              const Numerical::Double * vec2,
                                                              size_t count,
                                                              Numerical::Double * negPtr);

    /******************************************
     * Dot product dense to sparse, stable:
     *       - 32 or 64 bit
     *       - Unix or Windows arguments
     *       - SSE2 or AVX
     *****************************************/

    typedef Numerical::Double (*DenseToSparseDotProductStable)(const Numerical::Double * dense,
                                                               const Numerical::Double * sparse,
                                                               const unsigned int * indices,
                                                               size_t count,
                                                               Numerical::Double * negPtr);

    typedef Numerical::Double (*IndexedDenseToIndexedDenseDotProductUnstable)(const Numerical::Double * dense1,
                                                                              const Numerical::Double * dense2,
                                                                              const unsigned int * indices,
                                                                              size_t nonZeroCount);

    typedef Numerical::Double (*IndexedDenseToIndexedDenseDotProductStable)(const Numerical::Double * dense1,
                                                                            const Numerical::Double * dense2,
                                                                            const unsigned int * indices,
                                                                            size_t nonZeroCount,
                                                                            Numerical::Double * negPtr);

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


    typedef unsigned int (*AddVecDenseToDense_NonzCount)(const Numerical::Double * vec1,
                                                         const Numerical::Double * vec2,
                                                         Numerical::Double * result,
                                                         Numerical::Double lambda,
                                                         unsigned int count,
                                                         unsigned int nonzeros1,
                                                         unsigned int nonzeros2,
                                                         unsigned int tolerances);

    typedef void (*AddVecDenseToDense)(const Numerical::Double * vec1,
                                       const Numerical::Double * vec2,
                                       Numerical::Double * result,
                                       Numerical::Double lambda,
                                       unsigned int count,
                                       unsigned int nonzeros1,
                                       unsigned int nonzeros2,
                                       unsigned int tolerances);

    typedef void (*AddDenseToDenseAbsRel)(const Numerical::Double * vec1,
                                          const Numerical::Double * vec2,
                                          Numerical::Double * result,
                                          size_t count,
                                          Numerical::Double lambda,
                                          Numerical::Double absTolerance,
                                          Numerical::Double relTolerance);

    typedef void (*AddDenseToDenseAbs)(const Numerical::Double * vec1,
                                       const Numerical::Double * vec2,
                                       Numerical::Double * result,
                                       size_t count,
                                       Numerical::Double lambda,
                                       Numerical::Double absTolerance);

    typedef void (*AddDenseToDense)(const Numerical::Double * vec1,
                                    const Numerical::Double * vec2,
                                    Numerical::Double * result,
                                    size_t count,
                                    Numerical::Double lambda);

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
     * Custom architecture specific function for effective dense vector-dense vector addition.
     *
     * @return The result vector in a specific variable type.
     * For details, see Architecture::AddVecDenseToDense.
     */
    virtual AddVecDenseToDense getAddVecDenseToDense() const = 0;

    /**
     * Loads the architecture specific parameter file.
     */
    virtual void loadParameters() = 0;

    /**
     * Generates an architecture specific parameter file.
     */
    virtual void generateParameterFile() const = 0;
};

#endif // ARCHITECTUREINTERFACE_H
