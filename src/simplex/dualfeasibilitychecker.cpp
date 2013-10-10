/**
 * @file dualfeasibilitychecker.cpp
 */

#include <simplex/dualfeasibilitychecker.h>
#include <lp/model.h>
#include <simplex/simplex.h>
#include <simplex/simplexparameterhandler.h>
#include <simplex/pfibasis.h>

static const Numerical::Double optimalityTolerance =
        SimplexParameterHandler::getInstance().getParameterValue("e_optimality");

DualFeasibilityChecker::DualFeasibilityChecker(const SimplexModel& model,
                                               IndexList<const Numerical::Double*>* variableStates,
                                               IndexList<>* reducedCostFeasibilities,
                                               const Vector& reducedCosts,
                                               const Basis& basis,
                                               Numerical::Double* phaseIObjectiveValue):
    m_model(model),
    m_variableStates(variableStates),
    m_reducedCostFeasibilities(reducedCostFeasibilities),
    m_reducedCosts(reducedCosts),
    m_basis(basis),
    m_phaseIObjectiveValue(phaseIObjectiveValue)
{

}

//Numerical::Double DualFeasibilityChecker::getPhaseIreducedCost(const Vector& alpha)const{

////TODO: phase one reduced cost as a param?!!

//    Numerical::Double phaseIreducedCost = 0;
//    IndexList<>::Iterator it;
//    IndexList<>::Iterator endit;
//    Variable::VARIABLE_TYPE typeOfIthVariable;
//    m_reducedCostFeasibilities->getIterators(&it,&endit,0,2);
//    unsigned int variableIndex = it.getData();

//        for (; it != endit; it++) {
//            if (m_variableStates->where(variableIndex) != Simplex::BASIC) {
//                typeOfIthVariable = m_model.getVariable(variableIndex).getType();

//    //nonbasic variables with M type infeasibility

//                if (m_reducedCosts[variableIndex] < 0 &&
//                    (typeOfIthVariable == Variable::PLUS || typeOfIthVariable == Variable::FREE)) {
//                    phaseIreducedCost += alpha[variableIndex];
//                } else

//    //nonbasic variables with P type infeasibility

//                 if (m_reducedCosts[variableIndex] > 0 &&
//                    (typeOfIthVariable == Variable::MINUS || typeOfIthVariable == Variable::FREE)) {
//                    phaseIreducedCost -= alpha[variableIndex];
//                 }
//            }
//        }
//        return phaseIreducedCost;
//}

bool DualFeasibilityChecker::checkFeasibility(const IndexList<>& reducedCostFeasibilities){
    IndexList<>::Iterator setMit;
    IndexList<>::Iterator setMendit;
    IndexList<>::Iterator setPit;
    IndexList<>::Iterator setPendit;

    reducedCostFeasibilities.getIterators(&setMit,&setMendit,Simplex::MINUS,1);
    reducedCostFeasibilities.getIterators(&setPit,&setPendit,Simplex::PLUS,1);

    if ( (setMit == setMendit) && (setPit == setPendit) ) {
        return true;
    }
    return false;
}

