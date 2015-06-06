/**
 * @file dualsimplex.cpp
 * @author Peter Tar, Balint Stagel
 */

#include <algorithm>

#include <simplex/dualsimplex.h>
#include <simplex/pricing/dualdantzigpricing.h>
#include <simplex/pricing/dualdevexpricing.h>
#include <simplex/pricing/dualsteepestedgepricing.h>
#include <simplex/simplexcontroller.h>

#include <simplex/simplexparameterhandler.h>

const static char * INCOMING_NAME = "Incoming";
const static char * OUTGOING_NAME = "Outgoing";
const static char * PHASE_NAME = "Phase";
const static char * PHASE_1_STRING = "ph-1";
const static char * PHASE_2_STRING = "ph-2";
const static char * DUAL_INFEASIBILITY_STRING = "Dual infeasibility";
const static char * PRIMAL_INFEASIBILITY_STRING = "Primal infeasibility";
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
const static char * EXPORT_ASK_FOR_ANOTHER_ROW_COUNTER = "export_ask_for_another_row";

DualSimplex::DualSimplex(Basis *basis):
    Simplex(basis),
    m_pricing(nullptr),
    m_feasibilityChecker(nullptr),
    m_ratiotest(nullptr),
    m_askForAnotherRowCounter(0)
{
    //m_dualTheta.setDebugMode(true);
}

