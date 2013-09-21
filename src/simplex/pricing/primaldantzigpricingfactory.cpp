/* 
 * File:   primaldantzigpricingfactory.cpp
 * Author: smidla
 * 
 * Created on 2013. augusztus 26., 1:02
 */

#include <simplex/pricing/primaldantzigpricingfactory.h>
#include <simplex/pricing/primaldantzigpricing.h>
#include <simplex/pricing/primaldantzigpricingupdater.h>

PrimalDantzigPricingFactory::PrimalDantzigPricingFactory()
{
}

PrimalDantzigPricingFactory::PrimalDantzigPricingFactory(const PrimalDantzigPricingFactory& orig)
{
    __UNUSED(orig);
}

PrimalDantzigPricingFactory::~PrimalDantzigPricingFactory()
{
}

PrimalPricingUpdater * PrimalDantzigPricingFactory::createPrimalPricingUpdater() const
{
    return new PrimalPricingUpdater;
}

PrimalPricing * PrimalDantzigPricingFactory::createPrimalPricing() const
{
    return new PrimalDantzigPricing;
}
