#ifndef BASISHEADBAS_H
#define BASISHEADBAS_H

#include <globals.h>
#include <simplex/basisheadio.h>
#include <fstream>
#include <string>


class BasisHeadBAS: public BasisHeadIO {
public:
    BasisHeadBAS() {}
    ~BasisHeadBAS() {}

    virtual void startWrting(const char * fileName);

    void addInfo(const char * info);

    virtual void addBasicVariable(const Variable & variable,
                                  unsigned int basisPosition,
                                  Numerical::Double value);
    virtual void addNonbasicVariable(const Variable & variable,
                                     Simplex::VARIABLE_STATE state,
                                     Numerical::Double value);
    virtual void finishWriting();

private:
    std::ofstream m_file;

    struct BasicVariable {
        Variable m_variable;
        unsigned int m_basisPosition;
    };

    struct NonBasicVariable {
        Variable m_variable;
        Simplex::VARIABLE_STATE m_state;
    };

    std::vector<BasicVariable> m_basicVariables;

    std::vector<NonBasicVariable> m_nonBasicVariables;

    std::string m_info;
};

#endif // BASISHEADBAS_H
