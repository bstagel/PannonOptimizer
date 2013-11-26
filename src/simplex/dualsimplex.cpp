/**
 * @file dualsimplex.cpp
 */

#include <algorithm>

#include <simplex/dualsimplex.h>
#include <simplex/dualpricingfactory.h>
#include <simplex/pricing/dualdantzigpricingfactory.h>
#include <simplex/pfibasis.h>

#include <utils/thirdparty/prettyprint.h>

const static char * INCOMING_NAME = "incoming";
const static char * OUTGOING_NAME = "outgoing";
const static char * PHASE_NAME = "phase";
const static char * PHASE_1_STRING = "phase-1";
const static char * PHASE_2_STRING = "phase-2";
const static char * PHASE_UNKNOWN_STRING = "unknown";
const static char * PHASE_1_OBJ_VAL_STRING = "dual infeasibility";
const static char * OBJ_VAL_STRING = "objective value";
const static char * PRIMAL_REDUCED_COST_STRING = "primal reduced cost";
const static char * PRIMAL_THETA_STRING = "primal theta";
const static char * DUAL_THETA_STRING = "dual theta";

DualSimplex::DualSimplex():
    m_pricing(0),
    m_updater(0),
    m_feasibilityChecker(0),
    m_ratiotest(0),
    m_feasible(false),
    m_phaseName(PHASE_UNKNOWN_STRING)
{

}

// Interface of the iteration report provider:
std::vector<IterationReportField> DualSimplex::getIterationReportFields(
        enum ITERATION_REPORT_FIELD_TYPE & type) const {
    std::vector<IterationReportField> result = Simplex::getIterationReportFields(type);

    switch (type) {
    case IterationReportProvider::IRF_START:
        break;

    case IterationReportProvider::IRF_ITERATION:
    {
        IterationReportField incomingField(INCOMING_NAME, 10, 2, IterationReportField::IRF_RIGHT,
                                    IterationReportField::IRF_INT, *this);
        result.push_back(incomingField);

        IterationReportField outgoingField(OUTGOING_NAME, 10, 2, IterationReportField::IRF_RIGHT,
                                    IterationReportField::IRF_INT, *this);
        result.push_back(outgoingField);

        IterationReportField primalReducedCostField(PRIMAL_REDUCED_COST_STRING, 20, 3, IterationReportField::IRF_RIGHT,
                                               IterationReportField::IRF_FLOAT, *this,
                                               -1, IterationReportField::IRF_FIXED);
        result.push_back(primalReducedCostField);

        IterationReportField primalThetaField(PRIMAL_THETA_STRING, 19, 3, IterationReportField::IRF_RIGHT,
                                               IterationReportField::IRF_FLOAT, *this,
                                               -1, IterationReportField::IRF_FIXED);
        result.push_back(primalThetaField);

        IterationReportField dualThetaField(DUAL_THETA_STRING, 19, 3, IterationReportField::IRF_RIGHT,
                                               IterationReportField::IRF_FLOAT, *this,
                                               -1, IterationReportField::IRF_FIXED);
        result.push_back(dualThetaField);

        IterationReportField phase1ObjValField(PHASE_1_OBJ_VAL_STRING, 19, 1, IterationReportField::IRF_RIGHT,
                                               IterationReportField::IRF_FLOAT, *this,
                                               10, IterationReportField::IRF_SCIENTIFIC);
        result.push_back(phase1ObjValField);

        IterationReportField objValField(OBJ_VAL_STRING, 19, 1, IterationReportField::IRF_RIGHT,
                                               IterationReportField::IRF_FLOAT, *this,
                                               10, IterationReportField::IRF_SCIENTIFIC);
        result.push_back(objValField);

        IterationReportField phaseField(PHASE_NAME, 7, 1, IterationReportField::IRF_RIGHT,
                                        IterationReportField::IRF_STRING, *this);
        result.push_back(phaseField);
    }
        break;

    case IterationReportProvider::IRF_SOLUTION:
        break;

    }

    return result;
}

ReportEntry DualSimplex::getIterationReportEntry(const string &name,
                                                         ITERATION_REPORT_FIELD_TYPE &type) const {
    ReportEntry reply;
    reply.m_integer = 0;

    switch (type) {
    case IterationReportProvider::IRF_START:
        break;

    case IterationReportProvider::IRF_ITERATION:
        if (name == PHASE_NAME) {
            reply.m_string = new std::string(m_phaseName);
        } else if (name == INCOMING_NAME) {
            reply.m_integer = m_incomingIndex;
        } else if (name == OUTGOING_NAME) {
            reply.m_integer = m_outgoingIndex;
        } else if (name == PHASE_1_OBJ_VAL_STRING) {
            reply.m_float = m_phaseIObjectiveValue;
        } else if (name == OBJ_VAL_STRING) {
            reply.m_float = m_objectiveValue;
        } else if (name == PRIMAL_REDUCED_COST_STRING) {
            reply.m_float = m_primalReducedCost;
        } else if (name == PRIMAL_THETA_STRING) {
            reply.m_float = m_primalTheta;
        } else if (name == DUAL_THETA_STRING) {
            reply.m_float = m_dualTheta;
        } else {
            break;
        }
        return reply;

    case IterationReportProvider::IRF_SOLUTION:
        break;

    }

    return Simplex::getIterationReportEntry(name, type);
}


