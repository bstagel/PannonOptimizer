#include <utils/arch/x86.h>
#ifdef UNIX
    #include <sys/sysinfo.h>
#endif
#include <cstring>

ArchitectureX86::ArchitectureX86() {

}

void ArchitectureX86::detect() {
    m_architectureName = "X86";
    setCPUData();
    setCacheData();
    setFeatureList();
    setMemoryData();
}

unsigned int ArchitectureX86::getBits(unsigned int pattern,
                                      unsigned int start,
                                      unsigned int end) {
    unsigned int result = 0;
    unsigned int index = start;
    while (index <= end) {
        result |= (pattern & (1 << index)) >> start;
        index++;
    }
    return result;
}

ArchitectureX86::Registers ArchitectureX86::cpuid(Registers args) const {
    asm("cpuid"
    : "=a"(args.m_eax),
            "=b"(args.m_ebx),
            "=c"(args.m_ecx),
            "=d"(args.m_edx)
        : "a"(args.m_eax),
          "b"(args.m_ebx),
          "c"(args.m_ecx),
          "d"(args.m_edx)
        );
    return args;
}

void ArchitectureX86::setCPUData() {
    // CPU name
    char cpuName[50];
    Registers regs;
    regs.m_eax = 0x80000002;
    regs.m_ebx = regs.m_ecx = regs.m_edx = 0;
    regs = cpuid(regs);
    memcpy(cpuName, &regs, sizeof(regs));

    regs.m_eax = 0x80000003;
    regs.m_ebx = regs.m_ecx = regs.m_edx = 0;
    regs = cpuid(regs);
    memcpy(cpuName + 16, &regs, sizeof(regs));

    regs.m_eax = 0x80000004;
    regs.m_ebx = regs.m_ecx = regs.m_edx = 0;
    regs = cpuid(regs);
    memcpy(cpuName + 32, &regs, sizeof(regs));
    cpuName[48] = 0;
    m_cpuName = cpuName;
}

unsigned int ArchitectureX86::getCacheCount() const {
    Registers regs;
    unsigned int count = 0;
    do {
        regs.m_eax = 4;
        regs.m_ecx = count;
        regs.m_ebx = regs.m_edx = 0;

        regs = cpuid(regs);
        if (regs.m_ebx != 0) {
            count++;
        }
    } while ((regs.m_ebx & 0x1F) != 0);
    return count;
}

void ArchitectureX86::loadFeature(unsigned int reg, unsigned int bit,
                                  const std::string & name) {
    if (getBits(reg, bit, bit) == 1) {
        m_features.insert(name);
    }
}

