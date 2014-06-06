/**
 * @file primalsimplex.cpp
 */

#include <simplex/primalsimplex.h>
#include <simplex/pricing/primaldantzigpricing.h>

#include <simplex/simplexparameterhandler.h>

const static char * INCOMING_NAME = "Incoming";
const static char * OUTGOING_NAME = "Outgoing";
const static char * PHASE_NAME = "Phase";
const static char * PHASE_1_STRING = "ph-1";
const static char * PHASE_2_STRING = "ph-2";
const static char * PHASE_UNKNOWN_STRING = "unknown";
const static char * PHASE_1_OBJ_VAL_STRING = "Primal infeasibility";
const static char * OBJ_VAL_STRING = "Objective value";
const static char * PRIMAL_REDUCED_COST_STRING = "Reduced cost";
const static char * PRIMAL_THETA_STRING = "Theta";

PrimalSimplex::PrimalSimplex():
    m_pricing(0),
    m_feasibilityChecker(0),
    m_ratiotest(0),
    m_phaseName(PHASE_UNKNOWN_STRING)
{
    m_masterTolerance = SimplexParameterHandler::getInstance().getDoubleParameterValue("e_feasibility");
    m_toleranceMultiplier = SimplexParameterHandler::getInstance().getDoubleParameterValue("expand_multiplier_dphI");
    m_toleranceDivider = SimplexParameterHandler::getInstance().getIntegerParameterValue("expand_divider_dphI");
}

PrimalSimplex::~PrimalSimplex()
{
    releaseModules();
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

    default:
        break;
    }

    return result;
}

Entry PrimalSimplex::getIterationEntry(const string &name, ITERATION_REPORT_FIELD_TYPE &type) const {
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

    default:
        break;
    }

    return Simplex::getIterationEntry(name, type);
}

void PrimalSimplex::initModules() {
    Simplex::initModules();

    // TODO: ezt majd egy switch-case donti el, amit lehetne
    // kulon fuggvenybe is tenni akar

    m_pricing = new PrimalDantzigPricing(m_basicVariableValues,
                                         m_basicVariableFeasibilities,
                                         &m_reducedCostFeasibilities,
                                         m_variableStates,
                                         m_basisHead,
                                         *m_simplexModel,
                                         *m_basis,
                                         m_reducedCosts);

    Simplex::m_pricing = m_pricing;

    m_feasibilityChecker = new PrimalFeasibilityChecker(*m_simplexModel,
                                                        &m_variableStates,
                                                        &m_basicVariableFeasibilities,
                                                        m_basisHead);

    //Todo: Add the updater to the ratiotest
    m_ratiotest = new PrimalRatiotest(*m_simplexModel,
                                      m_basicVariableValues,
                                      m_basisHead,
                                      m_basicVariableFeasibilities,
                                      m_variableStates);


    //The alpha vector for the column calculations
    m_alpha.resize(m_simplexModel->getRowCount());
    m_alpha.setSparsityRatio(DENSE);
}

