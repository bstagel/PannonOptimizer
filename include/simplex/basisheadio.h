#ifndef BASISHEADIO_H
#define BASISHEADIO_H

#include <globals.h>
#include <simplex/simplex.h>
#include <lp/variable.h>
#include <lp/constraint.h>
#include <lp/panoptstate.h>
#include <utils/indexlist.h>
#include <vector>

class BasisHeadIO: public PanOptState {
public:
    BasisHeadIO() {
        m_basisHeadPtr = 0;
        m_variableStatesPtr = 0;
    }
    virtual ~BasisHeadIO() {}

    virtual void addBasicVariable(const Variable * variable,
                                  unsigned int basisPosition,
                                  unsigned int variableIndex,
                                  Numerical::Double value) = 0;
    virtual void addNonbasicVariable(const Variable * variable,
                                     unsigned int variableIndex,
                                     Simplex::VARIABLE_STATE state,
                                     Numerical::Double value) = 0;

    virtual void addVariable(const Variable & variable) = 0;

    void setBasisHead(std::vector<int> * basisHead) {
        m_basisHeadPtr = basisHead;
    }

    void setVariableStateList(IndexList<const Numerical::Double*> * variableStates) {
        m_variableStatesPtr = variableStates;
    }

protected:
    std::vector<int> * m_basisHeadPtr;

    IndexList<const Numerical::Double*> * m_variableStatesPtr;
};

#endif // BASISHEADIO_H
