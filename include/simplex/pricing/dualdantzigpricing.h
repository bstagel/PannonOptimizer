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

class DualDantzigPricing: public DualPricing
{
public:
    DualDantzigPricing(const SimplexModel & model,
                       const DualPricingUpdater & updater);
    DualDantzigPricing(const DualDantzigPricing& orig);
    virtual ~DualDantzigPricing();
private:

};

#endif	/* DUALDANTZIGPRICING_H */

