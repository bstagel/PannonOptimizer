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

PrimalPricingUpdater * PrimalDantzigPricingFactory::createPrimalPricingUpdater(const Vector & basicVariableValues,
                                                                               const IndexList<> & basicVariableFeasibilities,
                                                                               IndexList<> *reducedCostFeasibilities,
                                                                               const IndexList<const Numerical::Double*> & variableStates,
                                                                               const std::vector<int> &basisHead,
                                                                               const SimplexModel &simplexModel,
                                                                               const Basis &basis) const
{
    return new PrimalDantzigPricingUpdater(basicVariableValues, basicVariableFeasibilities,
                                           reducedCostFeasibilities, variableStates, basisHead, simplexModel,
                                           basis);
}

PrimalPricing * PrimalDantzigPricingFactory::createPrimalPricing(const SimplexModel & model,
                                                                 PrimalPricingUpdater * updater,
                                                                 const std::vector<int>& basisHead) const
{
    return new PrimalDantzigPricing(model, updater, basisHead);
}
