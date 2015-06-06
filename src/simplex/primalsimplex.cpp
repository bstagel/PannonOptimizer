/**
 * @file primalsimplex.cpp
 * @author Peter Tar, Balint Stagel
 */

#include <simplex/primalsimplex.h>
#include <simplex/pricing/primaldantzigpricing.h>
#include <simplex/pricing/primalsteepestedgepricing.h>
#include <simplex/pricing/primaldevexpricing.h>
#include <simplex/simplexcontroller.h>

#include <simplex/simplexparameterhandler.h>

const static char * INCOMING_NAME = "Incoming";
const static char * OUTGOING_NAME = "Outgoing";
const static char * PHASE_NAME = "Phase";
const static char * PHASE_1_STRING = "ph-1";
const static char * PHASE_2_STRING = "ph-2";
const static char * PRIMAL_INFEASIBILITY_STRING = "Primal infeasibility";
const static char * OBJ_VAL_STRING = "Objective value";
const static char * PRIMAL_REDUCED_COST_STRING = "Reduced cost";
const static char * PRIMAL_THETA_STRING = "Theta";

PrimalSimplex::PrimalSimplex(Basis *basis):
    Simplex(basis),
    m_pricing(0),
    m_feasibilityChecker(0),
    m_ratiotest(0)
{}

PrimalSimplex::~PrimalSimplex()
{
    if (m_pricing) {
        delete m_pricing;
        m_pricing = 0;
    }

    if (m_feasibilityChecker){
        delete m_feasibilityChecker;
        m_feasibilityChecker = 0;
    }

    if (m_ratiotest) {
        delete m_ratiotest;
        m_ratiotest = 0;
    }
}


// Interface of the iteration report provider:
std::vector<IterationReportField> PrimalSimplex::getIterationReportFields(
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

        result.push_back(IterationReportField (PRIMAL_INFEASIBILITY_STRING, 20, 1, IterationReportField::IRF_RIGHT,
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

    default:
        break;
    }

    return result;
}

Entry PrimalSimplex::getIterationEntry(const string &name, ITERATION_REPORT_FIELD_TYPE &type) const {
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
        } else if (name == PRIMAL_INFEASIBILITY_STRING) {
            reply.m_double = Numerical::DoubleToIEEEDouble(m_phaseIObjectiveValue);
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

    default:
        break;
    }

    return Simplex::getIterationEntry(name, type);
}

void PrimalSimplex::computeFeasibility() {
    if(m_feasibilityChecker == nullptr){
        m_feasibilityChecker = new PrimalFeasibilityChecker(*m_simplexModel,
                                                            m_basicVariableValues,
                                                            &m_basicVariableFeasibilities,
                                                            m_basisHead);
        m_masterTolerance = SimplexParameterHandler::getInstance().getDoubleParameterValue("Tolerances.e_feasibility");
        m_toleranceMultiplier = SimplexParameterHandler::getInstance().getDoubleParameterValue("Ratiotest.Expand.multiplier");
        m_toleranceDivider = SimplexParameterHandler::getInstance().getIntegerParameterValue("Ratiotest.Expand.divider");

        initWorkingTolerance();
    }
    m_lastFeasible = m_feasible;
    m_feasible = m_feasibilityChecker->computeFeasibility(m_workingTolerance);

    m_phaseIObjectiveValue = m_feasibilityChecker->getPhaseIObjectiveValue();
    //Becomes feasible
    if(m_lastFeasible == false && m_feasible == true){
        m_referenceObjective = m_objectiveValue;
        m_phase1Time = SimplexController::getSolveTimer().getCPURunningTime();
    } else if(m_lastFeasible == true && m_feasible == false){
        //Becomes infeasible
        m_fallbacks++;
    }
}

