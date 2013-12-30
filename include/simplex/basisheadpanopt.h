#ifndef BASISHEADPANOPT_H
#define BASISHEADPANOPT_H

#include <globals.h>
#include <simplex/basisheadio.h>
#include <fstream>
#include <vector>

class BasisHeadPanOpt: public BasisHeadIO {
public:
    BasisHeadPanOpt();

    virtual ~BasisHeadPanOpt() {}

    void startWrting(const char * fileName,
                     const Model & model);

    void finishWriting();

    void addInfo(const char * info);

    void startReading(const char * fileName,
                      const Model & model);

    void finishReading();

    void addBasicVariable(const Variable * variable,
                          unsigned int basisPosition,
                          unsigned int variableIndex,
                          Numerical::Double value);
    void addNonbasicVariable(const Variable * variable,
                             unsigned int variableIndex,
                             Simplex::VARIABLE_STATE state,
                             Numerical::Double value);

    void addVariable(const Variable & variable);

private:

    static const int sm_maxRowLength = 80;

    struct BasicVariableInfo {
        unsigned int m_basisIndex;
        unsigned int m_variableIndex;
        Numerical::Double m_value;
        const Variable * m_variablePtr;
        bool m_isLogical;
    };

    struct NonBasicVariableInfo {
        Simplex::VARIABLE_STATE m_state;
        unsigned int m_variableIndex;
        Numerical::Double m_value;
        const Variable * m_variablePtr;
        bool m_isLogical;
    };

    std::ofstream m_outputFile;

    std::ifstream m_inputFile;

    const Model * m_modelPtr;

    std::vector<BasicVariableInfo> m_basicVariables;

    std::vector<NonBasicVariableInfo> m_nonBasicVariables;

    std::vector<const Variable *> m_variables;

    std::string m_info;

    void writeGeneralInfo();

    void writeBasisHeadInfo();

    void writeNonBasisInfo();

    void writeString(const std::string & str);

    void release();
};

#endif // BASISHEADPANOPT_H
