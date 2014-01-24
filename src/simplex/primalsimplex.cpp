/**
 * @file primalsimplex.cpp
 */

#include <simplex/primalsimplex.h>
#include <simplex/primalpricingfactory.h>
#include <simplex/pricing/primaldantzigpricingfactory.h>

#include <simplex/simplexparameterhandler.h>

PrimalSimplex::PrimalSimplex():
    m_pricing(0),
    m_updater(0),
    m_feasibilityChecker(0),
    m_ratiotest(0)
{
    m_workingTolerance = SimplexParameterHandler::getInstance().getDoubleParameterValue("e_feasibility");
}

void PrimalSimplex::initModules() {
    Simplex::initModules();

    // TODO: ezt majd egy switch-case donti el, amit lehetne
    // kulon fuggvenybe is tenni akar
    PrimalPricingFactory * pricingFactory = new PrimalDantzigPricingFactory;

    m_feasibilityChecker = new PrimalFeasibilityChecker(*m_simplexModel,
                                                        &m_variableStates,
                                                        &m_variableFeasibilities,
                                                        &m_phaseIObjectiveValue);
    m_pricing = pricingFactory->createPrimalPricing();
    m_ratiotest = new PrimalRatiotest(*m_simplexModel,
                                      m_basicVariableValues,
                                      m_basisHead,
                                      m_variableFeasibilities,
                                      m_variableStates,
                                      m_reducedCosts);

    m_updater = new PrimalUpdater;
    m_updater->setPricingUpdater( pricingFactory->createPrimalPricingUpdater() );

    delete pricingFactory;
    pricingFactory = 0;
}

void PrimalSimplex::releaseModules() {
    Simplex::releaseModules();

    if (m_feasibilityChecker){
        delete m_feasibilityChecker;
        m_feasibilityChecker = 0;
    }
    if (m_pricing) {
        delete m_pricing;
        m_pricing = 0;
    }
    if (m_ratiotest) {
        delete m_ratiotest;
        m_ratiotest = 0;
    }
    if (m_updater) {
        delete m_updater;
        m_updater = 0;
    }
}

void PrimalSimplex::computeFeasibility() {
    m_feasibilityChecker->computeFeasibilities();
}

void PrimalSimplex::checkFeasibility() {

}

void PrimalSimplex::price() {

}

void PrimalSimplex::selectPivot() {

}

void PrimalSimplex::update() {
    //Call this with appropriate parameters:
//    void Simplex::transform(unsigned int incomingIndex,
//      unsigned int outgoingIndex,
//      const std::vector<unsigned int>& boundflips,
//      Numerical::Double reducedCost) {

    //Do dual specific using the updater
    m_updater->update();
}

void PrimalSimplex::setReferenceObjective() {

}

void PrimalSimplex::checkReferenceObjective() {

}

void PrimalSimplex::initWorkingTolerance() {

}

void PrimalSimplex::computeWorkingTolerance() {

}

void PrimalSimplex::releaseLocks() {

}