void PrimalSimplex::price() {
    if(m_pricing == nullptr){
        std::string pricingType = SimplexParameterHandler::getInstance().getStringParameterValue("Pricing.type");
        if (pricingType == "DANTZIG") {
            m_pricing = new PrimalDantzigPricing(m_basicVariableValues,
                                                 m_basicVariableFeasibilities,
                                                 &m_reducedCostFeasibilities,
                                                 m_variableStates,
                                                 m_basisHead,
                                                 *m_simplexModel,
                                                 *m_basis,
                                                 m_reducedCosts);
        }
        if (pricingType == "STEEPEST_EDGE") {
            m_pricing = new PrimalSteepestEdgePricing(m_basicVariableValues,
                                                      m_basicVariableFeasibilities,
                                                      &m_reducedCostFeasibilities,
                                                      m_variableStates,
                                                      m_basisHead,
                                                      *m_simplexModel,
                                                      *m_basis,
                                                      m_reducedCosts);
        }
        if (pricingType == "DEVEX") {
            m_pricing = new PrimalDevexPricing(m_basicVariableValues,
                                               m_basicVariableFeasibilities,
                                               &m_reducedCostFeasibilities,
                                               m_variableStates,
                                               m_basisHead,
                                               *m_simplexModel,
                                               *m_basis,
                                               m_reducedCosts);
        }
        Simplex::m_pricing = m_pricing;
    }

    if(!m_feasible){
        m_incomingIndex = m_pricing->performPricingPhase1();
        if(m_incomingIndex == -1){
            throw PrimalInfeasibleException("The problem is PRIMAL INFEASIBLE!");
        }
    } else {
        m_incomingIndex = m_pricing->performPricingPhase2();
        if(m_incomingIndex == -1){
            if(m_pricing->hasLockedVariable()){
                throw PrimalUnboundedException("The problem is PRIMAL UNBOUNDED!");
            }else{
                throw OptimalException("OPTIMAL SOLUTION found!");
            }
        }
    }

    if(m_incomingIndex != -1){
        m_primalReducedCost = m_reducedCosts.at(m_incomingIndex);
    } else {
        m_primalReducedCost = 0;
    }
}

void PrimalSimplex::selectPivot() {
    if(m_ratiotest == nullptr){
        m_ratiotest = new PrimalRatiotest(*m_simplexModel,
                                          m_basicVariableValues,
                                          m_basisHead,
                                          m_basicVariableFeasibilities,
                                          m_variableStates);
    }
    m_outgoingIndex = -1;

    unsigned int columnCount = m_simplexModel->getColumnCount();
    unsigned int rowCount = m_simplexModel->getRowCount();

    while(m_outgoingIndex == -1){
        m_pivotColumn.reInit(rowCount);

        if(m_incomingIndex < (int)columnCount){
            m_pivotColumn = m_simplexModel->getMatrix().column(m_incomingIndex);
        } else {
            m_pivotColumn.set(m_incomingIndex - columnCount, 1);
        }
        m_basis->Ftran(m_pivotColumn);

        if(!m_feasible){
            m_ratiotest->performRatiotestPhase1(m_incomingIndex, m_pivotColumn, m_pricing->getReducedCost(), m_phaseIObjectiveValue);
        } else {
            m_ratiotest->performRatiotestPhase2(m_incomingIndex, m_pivotColumn, m_reducedCosts.at(m_incomingIndex), m_workingTolerance);
        }
        m_outgoingIndex = m_ratiotest->getOutgoingVariableIndex();

        //If a boundflip is found, perform it
        if(!m_ratiotest->getBoundflips().empty()){
            break;
        }

        //Column disabling control
        if(m_outgoingIndex == -1){
            //Ask for another column
            LPWARNING("Ask for another column, column is unstable: "<<m_incomingIndex);
            m_pricing->lockLastIndex();
            price();
        }
    }

    m_primalTheta = m_ratiotest->getPrimalSteplength();
    if(m_degenerate){
        m_degenerate = m_ratiotest->isDegenerate();
    }
    if(m_ratiotest->isDegenerate()){
        m_degenerateIterations++;
    }
}

