/**
 * @file dualfeasibilitychecker.cpp
 */

#include <simplex/dualfeasibilitychecker.h>
#include <lp/model.h>
#include <simplex/simplex.h>
#include <simplex/simplexparameterhandler.h>
#include <simplex/pfibasis.h>

DualFeasibilityChecker::DualFeasibilityChecker(const SimplexModel& model,
                                               IndexList<const Numerical::Double*>* variableStates,
                                               IndexList<>* reducedCostFeasibilities,
                                               const Vector& reducedCosts,
                                               const Basis& basis):
    m_model(model),
    m_variableStates(variableStates),
    m_reducedCostFeasibilities(reducedCostFeasibilities),
    m_reducedCosts(reducedCosts),
    m_basis(basis)
{

}

bool DualFeasibilityChecker::computeFeasibility(Numerical::Double tolerance){
//this function determines M/F/P sets, phase I objective function value
    m_reducedCostFeasibilities->clearPartition(Simplex::FEASIBLE);
    m_reducedCostFeasibilities->clearPartition(Simplex::MINUS);
    m_reducedCostFeasibilities->clearPartition(Simplex::PLUS);
    m_phaseIObjectiveValue = 0;

    Variable::VARIABLE_TYPE typeOfIthVariable;

    for(unsigned int variableIndex = 0; variableIndex < m_reducedCosts.length(); variableIndex++){
        if (m_variableStates->where(variableIndex) != Simplex::BASIC) {
            typeOfIthVariable = m_model.getVariable(variableIndex).getType();

    //nonbasic variables with M type infeasibility
                if (m_reducedCosts.at(variableIndex) < -tolerance &&
                        (typeOfIthVariable == Variable::PLUS || typeOfIthVariable == Variable::FREE)) {
                    m_reducedCostFeasibilities->insert(Simplex::MINUS,variableIndex);
                    m_phaseIObjectiveValue += m_reducedCosts.at(variableIndex);
                } else

    //nonbasic variables with P type infeasibility
                if (m_reducedCosts.at(variableIndex) > tolerance &&
                        (typeOfIthVariable == Variable::MINUS || typeOfIthVariable == Variable::FREE)) {
                    m_reducedCostFeasibilities->insert(Simplex::PLUS,variableIndex);
                    m_phaseIObjectiveValue -= m_reducedCosts.at(variableIndex);
                } else{

    //nonbasic variables with F type infeasibility
                    m_reducedCostFeasibilities->insert(Simplex::FEASIBLE,variableIndex);
                }
        }
    }

    IndexList<>::Iterator setMit;
    IndexList<>::Iterator setMendit;
    IndexList<>::Iterator setPit;
    IndexList<>::Iterator setPendit;

    m_reducedCostFeasibilities->getIterators(&setMit,&setMendit,Simplex::MINUS);
    m_reducedCostFeasibilities->getIterators(&setPit,&setPendit,Simplex::PLUS);

    return ((setMit == setMendit) && (setPit == setPendit));
}

void DualFeasibilityChecker::feasibilityCorrection(Vector* basicVariableValues, Numerical::Double tolerance) {
    unsigned int rowCount = m_model.getRowCount();
    unsigned int columnCount = m_model.getColumnCount();

    Vector transformVector(basicVariableValues->length());
    transformVector.setSparsityRatio(DENSE);

    //TODO: Increase efficiency by checking only NONBASIC_AT_*B lists
    for(unsigned int variableIndex = 0; variableIndex < m_reducedCosts.length(); variableIndex++){
        const Variable& variable = m_model.getVariable(variableIndex);
        if (variable.getType() == Variable::BOUNDED){
            if (m_variableStates->where(variableIndex) == Simplex::NONBASIC_AT_LB && m_reducedCosts.at(variableIndex) < -tolerance){
//                LPINFO("Do a bound flip LB -> UB (T+ set)");
                //Do a bound flip LB -> UB (T+ set)
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
            } else
                if (m_variableStates->where(variableIndex) == Simplex::NONBASIC_AT_UB && m_reducedCosts.at(variableIndex) > tolerance){
//                LPINFO("Do a bound flip UB -> LB (T- set)");
                //Do a bound flip UB -> LB (T- set)
                //Swap states
                m_variableStates->move(variableIndex, Simplex::NONBASIC_AT_LB, &(variable.getLowerBound()));
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

void DualFeasibilityChecker::updateFeasibilities(const std::vector<int> & updateVector)
{

    for (unsigned int index = 0; index < updateVector.size(); index++) {
        if (updateVector[index] == 1) {
            if (m_reducedCostFeasibilities->where(index) == Simplex::MINUS) {
                m_reducedCostFeasibilities->move(index,Simplex::FEASIBLE);
            } else
            if (m_reducedCostFeasibilities->where(index) == Simplex::FEASIBLE) {
                m_reducedCostFeasibilities->move(index,Simplex::PLUS);
            }
        } else
        if (updateVector[index] == -1) {
            if (m_reducedCostFeasibilities->where(index) == Simplex::PLUS) {
                m_reducedCostFeasibilities->move(index,Simplex::FEASIBLE);
            } else
            if (m_reducedCostFeasibilities->where(index) == Simplex::FEASIBLE) {
                m_reducedCostFeasibilities->move(index,Simplex::MINUS);
            }
        } else
        if (updateVector[index] == 2 &&
             (m_reducedCostFeasibilities->where(index) == Simplex::MINUS)) {
                m_reducedCostFeasibilities->move(index,Simplex::PLUS);
        } else
        if (updateVector[index] == -2 &&
            (m_reducedCostFeasibilities->where(index) == Simplex::PLUS)) {
                m_reducedCostFeasibilities->move(index,Simplex::MINUS);
            }
    }
}
