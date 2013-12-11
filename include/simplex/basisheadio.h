#ifndef BASISHEADIO_H
#define BASISHEADIO_H

#include <globals.h>
#include <simplex/simplex.h>
#include <lp/variable.h>
#include <lp/constraint.h>
#include <utils/indexlist.h>
#include <vector>

class BasisHeadIO {
public:
    BasisHeadIO() {}
    virtual ~BasisHeadIO() {}

    virtual void startWrting(const char * fileName) = 0;

    virtual void addInfo(const char * info) = 0;

    virtual void addBasicVariable(const Variable * variable,
                                  unsigned int basisPosition,
                                  Numerical::Double value) = 0;
    virtual void addNonbasicVariable(const Variable * variable,
                                     Simplex::VARIABLE_STATE state,
                                     Numerical::Double value) = 0;
    virtual void finishWriting() = 0;

    virtual void startReading(const char * fileName,
                              unsigned int variableCount) = 0;

    virtual void addVariable(const Variable & variable) = 0;

    virtual void finishReading(std::vector<int> * basisHead,
                               IndexList<const Numerical::Double*> * variableStates) = 0;

};

#endif // BASISHEADIO_H
