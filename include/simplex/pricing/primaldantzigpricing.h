/**
 * @file   primaldantzigpricing.h
 */

#ifndef PRIMALDANTZIGPRICING_H
#define	PRIMALDANTZIGPRICING_H

#include <globals.h>

#include <linalg/vector.h>
#include <simplex/primalpricing.h>
#include <utils/exceptions.h>
#include <utils/indexlist.h>
#include <vector>

class PrimalDantzigPricing: public PrimalPricing
{
public:
    PrimalDantzigPricing(const Vector & basicVariableValues,
                         const IndexList<> & basicVariableFeasibilities,
                         IndexList<> * reducedCostFeasibilities,
                         const IndexList<const Numerical::Double*> & variableStates,
                         const std::vector<int> & basisHead,
                         const SimplexModel & model,
                         const Basis & basis,
                         const Vector & reducedCosts);
    virtual ~PrimalDantzigPricing();

    int performPricingPhase1();
    int performPricingPhase2();

    void update(int incomingIndex,
                int outgoingIndex,
                const Vector * incomingAlpha,
                const Vector * pivotRow);

private:

    void release();

};

#endif	/* PRIMALDANTZIGPRICING_H */

