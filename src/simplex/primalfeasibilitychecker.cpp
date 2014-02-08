#include <simplex/primalfeasibilitychecker.h>
#include <simplex/simplex.h>
#include <simplex/simplexparameterhandler.h>

#include <utils/thirdparty/prettyprint.h>

static const Numerical::Double feasibilityTolerance =
        SimplexParameterHandler::getInstance().getDoubleParameterValue("e_feasibility");

PrimalFeasibilityChecker::PrimalFeasibilityChecker(const SimplexModel& model,
                                                   IndexList<const Numerical::Double *> * variableStates,
                                                   IndexList<> * basicVariableFeasibilities,
                                                   const std::vector<int>& basisHead):
    m_model(model),
    m_variableStates(variableStates),
    m_basicVariableFeasibilities(basicVariableFeasibilities),
    m_basisHead(basisHead)
{

}

bool PrimalFeasibilityChecker::checkFeasibility()
{
    IndexList<>::Iterator setMit;
    IndexList<>::Iterator setMendit;
    IndexList<>::Iterator setPit;
    IndexList<>::Iterator setPendit;

    m_basicVariableFeasibilities->getIterators(&setMit,&setMendit,Simplex::MINUS,1);
    m_basicVariableFeasibilities->getIterators(&setPit,&setPendit,Simplex::PLUS,1);

    if ( (setMit == setMendit) && (setPit == setPendit) ) {
        return true;
    }
    return false;
}

void PrimalFeasibilityChecker::computeFeasibilities()
{
    //this function determines M/F/P sets, phaseI objective value

    m_basicVariableFeasibilities->clearPartition(Simplex::MINUS);
    m_basicVariableFeasibilities->clearPartition(Simplex::PLUS);
    m_basicVariableFeasibilities->clearPartition(Simplex::FEASIBLE);

    m_phaseIObjectiveValue = 0;

    Numerical::Double lbOfIthVariable = 0;
    Numerical::Double ubOfIthVariable = 0;
    Numerical::Double valueOfIthVariable = 0;

//    std::vector<int> mindex;
//    std::vector<int> pindex;

    for(unsigned int basisIndex = 0; basisIndex < m_basisHead.size(); basisIndex++){
        const Variable & basicVariable = m_model.getVariable(m_basisHead.at(basisIndex));
        lbOfIthVariable = basicVariable.getLowerBound();
        ubOfIthVariable = basicVariable.getUpperBound();
        valueOfIthVariable = *(m_variableStates->getAttachedData(m_basisHead.at(basisIndex)));

    //basic variables with M type infeasibility

        if ( Numerical::lessthan(valueOfIthVariable, lbOfIthVariable, feasibilityTolerance) ) {
            m_basicVariableFeasibilities->insert(Simplex::MINUS, basisIndex);
            m_phaseIObjectiveValue += (valueOfIthVariable - lbOfIthVariable);
//            mindex.push_back(basisIndex);;
        } else

    //basic variables with P type infeasibility

        if ( Numerical::lessthan(ubOfIthVariable, valueOfIthVariable, feasibilityTolerance) ) {
            m_basicVariableFeasibilities->insert(Simplex::PLUS, basisIndex);
            m_phaseIObjectiveValue -= (valueOfIthVariable - ubOfIthVariable);
//            pindex.push_back(basisIndex);
        } else

    //basic variables with F type infeasibility

//        if ( Numerical::lessOrEqual(valueOfIthVariable, ubOfIthVariable, feasibilityTolerance) &&
//             !Numerical::lessthan(valueOfIthVariable, lbOfIthVariable, feasibilityTolerance) )
        {
            m_basicVariableFeasibilities->insert(Simplex::FEASIBLE, basisIndex);
        }
    }
//    LPINFO("mindex "<<mindex);
//    LPINFO("pindex "<<pindex);
}
