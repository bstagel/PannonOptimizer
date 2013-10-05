#include <simplex/dualfeasibilitychecker.h>
#include <lp/model.h>
#include <simplex/simplex.h>
#include <simplex/simplexparameterhandler.h>

static const Numerical::Double optimalityTolerance =
        SimplexParameterHandler::getInstance().getParameterValue("e_optimality");

DualFeasibilityChecker::DualFeasibilityChecker(const SimplexModel& model,
                                               const IndexList<const Numerical::Double*>& variableStates,
                                               IndexList<>* reducedCostFeasibilities,
                                               const Vector& reducedCosts,
                                               Numerical::Double* phaseIObjectiveValue):
    m_model(model),
    m_variableStates(variableStates),
    m_reducedCostFeasibilities(reducedCostFeasibilities),
    m_reducedCosts(reducedCosts),
    m_phaseIObjectiveValue(phaseIObjectiveValue)
{

}
Numerical::Double DualFeasibilityChecker::getPhaseIreducedCost(const Vector& alpha)const{

//TODO: phase one reduced cost as a param?!!

    Numerical::Double phaseIreducedCost = 0;
    IndexList<>::Iterator it;
    IndexList<>::Iterator endit;
    Variable::VARIABLE_TYPE typeOfIthVariable;
    m_reducedCostFeasibilities->getIterators(&it,&endit,0,2);
    unsigned int variableIndex = it.getData();

        for(; it != endit; it++){
            if(m_variableStates.where(variableIndex) != Simplex::BASIC){
                typeOfIthVariable = m_model.getVariable(variableIndex).getType();

    //nonbasic variables with M type infeasibility

                if(m_reducedCosts[variableIndex] < 0 &&
                    (typeOfIthVariable == Variable::PLUS || typeOfIthVariable == Variable::FREE)){
                    phaseIreducedCost += alpha[variableIndex];
                }else

    //nonbasic variables with P type infeasibility

                 if(m_reducedCosts[variableIndex] > 0 &&
                    (typeOfIthVariable == Variable::MINUS || typeOfIthVariable == Variable::FREE)){
                    phaseIreducedCost -= alpha[variableIndex];
                 }
            }
        }
        return phaseIreducedCost;
}

bool DualFeasibilityChecker::checkFeasibility(const IndexList<>& reducedCostFeasibilities){
    IndexList<>::Iterator setMit;
    IndexList<>::Iterator setMendit;
    IndexList<>::Iterator setPit;
    IndexList<>::Iterator setPendit;

    reducedCostFeasibilities.getIterators(&setMit,&setMendit,Simplex::MINUS,1);
    reducedCostFeasibilities.getIterators(&setPit,&setPendit,Simplex::PLUS,1);

    if( (setMit == setMendit) && (setPit == setPendit) ){
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

    IndexList<>::Iterator it;
    IndexList<>::Iterator endit;
    m_reducedCostFeasibilities->getIterators(&it,&endit,0,3);
    Variable::VARIABLE_TYPE typeOfIthVariable;
    unsigned int variableIndex = it.getData();

        for(; it != endit; it++){
            if(m_variableStates.where(variableIndex) != Simplex::BASIC){
                typeOfIthVariable = m_model.getVariable(variableIndex).getType();

    //nonbasic variables with M type infeasibility

                if(Numerical::lessthan(m_reducedCosts[variableIndex],0,optimalityTolerance) &&
                    (typeOfIthVariable == Variable::PLUS || typeOfIthVariable == Variable::FREE)){
                    m_reducedCostFeasibilities->insert(variableIndex,Simplex::MINUS);
                        (*m_phaseIObjectiveValue) += m_reducedCosts[variableIndex];
                }else

    //nonbasic variables with P type infeasibility

                 if(Numerical::lessthan(0,m_reducedCosts[variableIndex],optimalityTolerance) &&
                    (typeOfIthVariable == Variable::MINUS || typeOfIthVariable == Variable::FREE)){
                     m_reducedCostFeasibilities->insert(variableIndex,Simplex::PLUS);
                        (*m_phaseIObjectiveValue) -= m_reducedCosts[variableIndex];
                 }else{

    //nonbasic variables with F type infeasibility

                     if(( !Numerical::lessthan(m_reducedCosts[variableIndex],0,optimalityTolerance) &&
                          typeOfIthVariable == Variable::PLUS) ||
                             (Numerical::lessOrEqual(m_reducedCosts[variableIndex],0,optimalityTolerance) &&
                              typeOfIthVariable == Variable::MINUS) ||
                             (Numerical::equal(m_reducedCosts[variableIndex],0,optimalityTolerance) &&
                              typeOfIthVariable == Variable::FREE)){
                         m_reducedCostFeasibilities->insert(variableIndex,Simplex::FEASIBLE);
                     }
                 }
            }
        }
}
