/* 
 * File:   dualdantzigpricingfactory.cpp
 * Author: smidla
 * 
 * Created on 2013. augusztus 26., 1:03
 */

#include <simplex/pricing/dualdantzigpricingfactory.h>
#include <simplex/pricing/dualdantzigpricing.h>
#include <simplex/pricing/dualdantzigreducedcostupdater.h>

DualDantzigPricingFactory::DualDantzigPricingFactory()
{
}

DualDantzigPricingFactory::DualDantzigPricingFactory(const DualDantzigPricingFactory& orig)
{
}

DualDantzigPricingFactory::~DualDantzigPricingFactory()
{
}

DualReducedCostUpdater * DualDantzigPricingFactory::createDualReducedCostUpdater() const
{

}

DualPricing * DualDantzigPricingFactory::createDualPricing() const
{

}