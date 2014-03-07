/**
 * @file dualsimplex.cpp
 */

#include <algorithm>

#include <simplex/dualsimplex.h>
#include <simplex/dualpricingfactory.h>
#include <simplex/pricing/dualdantzigpricingfactory.h>
#include <simplex/pfibasis.h>
#include <simplex/simplexparameterhandler.h>
#include <simplex/checker.h>

#include <utils/thirdparty/prettyprint.h>

const static char * INCOMING_NAME = "Incoming";
const static char * OUTGOING_NAME = "Outgoing";
const static char * PHASE_NAME = "Phase";
const static char * PHASE_1_STRING = "ph-1";
const static char * PHASE_2_STRING = "ph-2";
const static char * PHASE_UNKNOWN_STRING = "unknown";
const static char * PHASE_1_OBJ_VAL_STRING = "Dual infeasibility";
const static char * OBJ_VAL_STRING = "Objective value";
const static char * PRIMAL_REDUCED_COST_STRING = "Reduced cost";
const static char * PRIMAL_THETA_STRING = "Primal theta";
const static char * DUAL_THETA_STRING = "Dual theta";

const static char * EXPORT_STABLE_PIVOT_ACTIVATION_PHASE1 = "export_stable_pivot_activation_phase1";
const static char * EXPORT_STABLE_PIVOT_BACKWARD_STEPS_PHASE1 = "export_stable_pivot_backward_steps_phase1";
const static char * EXPORT_STABLE_PIVOT_FORWARD_STEPS_PHASE1 = "export_stable_pivot_forward_steps_phase1";
const static char * EXPORT_FAKE_FEASIBILITY_ACTIVATION_PHASE1 = "export_fake_feasibility_activation_phase1";
const static char * EXPORT_FAKE_FEASIBILITY_COUNTER_PHASE1 = "export_fake_feasibility_counter_phase1";
const static char * EXPORT_STABLE_PIVOT_NOT_FOUND_PHASE2 = "export_stable_pivot_not_found_phase2";
const static char * EXPORT_FAKE_FEASIBILITY_ACTIVATION_PHASE2 = "export_fake_feasibility_activation_phase2";
const static char * EXPORT_FAKE_FEASIBILITY_COUNTER_PHASE2 = "export_fake_feasibility_counter_phase2";

