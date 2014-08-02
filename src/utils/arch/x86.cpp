#include <utils/arch/x86.h>
#ifdef UNIX
#include <sys/sysinfo.h>
#endif
#include <cstring>
#include <iostream>
#include <cstring>
#include <utils/primitives.h>


ArchitectureX86::ArchitectureX86() {

}

void ArchitectureX86::detect() {
    std::cout << "X86 detect" << std::endl;
    m_architectureName = "X86";
    if (cpuidSupported() == true) {
        setCPUData();
        setCacheData();
        setFeatureList();
    } else {
        std::cout << "CPUID not supported" << std::endl;
    }
    setMemoryData();
    setPrimitives();
}

void ArchitectureX86::setPrimitives() {
    setMemcpy();
    setDenseToDenseDotProduct();
}

void ArchitectureX86::setMemcpy() {
    // memcpy, with cache
    if (featureExists("SSE2")) {
        sm_memCpyCachePtr = MEMCPY_CACHE_SSE2;
    } else {
        sm_memCpyCachePtr = memcpy;
    }
    // TODO: egy MMX-es verzio arra az esetre ha nem lenne SSE2
    // illetve egy SSE-s verzio is jol johet

    // memcpy, without cache
    if (featureExists("SSE4-1")) {
        sm_memCpyCachePtr = MEMCPY_NO_CACHE_SSE4_1;
        // ebbol csak sse4-1-es van
    } else {
        sm_memCpyCachePtr = memcpy;
    }

}

void ArchitectureX86::setDenseToDenseDotProduct() {
    if (featureExists("AVX")) {
        sm_denseToDenseDotProductUnstablePtr = DENSE_TO_DENSE_UNSTABLE_AVX;
    } else if (featureExists("SSE2")) {
        sm_denseToDenseDotProductUnstablePtr = DENSE_TO_DENSE_UNSTABLE_SSE2;
    } else {
        sm_denseToDenseDotProductUnstablePtr = denseToDenseDotProductUnstable;
    }
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

bool ArchitectureX86::cpuidSupported()
{
#ifdef ENVIRONMENT_32
    return _cpuinfo_supported_32();
#else
    return true;
#endif
}

ArchitectureX86::Registers ArchitectureX86::cpuid(Registers args) const {
    CPUID(args.m_eax,
          args.m_ebx,
          args.m_ecx,
          args.m_edx,
          &args.m_eax);

    return args;
}

void ArchitectureX86::setCPUData() {
    // CPU name
    char cpuName[50] = {0};
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
    m_cpuName = std::string(cpuName);
    std::cout << "CPU name: " << m_cpuName << std::endl;

    // CPU vendor
    regs.m_eax = 0;
    regs.m_ebx = 0;
    regs.m_ecx = 0;
    regs.m_edx = 0;
    regs = cpuid(regs);
    memcpy(cpuName, &regs.m_ebx, sizeof(unsigned int) * 3);
    cpuName[12] = 0;
    m_vendor = std::string(cpuName);
    std::cout << "Vendor: >" << m_vendor << "<" << std::endl;
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
        std::cout << name << " ";
        m_features.insert(name);
    }
}

void ArchitectureX86::setFeatureList() {
    Registers regs;
    std::cout << "CPU features: ";

    if (m_vendor == "AMDisbetter") {

    }
    if (m_vendor == "CentaurHauls") {

    }
    if (m_vendor == "CyrixInstead") {

    }
    if (m_vendor == "TransmetaCPU") {

    }
    if (m_vendor == "GenuineTMx86") {

    }
    if (m_vendor == "Geode by NSC") {

    }
    if (m_vendor == "NexGenDriven") {

    }
    if (m_vendor == "RiseRiseRise") {

    }
    if (m_vendor == "SiS SiS SiS") {

    }
    if (m_vendor == "UMC UMC UMC") {

    }
    if (m_vendor == "VIA VIA VIA") {

    }
    if (m_vendor == "Vortex86 SoC") {

    }
    if (m_vendor == "KVMKVMKVMKVM") {

    }
    if (m_vendor == "Microsoft Hv") {

    }
    if (m_vendor == "VMwareVMware") {

    }
    if (m_vendor == "XenVMMXenVMM") {

    }
    if (m_vendor == "GenuineIntel") {
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
        loadFeature(regs.m_ecx, 8, "TM2");
        loadFeature(regs.m_ecx, 9, "SSSE3");
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
    if (m_vendor == "AuthenticAMD") {
        regs.m_eax = 1;
        regs.m_ebx = regs.m_ecx = regs.m_edx = 0;
        regs = cpuid(regs);

        loadFeature(regs.m_ecx, 0, "SSE3");
        loadFeature(regs.m_ecx, 1, "PCLMULDQ");
        loadFeature(regs.m_ecx, 3, "MONITOR");
        loadFeature(regs.m_ecx, 9, "SSSE3");
        loadFeature(regs.m_ecx, 12, "FMA");
        loadFeature(regs.m_ecx, 13, "CX16");
        loadFeature(regs.m_ecx, 19, "SSE4-1");
        loadFeature(regs.m_ecx, 20, "SSE4-2");
        loadFeature(regs.m_ecx, 23, "POPCNT");
        loadFeature(regs.m_ecx, 25, "AES");
        loadFeature(regs.m_ecx, 26, "XSAVE");
        loadFeature(regs.m_ecx, 27, "OSXSAVE");
        loadFeature(regs.m_ecx, 28, "AVX");
        loadFeature(regs.m_ecx, 29, "F16C");

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
        loadFeature(regs.m_edx, 19, "CLFSH");
        loadFeature(regs.m_edx, 23, "MMX");
        loadFeature(regs.m_edx, 24, "FXSR");
        loadFeature(regs.m_edx, 25, "SSE");
        loadFeature(regs.m_edx, 26, "SSE2");
        loadFeature(regs.m_edx, 28, "HTT");

    }
    std::cout << std::endl;
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
    std::cout << "\tLevel: " << info.m_level << std::endl;
    std::cout << "\tSize: " << (info.m_size / 1024) << " Kbyte" << std::endl;

    if (sm_largestCacheSize < info.m_size) {
        sm_largestCacheSize = info.m_size;
    }

    m_caches.push_back(info);
}

void ArchitectureX86::setCacheData() {
    unsigned int cacheCount = getCacheCount();
    unsigned int index;
    std::cout << "Caches:" << std::endl;
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
    //Return to avoid warning
    return 0;
}

ArchitectureX86::AddVecDenseToDense ArchitectureX86::getAddVecDenseToDense() const {
    //Return to avoid warning
    return 0;
}
