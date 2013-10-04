#include <simplex/dualpricing.h>

DualPricing::DualPricing(const SimplexModel & model,
                         const DualPricingUpdater & updater):
    m_model(model),
    m_updater(updater)
{

}

DualPricing::~DualPricing() {

}

unsigned int DualPricing::performPricingPhase1() {
    return 0;
}

unsigned int DualPricing::performPricingPhase2() {
    return 0;
}