DualSimplex::DualSimplex():
    m_pricing(0),
    m_updater(0),
    m_feasibilityChecker(0),
    m_ratiotest(0),
    m_phaseName(PHASE_UNKNOWN_STRING)
{
    m_workingTolerance = SimplexParameterHandler::getInstance().getDoubleParameterValue("e_optimality");
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
        result.push_back(IterationReportField (INCOMING_NAME, 10, 2, IterationReportField::IRF_RIGHT,
                                               IterationReportField::IRF_INT, *this));

        result.push_back(IterationReportField (OUTGOING_NAME, 10, 2, IterationReportField::IRF_RIGHT,
                                               IterationReportField::IRF_INT, *this));

        result.push_back(IterationReportField (PRIMAL_REDUCED_COST_STRING, 15, 3, IterationReportField::IRF_RIGHT,
                                               IterationReportField::IRF_FLOAT, *this,
                                               -1, IterationReportField::IRF_FIXED));

        result.push_back(IterationReportField (PRIMAL_THETA_STRING, 15, 3, IterationReportField::IRF_RIGHT,
                                               IterationReportField::IRF_FLOAT, *this,
                                               -1, IterationReportField::IRF_FIXED));

        result.push_back(IterationReportField (DUAL_THETA_STRING, 15, 3, IterationReportField::IRF_RIGHT,
                                               IterationReportField::IRF_FLOAT, *this,
                                               -1, IterationReportField::IRF_FIXED));

        result.push_back(IterationReportField (PHASE_1_OBJ_VAL_STRING, 20, 1, IterationReportField::IRF_RIGHT,
                                               IterationReportField::IRF_FLOAT, *this,
                                               10, IterationReportField::IRF_SCIENTIFIC));

        result.push_back(IterationReportField (OBJ_VAL_STRING, 20, 1, IterationReportField::IRF_RIGHT,
                                               IterationReportField::IRF_FLOAT, *this,
                                               10, IterationReportField::IRF_SCIENTIFIC));

        result.push_back(IterationReportField (PHASE_NAME, 6, 1, IterationReportField::IRF_RIGHT,
                                               IterationReportField::IRF_STRING, *this));
    }
        break;

    case IterationReportProvider::IRF_SOLUTION:
    {
        result.push_back(IterationReportField (OBJ_VAL_STRING, 20, 1, IterationReportField::IRF_RIGHT,
                                               IterationReportField::IRF_FLOAT, *this,
                                               10, IterationReportField::IRF_SCIENTIFIC));
        break;
    }


    case IterationReportProvider::IRF_EXPORT:
    {
        // Ratio test research set
        if (m_exportType == 0) {

        } else if (m_exportType == 1) {
            result.push_back(IterationReportField(EXPORT_STABLE_PIVOT_ACTIVATION_PHASE1,  20, 0, IterationReportField::IRF_RIGHT,
                                                  IterationReportField::IRF_INT, *this));
            result.push_back(IterationReportField(EXPORT_STABLE_PIVOT_BACKWARD_STEPS_PHASE1,  20, 0, IterationReportField::IRF_RIGHT,
                                                  IterationReportField::IRF_INT, *this));
            result.push_back(IterationReportField(EXPORT_STABLE_PIVOT_FORWARD_STEPS_PHASE1,  20, 0, IterationReportField::IRF_RIGHT,
                                                  IterationReportField::IRF_INT, *this));
            result.push_back(IterationReportField(EXPORT_FAKE_FEASIBILITY_ACTIVATION_PHASE1,  20, 0, IterationReportField::IRF_RIGHT,
                                                  IterationReportField::IRF_INT, *this));
            result.push_back(IterationReportField(EXPORT_FAKE_FEASIBILITY_COUNTER_PHASE1,  20, 0, IterationReportField::IRF_RIGHT,
                                                  IterationReportField::IRF_INT, *this));
            result.push_back(IterationReportField(EXPORT_STABLE_PIVOT_NOT_FOUND_PHASE2,  20, 0, IterationReportField::IRF_RIGHT,
                                                  IterationReportField::IRF_INT, *this));
            result.push_back(IterationReportField(EXPORT_FAKE_FEASIBILITY_ACTIVATION_PHASE2,  20, 0, IterationReportField::IRF_RIGHT,
                                                  IterationReportField::IRF_INT, *this));
            result.push_back(IterationReportField(EXPORT_FAKE_FEASIBILITY_COUNTER_PHASE2,  20, 0, IterationReportField::IRF_RIGHT,
                                                  IterationReportField::IRF_INT, *this));
        } else {
            throw ParameterException("Invalid export type specified in the parameter file!");
        }
    }

    default:
        break;
    }

    return result;
}