void PrimalSimplex::update() {
    if (!m_ratiotest->isWolfeActive()) {
        std::vector<unsigned int>::const_iterator it = m_ratiotest->getBoundflips().begin();
        std::vector<unsigned int>::const_iterator itend = m_ratiotest->getBoundflips().end();

        for(; it < itend; ++it){
            //        LPWARNING("BOUNDFLIPPING at: "<<*it);
            const Variable& variable = m_simplexModel->getVariable(*it);
            if(m_variableStates.where(*it) == (int)Simplex::NONBASIC_AT_LB) {
                //            LPINFO("LB->UB");
                Numerical::Double boundDistance = variable.getUpperBound() - variable.getLowerBound();
                m_basicVariableValues.addVector(-1 * boundDistance, m_pivotColumn);
                m_variableStates.move(*it, Simplex::NONBASIC_AT_UB, &(variable.getUpperBound()));

            } else if(m_variableStates.where(*it) == (int)Simplex::NONBASIC_AT_UB){
                //            LPINFO("UB->LB");
                Numerical::Double boundDistance = variable.getLowerBound() - variable.getUpperBound();
                m_basicVariableValues.addVector(-1 * boundDistance, m_pivotColumn);
                m_variableStates.move(*it, Simplex::NONBASIC_AT_LB, &(variable.getLowerBound()));
            } else {
                throw PanOptException("Boundflipping variable in the basis (or superbasic)!");
            }
        }
    }

    //Perform the basis change
    if(m_outgoingIndex != -1 && m_incomingIndex != -1){

        Simplex::VARIABLE_STATE outgoingState;
        Variable::VARIABLE_TYPE outgoingType = m_simplexModel->getVariable(m_basisHead[m_outgoingIndex]).getType();

        if (outgoingType == Variable::FIXED) {
            outgoingState = NONBASIC_FIXED;
        }
        else if (outgoingType == Variable::BOUNDED) {
            if(m_ratiotest->outgoingAtUpperBound()){
                outgoingState = NONBASIC_AT_UB;
            } else {
                outgoingState = NONBASIC_AT_LB;
            }
        }
        else if (outgoingType == Variable::PLUS) {
            outgoingState = NONBASIC_AT_LB;
        }
        else if (outgoingType == Variable::FREE) {
            outgoingState = NONBASIC_FREE;
        }
        else if (outgoingType == Variable::MINUS) {
            outgoingState = NONBASIC_AT_UB;
        } else {
            throw PanOptException("Invalid variable type");
        }

        m_pricing->update( m_incomingIndex, m_outgoingIndex, &m_pivotColumn, 0);

        m_objectiveValue += m_primalReducedCost * m_primalTheta;

        //beta' = beta - theta * alpha for current depth only
        if (m_ratiotest->isWolfeActive()) {
            wolfeSpecialUpdate();
        } else {
            m_basicVariableValues.addVector(-1 * m_primalTheta, m_pivotColumn);
        }

        //The incoming variable is NONBASIC thus the attached data gives the appropriate bound or zero
        SparseVector gathered;
        gathered = m_pivotColumn;
        m_basis->append(gathered, m_outgoingIndex, m_incomingIndex, outgoingState);
        m_basicVariableValues.set(m_outgoingIndex, *(m_variableStates.getAttachedData(m_incomingIndex)) + m_primalTheta);
        m_variableStates.move(m_incomingIndex, Simplex::BASIC, &(m_basicVariableValues.at(m_outgoingIndex)));
    }

    computeReducedCosts();

    //Do this only in phase one
    if(!m_feasible){
        computeFeasibility();
    }
}

void PrimalSimplex::wolfeSpecialUpdate()
{
    IndexList<>& degenerateAtLB = m_ratiotest->getDegenerateAtLB();
    IndexList<>& degenerateAtUB = m_ratiotest->getDegenerateAtUB();
    IndexList<>::PartitionIterator it;
    IndexList<>::PartitionIterator endit;
    //update D_Lb
    degenerateAtLB.getIterators(&it,&endit,m_ratiotest->getDegenDepth());
    for (; it != endit; ++it) {
        int basisIndex = it.getData();
        if (basisIndex != m_outgoingIndex) {
            m_basicVariableValues.set(basisIndex,m_basicVariableValues[basisIndex] - m_primalTheta * m_pivotColumn.at(basisIndex));
        }
    }
    //update D_Ub
    degenerateAtUB.getIterators(&it,&endit,m_ratiotest->getDegenDepth());
    for (; it != endit; ++it) {
        int basisIndex = it.getData();
        if (basisIndex != m_outgoingIndex) {
            m_basicVariableValues.set(basisIndex,m_basicVariableValues[basisIndex] - m_primalTheta * m_pivotColumn.at(basisIndex));
        }
    }

    if (degenerateAtLB.where(m_outgoingIndex) < degenerateAtLB.getPartitionCount()) {
        degenerateAtLB.remove(m_outgoingIndex);
    } else if (degenerateAtUB.where(m_outgoingIndex) < degenerateAtUB.getPartitionCount()) {
        degenerateAtUB.remove(m_outgoingIndex);
    } else {
        LPINFO("Index is in neither of the D sets");
        LPINFO("m_outgoingIndex "<<m_outgoingIndex);
        LPINFO("theta_p "<<m_primalTheta);
        LPINFO("xb: "<<m_basicVariableValues[m_outgoingIndex]);
//        LPINFO(degenerateAtLB);
//        LPINFO(degenerateAtUB);
        exit(-1);
    }
    const Variable & var = m_simplexModel->getVariable(m_incomingIndex);
    if (var.getType() != Variable::FREE && Numerical::fabs(m_pivotColumn.at(m_outgoingIndex)) > m_pivotTolerance) {
        Numerical::Double ref_ub = Numerical::fabs(var.getUpperBound() - *(m_variableStates.getAttachedData(m_incomingIndex)) + m_primalTheta);
        Numerical::Double ref_lb = Numerical::fabs(var.getLowerBound() - *(m_variableStates.getAttachedData(m_incomingIndex)) + m_primalTheta);
        if ( ref_ub < ref_lb) {
            degenerateAtUB.insert(m_ratiotest->getDegenDepth(), m_outgoingIndex);
            if ( m_variableStates.where(m_incomingIndex) == Simplex::NONBASIC_AT_LB ) {
                LPERROR("wrong state");
                exit(-1);
            }
        } else {
            degenerateAtLB.insert(m_ratiotest->getDegenDepth(), m_outgoingIndex);
    //        LPINFO("m_outgoingIndex "<<m_outgoingIndex);
    //        LPINFO("x_b "<<m_basicVariableValues[m_outgoingIndex]);
    //        Numerical::Double lb = m_simplexModel->getVariable(m_basisHead[m_outgoingIndex]).getLowerBound();
    //        Numerical::Double ub = m_simplexModel->getVariable(m_basisHead[m_outgoingIndex]).getUpperBound();
    //        LPINFO("lb: "<<lb<<" ub: "<<ub);
    //        LPINFO("alpha: "<<m_pivotColumn.at(m_outgoingIndex));
            if ( m_variableStates.where(m_incomingIndex) == Simplex::NONBASIC_AT_UB ) {
                LPERROR("wrong state");
                exit(-1);
            }
        }
    }
}

