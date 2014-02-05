
/**
 * @file primalpricing.cpp
 */

#include <simplex/primalpricing.h>

PrimalPricing::PrimalPricing(const SimplexModel & model,
                         PrimalPricingUpdater * updater):
    m_model(model),
    m_updater(updater)
{
    m_reducedCost = 0.0;
}

PrimalPricing::~PrimalPricing() {

}
