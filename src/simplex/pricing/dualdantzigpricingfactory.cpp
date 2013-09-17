/* 
 * File:   dualdantzigpricingfactory.cpp
 * Author: smidla
 * 
 * Created on 2013. augusztus 26., 1:03
 */

#include <simplex/pricing/dualdantzigpricingfactory.h>
#include <simplex/pricing/dualdantzigpricing.h>
#include <simplex/pricing/dualdantzigpricingupdater.h>

DualDantzigPricingFactory::DualDantzigPricingFactory()
{
}

DualDantzigPricingFactory::DualDantzigPricingFactory(const DualDantzigPricingFactory& orig)
{
    __UNUSED(orig);
}

DualDantzigPricingFactory::~DualDantzigPricingFactory()
{
}

DualPricingUpdater * DualDantzigPricingFactory::createDualPricingUpdater() const
{
    return new DualDantzigPricingUpdater;
}

DualPricing * DualDantzigPricingFactory::createDualPricing() const
{
    return new DualDantzigPricing;
}
