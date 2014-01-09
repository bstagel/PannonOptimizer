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
    DualDantzigPricing(const SimplexModel & model,
                       DualPricingUpdater * updater,
                       const Vector& reducedCosts,
                       const std::vector<int> & basisHead);
    DualDantzigPricing(const DualDantzigPricing& orig);
    virtual ~DualDantzigPricing();

    int performPricingPhase1();
    int performPricingPhase2();

    void releaseUsed();
    void lockLastIndex();
private:

    DualDantzigPricingUpdater * m_updater;

    const Vector & m_reducedCosts;
    const std::vector<int> & m_basisHead;
    Numerical::Double * m_phase1ReducedCosts;

    int m_outgoingIndex;

    void clearPhase1ReducedCosts();

    void copy(const DualDantzigPricing & orig);

    void release();

    void initPhase1();
    void initPhase2();

};

#endif	/* DUALDANTZIGPRICING_H */

