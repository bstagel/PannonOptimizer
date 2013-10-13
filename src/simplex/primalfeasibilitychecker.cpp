#include <simplex/primalfeasibilitychecker.h>
#include <simplex/simplex.h>

PrimalFeasibilityChecker::PrimalFeasibilityChecker(const SimplexModel& model,
                                                   IndexList<const Numerical::Double *> * variableStates,
                                                   IndexList<> * variableFeasibilities,
                                                   Numerical::Double * phaseIReducedcost):
    m_model(model),m_variableStates(variableStates),m_variableFeasibilities(variableFeasibilities),
    m_phaseIReducedcost(phaseIReducedcost)
{

}

bool PrimalFeasibilityChecker::checkFeasibility(const IndexList<>& variableFeasibilities)
{
    IndexList<>::Iterator setMit;
    IndexList<>::Iterator setMendit;
    IndexList<>::Iterator setPit;
    IndexList<>::Iterator setPendit;

    (*m_variableFeasibilities) = variableFeasibilities;
    m_variableFeasibilities->getIterators(&setMit,&setMendit,Simplex::MINUS,1);
    m_variableFeasibilities->getIterators(&setPit,&setPendit,Simplex::PLUS,1);


    if ( (setMit == setMendit) && (setPit == setPendit) ) {
        return true;
    }
    return false;
}

void PrimalFeasibilityChecker::computeFeasibilities()
{

}
