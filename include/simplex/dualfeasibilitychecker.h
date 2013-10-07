#ifndef DUALFEASIBILITYCHECKER_H
#define DUALFEASIBILITYCHECKER_H

#include <globals.h>
#include <utils/indexlist.h>
#include <linalg/vector.h>

#include <simplex/simplexmodel.h>

class DualFeasibilityChecker{

public:
    DualFeasibilityChecker(const SimplexModel& model,
                           const IndexList<const Numerical::Double*>& variableStates,
                           IndexList<>* reducedCostFeasibilities,
                           const Vector& reducedCosts,
                           Numerical::Double* phaseIObjectiveValue);

    bool checkFeasibility(const IndexList<>& reducedCostFeasibilities);
    void computeFeasibility();

    //TODO: this function does not belong here
//    Numerical::Double getPhaseIreducedCost(const Vector& alpha)const;

private:
    const SimplexModel& m_model;
    const Vector& m_reducedCosts;
    const IndexList<const Numerical::Double*>& m_variableStates;
    IndexList<>* m_reducedCostFeasibilities;
    Numerical::Double* m_phaseIObjectiveValue;
};

#endif // DUALFEASIBILITYCHECKER_H
