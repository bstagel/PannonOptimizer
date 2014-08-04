#include <utils/arch/x86.h>
#ifdef UNIX
#include <sys/sysinfo.h>
#endif
#include <cstring>
#include <iostream>
#include <cstring>
#include <utils/primitives.h>
#include <utils/platform.h>

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
    return cpuinfo_supported_32();
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
    char temp[50] = {0};
    Registers regs;

    // CPU vendor
    regs.m_eax = 0;
    regs.m_ebx = 0;
    regs.m_ecx = 0;
    regs.m_edx = 0;
    regs = cpuid(regs);
    m_largestFunction = regs.m_eax;
    memcpy(temp, &regs.m_ebx, sizeof(unsigned int));
    memcpy(temp + 4, &regs.m_edx, sizeof(unsigned int));
    memcpy(temp + 8, &regs.m_ecx, sizeof(unsigned int));
    temp[12] = 0;
    m_vendor = std::string(temp);
    std::cout << "Vendor: " << m_vendor << std::endl;
    std::cout << "Largest function: " << m_largestFunction << std::endl;

    regs.m_eax = 0x80000000;
    regs.m_ebx = regs.m_ecx = regs.m_edx = 0;
    regs = cpuid(regs);
    m_largestExtendedFunction = regs.m_eax;
    std::cout << "Largest extended function: 0x" << std::hex << m_largestExtendedFunction << std::dec << std::endl;

    // CPU name
    if (m_largestExtendedFunction < 0x80000004) {
        return;
    }
    regs.m_eax = 0x80000002;
    regs.m_ebx = regs.m_ecx = regs.m_edx = 0;
    regs = cpuid(regs);
    memcpy(temp, &regs, sizeof(regs));

    regs.m_eax = 0x80000003;
    regs.m_ebx = regs.m_ecx = regs.m_edx = 0;
    regs = cpuid(regs);
    memcpy(temp + 16, &regs, sizeof(regs));

    regs.m_eax = 0x80000004;
    regs.m_ebx = regs.m_ecx = regs.m_edx = 0;
    regs = cpuid(regs);
    memcpy(temp + 32, &regs, sizeof(regs));
    temp[48] = 0;
    m_cpuName = std::string(temp);
    std::cout << "CPU name: " << m_cpuName << std::endl;
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
        // TODO: extra featureket is belerakni
        if (m_largestFunction < 1) {
            return;
        }
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
        if (m_largestFunction < 1) {
            return;
        }
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

        // extra features
        if (m_largestExtendedFunction < 0x80000001) {
            return;
        }
        regs.m_eax = 0x80000001;
        regs.m_ebx = regs.m_ecx = regs.m_edx = 0;
        regs = cpuid(regs);
        loadFeature(regs.m_ecx, 0, "LAHFSAHF");
        loadFeature(regs.m_ecx, 1, "CMPLEGACY");
        loadFeature(regs.m_ecx, 2, "SVM");
        loadFeature(regs.m_ecx, 3, "EXTAPICSPACE");
        loadFeature(regs.m_ecx, 4, "ALTMOVCR8");
        loadFeature(regs.m_ecx, 5, "ABM");
        loadFeature(regs.m_ecx, 6, "SSE4A");
        loadFeature(regs.m_ecx, 7, "MISALIGNSSE");
        loadFeature(regs.m_ecx, 8, "3DNOWPREFETCH");
        loadFeature(regs.m_ecx, 9, "OSVW");
        loadFeature(regs.m_ecx, 10, "IBS");
        loadFeature(regs.m_ecx, 11, "XOP");
        loadFeature(regs.m_ecx, 12, "SKINIT");
        loadFeature(regs.m_ecx, 13, "WDT");
        loadFeature(regs.m_ecx, 15, "LWP");
        loadFeature(regs.m_ecx, 16, "FMA4");
        loadFeature(regs.m_ecx, 19, "NODEID");
        loadFeature(regs.m_ecx, 21, "TBM");
        loadFeature(regs.m_ecx, 22, "TOPOLOGYEXTENSIONS");

        loadFeature(regs.m_edx, 11, "SYSCALLSYSRET");
        loadFeature(regs.m_edx, 20, "NX");
        loadFeature(regs.m_edx, 22, "MMXEXT");
        loadFeature(regs.m_edx, 25, "FFXSR");
        loadFeature(regs.m_edx, 26, "PAGE1GB");
        loadFeature(regs.m_edx, 27, "RDTSCP");
        loadFeature(regs.m_edx, 29, "LM");
        loadFeature(regs.m_edx, 30, "3DNOWEXT");
        loadFeature(regs.m_edx, 31, "3DNOW");
    }
    std::cout << std::endl;
}

