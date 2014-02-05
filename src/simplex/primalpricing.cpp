
/**
 * @file primalpricing.cpp
 */

#include <simplex/primalpricing.h>

PrimalPricing::PrimalPricing(const SimplexModel & model):
    m_model(model)
{
    m_reducedCost = 0.0;
}

PrimalPricing::~PrimalPricing() {

}
