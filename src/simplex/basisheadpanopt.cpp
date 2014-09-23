/**
 * @file basisheadpanopt.cpp
 */
#include <simplex/basisheadpanopt.h>
#include <utils/datetime.h>
#include <utils/system.h>
#include <utils/nodefile.h>

#define HEADER_COMMENT_STR "Pannon Optimizer simplex state file"
#define DATE_STR "Date"
#define TIME_STR "Time"
#define USER_STR "User"
#define ADDITIONAL_INFO_STR "Additional info"
#define MODEL_STR "Model"
#define NAME_STR "Name"
#define CONSTRAINT_COUNT_STR "ConstraintCount"
#define VARIABLE_COUNT_STR "StructuralVariableCount"
#define SHA1_STR "SHA1Hash"
#define HISTORY_STR "History"
#define STEPS_STR "Steps"
#define BASIS_STR "Basis"
#define BASIS_HEAD_COMMENT_STR "Basis head:"
#define INDICES_STR "Indices"
#define VARIABLE_STR "Variable"
#define LOWER_BOUND_STR "LowerBound"
#define UPPER_BOUND_STR "UpperBound"
#define VALUE_STR "Value"
#define STRUCTURAL_TYPE_STR "StructuralType"
#define LOGICAL_STR "LOGICAL"
#define STRUCTURAL_STR "STRUCTURAL"
#define BASIS_INDEX_STR "BasisIndex"
#define NONBASIS_PART_STR "NonBasisPart"
#define STATE_STR "State"
#define AT_LOWER_BOUND_STR "AT_LOWER_BOUND"
#define AT_UPPER_BOUND_STR "AT_UPPER_BOUND"
#define FREE_STR "FREE"
#define FIXED_STR "FIXED"
#define INDEX_STR "Index"

BasisHeadPanOpt::BasisHeadPanOpt() {
    m_modelPtr = 0;
}

void BasisHeadPanOpt::startWrting(const char * fileName,
                                  const Model & model) {
    m_outputFile.open(fileName);
    if (m_outputFile.is_open() == false) {
        if (m_outputFile.is_open() == false) {
            throw FileNotFoundException("Error when creating basis file!", fileName);
        }
    }
    m_modelPtr = &model;
}

void BasisHeadPanOpt::writeDouble(Numerical::Double value) {
    if ( value != -Numerical::Infinity ) {
        m_outputFile << value;
    } else {
        m_outputFile << "minus_inf";
    }
}

void BasisHeadPanOpt::writeString(const std::string & str) {
    unsigned int index;
    m_outputFile << '\"';
    for (index = 0; index < str.length(); index++) {
        switch (str[index]) {
        case '\\':
            m_outputFile << "\\\\";
            break;
        case '\"':
            m_outputFile << "\\\"";
            break;
        default:
            m_outputFile << str[index];
        }
    }
    m_outputFile << '\"';
}

void BasisHeadPanOpt::writeGeneralInfo() {
    m_outputFile << "# " HEADER_COMMENT_STR << std::endl;
    m_outputFile << "# " DATE_STR ": " << DateTime::getCurrentDate(DateTime::YEAR_FOUR_DIGITS,
                                                           DateTime::MONTH_FULL,
                                                           DateTime::WEEKDAY_FULL) << std::endl;
    m_outputFile << "# " TIME_STR ": " << DateTime::getCurrentTime() << std::endl;
    m_outputFile << "# " USER_STR ": " << System::getCurrentUserName() << std::endl;
    if (m_info.size() > 0) {
        m_outputFile << "# " ADDITIONAL_INFO_STR ": " << m_info << std::endl;
    }
    m_outputFile << std::endl;
    m_outputFile << MODEL_STR " {" << std::endl;
    m_outputFile << "\t" NAME_STR " = ";
    writeString(m_modelPtr->getName());
    m_outputFile << std::endl;
    m_outputFile << "\t" CONSTRAINT_COUNT_STR " = " << m_modelPtr->getConstraints().size() << std::endl;
    m_outputFile << "\t" VARIABLE_COUNT_STR " = " << m_modelPtr->getMatrix().columnCount() << std::endl;
    m_outputFile << "\t" SHA1_STR " = ";
    writeString(m_modelPtr->getHash());
    m_outputFile << std::endl;
    m_outputFile << "\t" HISTORY_STR " {" << std::endl;

    std::vector<std::string>::const_iterator historyIter = m_modelPtr->getHistory().begin();
    std::vector<std::string>::const_iterator historyIterEnd = m_modelPtr->getHistory().end();
    unsigned int historyIndex = 0;
    for (; historyIter != historyIterEnd; historyIter++, historyIndex++) {
        if (historyIndex < m_modelPtr->getHistory().size() - 1 ) {
            m_outputFile << std::endl;
        }
        m_outputFile << "\t\t" STEPS_STR "[" << historyIndex << "] = ";
        writeString(*historyIter);
    }

    m_outputFile << std::endl << "\t}" << std::endl;
    m_outputFile << "}" << std::endl << std::endl;
}