void ArchitectureX86::setFeatureList() {
    Registers regs;
    regs.m_eax = 1;
    regs.m_ebx = regs.m_ecx = regs.m_edx = 0;
    regs = cpuid(regs);
    loadFeature(regs.m_ecx, 0, "SSE3");
    loadFeature(regs.m_ecx, 1, "PCLMULDQ");
    loadFeature(regs.m_ecx, 2, "DTES64");
    loadFeature(regs.m_ecx, 3, "MONITOR");
    loadFeature(regs.m_ecx, 4, "DS-CPL");
    loadFeature(regs.m_ecx, 5, "VMX");
    loadFeature(regs.m_ecx, 6, "SMX");
    loadFeature(regs.m_ecx, 7, "EIST");
    loadFeature(regs.m_ecx, 8, "SSSE3");
    loadFeature(regs.m_ecx, 9, "SSE3");
    loadFeature(regs.m_ecx, 10, "CNXT-ID");
    loadFeature(regs.m_ecx, 12, "FMA");
    loadFeature(regs.m_ecx, 13, "CX16");
    loadFeature(regs.m_ecx, 14, "XTPR");
    loadFeature(regs.m_ecx, 15, "PDCM");
    loadFeature(regs.m_ecx, 17, "PCID");
    loadFeature(regs.m_ecx, 18, "DCA");
    loadFeature(regs.m_ecx, 19, "SSE4-1");
    loadFeature(regs.m_ecx, 20, "SSE4-2");
    loadFeature(regs.m_ecx, 21, "X2APIC");
    loadFeature(regs.m_ecx, 22, "MOVBE");
    loadFeature(regs.m_ecx, 23, "POPCNT");
    loadFeature(regs.m_ecx, 24, "TSC DEADLINE");
    loadFeature(regs.m_ecx, 25, "AES");
    loadFeature(regs.m_ecx, 26, "XSAVE");
    loadFeature(regs.m_ecx, 27, "OSXSAVE");
    loadFeature(regs.m_ecx, 28, "AVX");

    loadFeature(regs.m_edx, 0, "FPU");
    loadFeature(regs.m_edx, 1, "VME");
    loadFeature(regs.m_edx, 2, "DE");
    loadFeature(regs.m_edx, 3, "PSE");
    loadFeature(regs.m_edx, 4, "TSC");
    loadFeature(regs.m_edx, 5, "MSR");
    loadFeature(regs.m_edx, 6, "PAE");
    loadFeature(regs.m_edx, 7, "MCE");
    loadFeature(regs.m_edx, 8, "CX8");
    loadFeature(regs.m_edx, 9, "APIC");
    loadFeature(regs.m_edx, 11, "SEP");
    loadFeature(regs.m_edx, 12, "MTRR");
    loadFeature(regs.m_edx, 13, "PGE");
    loadFeature(regs.m_edx, 14, "MCA");
    loadFeature(regs.m_edx, 15, "CMOV");
    loadFeature(regs.m_edx, 16, "PAT");
    loadFeature(regs.m_edx, 17, "PSE-36");
    loadFeature(regs.m_edx, 18, "PSN");
    loadFeature(regs.m_edx, 19, "CLFSH");
    loadFeature(regs.m_edx, 21, "DS");
    loadFeature(regs.m_edx, 22, "ACPI");
    loadFeature(regs.m_edx, 23, "MMX");
    loadFeature(regs.m_edx, 24, "FXSR");
    loadFeature(regs.m_edx, 25, "SSE");
    loadFeature(regs.m_edx, 26, "SSE2");
    loadFeature(regs.m_edx, 27, "SS");
    loadFeature(regs.m_edx, 28, "HTT");
    loadFeature(regs.m_edx, 29, "TM");
    loadFeature(regs.m_edx, 31, "PBE");

}

void ArchitectureX86::loadCacheInfo(int cacheId) {
    Registers regs;
    regs.m_eax = 4;
    regs.m_ecx = cacheId;
    regs.m_ebx = regs.m_edx = 0;
    regs = cpuid(regs);

    Cache info;
    info.m_threads = getBits(regs.m_eax, 0, 4) + 1;
    info.m_fullAssociative = getBits(regs.m_eax, 9, 9);
    info.m_level = getBits(regs.m_eax, 5, 7);
    info.m_type = (CACHE_TYPE)getBits(regs.m_eax, 0, 4);
    info.m_associativity = getBits(regs.m_ebx, 22, 31) + 1;
    info.m_linePartitions = getBits(regs.m_ebx, 12, 21) + 1;
    info.m_lineSize = getBits(regs.m_ebx, 0, 11) + 1;
    info.m_sets = regs.m_ecx + 1;
    info.m_size = info.m_associativity *
            info.m_linePartitions *
            info.m_lineSize *
            info.m_sets;
    m_caches.push_back(info);
}

void ArchitectureX86::setCacheData() {
    unsigned int cacheCount = getCacheCount();
    unsigned int index;
    for (index = 0; index < cacheCount; index++) {
        loadCacheInfo(index);
    }
}

void ArchitectureX86::setMemoryData() {
#ifdef UNIX
    struct sysinfo info;
    if (sysinfo(&info) != 0) {
        m_totalMemory = info.totalram;
    } else {
        // TODO: exception
    }
#endif
#ifdef WIN32

#endif


}

/******************************************
 * Add dense to dense:
 *      - count nonzeros in result: function type
 *      - using relative tolerance: argument
 *      - using absolute tolerance: argument
 *      - using both tolerances: argument
 *      - 32 or 64 bit: different implementations
 *      - Unix or Windows arguments: different implementations
 *      - vectors are sparse or not: branch in the function
 *      - SSE2 or AVX: different implementations
 *****************************************/


ArchitectureX86::AddVecDenseToDense_NonzCount ArchitectureX86::getAddVecDenseToDense_NonzCount() const {

}

ArchitectureX86::AddVecDenseToDense ArchitectureX86::getAddVecDenseToDense() const {

}
