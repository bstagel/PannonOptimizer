/**
 * @file dualsimplex.cpp
 */

#include <simplex/dualsimplex.h>
#include <simplex/dualpricingfactory.h>
#include <simplex/pricing/dualdantzigpricingfactory.h>

DualSimplex::DualSimplex():
    m_pricing(0),
    m_updater(0),
    m_feasibilityChecker(0)
{

}

void DualSimplex::initModules() {
    Simplex::initModules();


    // TODO: ezt majd egy switch-case donti el, amit lehetne
    // kulon fuggvenybe is tenni akar
    DualPricingFactory * pricingFactory = new DualDantzigPricingFactory;

    m_updater = new DualUpdater;
    DualPricingUpdater * pricingUpdater = pricingFactory->createDualPricingUpdater(
                m_basicVariableValues,
                m_variableFeasibilities,
                m_reducedCostFeasibilities,
                m_basisHead,
                *m_simplexModel,
                *m_basis
                );
    m_updater->setPricingUpdater( pricingUpdater );

    m_pricing = pricingFactory->createDualPricing( *m_simplexModel, *pricingUpdater );
    m_updater->setPricingUpdater( pricingUpdater );

    m_feasibilityChecker=new DualFeasibilityChecker(*m_simplexModel,
                                                    m_variableStates,
                                                    &m_reducedCostFeasibilities,
                                                    m_reducedCosts,
                                                    &m_phaseIObjectiveValue);

    delete pricingFactory;
    pricingFactory = 0;
}

void DualSimplex::releaseModules() {
    Simplex::releaseModules();

    delete m_pricing;
    m_pricing = 0;

    delete m_updater;
    m_updater = 0;

    delete m_feasibilityChecker;
    m_feasibilityChecker = 0;
}


void DualSimplex::computeFeasibility() throw (NumericalException) {
    //meghivod
    m_feasibilityChecker->computeFeasibility();
}

void DualSimplex::checkFeasibility() throw (OptimizationResultException, NumericalException) {
    m_feasibilityChecker->checkFeasibility(m_reducedCostFeasibilities);
}

void DualSimplex::price() throw (OptimizationResultException, NumericalException) {

}

void DualSimplex::selectPivot() throw (OptimizationResultException, NumericalException) {

}

void DualSimplex::update()throw (NumericalException) {

}
