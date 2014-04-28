/**
 * @file dualupdater.cpp
 */

#include <simplex/dualupdater.h>

DualUpdater::DualUpdater():
    m_pricingUpdater(0)
{

}

DualUpdater::~DualUpdater() {
    if(m_pricingUpdater){
        delete m_pricingUpdater;
        m_pricingUpdater = 0;
    }
}

void DualUpdater::setPricingUpdater(DualPricingUpdater * pricingUpdater) {
    m_pricingUpdater = pricingUpdater;
}


void DualUpdater::update(int phase) {
    if (phase == 1) {
        m_pricingUpdater->updatePhase1();
    } else {
        m_pricingUpdater->updatePhase2();
    }
}
