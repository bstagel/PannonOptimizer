#ifndef X86_H
#define X86_H

#include <globals.h>
#include <utils/architecture.h>

class ArchitectureX86: public Architecture {
public:
    ArchitectureX86();

    void detect();

    AddVecDenseToDense_NonzCount getAddVecDenseToDense_NonzCount() const;

    AddVecDenseToDense getAddVecDenseToDense() const;
protected:

    struct Registers {
        unsigned int m_eax;
        unsigned int m_ebx;
        unsigned int m_ecx;
        unsigned int m_edx;
    };

    unsigned int getBits(unsigned int pattern,
                         unsigned int start,
                         unsigned int end);

    Registers cpuid(Registers args) const;

    void loadCacheInfo(int cacheId);

    unsigned int getCacheCount() const;

    void setCPUData();

    void loadFeature(unsigned int reg, unsigned int bit,
                     const std::string & name);

    void setFeatureList();

    void setCacheData();

    void setMemoryData();
};

#endif // X86_H
