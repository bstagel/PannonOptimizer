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

bool DualFeasibilityChecker::checkFeasibility(){
    IndexList<>::Iterator setMit;
    IndexList<>::Iterator setMendit;
    IndexList<>::Iterator setPit;
    IndexList<>::Iterator setPendit;

    m_reducedCostFeasibilities->getIterators(&setMit,&setMendit,Simplex::MINUS);
    m_reducedCostFeasibilities->getIterators(&setPit,&setPendit,Simplex::PLUS);

    if ( (setMit == setMendit) && (setPit == setPendit) ) {
        return true;
    }
    return false;
}

void DualFeasibilityChecker::computeFeasibility(){
//this function determines M/F/P sets, dual objective function value

    //TODO: JOCO _ CLEAR ALL PARTITION
    m_reducedCostFeasibilities->clearPartition(Simplex::FEASIBLE);
    m_reducedCostFeasibilities->clearPartition(Simplex::MINUS);
    m_reducedCostFeasibilities->clearPartition(Simplex::PLUS);
    *m_phaseIObjectiveValue = 0;

    Variable::VARIABLE_TYPE typeOfIthVariable;
    OBJECTIVE_TYPE objectiveType = m_model.getObjectiveType();

    for(unsigned int variableIndex = 0; variableIndex<m_reducedCosts.length(); variableIndex++){
        //TODO: Mi a helyzet a bazisvaltozokkal?...
        if (m_variableStates->where(variableIndex) != Simplex::BASIC) {
            typeOfIthVariable = m_model.getVariable(variableIndex).getType();

    //min problem

            if(objectiveType == MINIMIZE) {

//                for(int i=0; i< m_reducedCosts.length();i++){
//                    LPINFO("COMPUTE MINIMIZE m_reducedCost "<<variableIndex << " - " <<m_reducedCosts.at(variableIndex));
//                }
    //nonbasic variables with M type infeasibility

                if (Numerical::lessthan(m_reducedCosts.at(variableIndex), 0,optimalityTolerance) &&
                        (typeOfIthVariable == Variable::PLUS || typeOfIthVariable == Variable::FREE)) {
                    m_reducedCostFeasibilities->insert(Simplex::MINUS,variableIndex);
//                    LPINFO("nonbasic variables with M type infeasibility");
//                    LPINFO("typeOfIthVariable "<<typeOfIthVariable);
//                    LPINFO("variableIndex "<<variableIndex);
//                    LPERROR("m_reducedCosts.at(variableIndex) "<<m_reducedCosts.at(variableIndex));
                        (*m_phaseIObjectiveValue) += m_reducedCosts.at(variableIndex);
                } else

    //nonbasic variables with P type infeasibility

                if (Numerical::lessthan(0, m_reducedCosts.at(variableIndex), optimalityTolerance) &&
                        (typeOfIthVariable == Variable::MINUS || typeOfIthVariable == Variable::FREE)) {
                    m_reducedCostFeasibilities->insert(Simplex::PLUS,variableIndex);
                        (*m_phaseIObjectiveValue) -= m_reducedCosts.at(variableIndex);
                } else

    //nonbasic variables with F type infeasibility

                if (( !Numerical::lessthan(m_reducedCosts.at(variableIndex),0,optimalityTolerance) &&
                        typeOfIthVariable == Variable::PLUS) ||
                        (Numerical::lessOrEqual(m_reducedCosts.at(variableIndex),0,optimalityTolerance) &&
                        typeOfIthVariable == Variable::MINUS) ||
                        (Numerical::equal(m_reducedCosts.at(variableIndex),0,optimalityTolerance) &&
                        typeOfIthVariable == Variable::FREE)) {
//                                        LPINFO("nonbasic variables with F type infeasibility");
                    m_reducedCostFeasibilities->insert(Simplex::FEASIBLE,variableIndex);
                }

     //max problem

            } else{

    //nonbasic variables with M type infeasibility

                if (Numerical::lessthan(m_reducedCosts.at(variableIndex),0,optimalityTolerance) &&
                        (typeOfIthVariable == Variable::MINUS || typeOfIthVariable == Variable::FREE)) {
                    m_reducedCostFeasibilities->insert(Simplex::MINUS,variableIndex);
                        (*m_phaseIObjectiveValue) += m_reducedCosts.at(variableIndex);
                } else

    //nonbasic variables with P type infeasibility

                if (Numerical::lessthan(0, m_reducedCosts.at(variableIndex), optimalityTolerance) &&
                        (typeOfIthVariable == Variable::PLUS || typeOfIthVariable == Variable::FREE)) {
                    m_reducedCostFeasibilities->insert(Simplex::PLUS,variableIndex);
                        (*m_phaseIObjectiveValue) -= m_reducedCosts.at(variableIndex);
                } else

    //nonbasic variables with F type infeasibility

                if (( !Numerical::lessthan(m_reducedCosts.at(variableIndex),0,optimalityTolerance) &&
                        typeOfIthVariable == Variable::MINUS) ||
                        (Numerical::lessOrEqual(m_reducedCosts.at(variableIndex),0,optimalityTolerance) &&
                        typeOfIthVariable == Variable::PLUS) ||
                        (Numerical::equal(m_reducedCosts.at(variableIndex),0,optimalityTolerance) &&
                        typeOfIthVariable == Variable::FREE)) {
                    m_reducedCostFeasibilities->insert(Simplex::FEASIBLE,variableIndex);
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
