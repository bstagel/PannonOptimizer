#include <simplex/dualupdater.h>

DualUpdater::DualUpdater():
    m_pricingUpdater(0)
{

}

void DualUpdater::setPricingUpdater(DualPricingUpdater * pricingUpdater) {
    m_pricingUpdater = pricingUpdater;
}

void DualUpdater::update() {

}
