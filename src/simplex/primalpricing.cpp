
/**
 * @file primalpricing.cpp
 */

#include <simplex/primalpricing.h>

PrimalPricing::PrimalPricing(const SimplexModel & model,
                             const Vector &reducedCosts):
    m_model(model),
    m_reducedCosts(reducedCosts)
{
    m_reducedCost = 0.0;
}

PrimalPricing::~PrimalPricing() {

}
