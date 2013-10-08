/**
 * @file   primaldantzigpricingfactory.cpp
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
