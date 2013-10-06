/* 
 * File:   dualdantzigpricing.h
 * Author: smidla
 *
 * Created on 2013. augusztus 26., 0:59
 */

#ifndef DUALDANTZIGPRICING_H
#define	DUALDANTZIGPRICING_H

#include <globals.h>
#include <simplex/dualpricing.h>
#include <simplex/pricing/dualdantzigpricingupdater.h>
#include <utils/exceptions.h>

class DualDantzigPricing: public DualPricing
{
public:
    DualDantzigPricing(const SimplexModel & model,
                       const DualPricingUpdater & updater);
    DualDantzigPricing(const DualDantzigPricing& orig);
    virtual ~DualDantzigPricing();

    // TODO: mi van, ha nem talal semmit elso fazisban?
    // nincs feasible megoldas, vagy unbounded?
    unsigned int performPricingPhase1();
    unsigned int performPricingPhase2() throw (OptimalException);
private:

    const DualDantzigPricingUpdater & m_updater;
};

#endif	/* DUALDANTZIGPRICING_H */

