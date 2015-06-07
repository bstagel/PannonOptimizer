//=================================================================================================
/*!
//  This file is part of the Pannon Optimizer library. 
//  This library is free software; you can redistribute it and/or modify it under the 
//  terms of the GNU Lesser General Public License as published by the Free Software 
//  Foundation; either version 3.0, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
//  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//  See the GNU General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License; see the file 
//  COPYING. If not, see http://www.gnu.org/licenses/.
*/
//=================================================================================================

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