Entry DualSimplex::getIterationEntry(const string &name, ITERATION_REPORT_FIELD_TYPE &type) const {
    Entry reply;
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
            reply.m_double = m_phaseIObjectiveValue;
        } else if (name == OBJ_VAL_STRING) {
            if(m_simplexModel->getObjectiveType() == MINIMIZE){
                reply.m_double = m_objectiveValue;
            } else {
                reply.m_double = -m_objectiveValue;
            }
        } else if (name == PRIMAL_REDUCED_COST_STRING) {
            if(m_incomingIndex != -1){
                reply.m_double = m_primalReducedCost;
            } else {
                reply.m_double = 0;
            }
        } else if (name == PRIMAL_THETA_STRING) {
            reply.m_double = m_primalTheta;
        } else if (name == DUAL_THETA_STRING) {
            reply.m_double = m_dualTheta;
        } else {
            break;
        }
        return reply;

    case IterationReportProvider::IRF_SOLUTION:
        if (name == OBJ_VAL_STRING) {
            if(m_simplexModel->getObjectiveType() == MINIMIZE){
                reply.m_double = m_objectiveValue;
            } else {
                reply.m_double = -m_objectiveValue;
            }
        } else {
            break;
        }
        return reply;

    case IterationReportProvider::IRF_EXPORT:
        if (name == EXPORT_STABLE_PIVOT_ACTIVATION_PHASE1) {
            reply.m_integer = m_ratiotest->getStablePivotActivationPhase1();
        } else if (name == EXPORT_STABLE_PIVOT_BACKWARD_STEPS_PHASE1) {
            reply.m_integer = m_ratiotest->getStablePivotBackwardStepsPhase1();
        } else if (name == EXPORT_STABLE_PIVOT_FORWARD_STEPS_PHASE1) {
            reply.m_integer = m_ratiotest->getStablePivotForwardStepsPhase1();
        } else if (name == EXPORT_FAKE_FEASIBILITY_ACTIVATION_PHASE1) {
            reply.m_integer = m_ratiotest->getFakeFeasibilityActivationPhase1();
        } else if (name == EXPORT_FAKE_FEASIBILITY_COUNTER_PHASE1) {
            reply.m_integer = m_ratiotest->getFakeFeasibilityCounterPhase1();
        } else if (name == EXPORT_STABLE_PIVOT_NOT_FOUND_PHASE2) {
            reply.m_integer = m_ratiotest->getStablePivotNotFoundPhase2();
        } else if (name == EXPORT_FAKE_FEASIBILITY_ACTIVATION_PHASE2) {
            reply.m_integer = m_ratiotest->getFakeFeasibilityActivationPhase2();
        } else if (name == EXPORT_FAKE_FEASIBILITY_COUNTER_PHASE2) {
            reply.m_integer = m_ratiotest->getFakeFeasibilityCounterPhase2();
        } else {
            break;
        }
        return reply;

    default:
        break;
    }

    return Simplex::getIterationEntry(name, type);
}


