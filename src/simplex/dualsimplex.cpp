/**
 * @file dualsimplex.cpp
 */

#include <simplex/dualsimplex.h>
#include <simplex/dualpricingfactory.h>
#include <simplex/pricing/dualdantzigpricingfactory.h>
#include <simplex/pfibasis.h>

DualSimplex::DualSimplex():
    m_pricing(0),
    m_updater(0),
    m_feasibilityChecker(0),
    m_ratiotest(0),
    m_feasible(false)
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

    m_feasibilityChecker=new DualFeasibilityChecker(*m_simplexModel,
                                                    m_variableStates,
                                                    &m_reducedCostFeasibilities,
                                                    m_reducedCosts,
                                                    &m_phaseIObjectiveValue);

    DualRatiotestUpdater * ratiotestUpdater = new DualRatiotestUpdater(&m_reducedCostFeasibilities);
    m_updater->setRatiotestUpdater( ratiotestUpdater );
    m_ratiotest = new DualRatiotest(*m_simplexModel,
                                    m_reducedCosts,
                                    m_reducedCostFeasibilities,
                                    m_variableStates,
                                    *ratiotestUpdater);

    delete pricingFactory;
    pricingFactory = 0;
}

void DualSimplex::releaseModules() {
    Simplex::releaseModules();

    if(m_pricing){
        delete m_pricing;
        m_pricing = 0;
    }

    if(m_updater){
        delete m_updater;
        m_updater = 0;
    }

    if(m_feasibilityChecker){
        delete m_feasibilityChecker;
        m_feasibilityChecker = 0;
    }

    if(m_ratiotest){
        delete m_ratiotest;
        m_ratiotest = 0;
    }
}


void DualSimplex::computeFeasibility() throw (NumericalException) {
    //meghivod
    m_feasibilityChecker->computeFeasibility();
}

void DualSimplex::checkFeasibility() throw (OptimizationResultException, NumericalException) {
    m_feasible = m_feasibilityChecker->checkFeasibility(m_reducedCostFeasibilities);
}

void DualSimplex::price() throw (OptimizationResultException, NumericalException) {
    if(!m_feasible){
        LPINFO("PHASE1 PRICE");
        m_outgoingIndex = m_pricing->performPricingPhase1();
    } else {
        LPINFO("PHASE2 PRICE");
        m_outgoingIndex = m_pricing->performPricingPhase2();
    }
}

void DualSimplex::selectPivot() throw (OptimizationResultException, NumericalException) {
    Vector alpha = m_simplexModel->getMatrix().row(m_outgoingIndex);
    unsigned int rowCount = m_simplexModel->getRowCount();
    unsigned int columCount = m_simplexModel->getColumnCount();
    alpha.resize(rowCount + columCount);
    alpha.set(columCount + m_outgoingIndex, 1);
    m_basis->Btran(alpha);
    if(!m_feasible){
        m_ratiotest->performRatiotestPhase1(m_basisHead[m_outgoingIndex], alpha, m_pricing->getReducedCost(), m_phaseIObjectiveValue);
    } else {
        m_ratiotest->performRatiotestPhase2(m_basisHead[m_outgoingIndex], alpha, m_objectiveValue);
    }
    m_incomingIndex = m_ratiotest->getIncomingVariableIndex();
}

void DualSimplex::update()throw (NumericalException) {
    transform(m_incomingIndex, m_outgoingIndex, m_ratiotest->getBoundflips(), m_ratiotest->getPrimalSteplength());
    //Transform
    //Update objective value

    //RECOMPUTE NOW
    reinvert();
    computeBasicSolution();
    computeReducedCosts();
    computeFeasibility();

    //Do dual specific using the updater
//    m_updater->update();
}