DualSimplex::~DualSimplex()
{
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

        result.push_back(IterationReportField (DUAL_INFEASIBILITY_STRING, 20, 1, IterationReportField::IRF_RIGHT,
                                               IterationReportField::IRF_FLOAT, *this,
                                               10, IterationReportField::IRF_SCIENTIFIC));

        result.push_back(IterationReportField (PRIMAL_INFEASIBILITY_STRING, 21, 1, IterationReportField::IRF_RIGHT,
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
            result.push_back(IterationReportField(EXPORT_ASK_FOR_ANOTHER_ROW_COUNTER,  20, 0, IterationReportField::IRF_RIGHT,
                                                  IterationReportField::IRF_INT, *this));
        } else if (m_exportType == "REVISION_CHECK") {

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

    switch (type) {
    case IterationReportProvider::IRF_START:
        break;

    case IterationReportProvider::IRF_ITERATION:
        if (name == PHASE_NAME) {
            reply.m_string = new std::string(m_feasibleIteration?PHASE_2_STRING:PHASE_1_STRING);
        } else if (name == INCOMING_NAME) {
            reply.m_integer = m_incomingIndex;
        } else if (name == OUTGOING_NAME) {
            reply.m_integer = m_outgoingIndex;
        } else if (name == DUAL_INFEASIBILITY_STRING) {
            reply.m_double = Numerical::DoubleToIEEEDouble(m_phaseIObjectiveValue);
        } else if (name == PRIMAL_INFEASIBILITY_STRING) {
            reply.m_double = Numerical::DoubleToIEEEDouble(m_pricing->getPrimalInfeasibility());
        } else if (name == OBJ_VAL_STRING) {
            if(m_simplexModel->getObjectiveType() == MINIMIZE){
                reply.m_double = Numerical::DoubleToIEEEDouble(m_objectiveValue);
            } else {
                reply.m_double = Numerical::DoubleToIEEEDouble(-m_objectiveValue);
            }
        } else if (name == PRIMAL_REDUCED_COST_STRING) {
            if(m_incomingIndex != -1){
                reply.m_double = Numerical::DoubleToIEEEDouble(m_primalReducedCost);
            } else {
                reply.m_double = 0;
            }
        } else if (name == PRIMAL_THETA_STRING) {
            reply.m_double = Numerical::DoubleToIEEEDouble(m_primalTheta);
        } else if (name == DUAL_THETA_STRING) {
            reply.m_double = Numerical::DoubleToIEEEDouble(m_dualTheta);
        } else {
            break;
        }
        return reply;

    case IterationReportProvider::IRF_SOLUTION:
        if (name == OBJ_VAL_STRING) {
            if(m_simplexModel->getObjectiveType() == MINIMIZE){
                reply.m_double = Numerical::DoubleToIEEEDouble(m_objectiveValue);
            } else {
                reply.m_double = Numerical::DoubleToIEEEDouble(-m_objectiveValue);
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
        } else if (name == EXPORT_ASK_FOR_ANOTHER_ROW_COUNTER) {
            reply.m_integer = m_askForAnotherRowCounter;
        } else {
            break;
        }
        return reply;

    default:
        break;
    }

    return Simplex::getIterationEntry(name, type);
}

void DualSimplex::computeFeasibility() {
    if(m_feasibilityChecker == nullptr){
        m_feasibilityChecker = new DualFeasibilityChecker(*m_simplexModel,
                                                          &m_variableStates,
                                                          &m_reducedCostFeasibilities,
                                                          m_reducedCosts,
                                                          *m_basis);
        m_masterTolerance = SimplexParameterHandler::getInstance().getDoubleParameterValue("Tolerances.e_optimality");
        m_toleranceMultiplier = SimplexParameterHandler::getInstance().getDoubleParameterValue("Ratiotest.Expand.multiplier");
        m_toleranceDivider = SimplexParameterHandler::getInstance().getIntegerParameterValue("Ratiotest.Expand.divider");

        initWorkingTolerance();
    }

    m_lastFeasible = m_feasible;
    m_feasible = m_feasibilityChecker->computeFeasibility(m_workingTolerance);

    m_phaseIObjectiveValue = m_feasibilityChecker->getPhaseIObjectiveValue();
    //In phase II check whether the bounded variables are correct or not
    //Do the feasibility correction if we entered phase two
    if(m_feasible){
        m_feasibilityChecker->feasibilityCorrection(&m_basicVariableValues,m_workingTolerance);
    }
    //Becomes feasible
    if(m_lastFeasible == false && m_feasible == true){
        if(m_phase1Iteration == -1){
            m_phase1Iteration = m_iterationIndex;
            m_phase1Time = SimplexController::getSolveTimer().getCPURunningTime();
        }
    } else if(m_lastFeasible == true && m_feasible == false ){
        //Becomes infeasible, count the falback
        m_fallbacks++;
    }
}

void DualSimplex::price() {
    if(m_pricing == nullptr){
        std::string pricingType = SimplexParameterHandler::getInstance().getStringParameterValue("Pricing.type");

        if (pricingType == "DANTZIG") {
            m_pricing = new DualDantzigPricing (m_basicVariableValues,
                                                &m_basicVariableFeasibilities,
                                                m_reducedCostFeasibilities,
                                                m_basisHead,
                                                *m_simplexModel,
                                                *m_basis,
                                                false); // TODO: kell-e shadow steepest edge vagy sem?

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

        Simplex::m_pricing = m_pricing;
    }

    if(!m_feasible){
        m_outgoingIndex = m_pricing->performPricingPhase1();
        if(m_outgoingIndex == -1){
            throw DualInfeasibleException("The problem is DUAL INFEASIBLE!");
        }
    } else {
        m_outgoingIndex = m_pricing->performPricingPhase2();
        if(m_outgoingIndex == -1){
            if(m_pricing->hasLockedVariable()){
                throw DualUnboundedException("The problem is DUAL UNBOUNDED!");
            }else{
                throw OptimalException("OPTIMAL SOLUTION found!");
            }
        }
    }
}

void DualSimplex::selectPivot() {
    if(m_ratiotest == nullptr){
        m_ratiotest = new DualRatiotest(*m_simplexModel,
                                        m_reducedCosts,
                                        m_reducedCostFeasibilities,
                                        m_variableStates);
    }

    m_incomingIndex = -1;
    while(m_incomingIndex == -1 ){
        computeTransformedRow();
        if(!m_feasible){
            Numerical::Double reducedCost = m_pricing->getReducedCost();
            m_ratiotest->performRatiotestPhase1(m_pivotRow, reducedCost, m_phaseIObjectiveValue);
        } else {
            m_ratiotest->performRatiotestPhase2(m_basisHead[m_outgoingIndex], m_pivotRow, m_workingTolerance);
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
            m_askForAnotherRowCounter ++;
            m_pricing->lockLastIndex();
            price();
        }
    }
    m_dualTheta = m_ratiotest->getDualSteplength();
    if(m_degenerate){
        m_degenerate = m_ratiotest->isDegenerate();
    }
    if(m_ratiotest->isDegenerate()){
        m_degenerateIterations++;
    }

    if(m_incomingIndex != -1){
        m_primalReducedCost = m_reducedCosts.at(m_incomingIndex);
    } else {
        m_primalReducedCost = 0;
    }

}

void DualSimplex::update() {
    unsigned int rowCount = m_simplexModel->getRowCount();
    unsigned int columnCount = m_simplexModel->getColumnCount();
    bool secondPhase = m_feasible;
    setReferenceObjective(secondPhase);
    if (!m_ratiotest->isWolfeActive()) {
        std::vector<unsigned int>::const_iterator it = m_ratiotest->getBoundflips().begin();
        std::vector<unsigned int>::const_iterator itend = m_ratiotest->getBoundflips().end();

        for(; it < itend; ++it){
            //                LPWARNING("BOUNDFLIPPING at: "<<*it);
            DenseVector alpha(rowCount);
            if(*it < columnCount){
                alpha = m_simplexModel->getMatrix().column(*it);
            } else {
                alpha.set(*it - columnCount, 1);
            }

            m_basis->Ftran(alpha);

            const Variable& variable = m_simplexModel->getVariable(*it);
            //Alpha is not available, since we are in the dual
            if(m_variableStates.where(*it) == (int)Simplex::NONBASIC_AT_LB) {
                Numerical::Double boundDistance = variable.getUpperBound() - variable.getLowerBound();
                m_basicVariableValues.addVector(-1 * boundDistance, alpha);
                m_objectiveValue += m_reducedCosts.at(*it) * boundDistance;
                m_variableStates.move(*it, Simplex::NONBASIC_AT_UB, &(variable.getUpperBound()));

            } else if(m_variableStates.where(*it) == (int)Simplex::NONBASIC_AT_UB){
                Numerical::Double boundDistance = variable.getLowerBound() - variable.getUpperBound();
                m_basicVariableValues.addVector(-1 * boundDistance, alpha);
                m_objectiveValue += m_reducedCosts.at(*it) * boundDistance;
                m_variableStates.move(*it, Simplex::NONBASIC_AT_LB, &(variable.getLowerBound()));
            } else {
                throw PanOptException("Boundflipping variable in the basis (or superbasic)!");
            }
        }
    }

    //Perform the basis change
    if(m_outgoingIndex != -1 && m_incomingIndex != -1){

        //Compute the transformed column
        m_pivotColumn.reInit(rowCount);

        if(m_incomingIndex < (int)columnCount){
            m_pivotColumn = m_simplexModel->getMatrix().column(m_incomingIndex);
        } else {
            m_pivotColumn.set(m_incomingIndex - columnCount, 1);
        }

        m_basis->Ftran(m_pivotColumn);
        detectExcessivelyInstability();

        //Log the outgoing variable information
        Simplex::VARIABLE_STATE outgoingState;
        const Variable & outgoingVariable = m_simplexModel->getVariable(m_basisHead[m_outgoingIndex]);
        Variable::VARIABLE_TYPE outgoingType = outgoingVariable.getType();
        Numerical::Double outgoingVariableIndex= m_basisHead[m_outgoingIndex];
        Numerical::Double outgoingVariableValue = m_basicVariableValues.at(m_outgoingIndex);

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

        m_primalTheta = beta / m_pivotColumn.at(m_outgoingIndex);

        m_basicVariableValues.addVector(-1 * m_primalTheta, m_pivotColumn);

        m_objectiveValue += beta * m_dualTheta;


        //Do some updates before the basis change
        //Update the pricing

        m_pricing->update(m_incomingIndex, m_outgoingIndex,
                          m_pivotColumn, m_pivotRow,
                          m_pivotRowOfBasisInverse);


        //Update the reduced costs
        if (m_ratiotest->isWolfeActive()) {
            wolfeSpecialUpdate();
        } else {
            updateReducedCosts();
        }
        //Perform the basis change
        SparseVector scattered;
        scattered = m_pivotColumn;
        m_basis->append(scattered, m_outgoingIndex, m_incomingIndex, outgoingState);

        if (m_pricing) {
            m_pricing->checkAndFix();
        }
        m_basicVariableValues.set(m_outgoingIndex, *(m_variableStates.getAttachedData(m_incomingIndex)) + m_primalTheta);
        m_variableStates.move(m_incomingIndex, Simplex::BASIC, &(m_basicVariableValues.at(m_outgoingIndex)));

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

    //Update the feasibility sets in phase I
    if(!m_feasible){
        computeFeasibility();
    }

    checkReferenceObjective(secondPhase);
}

void DualSimplex::wolfeSpecialUpdate()
{
    IndexList<>& degenerateAtLB = m_ratiotest->getDegenerateAtLB();
    IndexList<>& degenerateAtUB = m_ratiotest->getDegenerateAtUB();
    IndexList<>::PartitionIterator it;
    IndexList<>::PartitionIterator endit;
    //update D_Lb
    degenerateAtLB.getIterators(&it,&endit,m_ratiotest->getDegenDepth());
    for (; it != endit; ++it) {
        int variableIndex = it.getData();
        if (variableIndex != m_incomingIndex) {
            m_reducedCosts.set(variableIndex, m_reducedCosts.at(variableIndex) - m_dualTheta * m_pivotRow.at(variableIndex));
        }
    }
    //update D_Ub
    degenerateAtUB.getIterators(&it,&endit,m_ratiotest->getDegenDepth());
    for (; it != endit; ++it) {
        int variableIndex = it.getData();
        if (variableIndex != m_incomingIndex) {
            m_reducedCosts.set(variableIndex, m_reducedCosts.at(variableIndex) - m_dualTheta * m_pivotRow.at(variableIndex));
        }
    }

    if (degenerateAtLB.where(m_incomingIndex) < degenerateAtLB.getPartitionCount()) {
        degenerateAtLB.remove(m_incomingIndex);
    } else if (degenerateAtUB.where(m_incomingIndex) < degenerateAtUB.getPartitionCount()) {
        degenerateAtUB.remove(m_incomingIndex);
    } else {
        LPINFO("Index is in neither of the D sets");
        LPINFO("m_incomingIndex "<<m_incomingIndex);
        LPINFO("theta_d "<<m_dualTheta);
        LPINFO("dj: "<<m_reducedCosts[m_incomingIndex]);
//        LPINFO(degenerateAtLB);
//        LPINFO(degenerateAtUB);
        exit(-1);
    }
    const Variable & var = m_simplexModel->getVariable(m_incomingIndex);
    if (var.getType() != Variable::FIXED && Numerical::fabs(m_pivotRow.at(m_incomingIndex)) > m_pivotTolerance) {
        if ( m_variableStates.where(m_incomingIndex) == Simplex::NONBASIC_AT_LB) {
            degenerateAtLB.insert(m_ratiotest->getDegenDepth(), m_incomingIndex);
        } else if (m_variableStates.where(m_incomingIndex) == Simplex::NONBASIC_AT_UB) {
            degenerateAtUB.insert(m_ratiotest->getDegenDepth(), m_incomingIndex);
        }
    }
    m_reducedCosts.set( m_basisHead[ m_outgoingIndex ], -m_dualTheta );
    m_reducedCosts.set( m_incomingIndex, 0.0 );
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
        }
    } else {
        if(Numerical::less(m_objectiveValue,m_referenceObjective)){
            LPWARNING("BAD ITERATION - PHASE II difference: "<<m_referenceObjective-m_objectiveValue);
            m_badIterations++;
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
        if (m_workingTolerance >= m_masterTolerance) {
            resetTolerances();
        }
    }
}


void DualSimplex::releaseLocks(){
    if(m_pricing != nullptr){
        m_pricing->releaseUsed();
    }
}

void DualSimplex::computeTransformedRow() {

    unsigned int rowCount = m_simplexModel->getRowCount();
    unsigned int columnCount = m_simplexModel->getColumnCount();

    m_pivotRow.reInit(rowCount + columnCount);

    if (m_pivotRowOfBasisInverse.length() != rowCount) {
        m_pivotRowOfBasisInverse.reInit(rowCount);
    } else {
        m_pivotRowOfBasisInverse.clear();
    }
    m_pivotRowOfBasisInverse.set(m_outgoingIndex, 1);

    m_basis->Btran(m_pivotRowOfBasisInverse);

    /*Vector otherRow(rowCount);
    otherRow.setNewNonzero(m_outgoingIndex, 1.00001);
    m_basis->Btran(otherRow);
    Vector diff = otherRow;
    diff.addVector(-1, m_pivotRowOfBasisInverse);

    LPINFO("Btran: " << diff);
    LPINFO(diff.absMaxElement() << " " << otherRow.euclidNorm() << "  " << m_pivotRowOfBasisInverse.euclidNorm());
    cin.get();*/

    //    IndexList<const Numerical::Double *>::Iterator it;
    //    IndexList<const Numerical::Double *>::Iterator itEnd;

    //    // columnwise version
    //    m_variableStates.getIterators(&it, &itEnd, Simplex::NONBASIC_AT_LB, Simplex::VARIABLE_STATE_ENUM_LENGTH-1);
    //    for(; it != itEnd ; ++it){
    //        unsigned int columnIndex = it.getData();
    //        if(columnIndex < columnCount){
    //            //LPINFO(m_pivotRowOfBasisInverse << "  " << m_simplexModel->getMatrix().column(columnIndex));
    //            //Numerical::Double dotProd = m_pivotRowOfBasisInverse.dotProduct(m_simplexModel->getMatrix().column(columnIndex));
    //            Numerical::Double dotProd = m_pivotRowOfBasisInverse.dotProduct(m_simplexModel->getMatrix().column(columnIndex));
    //            //Numerical::Double dotProd = m_simplexModel->getMatrix().column(columnIndex).dotProduct(m_pivotRowOfBasisInverse);
    //            m_pivotRow.set(columnIndex, dotProd);
    //        } else {
    //            m_pivotRow.set(columnIndex, m_pivotRowOfBasisInverse.at(columnIndex - columnCount));
    //        }
    //    }
    //    m_pivotRow.set( m_basisHead[ m_outgoingIndex ], 1.0 );

    /////////////////////////////////////////
    /*Vector otherAlpha(rowCount + columnCount);
    // columnwise version
    m_variableStates.getIterators(&it, &itEnd, Simplex::NONBASIC_AT_LB, Simplex::VARIABLE_STATE_ENUM_LENGTH-1);
    for(; it != itEnd ; ++it){
        unsigned int columnIndex = it.getData();
        if(columnIndex < columnCount){
            //LPINFO(m_pivotRowOfBasisInverse << "  " << m_simplexModel->getMatrix().column(columnIndex));
            otherAlpha.set(columnIndex, otherRow.dotProduct(m_simplexModel->getMatrix().column(columnIndex)));
            //std::cin.get();
        } else {
            otherAlpha.set(columnIndex, otherRow.at(columnIndex - columnCount));
        }
    }
    otherAlpha.set( m_basisHead[ m_outgoingIndex ], 1.0 );
    LPINFO("other alpha: " << otherAlpha);*/
    /////////////////////////////////////////
    // --------------------------------

    // rowwise version
    std::vector<Numerical::Summarizer> results( rowCount + columnCount );
    DenseVector::NonzeroIterator pivotRowIter = m_pivotRowOfBasisInverse.beginNonzero();
    DenseVector::NonzeroIterator pivotRowIterEnd = m_pivotRowOfBasisInverse.endNonzero();
    for (; pivotRowIter != pivotRowIterEnd; ++pivotRowIter) {
        const Numerical::Double lambda = *pivotRowIter;
        const SparseVector & row = m_simplexModel->getMatrix().row( pivotRowIter.getIndex() );
        // structural variables
        SparseVector::NonzeroIterator rowIter = row.beginNonzero();
        SparseVector::NonzeroIterator rowIterEnd = row.endNonzero();
        for (; rowIter != rowIterEnd; ++rowIter) {
            const unsigned int index = rowIter.getIndex();
            if ( m_variableStates.where(index) == (int)Simplex::BASIC ) {
                continue;
            }
            //m_reducedCosts.set(index, Numerical::stableAdd( m_reducedCosts.at( index ), - lambda * *rowIter));

            results[ index ].add( lambda * *rowIter );
            //LPWARNING( index << ".: " << results[index].getResult() );
        }
        const unsigned int index = pivotRowIter.getIndex() + columnCount;
        results[ index ].add( *pivotRowIter );

    }
    //m_pivotRow->clear();
    unsigned int index;
    for (index = 0; index < results.size(); index++) {
        Numerical::Double result = results[index].getResult();
        if (result != 0.0) {
            //TODO: Ez miert sima set?
            m_pivotRow.set(index, result);
        }
    }
    //    m_pivotRow->set( m_basisHead[ m_outgoingIndex ], 1.0 );
}

void DualSimplex::updateReducedCosts() {
    m_reducedCosts.addVector( -m_dualTheta, m_pivotRow);
    m_reducedCosts.set( m_basisHead[ m_outgoingIndex ], -m_dualTheta );
    m_reducedCosts.set( m_incomingIndex, 0.0 );
}

void DualSimplex::resetTolerances() {
    //reset the EXPAND tolerance
    m_recomputeReducedCosts = true;
    if(m_toleranceStep > 0 && m_workingTolerance - m_masterTolerance * m_toleranceMultiplier > m_toleranceStep){
//        LPINFO("Resetting EXPAND tolerance!");
        m_workingTolerance = m_masterTolerance * m_toleranceMultiplier;
    }
}

void DualSimplex::detectExcessivelyInstability()
{
    if ( SimplexParameterHandler::getInstance().getBoolParameterValue(
             "NumericalInstability.enable_numerical_instability_detection") == false) {
        return;
    }
    unsigned int columnCount = m_simplexModel->getColumnCount();
    unsigned int rowCount = m_simplexModel->getRowCount();
    DenseVector pivotColumn(rowCount);
    if(m_incomingIndex < (int)columnCount){
        pivotColumn = m_simplexModel->getMatrix().column(m_incomingIndex);
    } else {
        pivotColumn.set(m_incomingIndex - columnCount, 1);
    }

    m_basis->FtranCheck(pivotColumn);
    Numerical::Double norm1 = pivotColumn.euclidNorm();
    Numerical::Double norm2 = m_pivotColumn.euclidNorm();
    Numerical::Double ratio = (norm1 > norm2 ? norm1 : norm2) / (norm1 > norm2 ? norm2 : norm1);
    //LPINFO("RATIO: " << ratio << " " << (1.0 - ratio));
    if (1.0 - ratio < -1.0) {
        LPERROR("Error: Instable problem!");
        std::cin.get();
    }
    static Numerical::Double min = 0.0;
    if (min > 1.0 - ratio) {
        min = 1.0 - ratio;
        //LPINFO("MIN: " << min);
        //std::cin.get();
    }
    //std::cin.get();
}
