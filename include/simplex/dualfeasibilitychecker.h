/**
 * @file dualfeasibilitychecker.h
 */

#ifndef DUALFEASIBILITYCHECKER_H
#define DUALFEASIBILITYCHECKER_H

#include <globals.h>
#include <utils/indexlist.h>
#include <linalg/vector.h>
#include <simplex/simplexmodel.h>

class Basis;

class DualFeasibilityChecker{

public:
    DualFeasibilityChecker(const SimplexModel& model,
                           IndexList<const Numerical::Double *>* variableStates,
                           IndexList<>* reducedCostFeasibilities,
                           const Vector& reducedCosts,
                           const Basis& basis);

    bool checkFeasibility();
    void computeFeasibility(Numerical::Double tolerance);
    void feasibilityCorrection(Vector* basicVariableValues, Numerical::Double tolerance);

    void updateFeasibilities(const std::vector<int> & updateVector);

    inline Numerical::Double getPhaseIObjectiveValue() {return m_phaseIObjectiveValue;}

private:
    const SimplexModel& m_model;
    IndexList<const Numerical::Double*>* m_variableStates;
    IndexList<>* m_reducedCostFeasibilities;
    const Vector& m_reducedCosts;
    const Basis& m_basis;
    Numerical::Double m_phaseIObjectiveValue;
};

#endif // DUALFEASIBILITYCHECKER_H
