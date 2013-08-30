/* 
 * File:   primaldantzigpricingfactory.cpp
 * Author: smidla
 * 
 * Created on 2013. augusztus 26., 1:02
 */

#include <simplex/pricing/primaldantzigpricingfactory.h>
#include <simplex/pricing/primaldantzigpricing.h>
#include <simplex/pricing/primaldantzigreducedcostupdater.h>

PrimalDantzigPricingFactory::PrimalDantzigPricingFactory()
{
}

PrimalDantzigPricingFactory::PrimalDantzigPricingFactory(const PrimalDantzigPricingFactory& orig)
{
}

PrimalDantzigPricingFactory::~PrimalDantzigPricingFactory()
{
}

PrimalReducedCostUpdater * PrimalDantzigPricingFactory::createPrimalReducedCostUpdater() const
{
    return new PrimalDantzigReducedCostUpdater;
}

PrimalPricing * PrimalDantzigPricingFactory::createPrimalPricing() const
{
    return new PrimalDantzigPricing;
}
