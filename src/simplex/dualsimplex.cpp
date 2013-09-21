/**
 * @file dualsimplex.cpp
 */

#include <simplex/dualsimplex.h>
#include <simplex/dualpricingfactory.h>
#include <simplex/pricing/dualdantzigpricingfactory.h>

DualSimplex::DualSimplex():
    m_pricing(0),
    m_updater(0)
{

}

void DualSimplex::initModules() {
    Simplex::initModules();

    // TODO: ezt majd egy switch-case donti el, amit lehetne
    // kulon fuggvenybe is tenni akar
    DualPricingFactory * pricingFactory = new DualDantzigPricingFactory;

    m_pricing = pricingFactory->createDualPricing();

    m_updater = new DualUpdater;
    m_updater->setPricingUpdater( pricingFactory->createDualPricingUpdater() );

    delete pricingFactory;
    pricingFactory = 0;
}

void DualSimplex::releaseModules() {
    delete m_pricing;
    m_pricing = 0;

    delete m_updater;
    m_updater = 0;
}

void DualSimplex::iterate() {

}
