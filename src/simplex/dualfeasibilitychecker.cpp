#include <simplex/dualfeasibilitychecker.h>
#include <lp/model.h>
#include <simplex/simplex.h>
#include <simplex/simplexparameterhandler.h>

DualFeasibilityChecker::DualFeasibilityChecker(const SimplexModel& model,
                                               const IndexList<Numerical::Double>& variableStates,
                                               IndexList<>* reducedCostFeasibilities,
                                               const Vector& reducedCosts,
                                               Numerical::Double* phaseIObjectiveValue):
    m_model(model),
    m_variableStates(variableStates),
    m_reducedCostFeasibilities(reducedCostFeasibilities),
    m_reducedCosts(reducedCosts),
    m_phaseIObjectiveValue(phaseIObjectiveValue),
    m_optimalityTolerance(SimplexParameterHandler::getInstance().getParameterValue("e_optimality"))
{

}
Numerical::Double DualFeasibilityChecker::getPhaseIreducedCost(const Vector& alpha)const{

//TODO: phase one reduced cost as a param?!!

    Numerical::Double phaseIreducedCost = 0;
    IndexList<>::Iterator it;
    IndexList<>::Iterator endit;
    Variable::VARIABLE_TYPE typeOfIthVariable;
    m_reducedCostFeasibilities->getIterators(&it,&endit,Simplex::MINUS,2);
    unsigned int variableIndex = it.getData();
        for(;it!=endit;it++){
            if(m_variableStates.where(variableIndex) != Simplex::BASIC){
                typeOfIthVariable=m_model.getVariable(variableIndex).getType();
    //nonbasic variables with M type infeasibility
                if(m_reducedCosts[variableIndex]<0 &&
                    (typeOfIthVariable==Variable::PLUS || typeOfIthVariable==Variable::FREE)){
                    phaseIreducedCost+=alpha[variableIndex];
                }else
    //nonbasic variables with P type infeasibility
                 if(m_reducedCosts[variableIndex]>0 &&
                    (typeOfIthVariable==Variable::MINUS || typeOfIthVariable==Variable::FREE)){
                    phaseIreducedCost-=alpha[variableIndex];
                 }
            }
        }
        return phaseIreducedCost;
}

bool DualFeasibilityChecker::equal(Numerical::Double variable,Numerical::Double value){
    if( Numerical::fabs(variable-value) <= m_optimalityTolerance ){
        return true;
    }
    return false;
}
bool DualFeasibilityChecker::lessthan(Numerical::Double variable,Numerical::Double value){
    //var < val <=> var+e < val
    if( variable + m_optimalityTolerance < value ){
        return true;
    }
    return false;
}

bool DualFeasibilityChecker::checkFeasibility(const IndexList<>& reducedCostFeasibilities){
    IndexList<>::Iterator it;
    IndexList<>::Iterator endit;
    reducedCostFeasibilities.getIterators(&it,&endit,1,2);

    if(it!=endit){
        return false;
    }
    return true;
}
bool DualFeasibilityChecker::lessOrEqual(Numerical::Double variable,Numerical::Double value){
    return ( lessthan(variable,value) || equal(variable,value) );
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
    m_reducedCostFeasibilities->getIterators(&it,&endit,1,3);
    Variable::VARIABLE_TYPE typeOfIthVariable;
    unsigned int variableIndex = it.getData();
        for(;it!=endit;it++){
            if(m_variableStates.where(variableIndex) != Simplex::BASIC){
                typeOfIthVariable=m_model.getVariable(variableIndex).getType();

    //nonbasic variables with M type infeasibility

                if(lessthan(m_reducedCosts[variableIndex],0) &&
                    (typeOfIthVariable==Variable::PLUS || typeOfIthVariable==Variable::FREE)){
                    m_reducedCostFeasibilities->insert(variableIndex,Simplex::MINUS);
                        (*m_phaseIObjectiveValue)+=m_reducedCosts[variableIndex];
                }else

    //nonbasic variables with P type infeasibility

                 if(lessthan(0,m_reducedCosts[variableIndex]) &&
                    (typeOfIthVariable==Variable::MINUS || typeOfIthVariable==Variable::FREE)){
                     m_reducedCostFeasibilities->insert(variableIndex,Simplex::PLUS);
                        (*m_phaseIObjectiveValue)-=m_reducedCosts[variableIndex];
                 }else{

    //nonbasic variables with F type infeasibility

                     if(( !lessthan(m_reducedCosts[variableIndex],0) && typeOfIthVariable==Variable::PLUS) ||
                             (lessOrEqual(m_reducedCosts[variableIndex],0) && typeOfIthVariable==Variable::MINUS) ||
                             (equal(m_reducedCosts[variableIndex],0) && typeOfIthVariable==Variable::FREE)){
                         m_reducedCostFeasibilities->insert(variableIndex,Simplex::FEASIBLE);
                     }
                 }
            }
        }
}