void BasisHeadPanOpt::writeBasisHeadInfo() {
    m_outputFile << BASIS_STR " {" << std::endl;

    std::vector<BasicVariableInfo>::const_iterator iter =
            m_basicVariables.begin();
    std::vector<BasicVariableInfo>::const_iterator iterEnd =
            m_basicVariables.end();
    unsigned int index = 0;
    m_outputFile << "\t# " BASIS_HEAD_COMMENT_STR << std::endl;
    while (iter != iterEnd) {
        std::ostringstream rowStream;
        unsigned int startIndex = index;
        bool repeat;

        do {
            rowStream << iter->m_variableIndex << " ";
            ++iter;
            repeat = iter != iterEnd && (int)rowStream.str().length() < sm_maxRowLength;
            if (repeat == true) {
                index++;
            }
        } while (repeat == true);

        m_outputFile << "\t" INDICES_STR "[" << startIndex << " ... " << index <<
                        "] = " << rowStream.str();
        m_outputFile << std::endl;
        index++;
    }

    iter = m_basicVariables.begin();
    for (; iter != iterEnd; ++iter) {
        m_outputFile << "\t" VARIABLE_STR " {" << std::endl;
        m_outputFile << "\t\t" NAME_STR " = ";
        writeString(iter->m_variablePtr->getName());
        m_outputFile << std::endl;
        m_outputFile << "\t\t" LOWER_BOUND_STR " = ";
        writeDouble(iter->m_variablePtr->getLowerBound());
        m_outputFile << std::endl;
        m_outputFile << "\t\t" UPPER_BOUND_STR " = ";
        writeDouble(iter->m_variablePtr->getUpperBound());
        m_outputFile << std::endl;
        m_outputFile << "\t\t" VALUE_STR " = ";
        writeDouble(iter->m_value);
        m_outputFile << std::endl;
        m_outputFile << "\t\t" STRUCTURAL_TYPE_STR " = ";
        if ( iter->m_isLogical == true ) {
            m_outputFile << LOGICAL_STR;
        } else {
            m_outputFile << STRUCTURAL_STR;
        }
        m_outputFile << std::endl;
        m_outputFile << "\t\t" BASIS_INDEX_STR " = " << iter->m_basisIndex << std::endl;
        m_outputFile << "\t}" << std::endl;
    }
    m_outputFile << "}" << std::endl << std::endl;
}

