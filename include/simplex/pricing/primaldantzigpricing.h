/**
 * @file   primaldantzigpricing.h
 */

#ifndef PRIMALDANTZIGPRICING_H
#define	PRIMALDANTZIGPRICING_H

#include <globals.h>

#include <simplex/primalpricing.h>
#include <simplex/pricing/primaldantzigpricingupdater.h>
#include <utils/exceptions.h>
#include <vector>

class PrimalDantzigPricing: public PrimalPricing
{
public:
    PrimalDantzigPricing(const SimplexModel & model,
                         PrimalPricingUpdater * updater,
                         const std::vector<int> & basisHead,
                         const Vector & reducedCosts);
    PrimalDantzigPricing(const PrimalDantzigPricing& orig);
    virtual ~PrimalDantzigPricing();

    int performPricingPhase1();
    int performPricingPhase2();

    void releaseUsed();
    void lockLastIndex();
private:

    int m_incomingIndex;

    Numerical::Double m_reducedCost;

    PrimalDantzigPricingUpdater * m_updater;

    const std::vector<int> & m_basisHead;

    std::vector<Numerical::Double> m_negativeSums;

    std::vector<Numerical::Double> m_positiveSums;

    std::vector<Numerical::Double> m_phase1ReducedCosts;

    void copy(const PrimalDantzigPricing & orig);

    void release();

    void initPhase1();

};

#endif	/* PRIMALDANTZIGPRICING_H */

