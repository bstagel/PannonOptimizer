/**
 * @file dualsimplex.cpp
 */

#include <algorithm>

#include <simplex/dualsimplex.h>
#include <simplex/pricing/dualdantzigpricing.h>
#include <simplex/pricing/dualdevexpricing.h>
#include <simplex/pricing/dualsteepestedgepricing.h>
#include <simplex/pfibasis.h>
#include <simplex/simplexparameterhandler.h>
#include <simplex/checker.h>
#include <simplex/simplexcontroller.h>

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

DualSimplex::DualSimplex(SimplexController &simplexController):
    Simplex(simplexController),
    m_pricing(0),
    m_feasibilityChecker(0),
    m_ratiotest(0),
    m_phaseName(PHASE_UNKNOWN_STRING)
{
    m_masterTolerance = SimplexParameterHandler::getInstance().getDoubleParameterValue("Tolerances.e_optimality");
    m_toleranceMultiplier = SimplexParameterHandler::getInstance().getDoubleParameterValue("Ratiotest.Expand.multiplier");
    m_toleranceDivider = SimplexParameterHandler::getInstance().getIntegerParameterValue("Ratiotest.Expand.divider");
}

DualSimplex::~DualSimplex()
{
    releaseModules();
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
        if (m_exportType == "PARAMETER_STUDY") {

        } else if (m_exportType == "RATIOTEST_STUDY") {
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

    m_pricing = 0;
    m_pricingController = 0;
    std::string pricingType = SimplexParameterHandler::getInstance().getStringParameterValue("Pricing.type");
    if (pricingType == "DANTZIG") {
        m_pricing = new DualDantzigPricing (m_basicVariableValues,
                                            &m_basicVariableFeasibilities,
                                            m_reducedCostFeasibilities,
                                            m_basisHead,
                                            *m_simplexModel,
                                            *m_basis);
    }
    if (pricingType == "DEVEX") {
        m_pricing = new DualDevexPricing (m_basicVariableValues,
                                          &m_basicVariableFeasibilities,
                                          m_reducedCostFeasibilities,
                                          m_basisHead,
                                          *m_simplexModel,
                                          *m_basis);
    }
    if (pricingType == "STEEPEST_EDGE") {
        m_pricing = new DualSteepestEdgePricing (m_basicVariableValues,
                                                 &m_basicVariableFeasibilities,
                                                 m_reducedCostFeasibilities,
                                                 m_basisHead,
                                                 *m_simplexModel,
                                                 *m_basis);
    }
    /*case 10:
        m_pricingController = new DualPricingController (m_basicVariableValues,
                                                         &m_basicVariableFeasibilities,
                                                         m_reducedCostFeasibilities,
                                                         m_basisHead,
                                                         *m_simplexModel,
                                                         *m_basis);
        break;
    }*/

    Simplex::m_pricing = m_pricing;

    m_feasibilityChecker = new DualFeasibilityChecker(*m_simplexModel,
                                                      &m_variableStates,
                                                      &m_reducedCostFeasibilities,
                                                      m_reducedCosts,
                                                      *m_basis);

    m_ratiotest = new DualRatiotest(*m_simplexModel,
                                    m_reducedCosts,
                                    m_reducedCostFeasibilities,
                                    m_variableStates);

}

void DualSimplex::releaseModules() {
    Simplex::releaseModules();

    if(m_pricing){
        delete m_pricing;
        m_pricing = 0;
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
    m_lastFeasible = m_feasible;
    if(SimplexParameterHandler::getInstance().getStringParameterValue("Ratiotest.Expand.type") == "INACTIVE"){
        m_feasible = m_feasibilityChecker->computeFeasibility(m_masterTolerance);
    }else{
        m_feasible = m_feasibilityChecker->computeFeasibility(m_workingTolerance);
    }
    m_phaseIObjectiveValue = m_feasibilityChecker->getPhaseIObjectiveValue();
    //In phase II check whether the bounded variables are correct or not
    //Do the feasibility correction if we entered phase two
    if(m_feasible){
        if(SimplexParameterHandler::getInstance().getStringParameterValue("Ratiotest.Expand.type") == "INACTIVE"){
            m_feasibilityChecker->feasibilityCorrection(&m_basicVariableValues,m_masterTolerance);
        }else{
            m_feasibilityChecker->feasibilityCorrection(&m_basicVariableValues,m_workingTolerance);
        }
    }
    //Becomes feasible
    if(m_lastFeasible == false && m_feasible == true){
        if (SimplexParameterHandler::getInstance().getStringParameterValue("Global.switch_algorithm") == "SWITCH_WHEN_ENTER_PH2"){
            throw SwitchAlgorithmException("phase-2 entered!");
        }
        m_simplexController.logPhase1Iteration(m_solveTimer->getCPURunningTime());
    } else if(m_lastFeasible == true && m_feasible == false ){
        //Becomes infeasible, count the falback
        m_fallbacks++;
    }
}

void DualSimplex::price() {
    if(!m_feasible){
        m_phaseName = PHASE_1_STRING;
        m_outgoingIndex = m_pricing ? m_pricing->performPricingPhase1() : m_pricingController->performPricingPhase1() ;
        if(m_outgoingIndex == -1){
            throw DualInfeasibleException("The problem is DUAL INFEASIBLE!");
        }
    } else {
        m_phaseName = PHASE_2_STRING;
        m_outgoingIndex = m_pricing ? m_pricing->performPricingPhase2() : m_pricingController->performPricingPhase2();
        if(m_outgoingIndex == -1){
            throw OptimalException("OPTIMAL SOLUTION found!");
        }
    }
}

void DualSimplex::selectPivot() {

    m_incomingIndex = -1;
    while(m_incomingIndex == -1 ){
        computeTransformedRow(&m_pivotRow, m_outgoingIndex);

        if(!m_feasible){
            Numerical::Double reducedCost = m_pricing ? m_pricing->getReducedCost() : m_pricingController->getReducedCost();
            m_ratiotest->performRatiotestPhase1(m_pivotRow, reducedCost, m_phaseIObjectiveValue);
        } else {
            m_ratiotest->performRatiotestPhase2(m_basisHead[m_outgoingIndex], m_pivotRow, m_objectiveValue,m_workingTolerance);
        }
        m_incomingIndex = m_ratiotest->getIncomingVariableIndex();

        //If a boundflip is found, perform it
        if(m_feasible && !m_ratiotest->getBoundflips().empty()){
            break;
        }

        //Row disabling control
        if(m_incomingIndex == -1){
            //Ask for another row
            LPERROR("Ask for another row, row is unstable: "<<m_outgoingIndex);
            if (m_pricing) {
                m_pricing->lockLastIndex();
            } else {
                m_pricingController->lockLastIndex();
            }
            price();
        }
    }
    m_dualTheta = m_ratiotest->getDualSteplength();
    m_degenerate = m_ratiotest->isDegenerate();

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

    bool secondPhase = m_feasible;

    setReferenceObjective(secondPhase);

    //    Checker::checkBasicVariableFeasibilityStates(*this);

    //check whther the boundflips cause bad iterations
    //    int variableIndex = -1;
    //    for(; it < itend; it++){
    //        variableIndex = *it;
    //        if(m_variableStates.where(variableIndex) == Simplex::NONBASIC_AT_LB &&
    //                m_reducedCosts.at(variableIndex) > 0){
    //            LPERROR("Bad boundflip forseen. LB d: "<<m_reducedCosts.at(variableIndex));
    //            exit(-1);
    //        }
    //        if(m_variableStates.where(variableIndex) == Simplex::NONBASIC_AT_UB &&
    //                m_reducedCosts.at(variableIndex) < 0){
    //            LPERROR("Bad boundflip forseen. UB d: "<<m_reducedCosts.at(variableIndex));
    //            exit(-1);
    //        }
    //    }

    it = m_ratiotest->getBoundflips().begin();
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
            Numerical::Double referenceObjective = m_objectiveValue;
            m_objectiveValue += m_reducedCosts.at(*it) * boundDistance;
            if(referenceObjective > m_objectiveValue){
                LPERROR("Bad boundflip done! diff: " <<m_objectiveValue - referenceObjective);
            }
            m_variableStates.move(*it, Simplex::NONBASIC_AT_UB, &(variable.getUpperBound()));

        } else if(m_variableStates.where(*it) == Simplex::NONBASIC_AT_UB){
            Numerical::Double boundDistance = variable.getLowerBound() - variable.getUpperBound();
            m_basicVariableValues.addVector(-1 * boundDistance, alpha, Numerical::ADD_ABS);
            Numerical::Double referenceObjective = m_objectiveValue;
            m_objectiveValue += m_reducedCosts.at(*it) * boundDistance;
            if(referenceObjective > m_objectiveValue){
                LPERROR("Bad boundflip done! diff: " <<m_objectiveValue - referenceObjective);
            }
            m_variableStates.move(*it, Simplex::NONBASIC_AT_LB, &(variable.getLowerBound()));
        } else {
            throw PanOptException("Boundflipping variable in the basis (or superbasic)!");
        }
    }

    //Perform the basis change
    if(m_outgoingIndex != -1 && m_incomingIndex != -1){
        //Save whether the basis is to be changed
        m_baseChanged = true;

        //Compute the transformed column
        m_incomingAlpha.reInit(rowCount);
        if(m_incomingIndex < (int)columnCount){
            m_incomingAlpha = m_simplexModel->getMatrix().column(m_incomingIndex);
        } else {
            m_incomingAlpha.setNewNonzero(m_incomingIndex - columnCount, 1);
        }

        m_basis->Ftran(m_incomingAlpha);

        //Log the outgoing variable information
        Simplex::VARIABLE_STATE outgoingState;
        const Variable & outgoingVariable = m_simplexModel->getVariable(m_basisHead[m_outgoingIndex]);
        Variable::VARIABLE_TYPE outgoingType = outgoingVariable.getType();
        Numerical::Double outgoingVariableIndex= m_basisHead[m_outgoingIndex];
        Numerical::Double outgoingVariableValue = m_basicVariableValues.at(m_outgoingIndex);

//        LPINFO("outgoin var index: "<<outgoingVariableIndex);

        //Compute the outgoing state
        switch (outgoingType) {
        case Variable::FIXED:
            outgoingState = NONBASIC_FIXED;
            break;
        case Variable::BOUNDED:
            if(!m_feasible){
                //In Phase-I to maintain feasibility of the outgoing variable
                if(m_dualTheta < 0){
                    //The outgoing variable will have a reduced cost equal to -theta,
                    //which implies its state to satisfy its optimality condition
                    outgoingState = NONBASIC_AT_LB;
                } else {
                    outgoingState = NONBASIC_AT_UB;
                }
            } else {
                //In Phase-II
                const Variable & outgoingVariable = m_simplexModel->getVariable(m_basisHead[m_outgoingIndex]);
                if(m_basicVariableValues.at(m_outgoingIndex) < outgoingVariable.getLowerBound()){
                    //Phase-II: Bounded variable leaves at lower bound (comes from M)
                    outgoingState = NONBASIC_AT_LB;
                }else{
                    //Phase-II: Bounded variable leaves at upper bound (comes from P)
                    outgoingState = NONBASIC_AT_UB;
                }
            }
            break;
        case Variable::PLUS:
            outgoingState = NONBASIC_AT_LB;
            break;
        case Variable::FREE:
            outgoingState = NONBASIC_FREE;
            LPWARNING("FREE variable selected to leave basis!");
            break;
        case Variable::MINUS:
            outgoingState = NONBASIC_AT_UB;
            break;
        default:
            throw PanOptException("Invalid variable type");
        }


        //Compute the primal theta
        //Update the solution vector and the objective value
        Numerical::Double beta;
        switch(outgoingState){
        case NONBASIC_FIXED:
            beta = outgoingVariableValue - outgoingVariable.getLowerBound();
            break;
        case NONBASIC_AT_LB:
            beta = outgoingVariableValue - outgoingVariable.getLowerBound();
            break;
        case NONBASIC_AT_UB:
            beta = outgoingVariableValue - outgoingVariable.getUpperBound();
            break;
        case NONBASIC_FREE:
            LPWARNING("FREE variable is going to leave the basis!");
            beta = outgoingVariableValue;
        default:
            throw PanOptException("Invalid outgoing state!");
        }
        m_primalTheta = beta / m_incomingAlpha.at(m_outgoingIndex);

        //        LPINFO("Before going out("<<m_basisHead[m_outgoingIndex] <<")| value: "<<outgoingVariableValue<<" , LB: "<<outgoingVariable.getLowerBound()<<" , UB: "<<outgoingVariable.getUpperBound());


        m_basicVariableValues.addVector(-1 * m_primalTheta, m_incomingAlpha, Numerical::ADD_ABS);
        m_objectiveValue += beta * m_dualTheta;
        //Perform the basis change
        m_basis->append(m_incomingAlpha, m_outgoingIndex, m_incomingIndex, outgoingState);

        m_basicVariableValues.set(m_outgoingIndex, *(m_variableStates.getAttachedData(m_incomingIndex)) + m_primalTheta);
        m_variableStates.move(m_incomingIndex, Simplex::BASIC, &(m_basicVariableValues.at(m_outgoingIndex)));

        //        outgoingVariableValue = m_basicVariableValues.at(m_outgoingIndex);
        //        const Variable & incomingVariable = m_simplexModel->getVariable(m_basisHead[m_outgoingIndex]);
        //        LPINFO("After coming in ("<<m_basisHead[m_outgoingIndex] <<")| value: "<<outgoingVariableValue<<" , LB: "<<incomingVariable.getLowerBound()<<" , UB: "<<incomingVariable.getUpperBound());

        //        Checker::checkNonbasicVariableStates(*this);
        //        Checker::checkBasicVariableStates(*this);
        //        Checker::checkVariableStateAttachedValues(*this);

        //Update the pricing
        if (m_pricing) {
            m_pricing->update(m_incomingIndex, m_outgoingIndex,
                              m_incomingAlpha, m_pivotRow,
                              m_pivotRowOfBasisInverse);
        } else {
            m_pricingController->update(m_incomingIndex, m_outgoingIndex,
                                        m_incomingAlpha, m_pivotRow,
                                        m_pivotRowOfBasisInverse);
        }

        //Update feasibility of the basis change
        if(!m_feasible){
            m_reducedCostFeasibilities.remove(m_incomingIndex);

            //Bounded and fixed variables are left out from dual phase1
            //outgoing variable's reduced cost is always feasible
            if(outgoingType != Variable::BOUNDED && outgoingType != Variable::FIXED){
                m_reducedCostFeasibilities.insert(Simplex::FEASIBLE,outgoingVariableIndex);
            }
        }
    }



    //Update the reduced costs
    updateReducedCosts();

    //Update the feasibility sets in phase I
    if(!m_feasible){
        computeFeasibility();
//        m_feasibilityChecker->updateFeasibilities(m_ratiotest->getUpdateFeasibilitySets(),
//                                                  m_ratiotest->getBecomesFeasible());
//        Checker::checkFeasibilitySets(*this,true,m_workingTolerance);
    }

    checkReferenceObjective(secondPhase);
}