void BasisHeadPanOpt::writeNonBasisInfo() {
    m_outputFile << NONBASIS_PART_STR " {" << std::endl;

    std::vector<NonBasicVariableInfo>::const_iterator iter =
            m_nonBasicVariables.begin();

    std::vector<NonBasicVariableInfo>::const_iterator iterEnd =
            m_nonBasicVariables.end();
    for (; iter != iterEnd; ++iter) {
        m_outputFile << "\t" VARIABLE_STR " {" << std::endl;
        m_outputFile << "\t\t" NAME_STR " = ";
        writeString(iter->m_variablePtr->getName());
        m_outputFile << std::endl;
        m_outputFile << "\t\t" LOWER_BOUND_STR " = ";
        writeDouble( iter->m_variablePtr->getLowerBound() );
        m_outputFile << std::endl;
        m_outputFile << "\t\t" UPPER_BOUND_STR " = ";
        writeDouble( iter->m_variablePtr->getUpperBound() );
        m_outputFile << std::endl;
        m_outputFile << "\t\t" VALUE_STR " = ";
        writeDouble(iter->m_value);
        m_outputFile << std::endl;
        m_outputFile << "\t\t" STATE_STR " = ";
        if ( iter->m_state == Simplex::NONBASIC_AT_LB ) {
            m_outputFile << AT_LOWER_BOUND_STR;
        } else if ( iter->m_state == Simplex::NONBASIC_AT_UB ) {
            m_outputFile << AT_UPPER_BOUND_STR;
        } else if ( iter->m_state == Simplex::NONBASIC_FREE ) {
            m_outputFile << FREE_STR;
        } else if ( iter->m_state == Simplex::NONBASIC_FIXED ) {
            m_outputFile << FIXED_STR;
        }
        m_outputFile << std::endl;
        m_outputFile << "\t\t" INDEX_STR " = " << iter->m_variableIndex << std::endl;
        m_outputFile << "\t\t" STRUCTURAL_TYPE_STR " = ";
        if ( iter->m_isLogical == true ) {
            m_outputFile << LOGICAL_STR;
        } else {
            m_outputFile << STRUCTURAL_STR;
        }
        m_outputFile << std::endl;
        m_outputFile << "\t}" << std::endl;
    }

    m_outputFile << "}" << std::endl;
}

void BasisHeadPanOpt::finishWriting() {
    writeGeneralInfo();
    writeBasisHeadInfo();
    writeNonBasisInfo();

    release();
}

void BasisHeadPanOpt::addInfo(const char * info) {
    m_info = info;
}

void BasisHeadPanOpt::startReading(const char * fileName,
                                   const Model & model) {
    m_inputFile.open(fileName);
    if (m_inputFile.is_open() == false) {
        throw FileNotFoundException("Error when opening basis file!", fileName);
    }

    m_modelPtr = &model;
}

