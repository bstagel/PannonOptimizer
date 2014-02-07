#ifndef PRIMALFEASIBILITYCHECKER_H
#define PRIMALFEASIBILITYCHECKER_H

#include <utils/indexlist.h>
#include <linalg/vector.h>
#include <simplex/simplexmodel.h>

class PrimalFeasibilityChecker{

public:
    PrimalFeasibilityChecker(const SimplexModel& model,
                             IndexList<const Numerical::Double *>* variableStates,
                             IndexList<>* basicVariableFeasibilities,
                             const std::vector<int> &basisHead);

    bool checkFeasibility();
    void computeFeasibilities();

    inline Numerical::Double getPhaseIObjectiveValue() {return m_phaseIObjectiveValue;}

private:
    const SimplexModel& m_model;
    IndexList<const Numerical::Double*>* m_variableStates;
    IndexList<>* m_basicVariableFeasibilities;
    const std::vector<int>& m_basisHead;
    Numerical::Double m_phaseIObjectiveValue;
};

#endif // PRIMALFEASIBILITYCHECKER_H
