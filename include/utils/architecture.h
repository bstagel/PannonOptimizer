#ifndef ARCHITECTURE_H
#define ARCHITECTURE_H

#include <globals.h>
#include <string>
#include <set>
#include <vector>


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


    enum CACHE_TYPE {
        DATA_CACHE = 1,
        INSTRUCTION_CACHE = 2,
        UNIFIED_CACHE = 3
    };

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

    Architecture():
        m_cpuCount(0),
        m_coreCount(0),
        m_totalMemory(0)
    {

    }

    virtual void detect() = 0;

    bool featureExists(const std::string & feature) const {
        return m_features.find(feature) != m_features.end();
    }

    unsigned int getCPUCount() const {
        return m_cpuCount;
    }

    unsigned int getCoreCount() const {
        return m_coreCount;
    }

    const std::string & getCPUName() const {
        return m_cpuName;
    }

    unsigned int getCacheLevelCount() const {
        return m_caches.size();
    }

    const Cache & getCache(unsigned int index) const {
        return m_caches[index];
    }

    const std::string & getArchitectureName() const {
        return m_architectureName;
    }

    const std::string & getCPUArchitectureName() const {
        return m_cpuArchitectureName;
    }

    unsigned long long int getTotalMemorySize() const {
        return m_totalMemory;
    }

    virtual AddVecDenseToDense_NonzCount getAddVecDenseToDense_NonzCount() const = 0;

    virtual AddVecDenseToDense getAddVecDenseToDense() const = 0;

protected:

    std::set<std::string> m_features;

    unsigned int m_cpuCount;

    unsigned int m_coreCount;

    std::string m_cpuName;

    std::vector<Cache> m_caches;

    std::string m_architectureName;

    std::string m_cpuArchitectureName;

    unsigned int m_totalMemory;
};

#endif // ARCHITECTURE_H