void BasisHeadPanOpt::finishReading() {
    NodeFile nodes;
    nodes.loadFromStream(m_inputFile);
    const NodeFile::Node & document = nodes.getDocumentNode();
    std::vector<NodeFile::Node>::const_iterator nodeIter;
    std::vector<NodeFile::Node>::const_iterator nodeIterEnd;
    document.getNodes(MODEL_STR, &nodeIter, &nodeIterEnd);
    const NodeFile::Node & modelNode = *nodeIter;

    // checking the model
    if (modelNode.getValue(NAME_STR) != ("\"" + m_modelPtr->getName() + "\"") ) {
        throw PanOptException("Missing Model node! ");
    }
    std::string countStr = modelNode.getValue(CONSTRAINT_COUNT_STR);
    if ( NodeFile::Node::checkInteger(countStr) == false ) {
        throw PanOptException("Invalid constraint count: " + countStr);
    }
    if ( atoi(countStr.c_str()) != (int)m_modelPtr->getConstraints().size() ) {
        throw PanOptException("Invalid constraint count: " + countStr);
    }
    countStr = modelNode.getValue(VARIABLE_COUNT_STR);
    if ( NodeFile::Node::checkInteger(countStr) == false ) {
        throw PanOptException("Invalid structural variable count: " + countStr);
    }
    if ( atoi(countStr.c_str()) != (int)m_modelPtr->getVariables().size() ) {
        throw PanOptException("Invalid structural variable count: " + countStr);
    }
    if (modelNode.getValue(SHA1_STR) != ("\"" + m_modelPtr->getHash() + "\"") ) {
        throw PanOptException("Invalid SHA1 hash");
    }

    // read the basis
    document.getNodes(BASIS_STR, &nodeIter, &nodeIterEnd);
    const NodeFile::Node & basisNode = *nodeIter;

    std::map<unsigned int, std::string>::const_iterator arrayIter;
    std::map<unsigned int, std::string>::const_iterator arrayIterEnd;
    basisNode.getArray(INDICES_STR, &arrayIter, &arrayIterEnd);
    m_variableStatesPtr->clearPartition(Simplex::BASIC);
    m_variableStatesPtr->clearPartition(Simplex::NONBASIC_AT_LB );
    m_variableStatesPtr->clearPartition(Simplex::NONBASIC_AT_UB );
    m_variableStatesPtr->clearPartition(Simplex::NONBASIC_FIXED );
    m_variableStatesPtr->clearPartition(Simplex::NONBASIC_FREE );
    for (; arrayIter != arrayIterEnd; arrayIter++) {
        if (NodeFile::Node::checkInteger(arrayIter->second) == false ) {
            throw PanOptException("Invalid index format at the basis head: " + arrayIter->second);
        }
        (*m_basisHeadPtr)[arrayIter->first] = atoi( arrayIter->second.c_str() );
        m_variableStatesPtr->insert(Simplex::BASIC, atoi( arrayIter->second.c_str() ), 0);
    }

    // read nonbasis part
    document.getNodes(NONBASIS_PART_STR, &nodeIter, &nodeIterEnd);
    const NodeFile::Node & nonbasisNode = *nodeIter;

    std::vector<NodeFile::Node>::const_iterator variableIter;
    std::vector<NodeFile::Node>::const_iterator variableIterEnd;
    nonbasisNode.getNodes(VARIABLE_STR, &variableIter, &variableIterEnd);

    unsigned int index = 0;
    for (; variableIter != variableIterEnd; variableIter++, index++) {
        std::string indexStr = variableIter->getValue(INDEX_STR);
        if (NodeFile::Node::checkInteger(indexStr) == false ) {
            throw PanOptException("Invalid nonbasis variable index format: " + indexStr);
        }
        unsigned int variableIndex = atoi( indexStr.c_str() );
        std::string stateStr = variableIter->getValue(STATE_STR);
        //m_variableStatesPtr->remove( variableIndex );
        if (stateStr == AT_LOWER_BOUND_STR) {
            //LPINFO("LOWER_BOUND " << variableIndex);
            //std::cin.get();
            m_variableStatesPtr->insert( Simplex::NONBASIC_AT_LB,
                                         variableIndex,
                                         &m_variables[variableIndex]->getLowerBound() );
        } else if (stateStr == AT_UPPER_BOUND_STR) {
            //LPINFO("UPPER_BOUND " << variableIndex);
            //std::cin.get();
            m_variableStatesPtr->insert( Simplex::NONBASIC_AT_UB,
                                         variableIndex,
                                         &m_variables[variableIndex]->getUpperBound() );
        } else if (stateStr == FREE_STR) {
            //LPINFO("FREE " << variableIndex);
            //std::cin.get();
            m_variableStatesPtr->insert( Simplex::NONBASIC_FREE,
                                         variableIndex, &ZERO );
        } else if (stateStr == FIXED_STR) {
            //LPINFO("FIXED " << variableIndex);
            //std::cin.get();
            m_variableStatesPtr->insert( Simplex::NONBASIC_FIXED,
                                         variableIndex, &ZERO );
        } else {
            throw PanOptException("Illegal variable state: " + stateStr);
        }

    }
    release();
}

void BasisHeadPanOpt::addBasicVariable(const Variable * variable,
                                       unsigned int basisPosition,
                                       unsigned int variableIndex,
                                       Numerical::Double value) {
    BasicVariableInfo info;
    info.m_basisIndex = basisPosition;
    info.m_isLogical = m_modelPtr->getMatrix().columnCount() <= variableIndex;
    info.m_value = value;
    info.m_variablePtr = variable;
    info.m_variableIndex = variableIndex;
    m_basicVariables.push_back(info);
}

void BasisHeadPanOpt::addNonbasicVariable(const Variable * variable,
                                          unsigned int variableIndex,
                                          Simplex::VARIABLE_STATE state,
                                          Numerical::Double value) {
    NonBasicVariableInfo info;
    info.m_isLogical = m_modelPtr->getMatrix().columnCount() <= variableIndex;
    info.m_state = state;
    info.m_value = value;
    info.m_variablePtr = variable;
    info.m_variableIndex = variableIndex;
    m_nonBasicVariables.push_back(info);
}

void BasisHeadPanOpt::addVariable(const Variable & variable) {
    m_variables.push_back(&variable);
}

void BasisHeadPanOpt::release() {
    if ( m_outputFile.is_open() ) {
        m_outputFile.close();
    }
    if ( m_inputFile.is_open() ) {
        m_inputFile.close();
    }
    m_modelPtr = 0;
    m_basicVariables.clear();
    m_nonBasicVariables.clear();
    m_variables.clear();
    m_info = "";
}
