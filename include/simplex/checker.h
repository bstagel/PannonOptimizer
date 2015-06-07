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
 * @file checker.h This file contains the Checker class.
 */
#ifndef CHECKER_H
#define CHECKER_H

#include <simplex/simplex.h>

/**
 * Describes a general checker object. Arbitrary checker functions can be implemented.
 * It makes a good use if we want to check the correctness of a value updated in every iteration.
 * Certain numerical inaccuracy can be detected this way.
 *
 * @class Checker
 */
class Checker {
public:

    /**
     * Checks the basis with Ftran method.
     * It computes the basis(B) from the basishead and from the matrix of the problem and checks
     * whether they are equal or not.
     *
     * @param simplex The Simplex object with the current state of the problem being solved.
     * @return true if the computed basis is correct.
     */
    static bool checkBasisWithFtran(const Simplex& simplex);

    /**
     * Checks the basis with Btran method.
     * It computes the basis(B) from the basishead and from the matrix of the problem and checks
     * whether they are equal or not.
     *
     * @param simplex The Simplex object with the current state of the problem being solved.
     * @return true if the computed basis is correct.
     */
    static bool checkBasisWithBtran(const Simplex& simplex);

    /**
     * Checks whether the reduced cost values of the basic variables are zero or not.
     * The reduced cost vector can be computed as d = c_j - c_B^T * B^-1 * a_j.
     *
     * @param simplex The Simplex object with the current state of the problem being solved.
     * @return true if the reduced costs of the basicvariables is zero.
     */
    static bool checkBasisWithReducedCost(const Simplex& simplex);

    /**
     * Checks the reduced cost values of the nonbasic variables.
     * It computes the values with Ftran and Btran, if the values are equal considering
     * some tolerance the check passes.
     * The reduced cost vector can be computed as d = c - c_B^T * B^-1 * a.
     *
     * @param simplex The Simplex object with the current state of the problem being solved.
     * @return true if the reduced cost values of the nonbasic variables are correct.
     */
    static bool checkBasisWithNonbasicReducedCost(const Simplex& simplex);

    /**
     * Checks whether the basicvariable values are correct.
     * The values of X_B vector are stored attached to the basicvariables, and as a separate (X_B) vector.
     * This function checks if there is any inconsistency among these values.
     *
     * @param simplex The Simplex object with the current state of the problem being solved.
     * @return true if the basic variable values are correct.
     */
    static bool checkVariableStateAttachedValues(const Simplex& simplex);

    /**
     * Checks an alpha value with Ftran and Btran.
     * Computes alpha as a row and column vector and compares it.
     *
     * @param simplex The Simplex object with the current state of the problem being solved.
     * @param rowIndex The index of the alpha row vector
     * @param columnIndex The index of the alpha column vector
     * @param columnAlpha The alpha value computed from a column vector.
     * @param rowAlpha The alpha value computed from a row vector.
     * @return true if the computed alpha values are equal.
     */
    static bool checkAlphaValue(const Simplex& simplex,
                               int rowIndex, int columnIndex, Numerical::Double & columnAlpha, Numerical::Double & rowAlpha);

    /**
     * Checks the primal theta value with a pivot computed with Ftran and Btran.
     * Computes the primal theta as (XB - LB) / alpha or (XB - UB) / alpha. The alpha values
     * are computed both from row and column vector.
     *
     * @param simplex The Simplex object with the current state of the problem being solved.
     * @param rowIndex The index of the alpha row vector.
     * @param columnIndex The index of the alpha column vector.
     * @param thetaFromCol The theta value computed from alpha column vector.
     * @param thetaFromRow The theta value computed from alpha row vector.
     * @return true if the computed values are equal.
     */
    static bool checkPrimalTheta(const Simplex& simplex,
                                   int rowIndex, int columnIndex,
                                   Numerical::Double& thetaFromCol, Numerical::Double& thetaFromRow);

    /**
     * Checks the feasibility of the basic variables considering the given tolerance.
     *
     * @param simplex The Simplex object with the current state of the problem being solved.
     * @param print Prints information to the output if true.
     * @param tolerance The feasibility tolerance.
     * @return true if no variable's feasibility is violated.
     */
    static bool checkFeasibilityConditions(const Simplex& simplex, bool print = true, Numerical::Double tolerance = 0);

    /**
     * Checks the optimality conditions (the feasibility of the reduced costs).
     *
     * @param simplex The Simplex object with the current state of the problem being solved.
     * @param print Prints information to the output if true.
     * @param tolerance The optimality tolerance.
     * @return true if no reduced cost's feasibility is violated.
     */
    static bool checkOptimalityConditions(const Simplex& simplex, bool print = true, Numerical::Double tolerance = 0);

    /**
     * Checks whether the dual feasibility sets (M,F,P) are correct.
     *
     * @param simplex The Simplex object with the current state of the problem being solved.
     * @param print Prints information to the output if true.
     * @param tolerance The optimality tolerance.
     * @return true if the feasibility sets are correct.
     */
    static bool checkFeasibilitySets(const Simplex& simplex, bool print=true, Numerical::Double tolerance = 0);

    /**
     * Checks wther the constraints of the problem are satisfied or not.
     *
     * @param simplex The Simplex object with the current state of the problem being solved.
     * @param print Prints information to the output if true.
     * @param tolerance The constraint feasibility tolerance.
     * @return true if all constriants are satisfied.
     */
    static bool checkAllConstraints(const Simplex& simplex, bool print = true, Numerical::Double tolerance = 0);

    /**
     * Checks the states of the nonbasic variables.
     * The check passes if the value of the nonbasic variables corresponds to the current state (lower/upper bound).
     *
     * @param simplex The Simplex object with the current state of the problem being solved.
     * @param print Prints information to the output if true.
     * @return true if the nonbasic variable's states are correct.
     */
    static bool checkNonbasicVariableStates(const Simplex& simplex, bool print = true);

    /**
     * Checks the state of the basic variables.
     * It passes if the state of the variables kept in the basishead is Simplex::BASIC.
     *
     * @param simplex The Simplex object with the current state of the problem being solved.
     * @param print Prints information to the output if true.
     * @return true if the basic variable's states are correct.
     */
    static bool checkBasicVariableStates(const Simplex& simplex, bool print = true);

    /**
     * Checks the feasibility sets M,F,P of the basic variables.
     *
     * @param simplex The Simplex object with the current state of the problem being solved.
     * @param print Prints information to the output if true.
     * @param tolerance feasibility tolerance
     * @return true if the feasibility sets are correct.
     */
    static bool checkBasicVariableFeasibilityStates(const Simplex& simplex, bool print = true, Numerical::Double tolerance = 0);

    /**
     * Checks the solution given by the postsolve with the original model.
     *
     * @param originalModel The original model before the presolve.
     * @param solution The solution yielded by the postsolve.
     * @param objectiveValue The objective value of the solution.
     * @return true if the postsolved solution is correct.
     */
    static bool checkPresolvedSolution(const Model& originalModel, const DenseVector & solution, Numerical::Double objectiveValue);
};

#endif // CHECKER_H
