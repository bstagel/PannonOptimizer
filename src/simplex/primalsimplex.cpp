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
    Simplex::initModules();

    // TODO: ezt majd egy switch-case donti el, amit lehetne
    // kulon fuggvenybe is tenni akar
    PrimalPricingFactory * pricingFactory = new PrimalDantzigPricingFactory;

    m_pricing = pricingFactory->createPrimalPricing();

    m_updater = new PrimalUpdater;
    m_updater->setPricingUpdater( pricingFactory->createPrimalPricingUpdater() );

    delete pricingFactory;
    pricingFactory = 0;
}

void PrimalSimplex::releaseModules() {
    delete m_pricing;
    m_pricing = 0;

    delete m_updater;
    m_updater = 0;
}

void PrimalSimplex::computeFeasibility() throw (NumericalException) {

}

void PrimalSimplex::checkFeasibility() throw (OptimizationResultException, NumericalException) {

}

void PrimalSimplex::price() throw (OptimizationResultException, NumericalException) {

}

void PrimalSimplex::selectPivot() throw (OptimizationResultException, NumericalException) {

}

void PrimalSimplex::update()throw (NumericalException) {

}
