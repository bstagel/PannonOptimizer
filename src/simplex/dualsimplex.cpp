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
                                                    &m_variableStates,
                                                    &m_reducedCostFeasibilities,
                                                    m_reducedCosts,
                                                    *m_basis,
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
    bool lastFeasible = m_feasible;
    m_feasible = m_feasibilityChecker->checkFeasibility();
    if(lastFeasible == false && m_feasible == true){
        m_feasibilityChecker->feasiblityCorrection(&m_basicVariableValues);
    }
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
    unsigned int rowCount = m_simplexModel->getRowCount();
    unsigned int columCount = m_simplexModel->getColumnCount();

    Vector alpha(rowCount + columCount);
    computeTransformedRow(&alpha, m_outgoingIndex);

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
    LPINFO("transformed");
    LPINFO("reinvert");
    reinvert();
    LPINFO("computeBasicSolution");
    computeBasicSolution();
    LPINFO("computeReducedCosts");
    computeReducedCosts();
    LPINFO("computeFeasibility");
    computeFeasibility();

    //Do dual specific using the updater
//    m_updater->update();
}

void DualSimplex::computeTransformedRow(Vector* alpha, unsigned int rowIndex) throw(NumericalException) {

    unsigned int rowCount = m_simplexModel->getRowCount();
    unsigned int columnCount = m_simplexModel->getColumnCount();

    if (rowIndex > rowCount){
        LPERROR("Invalid row index!");
        throw NumericalException("Invalid row index, the transformed row cannot be computed");
    }

    //TODO:Nem lenne jobb konstruktorban megadni hogy dense?
    Vector rho(rowCount); //Row of the inverse of the basis
    rho.setSparsityRatio(DENSE);
    rho.setNewNonzero(rowIndex, 1);

    m_basis->Btran(rho);
    IndexList<const Numerical::Double *>::Iterator it;
    IndexList<const Numerical::Double *>::Iterator itEnd;
    m_variableStates.getIterators(&it, &itEnd, Simplex::NONBASIC_AT_LB, 3);
    for(; it != itEnd ; it++){
        unsigned int columnIndex = it.getData();
        if(columnIndex < columnCount){
            alpha->set(columnIndex, rho.dotProduct(m_simplexModel->getMatrix().column(columnIndex)));
        } else {
            alpha->set(columnIndex, rho.at(columnIndex - columnCount));
        }
    }
}
