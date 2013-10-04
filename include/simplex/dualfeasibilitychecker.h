#ifndef DUALFEASIBILITYCHECKER_H
#define DUALFEASIBILITYCHECKER_H

#include <utils/indexlist.h>
#include <linalg/vector.h>

#include <simplex/simplexmodel.h>

class DualFeasibilityChecker{

public:
    DualFeasibilityChecker(const SimplexModel& model,
                           const IndexList<Numerical::Double>& variableStates,
                           IndexList<>* reducedCostFeasibilities,
                           const Vector& reducedCosts,
                           Numerical::Double* phaseIObjectiveValue);

    bool nonBasic(int id,const IndexList<>& il);
    bool checkFeasibility(const IndexList<>& reducedCostFeasibilities);
    void computeFeasibility();
    Numerical::Double getPhaseIreducedCost(const Vector& alpha)const;

private:
    const SimplexModel& m_model;
    const IndexList<Numerical::Double>& m_variableStates;
    IndexList<>* m_reducedCostFeasibilities;
    const Vector& m_reducedCosts;
    Numerical::Double* m_phaseIObjectiveValue;
    Numerical::Double m_optimalityTolerance;
    bool equal(Numerical::Double variable,Numerical::Double value);
    bool lessthan(Numerical::Double variable,Numerical::Double value);
    bool lessOrEqual(Numerical::Double variable,Numerical::Double value);
};

#endif // DUALFEASIBILITYCHECKER_H