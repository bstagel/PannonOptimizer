/**
 * @file dualpricing.cpp
 */

#include <simplex/dualpricing.h>

DualPricing::DualPricing(const SimplexModel & model,
                         DualPricingUpdater * updater):
    m_model(model),
    m_updater(updater)
{
    m_reducedCost = 0.0;
}

DualPricing::~DualPricing() {

}
