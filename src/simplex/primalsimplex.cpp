/**
 * @file primalsimplex.cpp
 */

#include <simplex/primalsimplex.h>
#include <simplex/primalpricingfactory.h>
#include <simplex/pricing/primaldantzigpricingfactory.h>

PrimalSimplex::PrimalSimplex():
    m_pricing(0),
    m_updater(0)
{

}

void PrimalSimplex::initModules() {
    // TODO: ezt majd egy switch-case donti el, amit lehetne
    // kulon fuggvenybe is tenni akar
    PrimalPricingFactory * pricingFactory = new PrimalDantzigPricingFactory;

    m_pricing = pricingFactory->createPrimalPricing();

    m_updater = new PrimalUpdater;
    m_updater->setReducedCostUpdater( pricingFactory->createPrimalReducedCostUpdater() );

    delete pricingFactory;
    pricingFactory = 0;
}

void PrimalSimplex::releaseModules() {
    delete m_pricing;
    m_pricing = 0;

    delete m_updater;
    m_updater = 0;
}

void PrimalSimplex::iterate() {

}
