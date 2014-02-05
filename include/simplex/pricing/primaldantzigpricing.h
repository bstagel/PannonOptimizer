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
                         const std::vector<int> & basisHead);
    PrimalDantzigPricing(const PrimalDantzigPricing& orig);
    virtual ~PrimalDantzigPricing();

    int performPricingPhase1();
    int performPricingPhase2();

    void releaseUsed();
    void lockLastIndex();
private:

    PrimalDantzigPricingUpdater * m_updater;

    const std::vector<int> & m_basisHead;

    void copy(const PrimalDantzigPricing & orig);

    void release();

    void initPhase1();
    void initPhase2();

};

#endif	/* PRIMALDANTZIGPRICING_H */

