/**
 * @file dualpricing.cpp
 */

#include <simplex/dualpricing.h>

DualPricing::DualPricing(const SimplexModel & model):
    m_model(model)
{
    m_reducedCost = 0.0;
}

DualPricing::~DualPricing() {

}
