/**
 * @file dualfeasibilitychecker.h This file contains the API of the DualFeasibilityChecker class.
 */

#ifndef DUALFEASIBILITYCHECKER_H
#define DUALFEASIBILITYCHECKER_H

#include <globals.h>
#include <utils/indexlist.h>
#include <linalg/vector.h>
#include <simplex/simplexmodel.h>

class Basis;

/**
 * The aim of this class is to check / compute the dual feasibility of the reduced costs.
 *
 * @class DualFeasibilityChecker
 */
class DualFeasibilityChecker{
public:

    /**
     * Initializing constructor of the class.
     * @param model provides the class access to the model so it can get the values, bounds of a variable etc
     * @param variableStates is used in the feasibility correction to check whether a nonbasic variable is on its lower
     * or upper bound
     * @param reducedCostFeasibilities the linked list that stores the feasibilities of the reduced costs. It's computed
     * in the DualFeasibilityChecker::computeFeasibility function
     * @param reducedCosts the vector of the reduced costs
     * @param basis is used in the feasibility correction, to be able to perform an Ftran operation on the basis
     *
     * @constructor
     */
    DualFeasibilityChecker(const SimplexModel& model,
                           IndexList<const Numerical::Double *>* variableStates,
                           IndexList<>* reducedCostFeasibilities,
                           const Vector& reducedCosts,
                           const Basis& basis);

    /**
     * This function checks, if there is any reduced cost with M or P type infeasibility.
     * @return the boolean value of the feasibility property.
     */
    bool checkFeasibility();

    /**
     * This function computes the feasibilities of the reduced costs.
     * It determines the M, F, P feasibility sets, and the phase 1 objective value,
     * the sum of dual infeasibilities.
     * @param tolerance is the currently used dual feasibility tolerance
     */
    void computeFeasibility(Numerical::Double tolerance);

    /**
     * This function performs the feasibility correction.
     * By doing so it maintaines the feasibility property of the bounded variables at the end of dual phase 1.
     * @param basicVariableValues is needed to compute the basic solution vector
     * @param tolerance is the currently used dual feasibility tolerance
     */
    void feasibilityCorrection(Vector* basicVariableValues, Numerical::Double tolerance);

    /**
     * The aim of this function is to be able to update the reduced cost feasibilities in an efficient way.
     * @param updateVector stores the changes in the feasibilities
     */
    void updateFeasibilities(const std::vector<int> & updateVector);

    /**
     * Getter that returns the dual phase 1 objective value, the sum of dual infeasibilities.
     * @return DualFeasibilityChecker::m_phaseIObjectiveValue.
     */
    inline Numerical::Double getPhaseIObjectiveValue() {return m_phaseIObjectiveValue;}

private:

    /**
     * The model provides the class inforamtion such as the values or bounds of the variables.
     */
    const SimplexModel& m_model;

    /**
     * This indexlist stores the state (being at lower/upper bound) of the nonbasic variables.
     */
    IndexList<const Numerical::Double*>* m_variableStates;

    /**
     * This indexlist stores the feasibilities of the reduced costs, which are computed by this class.
     */
    IndexList<>* m_reducedCostFeasibilities;

    /**
     * The vector of the reduced costs.
     */
    const Vector& m_reducedCosts;

    /**
     * A reference to the basis, so that during the feasibility correction an Ftran operation can be performed.
     */
    const Basis& m_basis;

    /**
     * The phase 1 dual objective value, the sum of dual infeasibilities.
     */
    Numerical::Double m_phaseIObjectiveValue;
};

#endif // DUALFEASIBILITYCHECKER_H