void PrimalSimplex::releaseModules() {
    Simplex::releaseModules();

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

void PrimalSimplex::computeFeasibility() {
    m_feasibilityChecker->computeFeasibilities();
    m_phaseIObjectiveValue = m_feasibilityChecker->getPhaseIObjectiveValue();
}

void PrimalSimplex::checkFeasibility() {
    bool lastFeasible = m_feasible;
    m_feasible = m_feasibilityChecker->checkFeasibility();
    //Becomes feasible
    if(lastFeasible == false && m_feasible == true){
        //throw SwitchAlgorithmException
        if (SimplexParameterHandler::getInstance().getIntegerParameterValue("switch_algorithm") == 2){
            throw SwitchAlgorithmException("phase-2 entered!");
        }
        m_referenceObjective = m_objectiveValue;
    } else if(lastFeasible == true && m_feasible == false){
        //Becomes infeasible
        m_fallbacks++;
    }

}

void PrimalSimplex::price() {
    if(!m_feasible){
        m_phaseName = PHASE_1_STRING;
        m_incomingIndex = m_pricing->performPricingPhase1();
        if(m_incomingIndex == -1){
            throw PrimalInfeasibleException("The problem is PRIMAL INFEASIBLE!");
        }
    } else {
        m_phaseName = PHASE_2_STRING;
        m_incomingIndex = m_pricing->performPricingPhase2();
        if(m_incomingIndex == -1){
            throw OptimalException("OPTIMAL SOLUTION found!");
        }
    }

//    LPWARNING("rc: "<<m_pricing->getReducedCost());

    if(m_incomingIndex != -1){
        m_primalReducedCost = m_reducedCosts.at(m_incomingIndex);
    } else {
        m_primalReducedCost = 0;
    }
}

void PrimalSimplex::selectPivot() {
    m_outgoingIndex = -1;

    unsigned int columnCount = m_simplexModel->getColumnCount();

    while(m_outgoingIndex == -1){

        m_alpha.clear();
        if(m_incomingIndex < (int)columnCount){
            m_alpha = m_simplexModel->getMatrix().column(m_incomingIndex);
            m_alpha.setSparsityRatio(DENSE);
        } else {
            m_alpha.setNewNonzero(m_incomingIndex - columnCount, 1);
        }
        m_basis->Ftran(m_alpha);

        //Test code to check primal ph_1 reduced cost
//        Numerical::Double rcCheck=0;
//        if(!m_feasible){
//            for(int i=0; i<m_alpha.length(); i++){
//                if(m_basicVariableFeasibilities.where(i) == Simplex::MINUS){
//                    rcCheck += m_alpha.at(i);
//                } else if(m_basicVariableFeasibilities.where(i) == Simplex::PLUS){
//                    rcCheck -= m_alpha.at(i);
//                }
//            }

//            if(!Numerical::equals(rcCheck, m_pricing->getReducedCost())){
//                LPERROR("Phase 1 reduced cost error: pricing: "<<m_pricing->getReducedCost()<< " ; alpha: "<<rcCheck);
//                exit(-1);
//            }
//        }

        if(!m_feasible){
            m_ratiotest->performRatiotestPhase1(m_incomingIndex, m_alpha, m_pricing->getReducedCost(), m_phaseIObjectiveValue);
        } else {
#ifndef NDEBUG
            for(unsigned i=0;i<m_basicVariableValues.length();i++){
                const Variable& var = m_simplexModel->getVariable(m_basisHead[i]);
                Numerical::Double value = m_basicVariableValues.at(i);
                Numerical::Double lb = var.getLowerBound();
                Numerical::Double ub = var.getUpperBound();
                if (value < lb || value > ub){
                    LPWARNING("Infeasible variable in phase 2: "<<i);
                    LPINFO("value: "<<value<<" lb: "<<lb<<" ub: "<<ub);
                }
            }
#endif
            m_ratiotest->performRatiotestPhase2(m_incomingIndex, m_alpha, m_reducedCosts.at(m_incomingIndex));
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

}

void PrimalSimplex::update() {
    std::vector<unsigned int>::const_iterator it = m_ratiotest->getBoundflips().begin();
    std::vector<unsigned int>::const_iterator itend = m_ratiotest->getBoundflips().end();

    for(; it < itend; it++){
//        LPWARNING("BOUNDFLIPPING at: "<<*it);
        const Variable& variable = m_simplexModel->getVariable(*it);
        if(m_variableStates.where(*it) == Simplex::NONBASIC_AT_LB) {
//            LPINFO("LB->UB");
            Numerical::Double boundDistance = variable.getUpperBound() - variable.getLowerBound();
            m_basicVariableValues.addVector(-1 * boundDistance, m_alpha, Numerical::ADD_ABS);
            m_variableStates.move(*it, Simplex::NONBASIC_AT_UB, &(variable.getUpperBound()));

        } else if(m_variableStates.where(*it) == Simplex::NONBASIC_AT_UB){
//            LPINFO("UB->LB");
            Numerical::Double boundDistance = variable.getLowerBound() - variable.getUpperBound();
            m_basicVariableValues.addVector(-1 * boundDistance, m_alpha, Numerical::ADD_ABS);
            m_variableStates.move(*it, Simplex::NONBASIC_AT_LB, &(variable.getLowerBound()));
        } else {
            LPERROR("Boundflipping variable in the basis (or superbasic)!");
            //TODO Throw some exception here
        }
    }


    //debug
//    Numerical::Double lbOfOutgoingVariable = m_simplexModel->getVariable(m_basisHead.at(m_outgoingIndex)).getLowerBound();
//    Numerical::Double ubOfOutgoingVariable = m_simplexModel->getVariable(m_basisHead.at(m_outgoingIndex)).getUpperBound();
//    Numerical::Double valueOfOutgoingVariable = m_basicVariableValues.at(m_outgoingIndex);

    if(m_outgoingIndex != -1 && m_incomingIndex != -1){
        //Save whether the basis is to be changed
        m_baseChanged = true;

        Simplex::VARIABLE_STATE outgoingState;
        Variable::VARIABLE_TYPE outgoingType = m_simplexModel->getVariable(m_basisHead.at(m_outgoingIndex)).getType();

        if (outgoingType == Variable::FIXED) {
            outgoingState = NONBASIC_FIXED;
//            LPINFO("ougoing variable fix, value: "<<valueOfOutgoingVariable);
        }
        else if (outgoingType == Variable::BOUNDED) {
            if(m_ratiotest->outgoingAtUpperBound()){
                outgoingState = NONBASIC_AT_UB;
//                LPINFO("outgoing variable bounded, leaves at LB, value: "<<valueOfOutgoingVariable<<
//                       " lb: "<<lbOfOutgoingVariable);
            } else {
                outgoingState = NONBASIC_AT_LB;
//                LPINFO("outgoing variable vounded, leaves at UB, value: "<<valueOfOutgoingVariable<<
//                       " ub: "<<ubOfOutgoingVariable);
            }
//            LPWARNING("state out: "<<outgoingState);
        }
        else if (outgoingType == Variable::PLUS) {
//            LPINFO("outgoing variable PLUS type, val:"<<valueOfOutgoingVariable);
            outgoingState = NONBASIC_AT_LB;
        }
        else if (outgoingType == Variable::FREE) {
//            LPINFO("outgoing variable FREE type, val:"<<valueOfOutgoingVariable);
            outgoingState = NONBASIC_FREE;
        }
        else if (outgoingType == Variable::MINUS) {
//            LPINFO("outgoing variable MINUS type, val:"<<valueOfOutgoingVariable);
            outgoingState = NONBASIC_AT_UB;
        } else {
            throw PanOptException("Invalid variable type");
        }

        m_pricing->update( m_incomingIndex, m_outgoingIndex, 0, 0);

        m_basicVariableValues.addVector(-1 * m_primalTheta, m_alpha, Numerical::ADD_ABS);

        m_objectiveValue += m_primalReducedCost * m_primalTheta;

        //The incoming variable is NONBASIC thus the attached data gives the appropriate bound or zero
        m_basis->append(m_alpha, m_outgoingIndex, m_incomingIndex, outgoingState);

        m_basicVariableValues.set(m_outgoingIndex, *(m_variableStates.getAttachedData(m_incomingIndex)) + m_primalTheta);
        m_variableStates.move(m_incomingIndex, Simplex::BASIC, &(m_basicVariableValues.at(m_outgoingIndex)));
    }

    computeReducedCosts();

    //Do this only in phase one
    if(!m_feasible){
        computeFeasibility();
    }

//    LPWARNING("outgoing variable val: "<<valueOfOutgoingVariable<<
//              " lb: "<<lbOfOutgoingVariable<<
//              " ub: "<<ubOfOutgoingVariable<<" state: " <<outgoingState<<" type: "<<typeOfIthVariable);
//    LPWARNING("incoming variable LB: "<<m_simplexModel->getVariable(m_incomingIndex).getLowerBound()<<
//              " UB: "<<m_simplexModel->getVariable(m_incomingIndex).getUpperBound());
//    LPWARNING("teta: "<<m_primalTheta<< " sum: "<<valueOfOutgoingVariable+m_primalTheta);
}

void PrimalSimplex::setReferenceObjective() {
    if(!m_feasible){
        m_referenceObjective = m_phaseIObjectiveValue;
    } else {
        m_referenceObjective = m_objectiveValue;
    }
}

void PrimalSimplex::checkReferenceObjective() {
    if(!m_feasible){
        if( Numerical::fabs(m_referenceObjective - m_phaseIObjectiveValue) > 1e10 ){
            LPWARNING("BAD ITERATION - PHASE I");
            m_badIterations++;
        } else if(m_referenceObjective == m_phaseIObjectiveValue){
//            LPWARNING("DEGENERATE - PHASE I");
            m_degenerateIterations++;
        }
    } else {
        if(Numerical::fabs(m_phaseIObjectiveValue - m_referenceObjective) > 1e10 ){
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
    if (SimplexParameterHandler::getInstance().getIntegerParameterValue("nonlinear_primal_phaseI_function") == 3) {
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
         //reset the EXPAND tolerance
        if (m_workingTolerance >= m_masterTolerance) {
            m_workingTolerance = m_masterTolerance * m_toleranceMultiplier;
        }
    }
}

void PrimalSimplex::releaseLocks() {
    m_pricing->releaseUsed();
}
