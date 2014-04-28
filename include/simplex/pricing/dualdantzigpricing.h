/**
 * @file   dualdantzigpricing.h
 */

#ifndef DUALDANTZIGPRICING_H
#define	DUALDANTZIGPRICING_H

#include <globals.h>

#include <simplex/dualpricing.h>
#include <simplex/pricing/dualdantzigpricingupdater.h>
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
    DualDantzigPricing(const DualDantzigPricing& orig);
    virtual ~DualDantzigPricing();

    int performPricingPhase1();
    int performPricingPhase2();

    void releaseUsed();
    void lockLastIndex();
private:

    int m_phase2Index;

    Numerical::Double m_phase2ReducedCost;

    std::vector<char> m_used;

    Simpri m_phase1Simpri;

    Simpri m_phase2Simpri;

    Numerical::Double * m_phase1ReducedCosts;

    int m_outgoingIndex;

    //Parameter references
    const double & m_feasibilityTolerance;

    void clearPhase1ReducedCosts();

    void copy(const DualDantzigPricing & orig);

    void release();

    void initPhase1();
    void initPhase2();

};

#endif	/* DUALDANTZIGPRICING_H */

