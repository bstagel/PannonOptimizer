#include <simplex/primalfeasibilitychecker.h>
#include <simplex/simplex.h>
#include <simplex/simplexparameterhandler.h>

static const Numerical::Double feasibilityTolerance =
        SimplexParameterHandler::getInstance().getDoubleParameterValue("e_feasibility");

PrimalFeasibilityChecker::PrimalFeasibilityChecker(const SimplexModel& model,
                                                   IndexList<const Numerical::Double *> * variableStates,
                                                   IndexList<> * basicVariableFeasibilities,
                                                   Numerical::Double * phaseIObjectiveValue):
    m_model(model),m_variableStates(variableStates),m_basicVariableFeasibilities(basicVariableFeasibilities),
    m_phaseIObjectiveValue(phaseIObjectiveValue)
{

}

bool PrimalFeasibilityChecker::checkFeasibility(const IndexList<>& basicVariableFeasibilities)
{
    IndexList<>::Iterator setMit;
    IndexList<>::Iterator setMendit;
    IndexList<>::Iterator setPit;
    IndexList<>::Iterator setPendit;

    (*m_basicVariableFeasibilities) = basicVariableFeasibilities;
    m_basicVariableFeasibilities->getIterators(&setMit,&setMendit,Simplex::MINUS,1);
    m_basicVariableFeasibilities->getIterators(&setPit,&setPendit,Simplex::PLUS,1);


    if ( (setMit == setMendit) && (setPit == setPendit) ) {
        return true;
    }
    return false;
}

void PrimalFeasibilityChecker::computeFeasibilities(){
    //this function determines M/F/P sets, phaseI objective value

    m_basicVariableFeasibilities->clearPartition(Simplex::MINUS);
    m_basicVariableFeasibilities->clearPartition(Simplex::PLUS);
    m_basicVariableFeasibilities->clearPartition(Simplex::FEASIBLE);

    (*m_phaseIObjectiveValue) = 0;

    Numerical::Double lbOfIthVariable = 0;
    Numerical::Double ubOfIthVariable = 0;
    Numerical::Double valueOfIthVariable = 0;

  //TODO: bejárás nem hatékony
    for(unsigned int variableIndex = 0; variableIndex <
        (m_model.getColumnCount() + m_model.getRowCount()); variableIndex++){
        if (m_variableStates->where(variableIndex) == Simplex::BASIC) {
            lbOfIthVariable = m_model.getVariable(variableIndex).getLowerBound();
            ubOfIthVariable = m_model.getVariable(variableIndex).getUpperBound();
            valueOfIthVariable = *(m_variableStates->getAttachedData(variableIndex));

        //basic variables with M type infeasibility

            if ( Numerical::lessthan(valueOfIthVariable, lbOfIthVariable, feasibilityTolerance) ) {
                m_basicVariableFeasibilities->insert(Simplex::MINUS, variableIndex);
                (*m_phaseIObjectiveValue) += (valueOfIthVariable - lbOfIthVariable);
            } else

        //basic variables with P type infeasibility

            if ( !Numerical::lessOrEqual(valueOfIthVariable, ubOfIthVariable, feasibilityTolerance) ) {
                m_basicVariableFeasibilities->insert(Simplex::PLUS, variableIndex);
                (*m_phaseIObjectiveValue) -= (valueOfIthVariable - ubOfIthVariable);
            } else

        //basic variables with F type infeasibility

            if ( Numerical::lessOrEqual(valueOfIthVariable, ubOfIthVariable, feasibilityTolerance) &&
                 !Numerical::lessthan(valueOfIthVariable, lbOfIthVariable, feasibilityTolerance) ) {
                m_basicVariableFeasibilities->insert(Simplex::FEASIBLE, variableIndex);
            }
        }
    }
}
