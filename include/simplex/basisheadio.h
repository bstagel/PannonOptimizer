#ifndef BASISHEADIO_H
#define BASISHEADIO_H

#include <globals.h>
#include <simplex/simplex.h>
#include <lp/variable.h>
#include <lp/constraint.h>
#include <vector>

enum Simplex::VARIABLE_STATE;

class BasisHeadIO {
public:
    BasisHeadIO() {}
    virtual ~BasisHeadIO() {}

    virtual void startWrting(const char * fileName) = 0;

    virtual void addInfo(const char * info) = 0;

    virtual void addBasicVariable(const Variable & variable,
                                  unsigned int basisPosition,
                                  Numerical::Double value) = 0;
    virtual void addNonbasicVariable(const Variable & variable,
                                     Simplex::VARIABLE_STATE state,
                                     Numerical::Double value) = 0;
    virtual void finishWriting() = 0;

};

#endif // BASISHEADIO_H
