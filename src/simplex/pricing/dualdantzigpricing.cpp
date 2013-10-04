/* 
 * File:   dualdantzigpricing.cpp
 * Author: smidla
 * 
 * Created on 2013. augusztus 26., 0:59
 */

#include <simplex/pricing/dualdantzigpricing.h>

DualDantzigPricing::DualDantzigPricing(const SimplexModel & model,
                                       const DualPricingUpdater &updater):
    DualPricing(model, updater)
{
}

DualDantzigPricing::DualDantzigPricing(const DualDantzigPricing& orig):
    DualPricing(orig)
{
    __UNUSED(orig);
}

DualDantzigPricing::~DualDantzigPricing()
{
}

