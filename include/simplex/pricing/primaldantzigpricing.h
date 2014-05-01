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
    PrimalDantzigPricing(const PrimalDantzigPricing& orig);
    virtual ~PrimalDantzigPricing();

    int performPricingPhase1();
    int performPricingPhase2();

    void releaseUsed();
    void lockLastIndex();
private:

    int m_incomingIndex;

    std::vector<Numerical::Double> m_negativeSums;
    std::vector<Numerical::Double> m_positiveSums;
    std::vector<Numerical::Double> m_phase1ReducedCosts;

    //Parameter references
    const double & m_feasibilityTolerance;
    const double & m_optimalityTolerance;

    void copy(const PrimalDantzigPricing & orig);

    void release();

    void initPhase1();

};

#endif	/* PRIMALDANTZIGPRICING_H */

