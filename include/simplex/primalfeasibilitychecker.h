#ifndef PRIMALFEASIBILITYCHECKER_H
#define PRIMALFEASIBILITYCHECKER_H

#include <utils/indexlist.h>
#include <linalg/vector.h>
#include <simplex/simplexmodel.h>

class PrimalFeasibilityChecker
{
public:
    PrimalFeasibilityChecker(const SimplexModel& model,
                             IndexList<const Numerical::Double *>* variableStates,
                             IndexList<>* variableFeasibilities,
                             Numerical::Double * phaseIReducedcost,
                             Numerical::Double * phaseIObjectiveValue);

    bool checkFeasibility(const IndexList<>& variableFeasibilities);
    void computeFeasibilities();
private:
    const SimplexModel& m_model;
    IndexList<const Numerical::Double*>* m_variableStates;
    IndexList<>* m_variableFeasibilities;
    Numerical::Double * m_phaseIReducedcost;
    Numerical::Double * m_phaseIObjectiveValue;
};

#endif // PRIMALFEASIBILITYCHECKER_H
