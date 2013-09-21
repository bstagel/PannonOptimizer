#include <simplex/primalupdater.h>

PrimalUpdater::PrimalUpdater():
    m_pricingUpdater(0)
{

}

void PrimalUpdater::setPricingUpdater(PrimalPricingUpdater * pricingUpdater) {
    m_pricingUpdater = pricingUpdater;
}

void PrimalUpdater::update() {

}