void DualSimplex::setReferenceObjective(bool secondPhase) {
    if(!secondPhase){
        m_referenceObjective = m_phaseIObjectiveValue;
    } else {
        m_referenceObjective = m_objectiveValue;
    }
}

void DualSimplex::checkReferenceObjective(bool secondPhase) {
    if(!secondPhase){
        if(Numerical::less(m_phaseIObjectiveValue,m_referenceObjective)){
            LPWARNING("BAD ITERATION - PHASE I difference: "<<m_referenceObjective-m_phaseIObjectiveValue);
            m_badIterations++;
        } else if(m_referenceObjective == m_phaseIObjectiveValue){
            m_degenerateIterations++;
        }
    } else {
        if(Numerical::less(m_objectiveValue,m_referenceObjective)){
            LPWARNING("BAD ITERATION - PHASE II difference: "<<m_referenceObjective-m_objectiveValue);
            m_badIterations++;
        } else if(m_referenceObjective == m_objectiveValue){
            m_degenerateIterations++;
        }
    }
}

void DualSimplex::initWorkingTolerance() {
    //initializing EXPAND tolerance
    if (SimplexParameterHandler::getInstance().getStringParameterValue("Ratiotest.Expand.type") != "INACTIVE" ) {
        m_workingTolerance = m_masterTolerance * m_toleranceMultiplier;
        m_toleranceStep = (m_masterTolerance - m_workingTolerance) / m_toleranceDivider;
    } else {
        m_workingTolerance = m_masterTolerance;
        m_toleranceStep = 0;
    }
}

