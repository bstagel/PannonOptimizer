#include <simplex/basisheadbas.h>

#define BUFF_SIZE 102400
#define FIELD_2_START 14
#define BAS_RECORD_SIZE 80
#define MPS_NAME_LENGTH 8

void BasisHeadBAS::startWrting(const char * fileName) {
    if (m_file.is_open() == true) {
        m_file.close();
    }
    m_file.open(fileName);
    m_basicVariables.clear();
    m_nonBasicVariables.clear();
    m_info.clear();
}

void BasisHeadBAS::addInfo(const char * info) {
    m_info = std::string(info);
}

void BasisHeadBAS::addBasicVariable(const Variable & variable,
                                    unsigned int basisPosition,
                                    Numerical::Double value) {
    BasicVariable basicVariable;
    basicVariable.m_variable = variable;
    basicVariable.m_basisPosition = basisPosition;
    m_basicVariables.push_back(basicVariable);
    __UNUSED(value);
}

void BasisHeadBAS::addNonbasicVariable(const Variable & variable,
                                       Simplex::VARIABLE_STATE state,
                                       Numerical::Double value) {
    NonBasicVariable nonbasicVariable;
    nonbasicVariable.m_variable = variable;
    nonbasicVariable.m_state = state;
    m_nonBasicVariables.push_back(nonbasicVariable);
    __UNUSED(value);
}

void BasisHeadBAS::finishWriting() {
    m_file << "NAME          " << m_info << std::endl;

    unsigned int nonbasicIndex = 0;
    STL_FOREACH(std::vector<BasicVariable>, m_basicVariables, basicIter) {
        m_file << " ";
        if ( m_nonBasicVariables[nonbasicIndex].m_state == Simplex::NONBASIC_AT_LB ) {
            m_file << "XL " << std::setw(MPS_NAME_LENGTH) << std::setfill(' ') <<
                      basicIter->m_variable.getName() << "  ";
        } else if (m_nonBasicVariables[nonbasicIndex].m_state == Simplex::NONBASIC_AT_UB) {
            m_file << "XU " << std::setw(MPS_NAME_LENGTH) << std::setfill(' ') <<
                      basicIter->m_variable.getName() << "  ";
        } else {
            // TODO: exception
        }
        m_file << std::setw(MPS_NAME_LENGTH) << std::setfill(' ') <<
                  m_nonBasicVariables[nonbasicIndex].m_variable.getName() << std::endl;
        nonbasicIndex++;
    }

    for (; nonbasicIndex < m_nonBasicVariables.size(); nonbasicIndex++) {
        if ( m_nonBasicVariables[nonbasicIndex].m_state == Simplex::NONBASIC_AT_LB ) {
            m_file << " LL " << std::setw(MPS_NAME_LENGTH) << std::setfill(' ') <<
                      m_nonBasicVariables[nonbasicIndex].m_variable.getName() << std::endl;
        } else if (m_nonBasicVariables[nonbasicIndex].m_state == Simplex::NONBASIC_AT_UB) {
            m_file << " UL " << std::setw(MPS_NAME_LENGTH) << std::setfill(' ') <<
                      m_nonBasicVariables[nonbasicIndex].m_variable.getName() << std::endl;
        } else {
            // TODO: exception
        }
    }

    m_file << "ENDATA" << std::endl;
    m_file.close();
}
