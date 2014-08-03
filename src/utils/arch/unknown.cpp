#include <utils/arch/unknown.h>

ArchitectureUnknown::ArchitectureUnknown() {

}

void ArchitectureUnknown::detect() {

}

Architecture::AddVecDenseToDense_NonzCount ArchitectureUnknown::getAddVecDenseToDense_NonzCount() const {
    return 0;
}

Architecture::AddVecDenseToDense ArchitectureUnknown::getAddVecDenseToDense() const {
    return 0;
}
