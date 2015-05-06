/**
 * @file primalfeasibilitychecker.h This file contains the API of the PrimalFeasibilityChecker class.
 */

#ifndef PRIMALFEASIBILITYCHECKER_H
#define PRIMALFEASIBILITYCHECKER_H

#include <utils/indexlist.h>
#include <simplex/simplexmodel.h>

/**
 * This class computes and checks the primal feasibility of the basic variables.
 *
 * @class PrimalFeasibilityChecker
 */
class PrimalFeasibilityChecker{
public:

    /**
     * Initializing constructor.
     *
     * @param model The model of the LP problem.
     * @param variableStates The states (being at upper / lower bound) of the variables
     * @param basicVariableFeasibilities The feasibility sets defined in the feasibility checker.
     * @param basisHead The variable indices of the basic variables.
     * @constructor
     */
    PrimalFeasibilityChecker(const SimplexModel& model,
                             const DenseVector & basicVariableValues,
                             IndexList<>* basicVariableFeasibilities,
                             const std::vector<int> &basisHead);

    /**
     * This function computes the feasibilities of the basic variables.
     * It determines the M, F, P feasibility sets, and the phase 1 objective value, the sum of primal infeasibilities.
     *
     * @param tolerance The currently used primal feasibility tolerance.
     * @return true if everything is dual feasible.
     */
    bool computeFeasibility(Numerical::Double tolerance);

    /**
     * Getter of the primal phase 1 objective value, the sum of primal infeasibilities.
     *
     * @return PrimalFeasibilityChecker::m_phaseIObjectiveValue;
     */
    inline Numerical::Double getPhaseIObjectiveValue() {return m_phaseIObjectiveValue;}

private:

    /**
     * The simplex model of the LP problem.
     */
    const SimplexModel& m_model;

    /**
     * This vector(X_B) stores the basic variable values.
     * Do not set to const reference, because in Wolfe's 'ad hoc' method the ratiotest should be able to
     * change the basic variable values thus it is a virtual perturbation.
     */
    const DenseVector& m_basicVariableValues;

    /**
     * Index list storing the feasibilities of the basic variables, which are computed by this class.
     */
    IndexList<>* m_basicVariableFeasibilities;

    /**
     * The basishead containing the variable indices of the basic variables.
     */
    const std::vector<int>& m_basisHead;

    /**
     * The phase 1 primal objective value, the sum of primal infeasibilities.
     */
    Numerical::Double m_phaseIObjectiveValue;
};

#endif // PRIMALFEASIBILITYCHECKER_H
