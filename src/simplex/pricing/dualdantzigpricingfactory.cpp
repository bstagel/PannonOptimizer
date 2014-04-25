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
                                                                         IndexList<> * basicVariableFeasibilities,
                                                                         const IndexList<> &reducedCostFeasibilities,
                                                                         const std::vector<int> &basisHead,
                                                                         const SimplexModel &simplexModel,
                                                                         const Basis &basis) const
{
    return new DualDantzigPricingUpdater(basicVariableValues, basicVariableFeasibilities,
                                         reducedCostFeasibilities, basisHead, simplexModel,
                                         basis);
}

DualPricing * DualDantzigPricingFactory::createDualPricing(const SimplexModel & model,
                                                           DualPricingUpdater * updater,
                                                           const std::vector<int>& basisHead) const
{
    return 0; //return new DualDantzigPricing(model, updater, basisHead);
}
