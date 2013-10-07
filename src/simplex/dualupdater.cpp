#include <simplex/dualupdater.h>

DualUpdater::DualUpdater():
    m_pricingUpdater(0),
    m_ratiotestUpdater(0)
{

}

DualUpdater::~DualUpdater() {
    if(m_pricingUpdater){
        delete m_pricingUpdater;
        m_pricingUpdater = 0;
    }
    if(m_ratiotestUpdater){
        delete m_ratiotestUpdater;
        m_ratiotestUpdater = 0;
    }
}

void DualUpdater::setPricingUpdater(DualPricingUpdater * pricingUpdater) {
    m_pricingUpdater = pricingUpdater;
}

void DualUpdater::setRatiotestUpdater(DualRatiotestUpdater *ratiotestUpdater){
    m_ratiotestUpdater=ratiotestUpdater;
}

void DualUpdater::update() {

}
