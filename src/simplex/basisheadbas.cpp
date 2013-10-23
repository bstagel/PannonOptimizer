#include <simplex/basisheadbas.h>
#include <utils/hashtable.h>

#define BUFF_SIZE 102400
#define FIELD_2_START 14
#define BAS_RECORD_SIZE 80
#define MPS_NAME_LENGTH 8

void BasisHeadBAS::startWrting(const char * fileName) {
    if (m_outputFile.is_open() == true) {
        m_outputFile.close();
    }
    m_outputFile.open(fileName);
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
    m_outputFile << "NAME          " << m_info << std::endl;

    unsigned int nonbasicIndex = 0;
    STL_FOREACH(std::vector<BasicVariable>, m_basicVariables, basicIter) {
        m_outputFile << " ";
        if ( m_nonBasicVariables[nonbasicIndex].m_state == Simplex::NONBASIC_AT_LB ) {
            m_outputFile << "XL " << std::setw(MPS_NAME_LENGTH) << std::setfill(' ') <<
                            basicIter->m_variable.getName() << "  ";
        } else if (m_nonBasicVariables[nonbasicIndex].m_state == Simplex::NONBASIC_AT_UB) {
            m_outputFile << "XU " << std::setw(MPS_NAME_LENGTH) << std::setfill(' ') <<
                            basicIter->m_variable.getName() << "  ";
        } else {
            // TODO: exception
        }
        m_outputFile << std::setw(MPS_NAME_LENGTH) << std::setfill(' ') <<
                        m_nonBasicVariables[nonbasicIndex].m_variable.getName() << std::endl;
        nonbasicIndex++;
    }

    for (; nonbasicIndex < m_nonBasicVariables.size(); nonbasicIndex++) {
        if ( m_nonBasicVariables[nonbasicIndex].m_state == Simplex::NONBASIC_AT_LB ) {
            m_outputFile << " LL " << std::setw(MPS_NAME_LENGTH) << std::setfill(' ') <<
                            m_nonBasicVariables[nonbasicIndex].m_variable.getName() << std::endl;
        } else if (m_nonBasicVariables[nonbasicIndex].m_state == Simplex::NONBASIC_AT_UB) {
            m_outputFile << " UL " << std::setw(MPS_NAME_LENGTH) << std::setfill(' ') <<
                            m_nonBasicVariables[nonbasicIndex].m_variable.getName() << std::endl;
        } else {
            // TODO: exception
        }
    }

    m_outputFile << "ENDATA" << std::endl;
    m_outputFile.close();
}

void BasisHeadBAS::startReading(const char * fileName,
                                unsigned int variableCount) {
    if (m_inputFile.is_open() == true) {
        m_inputFile.close();
    }
    m_inputFile.open(fileName);
    m_hashTable.setSize(variableCount);
}

void BasisHeadBAS::addVariable(const Variable & variable) {
    VariableIndex vi;
    vi.m_index = m_hashTable.getCount();
    vi.m_name = variable.getName();
    if (m_hashTable.addUnique(vi) == false) {
        // TODO: exception, ha van tobb azonos nevu valtozo
    }
}

void BasisHeadBAS::finishReading(std::vector<int> * basisHead,
                                 IndexList<const Numerical::Double*> * variableStates) {
    basisHead->resize(basisHead->size(), -1);
    variableStates->init( m_hashTable.getCount(), Simplex::VARIABLE_STATE_ENUM_LENGTH );
    int index;
    for (index = 0; index < m_hashTable.getCount(); index++) {
        variableStates->insert( Simplex::NONBASIC_FREE, index );
    }
    std::string line;
    VariableIndex name1, name2;
    name1.m_index = 0;
    name2.m_index = 0;
    unsigned int basisIndex = 0;
    while ( m_inputFile.eof() == false ) {
        std::getline(m_inputFile, line);

        if (line[0] != ' ' || line[3] != ' ') {
            //TODO: exception ha nem NAME, ENDATA, vagy komment
        } else {
            char ch1 = line[1];
            char ch2 = line[2];
            if (ch1 == 'X') {
                if (ch2 != 'U' && ch2 != 'L') {

                } else {
                    if (line[12] != ' ' || line[13] != ' ') {
                        // TODO: exception, vagy itt is ingelligens
                        // es hibaturo beolvaso
                    }
                    name1.m_name = line.substr(4, 8);
                    name2.m_name = line.substr(14, 8);
                    // XL ...166    ...119
                    const VariableIndex * ptr = m_hashTable.get(name1);

                    if (ptr != 0) {
                        (*basisHead)[ basisIndex++ ] = ptr->m_index;
                        variableStates->insert( Simplex::BASIC, ptr->m_index );
                    } else {
                        // TODO: exception
                    }
                    ptr = m_hashTable.get(name2);
                    if (ptr != 0) {
                        if (ch2 == 'L') {
                            variableStates->insert( Simplex::NONBASIC_AT_LB, ptr->m_index );
                        } else {
                            variableStates->insert( Simplex::NONBASIC_AT_UB, ptr->m_index );
                        }
                    } else {
                        // TODO: exception
                    }
                }
            } else if (ch2 == 'L') {
                if (ch1 != 'U' && ch2 != 'L') {

                } else {
                    name1.m_name = line.substr(4, 8);
                    const VariableIndex * ptr = m_hashTable.get(name1);
                    if (ptr != 0) {
                        if (ch1 == 'L') {
                            variableStates->insert( Simplex::NONBASIC_AT_LB, ptr->m_index );
                        } else {
                            variableStates->insert( Simplex::NONBASIC_AT_UB, ptr->m_index );
                        }
                    } else {
                        // TODO: exception
                    }
                }
            } else {
                // TODO: exception, vagy itt is ingelligens
                // es hibaturo beolvaso
            }
        }
        // TODO: ellenorizni a duplikaciokat is!
    }

}