void DualSimplex::computeWorkingTolerance() {
    //increment the EXPAND tolerance
    if (m_toleranceStep != 0) {
        m_workingTolerance += m_toleranceStep;
        //reset the EXPAND tolerance
        if (m_workingTolerance >= m_masterTolerance) {
            LPINFO("Resetting EXPAND tolerance!");
            m_workingTolerance = m_masterTolerance * m_toleranceMultiplier;
        }
    }
}


void DualSimplex::releaseLocks(){
    if (m_pricing) {
        m_pricing->releaseUsed();
    } else {
        m_pricingController->releaseUsed();
    }
}

void DualSimplex::computeTransformedRow(Vector* alpha, int rowIndex) {

    unsigned int rowCount = m_simplexModel->getRowCount();
    unsigned int columnCount = m_simplexModel->getColumnCount();

    if (rowIndex == -1 || rowIndex > (int)rowCount){
        throw NumericalException(std::string("Invalid row index, the transformed row cannot be computed"));
    }

    alpha->reInit(rowCount + columnCount);
    alpha->setSparsityRatio(DENSE);

    //Vector rho(rowCount); //Row of the inverse of the basis
    m_pivotRowOfBasisInverse.reInit(rowCount);
    m_pivotRowOfBasisInverse.setSparsityRatio(DENSE);
    m_pivotRowOfBasisInverse.setNewNonzero(rowIndex, 1);
    m_basis->Btran(m_pivotRowOfBasisInverse);

    /*Vector otherRow(rowCount);
    otherRow.setNewNonzero(rowIndex, 1.00001);
    m_basis->Btran(otherRow);
    Vector diff = otherRow;
    diff.addVector(-1, m_pivotRowOfBasisInverse);

    LPINFO("Btran: " << diff);
    LPINFO(diff.absMaxElement() << " " << otherRow.euclidNorm() << "  " << m_pivotRowOfBasisInverse.euclidNorm());
    cin.get();*/

    IndexList<const Numerical::Double *>::Iterator it;
    IndexList<const Numerical::Double *>::Iterator itEnd;

    // columnwise version
    m_variableStates.getIterators(&it, &itEnd, Simplex::NONBASIC_AT_LB, Simplex::VARIABLE_STATE_ENUM_LENGTH-1);
    for(; it != itEnd ; it++){
        unsigned int columnIndex = it.getData();
        if(columnIndex < columnCount){
            //LPINFO(m_pivotRowOfBasisInverse << "  " << m_simplexModel->getMatrix().column(columnIndex));
            //Numerical::Double dotProd = m_pivotRowOfBasisInverse.dotProduct(m_simplexModel->getMatrix().column(columnIndex));
            Numerical::Double dotProd = m_pivotRowOfBasisInverse.dotProduct(m_simplexModel->getMatrix().column(columnIndex));
            //Numerical::Double dotProd = m_simplexModel->getMatrix().column(columnIndex).dotProduct(m_pivotRowOfBasisInverse);
            alpha->set(columnIndex, dotProd);
            //std::cin.get();
        } else {
            alpha->set(columnIndex, m_pivotRowOfBasisInverse.at(columnIndex - columnCount));
        }
    }
    alpha->set( m_basisHead[ rowIndex ], 1.0 );

    /////////////////////////////////////////
    /*Vector otherAlpha(rowCount + columnCount);
    // columnwise version
    m_variableStates.getIterators(&it, &itEnd, Simplex::NONBASIC_AT_LB, Simplex::VARIABLE_STATE_ENUM_LENGTH-1);
    for(; it != itEnd ; it++){
        unsigned int columnIndex = it.getData();
        if(columnIndex < columnCount){
            //LPINFO(m_pivotRowOfBasisInverse << "  " << m_simplexModel->getMatrix().column(columnIndex));
            otherAlpha.set(columnIndex, otherRow.dotProduct(m_simplexModel->getMatrix().column(columnIndex)));
            //std::cin.get();
        } else {
            otherAlpha.set(columnIndex, otherRow.at(columnIndex - columnCount));
        }
    }
    otherAlpha.set( m_basisHead[ rowIndex ], 1.0 );
    LPINFO("other alpha: " << otherAlpha);*/
    /////////////////////////////////////////
    // --------------------------------

    // rowwise version
    /*std::vector<Numerical::Summarizer> results( rowCount + columnCount );
    Vector::NonzeroIterator pivotRowIter = m_pivotRowOfBasisInverse.beginNonzero();
    Vector::NonzeroIterator pivotRowIterEnd = m_pivotRowOfBasisInverse.endNonzero();
    for (; pivotRowIter != pivotRowIterEnd; pivotRowIter++) {
        const Numerical::Double lambda = *pivotRowIter;
        const Vector & row = m_simplexModel->getMatrix().row( pivotRowIter.getIndex() );
        // structural variables
        Vector::NonzeroIterator rowIter = row.beginNonzero();
        Vector::NonzeroIterator rowIterEnd = row.endNonzero();
        for (; rowIter != rowIterEnd; rowIter++) {
            const unsigned int index = rowIter.getIndex();
            if ( m_variableStates.where(index) == Simplex::BASIC ) {
                continue;
            }
            //m_reducedCosts.set(index, Numerical::stableAdd( m_reducedCosts.at( index ), - lambda * *rowIter));

            results[ index ].add( lambda * *rowIter );
            //LPWARNING( index << ".: " << results[index].getResult() );
        }
        const unsigned int index = pivotRowIter.getIndex() + columnCount;
        results[ index ].add( *pivotRowIter );

    }
    //alpha->clear();
    unsigned int index;
    for (index = 0; index < results.size(); index++) {
        Numerical::Double result = results[index].getResult();
        if (result != 0.0) {
            alpha->set(index, result);
        }
    }
    alpha->set( m_basisHead[ rowIndex ], 1.0 );*/


    /*{
        Vector a = *alpha;
    Vector ALFA = *alpha;
    alpha->clear();
    m_variableStates.getIterators(&it, &itEnd, Simplex::NONBASIC_AT_LB, Simplex::VARIABLE_STATE_ENUM_LENGTH-1);
        for(; it != itEnd ; it++){
            unsigned int columnIndex = it.getData();
            if(columnIndex < columnCount){
                alpha->set(columnIndex, m_pivotRowOfBasisInverse.dotProduct(m_simplexModel->getMatrix().column(columnIndex)));
            } else {
                alpha->set(columnIndex, m_pivotRowOfBasisInverse.at(columnIndex - columnCount));
            }
        }
        alpha->set( m_basisHead[ rowIndex ], 1.0 );

        ALFA.addVector(-1, *alpha);
        ALFA.setSparsityRatio(0.35);

        if (ALFA.nonZeros()) {
            Vector::NonzeroIterator iter = ALFA.beginNonzero();
            Vector::NonzeroIterator iterEnd = ALFA.endNonzero();
            for (; iter != iterEnd; iter++) {
                if (Numerical::fabs(*iter) > 0) {
                    LPERROR(ALFA);
                    break;
                }
            }
           // LPERROR(ALFA);
           // std::cin.get();
        }
        *alpha = a;
    }*/
}

void DualSimplex::updateReducedCosts() {

    //    Numerical::Double dualTheta = (m_reducedCosts.at( m_incomingIndex ) / m_incomingAlpha.at( m_outgoingIndex ) );
    /*if (Numerical::fabs(m_dualTheta - dualTheta) > 1e-14) {

        LPERROR( m_dualTheta << " vs " << dualTheta << "  "  << (m_dualTheta - dualTheta));
        std::cin.get();
    }*/
    //LPWARNING(dualTheta << " vs " << m_dualTheta << "  " << (m_dualTheta - dualTheta) << "   = " << (m_reducedCosts.at( m_incomingIndex )) <<
    //          " / " << m_incomingAlpha.at( m_outgoingIndex ) << "   " << m_pivotRow.at( m_incomingIndex ));

    m_reducedCosts.addVector( -m_dualTheta, m_pivotRow, Numerical::ADD_ABS_REL );
    m_reducedCosts.set( m_basisHead[ m_outgoingIndex ], -m_dualTheta );
    m_reducedCosts.set( m_incomingIndex, 0.0 );

}