void DualSimplex::initModules() {
    Simplex::initModules();


    // TODO: ezt majd egy switch-case donti el, amit lehetne
    // kulon fuggvenybe is tenni akar
    DualPricingFactory * pricingFactory = new DualDantzigPricingFactory;

    m_updater = new DualUpdater;

    DualPricingUpdater * pricingUpdater = pricingFactory->createDualPricingUpdater(
                m_basicVariableValues,
                &m_variableFeasibilities,
                m_reducedCostFeasibilities,
                m_basisHead,
                *m_simplexModel,
                *m_basis
                );
    m_updater->setPricingUpdater( pricingUpdater );
    m_pricing = pricingFactory->createDualPricing( *m_simplexModel,
                                                   *pricingUpdater,
                                                   m_reducedCosts,
                                                   m_basisHead);

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

    //registerIntoIterationReport(*this);
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
        m_phaseName = PHASE_1_STRING;
        //LPINFO("PHASE1 PRICE");
        m_outgoingIndex = m_pricing->performPricingPhase1();
        if(m_outgoingIndex == -1){
            throw DualInfeasibleException("The problem is DUAL INFEASIBLE!");
        }
    } else {
        m_phaseName = PHASE_2_STRING;
        //LPINFO("PHASE2 PRICE");
        m_outgoingIndex = m_pricing->performPricingPhase2();
        if(m_outgoingIndex == -1){
            throw OptimalException("OPTIMAL SOLUTION found!");
        }
    }
}

void DualSimplex::selectPivot() throw (OptimizationResultException, NumericalException) {
    unsigned int rowCount = m_simplexModel->getRowCount();
    unsigned int columCount = m_simplexModel->getColumnCount();

    Vector alpha(rowCount + columCount);
    computeTransformedRow(&alpha, m_outgoingIndex);


//    LPINFO("m_reducedCostFeasibilities: "<<m_reducedCostFeasibilities);
//    LPINFO("m_reducedCosts: "<<m_reducedCosts);
//    LPINFO("m_basisHead: "<<m_basisHead);
//    LPINFO("m_basicVariableValues: "<<m_basicVariableValues);
//    LPINFO("m_pricing->getReducedCost(): "<<m_pricing->getReducedCost());
//    LPINFO("transformedRow: "<<alpha);
//    LPINFO("PHASE I OBJ VAL: "<< setw(19) << setprecision(10) << scientific << m_phaseIObjectiveValue);
//    LPINFO("OBJ VAL:         "<< setw(19) << setprecision(10) << scientific << m_objectiveValue);

    if(!m_feasible){
        m_ratiotest->performRatiotestPhase1(m_basisHead[m_outgoingIndex], alpha, m_pricing->getReducedCost(), m_phaseIObjectiveValue);
    } else {
        m_ratiotest->performRatiotestPhase2(m_basisHead[m_outgoingIndex], alpha, m_objectiveValue);
    }
    m_incomingIndex = m_ratiotest->getIncomingVariableIndex();
}

void DualSimplex::update()throw (NumericalException) {

  //  LPINFO("incomingIndex: "<<m_incomingIndex);
  //  LPINFO("outgoingIndex: "<<m_outgoingIndex);
    if(m_incomingIndex != -1){
        //LPINFO("primalReducedCost: " << m_reducedCosts.at(m_incomingIndex));
        m_primalReducedCost = m_reducedCosts.at(m_incomingIndex);
    } else {
        m_primalReducedCost = std::numeric_limits<double>::signaling_NaN();
    }
    //LPINFO("primalTheta: "<<m_ratiotest->getPrimalSteplength());
    //LPINFO("dualTheta: "<<m_ratiotest->getDualSteplength());
    m_primalTheta = m_ratiotest->getPrimalSteplength();
    m_dualTheta = m_ratiotest->getDualSteplength();
//LPINFO("m_basicVariableValues: "<<m_basicVariableValues);
    transform(m_incomingIndex, m_outgoingIndex, m_ratiotest->getBoundflips(), m_ratiotest->getPrimalSteplength());
//    LPINFO("m_basicVariableValues: "<<m_basicVariableValues);
//    Transform
    //Update objective value

    //RECOMPUTE NOW
//    LPINFO("transformed");
//    LPINFO("reinvert");
//    reinvert();
//    LPINFO("computeBasicSolution");
    computeBasicSolution();

//    LPINFO("computeReducedCosts");
    computeReducedCosts();

//    LPINFO("computeFeasibility");
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
    //TODO: A bazisvaltozo egyeset kulon kellene majd bebillenteni hogy gyorsabb legyen
    m_variableStates.getIterators(&it, &itEnd, Simplex::BASIC, 4);
    for(; it != itEnd ; it++){
        unsigned int columnIndex = it.getData();
        if(columnIndex < columnCount){
            alpha->set(columnIndex, rho.dotProduct(m_simplexModel->getMatrix().column(columnIndex)));
        } else {
            alpha->set(columnIndex, rho.at(columnIndex - columnCount));
        }
    }
}
