/**
 * @file   dualdantzigpricingfactory.cpp
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

DualPricingUpdater * DualDantzigPricingFactory::createDualPricingUpdater(const Vector & basicVariableValues,
                                                                         IndexList<> * variableFeasibilities,
                                                                         const IndexList<> &reducedCostFeasibilities,
                                                                         const std::vector<int> &basisHead,
                                                                         const SimplexModel &simplexModel,
                                                                         const Basis &basis) const
{
    return new DualDantzigPricingUpdater(basicVariableValues, variableFeasibilities,
                                         reducedCostFeasibilities, basisHead, simplexModel,
                                         basis);
}

DualPricing * DualDantzigPricingFactory::createDualPricing(const SimplexModel & model,
                                                           DualPricingUpdater * updater,
                                                           const Vector& reducedCosts,
                                                           const std::vector<int>& basisHead) const
{
    return new DualDantzigPricing(model, updater, reducedCosts, basisHead);
}
