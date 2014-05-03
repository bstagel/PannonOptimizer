/**
 * @file   dualdantzigpricing.h
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
                            const Vector & pivotRow);
private:


};

#endif	/* DUALDANTZIGPRICING_H */

