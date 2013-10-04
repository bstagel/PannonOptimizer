#include <simplex/dualupdater.h>

DualUpdater::DualUpdater():
    m_pricingUpdater(0),
    m_ratiotestUpdater(0)
{

}

void DualUpdater::setPricingUpdater(DualPricingUpdater * pricingUpdater) {
    m_pricingUpdater = pricingUpdater;
}

void DualUpdater::setRatiotestUpdater(DualRatiotestUpdater *ratiotestUpdater){
    m_ratiotestUpdater=ratiotestUpdater;
}

void DualUpdater::update() {

}