void DualSimplex::initModules() {
    Simplex::initModules();

    // TODO: ezt majd egy switch-case donti el, amit lehetne
    // kulon fuggvenybe is tenni akar
    DualPricingFactory * pricingFactory = new DualDantzigPricingFactory;

    m_updater = new DualUpdater;

    DualPricingUpdater * pricingUpdater = pricingFactory->createDualPricingUpdater(
                m_basicVariableValues,
                &m_basicVariableFeasibilities,
                m_reducedCostFeasibilities,
                m_basisHead,
                *m_simplexModel,
                *m_basis
                );
    m_updater->setPricingUpdater( pricingUpdater );

    m_pricing = pricingFactory->createDualPricing(*m_simplexModel,
                                                  pricingUpdater,
                                                  m_basisHead);

    m_feasibilityChecker=new DualFeasibilityChecker(*m_simplexModel,
                                                    &m_variableStates,
                                                    &m_reducedCostFeasibilities,
                                                    m_reducedCosts,
                                                    *m_basis);

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


void DualSimplex::computeFeasibility() {
    m_feasibilityChecker->computeFeasibility(m_workingTolerance);
    m_phaseIObjectiveValue = m_feasibilityChecker->getPhaseIObjectiveValue();
    //In phase II check whether the basic variables are correct or not
    if(m_feasible){
        m_feasibilityChecker->feasibilityCorrection(&m_basicVariableValues);
    }
}

void DualSimplex::checkFeasibility() {
    bool lastFeasible = m_feasible;
    m_feasible = m_feasibilityChecker->checkFeasibility();
    if(lastFeasible == false && m_feasible == true){
        //Becomes feasible
        if(m_phase1Iteration == -1){
            m_phase1Iteration = m_iterationIndex;
            m_phase1Time = m_solveTimer.getCPURunningTime();
        }
        m_feasibilityChecker->feasibilityCorrection(&m_basicVariableValues);
        m_referenceObjective = m_objectiveValue;
    } else if(lastFeasible == true && m_feasible == false ){
        //Becomes infeasible
        m_fallbacks++;
    }
}

void DualSimplex::price() {
    if(!m_feasible){
        m_phaseName = PHASE_1_STRING;
        m_outgoingIndex = m_pricing->performPricingPhase1();
        if(m_outgoingIndex == -1){
            throw DualInfeasibleException("The problem is DUAL INFEASIBLE!");
        }
    } else {
        m_phaseName = PHASE_2_STRING;
        m_outgoingIndex = m_pricing->performPricingPhase2();
        if(m_outgoingIndex == -1){
//            LPINFO("OPTIMAL SOLUTION found!");
//            Checker::checkFeasibilityConditions(*this, true);
//            Checker::checkNonbasicVariableStates(*this, true);
//            Checker::checkBasicVariableStates(*this, true);
//            Checker::checkBasicVariableFeasibilities(*this);
            throw OptimalException("OPTIMAL SOLUTION found!");
        }
    }
}

void DualSimplex::selectPivot() {

    Vector alpha;

//    LPINFO("rc: "<<m_simplexModel->getRowCount());
//    LPINFO("cc: "<<m_simplexModel->getColumnCount());
//    Vector alpha151(m_simplexModel->getRowCount());
//    alpha151.set(151-143,1);
//    m_basis->Ftran(alpha151);
//    LPINFO("alpha151 "<<alpha151);
//    LPINFO("m_reducedCosts "<<m_reducedCosts);

//    if(m_iterationIndex>171){
//        for(int i=0; i<m_basicVariableValues.length(); i++){
//            std::cout<<"xbi: ("<<i <<","<<m_basicVariableValues.at(i)<<")"<<"\n";
//        }
//        Checker::checkFeasibilityConditions(*this, true);
//        Checker::checkNonbasicVariableStates(*this, true);
//        Checker::checkBasicVariableStates(*this, true);
//        Checker::checkBasicVariableFeasibilities(*this);
//        Checker::checkPfiWithFtran(*this);
//        Checker::checkPfiWithBtran(*this);
//        Checker::checkPfiWithReducedCost(*this);
//        LPINFO("FRESH "<<m_freshBasis);
//    }
    m_incomingIndex = -1;
    while(m_incomingIndex == -1 ){

        computeTransformedRow(&alpha, m_outgoingIndex);

        if(!m_feasible){
            m_ratiotest->performRatiotestPhase1(alpha, m_pricing->getReducedCost(), m_phaseIObjectiveValue);
        } else {
            m_ratiotest->performRatiotestPhase2(m_basisHead[m_outgoingIndex], alpha, m_objectiveValue);
        }
        m_incomingIndex = m_ratiotest->getIncomingVariableIndex();

        //If a boundflip is found, perform it
        if(m_feasible && !m_ratiotest->getBoundflips().empty()){
            break;
        }

        //Row disabling control
        if(m_incomingIndex == -1){
            //Ask for another row
#ifndef NDEBUG
            int thresholdReportLevel = SimplexParameterHandler::getInstance().getIntegerParameterValue("threshold_report_level");
            if(thresholdReportLevel > 0){
                LPERROR("Ask for another row, row is unstable: "<<m_outgoingIndex);
            }
#endif
            m_pricing->lockLastIndex();
            price();
        }
    }
    m_dualTheta = m_ratiotest->getDualSteplength();
    if(m_incomingIndex != -1){
        m_primalReducedCost = m_reducedCosts.at(m_incomingIndex);
    } else {
        m_primalReducedCost = 0;
    }

}

void DualSimplex::update() {
    unsigned int rowCount = m_simplexModel->getRowCount();
    unsigned int columnCount = m_simplexModel->getColumnCount();

    std::vector<unsigned int>::const_iterator it = m_ratiotest->getBoundflips().begin();
    std::vector<unsigned int>::const_iterator itend = m_ratiotest->getBoundflips().end();

    for(; it < itend; it++){
//        LPWARNING("BOUNDFLIPPING at: "<<*it);
        Vector alpha(rowCount);
        if(*it < columnCount){
            alpha = m_simplexModel->getMatrix().column(*it);
        } else {
            alpha.setNewNonzero(*it - columnCount, 1);
        }
        m_basis->Ftran(alpha);

        const Variable& variable = m_simplexModel->getVariable(*it);
        //Alpha is not available, since we are in the dual
        if(m_variableStates.where(*it) == Simplex::NONBASIC_AT_LB) {
            Numerical::Double boundDistance = variable.getUpperBound() - variable.getLowerBound();
            m_basicVariableValues.addVector(-1 * boundDistance, alpha, Numerical::ADD_ABS);
            m_variableStates.move(*it, Simplex::NONBASIC_AT_UB, &(variable.getUpperBound()));

        } else if(m_variableStates.where(*it) == Simplex::NONBASIC_AT_UB){
            Numerical::Double boundDistance = variable.getLowerBound() - variable.getUpperBound();
            m_basicVariableValues.addVector(-1 * boundDistance, alpha, Numerical::ADD_ABS);
            m_variableStates.move(*it, Simplex::NONBASIC_AT_LB, &(variable.getLowerBound()));
        } else {
            LPERROR("Boundflipping variable in the basis (or superbasic)!");
            //TODO Throw some exception here
        }
    }

    if(m_outgoingIndex != -1 && m_incomingIndex != -1){
        //Save whether the basis is to be changed
        m_baseChanged = true;

        Vector alpha(rowCount);
        if(m_incomingIndex < (int)columnCount){
            alpha = m_simplexModel->getMatrix().column(m_incomingIndex);
        } else {
            alpha.setNewNonzero(m_incomingIndex - columnCount, 1);
        }
        m_basis->Ftran(alpha);

        Simplex::VARIABLE_STATE outgoingState;

        if(!m_feasible){
            //Phase-I: Bounded variable leaves at lower bound (feasibility correction will handle it)
            outgoingState = NONBASIC_AT_LB;
            m_primalTheta = computePrimalTheta(alpha, m_outgoingIndex, &outgoingState);
        } else{
            if(m_feasible &&
                    (m_basicVariableValues.at(m_outgoingIndex) -
                    m_simplexModel->getVariable(m_basisHead[m_outgoingIndex]).getLowerBound()) < 0){
                //Phase-II: Bounded variable leaves at lower bound (comes from M)
                outgoingState = NONBASIC_AT_LB;
                m_primalTheta = computePrimalTheta(alpha, m_outgoingIndex, &outgoingState);
            }else{
                //Phase-II: Bounded variable leaves at upper bound (comes from P)
                outgoingState = NONBASIC_AT_UB;
                m_primalTheta = computePrimalTheta(alpha, m_outgoingIndex, &outgoingState);
            }
        }

//        Checker::checkPfiWithBtran(*this);

        m_basicVariableValues.addVector(-1 * m_primalTheta, alpha, Numerical::ADD_ABS);
        m_objectiveValue += m_primalReducedCost * m_primalTheta;

        //The incoming variable is NONBASIC thus the attached data gives the appropriate bound or zero
        m_basis->append(alpha, m_outgoingIndex, m_incomingIndex, outgoingState);

        m_basicVariableValues.set(m_outgoingIndex, *(m_variableStates.getAttachedData(m_incomingIndex)) + m_primalTheta);
        m_variableStates.move(m_incomingIndex, Simplex::BASIC, &(m_basicVariableValues.at(m_outgoingIndex)));

    }

    computeReducedCosts();

    //Do this only in phase one
    if(!m_feasible){
        computeFeasibility();
    }

    //Do dual specific using the updater
    m_updater->update(m_feasible ? 2 : 1);
}

void DualSimplex::setReferenceObjective() {
    if(!m_feasible){
        m_referenceObjective = m_phaseIObjectiveValue;
    } else {
        m_referenceObjective = m_objectiveValue;
    }
//                LPINFO("m_referenceObjective " <<m_referenceObjective);
}

void DualSimplex::checkReferenceObjective() {
    if(!m_feasible){
        if(m_referenceObjective > m_phaseIObjectiveValue){
            LPWARNING("BAD ITERATION - PHASE I");
            m_badIterations++;
        } else if(m_referenceObjective == m_phaseIObjectiveValue){
//            LPWARNING("DEGENERATE - PHASE I");
            m_degenerateIterations++;
        }
    } else {
        if(m_referenceObjective > m_objectiveValue ){
            LPWARNING("BAD ITERATION - PHASE II");
            m_badIterations++;
        } else if(m_referenceObjective == m_objectiveValue){
//            LPWARNING("DEGENERATE - PHASE II");
            m_degenerateIterations++;
        }
    }
}

void DualSimplex::initWorkingTolerance() {
    //initializing EXPAND tolerance
    m_masterTolerance = SimplexParameterHandler::getInstance().getDoubleParameterValue("e_optimality");
    if (SimplexParameterHandler::getInstance().getIntegerParameterValue("nonlinear_dual_phaseI_function") == 3) {
        m_workingTolerance = m_masterTolerance *
                SimplexParameterHandler::getInstance().getDoubleParameterValue("expand_multiplier_dphI");
        m_toleranceStep = (m_masterTolerance - m_workingTolerance) /
            SimplexParameterHandler::getInstance().getIntegerParameterValue("expand_divider_dphI");
    } else {
        m_workingTolerance = m_masterTolerance;
        m_toleranceStep = 0;
    }
}

void DualSimplex::computeWorkingTolerance() {
    //increment the EXPAND tolerance
    if (m_toleranceStep != 0)
    {
        m_workingTolerance += m_toleranceStep;
         //reset the EXPAND tolerance
        if (m_workingTolerance >= m_masterTolerance) {
            m_workingTolerance = m_masterTolerance *
                    SimplexParameterHandler::getInstance().getDoubleParameterValue("expand_multiplier_dphI");
        }
    }
}


void DualSimplex::releaseLocks(){
    m_pricing->releaseUsed();
}

void DualSimplex::computeTransformedRow(Vector* alpha, int rowIndex) {

    unsigned int rowCount = m_simplexModel->getRowCount();
    unsigned int columnCount = m_simplexModel->getColumnCount();

    if (rowIndex == -1 || rowIndex > (int)rowCount){
        throw NumericalException(std::string("Invalid row index, the transformed row cannot be computed"));
    }

    alpha->reInit(rowCount + columnCount);
    alpha->setSparsityRatio(DENSE);

    Vector rho(rowCount); //Row of the inverse of the basis
    rho.setSparsityRatio(DENSE);
    rho.setNewNonzero(rowIndex, 1);
    m_basis->Btran(rho);

//    alpha = new Vector(columnCount);
//    alpha->setSparsityRatio(DENSE);
//    Vector::NonzeroIterator it = rho.beginNonzero();
//    Vector::NonzeroIterator itend = rho.endNonzero();
//    for(; it != itend; it++){
//        alpha->addVector(*it,m_simplexModel->getMatrix().row(it.getIndex()));
//    }
//    alpha->appendVector(rho);

    IndexList<const Numerical::Double *>::Iterator it;
    IndexList<const Numerical::Double *>::Iterator itEnd;
    //TODO: A bazisvaltozo egyeset kulon kellene majd bebillenteni hogy gyorsabb legyen
    m_variableStates.getIterators(&it, &itEnd, 0, Simplex::VARIABLE_STATE_ENUM_LENGTH);
    for(; it != itEnd ; it++){
        unsigned int columnIndex = it.getData();
        if(columnIndex < columnCount){
            alpha->set(columnIndex, rho.dotProduct(m_simplexModel->getMatrix().column(columnIndex)));
        } else {
            alpha->set(columnIndex, rho.at(columnIndex - columnCount));
        }
    }

//    Vector alpha2(rowCount + columnCount);
//    m_variableStates.getIterators(&it, &itEnd, 1, Simplex::VARIABLE_STATE_ENUM_LENGTH-1);
//    for(; it != itEnd ; it++){
//        unsigned int columnIndex = it.getData();
//        if(columnIndex < columnCount){
//            alpha2.set(columnIndex, rho.dotProduct(m_simplexModel->getMatrix().column(columnIndex)));
//        } else {
//            alpha2.set(columnIndex, rho.at(columnIndex - columnCount));
//        }
//    }

//    for(int i=0; i<alpha->length(); i++){
//        if(alpha->at(i) != alpha2.at(i)){
//            LPERROR("alpha->at(i): "<<alpha->at(i)<< " - alpha2->at(i): "<<alpha->at(i));
//        }
//    }


//    LPINFO("rho: "<<rho);
//    LPINFO("alpha: "<<*alpha);
//    LPINFO("alpha2: "<<alpha2);

//    Numerical::Double x1,x2;
//    for(int i=0; i<alpha->length();i++){
//        if(alpha->at(i) != alpha2.at(i)){
//            checkAlphaValue(rowIndex,i,x1,x2);
//            LPINFO("ALPHA: "<<alpha->at(i)<<" - "<<alpha2.at(i) << " col: "<<x1);
//        }
//    }
}

Numerical::Double DualSimplex::computePrimalTheta(const Vector& alpha, int rowIndex, Simplex::VARIABLE_STATE* outgoingState){
    Variable::VARIABLE_TYPE typeOfIthVariable = m_simplexModel->getVariable(m_basisHead.at(rowIndex)).getType();
    Numerical::Double valueOfOutgoingVariable = m_basicVariableValues.at(rowIndex);

    if (typeOfIthVariable == Variable::FIXED) {
        *outgoingState = NONBASIC_FIXED;
        return (valueOfOutgoingVariable - m_simplexModel->getVariable(m_basisHead.at(rowIndex)).getLowerBound()) /
                alpha.at(rowIndex);
    }
    else if (typeOfIthVariable == Variable::BOUNDED) {
        if(*outgoingState == Simplex::NONBASIC_AT_UB){
            return (valueOfOutgoingVariable - m_simplexModel->getVariable(m_basisHead.at(rowIndex)).getUpperBound()) /
                    alpha.at(rowIndex);
        } else if(*outgoingState == Simplex::NONBASIC_AT_LB){
            return (valueOfOutgoingVariable - m_simplexModel->getVariable(m_basisHead.at(rowIndex)).getLowerBound()) /
                    alpha.at(rowIndex);
        } else {
            throw PanOptException("Outgoing variable state is not specified!");
        }
    }
    else if (typeOfIthVariable == Variable::PLUS) {
        *outgoingState = NONBASIC_AT_LB;
        return (valueOfOutgoingVariable - m_simplexModel->getVariable(m_basisHead.at(rowIndex)).getLowerBound()) /
                alpha.at(rowIndex);
    }
    else if (typeOfIthVariable == Variable::FREE) {
        *outgoingState = NONBASIC_FREE;
        return valueOfOutgoingVariable / alpha.at(rowIndex);
    }
    else if (typeOfIthVariable == Variable::MINUS) {
        *outgoingState = NONBASIC_AT_UB;
        return (valueOfOutgoingVariable - m_simplexModel->getVariable(m_basisHead.at(rowIndex)).getUpperBound()) /
                alpha.at(rowIndex);
    }
    return 0;
}
