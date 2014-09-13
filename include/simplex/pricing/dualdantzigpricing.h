/**
 * @file   dualdantzigpricing.h This file contains the API of the DualDantzigPricing class.
 */

#ifndef DUALDANTZIGPRICING_H
#define	DUALDANTZIGPRICING_H

#include <globals.h>

#include <simplex/dualpricing.h>
#include <utils/exceptions.h>
#include <vector>

class DualDantzigPricing: public DualPricing
{
public:
    DualDantzigPricing(const Vector &basicVariableValues,
                       IndexList<> *basicVariableFeasibilities,
                       const IndexList<> &reducedCostFeasibilities,
                       const std::vector<int> & basisHead,
                       const SimplexModel & simplexModel,
                       const Basis &basis);
    virtual ~DualDantzigPricing();

    int performPricingPhase1();
    int performPricingPhase2();

    void update(int incomingIndex,
                int outgoingIndex,
                const Vector & incomingAlpha,
                const Vector & pivotRow,
                const Vector & pivotRowOfBasisInverse);

    void check() const {

    }
private:


};

#endif	/* DUALDANTZIGPRICING_H */

