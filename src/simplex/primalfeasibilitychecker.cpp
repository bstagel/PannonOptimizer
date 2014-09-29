/**
 * @file primalfeasibilitychecker.cpp
 */
#include <simplex/primalfeasibilitychecker.h>
#include <simplex/simplex.h>
#include <simplex/simplexparameterhandler.h>
#include <utils/thirdparty/prettyprint.h>

PrimalFeasibilityChecker::PrimalFeasibilityChecker(const SimplexModel& model,
                                                   IndexList<const Numerical::Double *> * variableStates,
                                                   IndexList<> * basicVariableFeasibilities,
                                                   const std::vector<int>& basisHead):
    m_model(model),
    m_variableStates(variableStates),
    m_basicVariableFeasibilities(basicVariableFeasibilities),
    m_basisHead(basisHead),
    m_feasibilityTolerance(SimplexParameterHandler::getInstance().getDoubleParameterValue("Tolerances.e_feasibility"))
{

}

bool PrimalFeasibilityChecker::computeFeasibilities(Numerical::Double tolerance)
{
    //TODO: Toleranciakezeles
    __UNUSED(tolerance);
    //this function determines M/F/P sets, phaseI objective value

    m_basicVariableFeasibilities->clearPartition(Simplex::MINUS);
    m_basicVariableFeasibilities->clearPartition(Simplex::PLUS);
    m_basicVariableFeasibilities->clearPartition(Simplex::FEASIBLE);

    m_phaseIObjectiveValue = 0;

    Numerical::Double lbOfVariable = 0;
    Numerical::Double ubOfVariable = 0;
    Numerical::Double valueOfVariable = 0;

    for(unsigned int basisIndex = 0; basisIndex < m_basisHead.size(); basisIndex++){
        const Variable & basicVariable = m_model.getVariable(m_basisHead[basisIndex]);
        lbOfVariable = basicVariable.getLowerBound();
        ubOfVariable = basicVariable.getUpperBound();
        valueOfVariable = *(m_variableStates->getAttachedData(m_basisHead[basisIndex]));

    //basic variables with M type infeasibility
        if ( Numerical::lessthan(valueOfVariable, lbOfVariable, m_feasibilityTolerance) ) {
            m_basicVariableFeasibilities->insert(Simplex::MINUS, basisIndex);
            m_phaseIObjectiveValue += (valueOfVariable - lbOfVariable);
        } else

    //basic variables with P type infeasibility
        if ( Numerical::lessthan(ubOfVariable, valueOfVariable, m_feasibilityTolerance) ) {
            m_basicVariableFeasibilities->insert(Simplex::PLUS, basisIndex);
            m_phaseIObjectiveValue -= (valueOfVariable - ubOfVariable);
    //basic variables with F type infeasibility
        } else {
            m_basicVariableFeasibilities->insert(Simplex::FEASIBLE, basisIndex);
        }
    }

    IndexList<>::PartitionIterator setMit;
    IndexList<>::PartitionIterator setMendit;
    IndexList<>::PartitionIterator setPit;
    IndexList<>::PartitionIterator setPendit;

    m_basicVariableFeasibilities->getIterators(&setMit,&setMendit,Simplex::MINUS);
    m_basicVariableFeasibilities->getIterators(&setPit,&setPendit,Simplex::PLUS);

    return ( (setMit == setMendit) && (setPit == setPendit) );
}
