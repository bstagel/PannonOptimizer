/**
 * @file primalfeasibilitychecker.h This file contains the API of the PrimalFeasibilityChecker class.
 */
#ifndef PRIMALFEASIBILITYCHECKER_H
#define PRIMALFEASIBILITYCHECKER_H

#include <utils/indexlist.h>
#include <linalg/vector.h>
#include <simplex/simplexmodel.h>

/**
 * This class computes and checkes the primal feasibility of the basicvariables.
 *
 * @class PrimalFeasibilityChecker
 */
class PrimalFeasibilityChecker{
public:

    /**
     * Initializing constructor of the class.
     * @param model provides information such as variable bounds etc.
     * @param variableStates holds the states (being at upper / lower bound) of the variables
     * @param basicVariableFeasibilities holds the feasibility sets defined in the feasibility checker.
     * @param basisHead contains the variable indices of the basicvariables
     * @constructor
     */
    PrimalFeasibilityChecker(const SimplexModel& model,
                             IndexList<const Numerical::Double *>* variableStates,
                             IndexList<>* basicVariableFeasibilities,
                             const std::vector<int> &basisHead);

    /**
     * This function computes the feasibilities of the basic variables.
     * It determines the M, F, P feasibility sets, and the phase 1 objective value, the sum of primal infeasibilities.
     * @param tolerance is the currently used primal feasibility tolerance
     * @return True if everything is dual feasible
     */
    bool computeFeasibilities(Numerical::Double tolerance);

    /**
     * Getter of the primal phase 1 objective value, the sum of primal infeasibilities.
     * @return PrimalFeasibilityChecker::m_phaseIObjectiveValue;
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
     * This indexlist stores the feasibilities of the basic variables, which are computed by this class.
     */
    IndexList<>* m_basicVariableFeasibilities;

    /**
     * The basishead containtes the variable indices of the basicvariables.
     */
    const std::vector<int>& m_basisHead;

    /**
     * The phase 1 primal objective value, the sum of primal infeasibilities.
     */
    Numerical::Double m_phaseIObjectiveValue;

    //Parameter references
    /**
     * This is a parameter reference to "e_feasibility" run-time parameter,
     * for details see SimplexParameterHandler.
     */
    const double & m_feasibilityTolerance;
};

#endif // PRIMALFEASIBILITYCHECKER_H