void PrimalSimplex::setReferenceObjective(bool secondPhase) {
    if(!secondPhase){
        m_referenceObjective = m_phaseIObjectiveValue;
    } else {
        m_referenceObjective = m_objectiveValue;
    }
}

void PrimalSimplex::checkReferenceObjective(bool secondPhase) {
    if(!secondPhase){
        if( Numerical::less(m_phaseIObjectiveValue,m_referenceObjective) ){
            LPWARNING("BAD ITERATION - PHASE I");
            m_badIterations++;
        } else if(m_referenceObjective == m_phaseIObjectiveValue){
            //            LPWARNING("DEGENERATE - PHASE I");
            //TODO: Mashogy kell merni
            m_degenerateIterations++;
        }
    } else {
        if(Numerical::less(m_objectiveValue,m_referenceObjective)){
            LPWARNING("BAD ITERATION - PHASE II");
            m_badIterations++;
        } else if(m_referenceObjective == m_objectiveValue){
            //            LPWARNING("DEGENERATE - PHASE II");
            m_degenerateIterations++;
        }
    }
}

void PrimalSimplex::initWorkingTolerance() {
    //initializing EXPAND tolerance
    if (SimplexParameterHandler::getInstance().getStringParameterValue("Ratiotest.Expand.type") != "INACTIVE" ) {
        m_workingTolerance = m_masterTolerance * m_toleranceMultiplier;
        m_toleranceStep = (m_masterTolerance - m_workingTolerance) / m_toleranceDivider;
    } else {
        m_workingTolerance = m_masterTolerance;
        m_toleranceStep = 0;
    }
}

void PrimalSimplex::computeWorkingTolerance() {
    //increment the EXPAND tolerance
    if (m_toleranceStep != 0) {
        m_workingTolerance += m_toleranceStep;
        if (m_workingTolerance >= m_masterTolerance) {
            resetTolerances();
        }
    }
}

void PrimalSimplex::releaseLocks() {
    if(m_pricing != nullptr){
        m_pricing->releaseUsed();
    }
}

void PrimalSimplex::updateReducedCosts() {
    const Numerical::Double lastReducedCost = m_reducedCosts.at( m_incomingIndex );
    if (lastReducedCost == 0.0) {
        return;
    }
    const unsigned int structuralVariableCount = m_simplexModel->getMatrix().columnCount();
    DenseVector ro( m_simplexModel->getRowCount());
    ro.set( m_outgoingIndex, 1.0 );

    m_basis->Btran(ro);
    DenseVector::NonzeroIterator roIter = ro.beginNonzero();
    DenseVector::NonzeroIterator roIterEnd = ro.endNonzero();
    for (; roIter != roIterEnd; ++roIter) {
        const Numerical::Double lambda = *roIter * lastReducedCost;
        const SparseVector & row = m_simplexModel->getMatrix().row( roIter.getIndex() );
        // structural variables
        SparseVector::NonzeroIterator rowIter = row.beginNonzero();
        SparseVector::NonzeroIterator rowIterEnd = row.endNonzero();
        for (; rowIter != rowIterEnd; ++rowIter) {
            const unsigned int index = rowIter.getIndex();
            m_reducedCosts.set(index, Numerical::stableAdd( m_reducedCosts.at( index ), - lambda * *rowIter));
        }
        // logical variables
        const unsigned int index = roIter.getIndex() + structuralVariableCount;;

        m_reducedCosts.set( index, Numerical::stableAdd( m_reducedCosts.at(index), - lambda) );
    }
}

void PrimalSimplex::resetTolerances() {
    //reset the EXPAND tolerance
    m_recomputeReducedCosts = true;
    if(m_toleranceStep > 0 && m_workingTolerance - m_masterTolerance * m_toleranceMultiplier > m_toleranceStep){
        //        LPINFO("Resetting EXPAND tolerance!");
        m_workingTolerance = m_masterTolerance * m_toleranceMultiplier;
    }
}