void DualFeasibilityChecker::computeFeasibility(){
//this function determines M/F/P sets, dual objective function value, dual reduced cost

    //TODO: JOCO _ CLEAR ALL PARTITION
    m_reducedCostFeasibilities->clearPartition(Simplex::MINUS);
    m_reducedCostFeasibilities->clearPartition(Simplex::FEASIBLE);
    m_reducedCostFeasibilities->clearPartition(Simplex::PLUS);
    *m_phaseIObjectiveValue = 0;

    Variable::VARIABLE_TYPE typeOfIthVariable;
    OBJECTIVE_TYPE objectiveType = m_model.getObjectiveType();

    for(unsigned int variableIndex = 0; variableIndex<m_reducedCosts.length(); variableIndex++){
        if (m_variableStates->where(variableIndex) != Simplex::BASIC) {
            typeOfIthVariable = m_model.getVariable(variableIndex).getType();

    //min problem

            if(objectiveType == MINIMIZE) {

    //nonbasic variables with M type infeasibility
                if (Numerical::lessthan(m_reducedCosts[variableIndex],0,optimalityTolerance) &&
                        (typeOfIthVariable == Variable::PLUS || typeOfIthVariable == Variable::FREE)) {
                    m_reducedCostFeasibilities->insert(Simplex::MINUS,variableIndex);
                        (*m_phaseIObjectiveValue) += m_reducedCosts[variableIndex];
                    std::cout<<*m_reducedCostFeasibilities;
                } else

    //nonbasic variables with P type infeasibility

                if (Numerical::lessthan(0,m_reducedCosts[variableIndex],optimalityTolerance) &&
                        (typeOfIthVariable == Variable::MINUS || typeOfIthVariable == Variable::FREE)) {
                    m_reducedCostFeasibilities->insert(Simplex::PLUS,variableIndex);
                        (*m_phaseIObjectiveValue) -= m_reducedCosts[variableIndex];
                    LPINFO("P TYPE d_j found");
                } else

    //nonbasic variables with F type infeasibility

                if (( !Numerical::lessthan(m_reducedCosts[variableIndex],0,optimalityTolerance) &&
                        typeOfIthVariable == Variable::PLUS) ||
                        (Numerical::lessOrEqual(m_reducedCosts[variableIndex],0,optimalityTolerance) &&
                        typeOfIthVariable == Variable::MINUS) ||
                        (Numerical::equal(m_reducedCosts[variableIndex],0,optimalityTolerance) &&
                        typeOfIthVariable == Variable::FREE)) {
                    m_reducedCostFeasibilities->insert(Simplex::FEASIBLE,variableIndex);
                    LPINFO("F TYPE d_j found");
                }

     //max problem

            } else{

    //nonbasic variables with M type infeasibility

                if (Numerical::lessthan(m_reducedCosts[variableIndex],0,optimalityTolerance) &&
                        (typeOfIthVariable == Variable::MINUS || typeOfIthVariable == Variable::FREE)) {
                    m_reducedCostFeasibilities->insert(Simplex::MINUS,variableIndex);
                        (*m_phaseIObjectiveValue) += m_reducedCosts[variableIndex];
                    LPINFO("M TYPE d_j found");
                } else

    //nonbasic variables with P type infeasibility

                if (Numerical::lessthan(0,m_reducedCosts[variableIndex],optimalityTolerance) &&
                        (typeOfIthVariable == Variable::PLUS || typeOfIthVariable == Variable::FREE)) {
                    m_reducedCostFeasibilities->insert(Simplex::PLUS,variableIndex);
                        (*m_phaseIObjectiveValue) -= m_reducedCosts[variableIndex];
                    LPINFO("P TYPE d_j found");
                } else

    //nonbasic variables with F type infeasibility

                if (( !Numerical::lessthan(m_reducedCosts[variableIndex],0,optimalityTolerance) &&
                        typeOfIthVariable == Variable::MINUS) ||
                        (Numerical::lessOrEqual(m_reducedCosts[variableIndex],0,optimalityTolerance) &&
                        typeOfIthVariable == Variable::PLUS) ||
                        (Numerical::equal(m_reducedCosts[variableIndex],0,optimalityTolerance) &&
                        typeOfIthVariable == Variable::FREE)) {
                    m_reducedCostFeasibilities->insert(Simplex::FEASIBLE,variableIndex);
                    LPINFO("F TYPE d_j found");
                }
            }
        }
    }
}

void DualFeasibilityChecker::feasiblityCorrection(Vector* basicVariableValues) {

    unsigned int rowCount = m_model.getRowCount();
    unsigned int columnCount = m_model.getColumnCount();

    Vector transformVector(basicVariableValues->length());
    transformVector.setSparsityRatio(DENSE);

    for(unsigned int variableIndex = 0; variableIndex<m_reducedCosts.length(); variableIndex++){
        const Variable& variable = m_model.getVariable(variableIndex);
        if (variable.getType() == Variable::BOUNDED){
            if (m_variableStates->where(variableIndex) == Simplex::NONBASIC_AT_LB &&
                    (m_reducedCosts.at(variableIndex) < 0)) {
                //Do a bound flip LB -> UB (T+ set)
                //Swap states
                m_variableStates->move(variableIndex, Simplex::NONBASIC_AT_UB, &(variable.getUpperBound()));
                //Compute (l_j-u_j)*a_j
                Numerical::Double theta = variable.getUpperBound() - variable.getLowerBound();
                if(variableIndex < columnCount){
                    transformVector.addVector(-1 * theta, m_model.getMatrix().column(variableIndex));
                } else {
                    Vector logical(rowCount);
                    logical.setNewNonzero(variableIndex - columnCount,1);
                    transformVector.addVector(-1 * theta, logical);
                }
            } else if (m_variableStates->where(variableIndex) == Simplex::NONBASIC_AT_UB &&
                   (m_reducedCosts.at(variableIndex) < 0)) {
                //Do a bound flip UB -> LB (T- set)
                //Swap states
                m_variableStates->move(variableIndex, Simplex::NONBASIC_AT_UB, &(variable.getUpperBound()));
                //Compute (u_j-l_j)*a_j
                Numerical::Double theta = variable.getLowerBound() - variable.getUpperBound();
                if(variableIndex < columnCount){
                    transformVector.addVector(-1 * theta, m_model.getMatrix().column(variableIndex));
                } else {
                    Vector logical(rowCount);
                    logical.setNewNonzero(variableIndex - columnCount,1);
                    transformVector.addVector(-1 * theta, logical);
                }
            }
        }
    }
    //Transform x_b
    m_basis.Ftran(transformVector);
    basicVariableValues->addVector(1, transformVector);
}