unsigned int ArchitectureX86::getCacheCountIntel() const {
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

void ArchitectureX86::loadCacheInfoIntel(int cacheId) {
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

void ArchitectureX86::setCacheDataIntel() {
    if (m_largestFunction < 4) {
        return;
    }
    unsigned int cacheCount = getCacheCountIntel();
    unsigned int index;
    std::cout << "Caches:" << std::endl;
    for (index = 0; index < cacheCount; index++) {
        loadCacheInfoIntel(index);
    }
}

unsigned int ArchitectureX86::getAMDL2L3CacheAssociativity(unsigned int pattern) {
    switch (pattern) {
        case 0x00:
            // disabled cache
            return 0;
            break;
        case 0x01:
        case 0x02:
        case 0x04:
            // remains the same
            return pattern;
            break;
        case 0x03:
        case 0x05:
        case 0x07:
        case 0x09:
            return 0;
            break;
        case 0x06:
            return 8;
            break;
        case 0x08:
            return 16;
            break;
        case 0x0A:
            return 32;
            break;
        case 0x0B:
            return 48;
            break;
        case 0x0C:
            return 64;
            break;
        case 0x0D:
            return 96;
            break;
        case 0x0E:
            return 128;
            break;
    }
    return 0;
}

void ArchitectureX86::showAMDCacheInfo(const Cache & info) {
    std::cout << "\tLevel : " << info.m_level << std::endl;
    switch (info.m_type) {
        case DATA_CACHE:
            std::cout << "\tType : data" << std::endl;
            break;
        case INSTRUCTION_CACHE:
            std::cout << "\tType : instruction" << std::endl;
            break;
        case UNIFIED_CACHE:
            std::cout << "\tType : unified" << std::endl;
            break;

    }
    std::cout << "\tSize : " << info.m_size / 1024 << " Kbyte" << std::endl;
    std::cout << "\tFull associative : " << (info.m_fullAssociative ? "true" : "false") << std::endl;
    if (info.m_fullAssociative == false) {
        std::cout << "\tAssociativity : " << info.m_associativity << std::endl;
    }
    std::cout << "\tCache lines per tag : " << info.m_linePartitions << std::endl;
    std::cout << "\tCache line size : " << info.m_lineSize << std::endl;
    std::cout << std::endl;
}

void ArchitectureX86::setCacheDataAMD() {
    if (m_largestExtendedFunction < 0x80000005) {
        return;
    }
    Registers regs;
    // Level 1

    regs.m_eax = 0x80000005;
    regs.m_ebx = regs.m_ecx = regs.m_edx = 0;
    regs = cpuid(regs);
    // Data
    Cache info;
    memset(&info, 0, sizeof(info));
    info.m_level = 1;
    info.m_type = DATA_CACHE;
    info.m_size = getBits(regs.m_ecx, 24, 31) * 1024;
    if (sm_largestCacheSize < info.m_size) {
        sm_largestCacheSize = info.m_size;
    }
    info.m_fullAssociative = getBits(regs.m_ecx, 16, 23) == 0xFF;
    info.m_associativity = getBits(regs.m_ecx, 16, 23);
    info.m_linePartitions = getBits(regs.m_ecx, 8, 15);
    info.m_lineSize = getBits(regs.m_ecx, 0, 7);
    m_caches.push_back(info);
    showAMDCacheInfo(info);

    // Instruction
    info.m_level = 1;
    info.m_type = INSTRUCTION_CACHE;
    info.m_size = getBits(regs.m_edx, 24, 31) * 1024;
    if (sm_largestCacheSize < info.m_size) {
        sm_largestCacheSize = info.m_size;
    }
    info.m_fullAssociative = getBits(regs.m_edx, 16, 23) == 0xFF;
    info.m_associativity = getBits(regs.m_edx, 16, 23);
    info.m_linePartitions = getBits(regs.m_edx, 8, 15);
    info.m_lineSize = getBits(regs.m_edx, 0, 7);
    m_caches.push_back(info);
    showAMDCacheInfo(info);

    // Level 2
    if (m_largestExtendedFunction < 0x80000006) {
        return;
    }
    regs.m_eax = 0x80000006;
    regs.m_ebx = regs.m_ecx = regs.m_edx = 0;
    regs = cpuid(regs);
    memset(&info, 0, sizeof(info));
    info.m_level = 2;
    info.m_type = UNIFIED_CACHE;

    info.m_size = getBits(regs.m_ecx, 16, 31) * 1024;
    if (info.m_size == 0) {
        return;
    }
    if (sm_largestCacheSize < info.m_size) {
        sm_largestCacheSize = info.m_size;
    }
    info.m_fullAssociative = getBits(regs.m_ecx, 12, 15) == 0xFF;
    info.m_associativity = getAMDL2L3CacheAssociativity(getBits(regs.m_ecx, 12, 15));

    info.m_linePartitions = getBits(regs.m_ecx, 8, 11);
    info.m_lineSize = getBits(regs.m_ecx, 0, 7);
    showAMDCacheInfo(info);

    // Level 3
    info.m_level = 3;
    info.m_type = UNIFIED_CACHE;

    info.m_size = getBits(regs.m_edx, 18, 31) * 512 * 1024;
    if (info.m_size == 0) {
        return;
    }
    if (sm_largestCacheSize < info.m_size) {
        sm_largestCacheSize = info.m_size;
    }
    info.m_fullAssociative = getBits(regs.m_edx, 12, 15) == 0xFF;
    info.m_associativity = getAMDL2L3CacheAssociativity(getBits(regs.m_ecx, 12, 15));
    info.m_linePartitions = getBits(regs.m_edx, 8, 11);
    info.m_lineSize = getBits(regs.m_edx, 0, 7);
    showAMDCacheInfo(info);

}

void ArchitectureX86::setCacheData() {
    if (m_vendor == "GenuineIntel") {
        setCacheDataIntel();
    }
    if (m_vendor == "AuthenticAMD") {
        setCacheDataAMD();
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
