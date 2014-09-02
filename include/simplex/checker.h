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
     * @param simplex holds the basishead, the matrix of the problem
     * @return true if the computed values are equal
     */
    static bool checkBasisWithFtran(const Simplex& simplex);

    /**
     * Checks the basis with Btran method.
     * It computes the basis(B) from the basishead and from the matrix of the problem and checks
     * whether they are equal or not.
     * @param simplex provides the basishead and the matrix
     * @return true if the computed values are equal
     */
    static bool checkBasisWithBtran(const Simplex& simplex);

    /**
     * Checks whether the reduced cost values of the basicvariables are zero or not.
     * The reduced cost vector can be computed as d = c_j - c_B^T * B^-1 * a_j.
     *
     * @param simplex provides the cost vector, basis and the matrix
     * @return true if the reduced costs of the basicvariables is zero.
     */
    static bool checkBasisWithReducedCost(const Simplex& simplex);

    /**
     * Checks the reduced cost values of the nonbasic variables.
     * It computes the values with Ftran and Btran, if the values are equal considering
     * some tolerance the check passes.
     * The reduced cost vector can be computed as d = c - c_B^T * B^-1 * a.
     * @param simplex provides the cost vector and basis and the matrix
     * @return true if the reduced cost values of the nonbasic variables is correct.
     */
    static bool checkBasisWithNonbasicReducedCost(const Simplex& simplex);

    /**
     * Checks whether the basicvariable values are correct.
     * The values of X_B vector are stored attached to the basicvariables, and as a separate (X_B) vector.
     * This function checks if there is any inconsitency among these values.
     * @param simplex provides the variables, and the X_B vector
     * @return true if the basicvariable values are correct.
     */
    static bool checkVariableStateAttachedValues(const Simplex& simplex);

    /**
     * Checks an alpha value with Ftran and Btran.
     * Computes alpha as a row and column vector and compares it.
     * @param simplex provides the matrix, the basis
     * @param rowIndex the row index shows the alpha row vector
     * @param columnIndex the column index shows the alpha column vector
     * @param columnAlpha the alpha value computed from a column vector
     * @param rowAlpha the alpha value computed from a row vector
     * @return true if the computed alpha values are equal.
     */
    static bool checkAlphaValue(const Simplex& simplex,
                               int rowIndex, int columnIndex, Numerical::Double & columnAlpha, Numerical::Double & rowAlpha);

    /**
     * Checks the primal theta value with a pivot computed with Ftran and Btran.
     * Computes the primal theta as (XB - LB) / alpha or (XB - UB) / alpha. The alpha values
     * are computed both from row and column vector.
     * @param simplex provides the basis and the matrix
     * @param rowIndex the row index shows the alpha row vector
     * @param columnIndex the column index shows the alpha column vector
     * @param thetaFromCol is the theta value computed with alpha column vector
     * @param thetaFromRow is the theta value computed with alpha row vector
     * @return true if the computed values are equal.
     */
    static bool checkPrimalTheta(const Simplex& simplex,
                                   int rowIndex, int columnIndex,
                                   Numerical::Double& thetaFromCol, Numerical::Double& thetaFromRow);

    /**
     * Checks the feasibility of the basicvariables considering the given tolerance.
     * @param simplex provides the basis and the matrix
     * @param print can be turned on to print information on screen
     * @param tolerance feasibility tolerance
     * @return true if no variable's feasibility is violated.
     */
    static bool checkFeasibilityConditions(const Simplex& simplex, bool print = true, Numerical::Double tolerance = 0);

    /**
     * Checks the optimality conditions (the feasibility of the reduced costs).
     * @param simplex provides the basis and the matrix
     * @param print can be turned on to pring information on the screen
     * @param tolerance the oprimality tolerance
     * @return true if no reduced cost's feasibility is violated.
     */
    static bool checkOptimalityConditions(const Simplex& simplex, bool print = true, Numerical::Double tolerance = 0);

    /**
     * Checks wther the constraints of the problem are satisfied or not.
     * @param simplex provied the constraints
     * @param print can be turned on to print information on screen
     * @param tolerance the tolerance determines the largest possible difference in which a constraint is satisfied
     * @return true if all constriants are satisfied.
     */
    static bool checkAllConstraints(const Simplex& simplex, bool print = true, Numerical::Double tolerance = 0);

    /**
     * Checks the states of the nonbasic variables.
     * The check passes if the value of the nonbasic variables corresponds to the current state (lower/upper bound).
     * @param simplex provides the variable states and nonbasic variables
     * @param print can be turned on to print information on screen
     * @return true if the nonbasic variable's states are correct.
     */
    static bool checkNonbasicVariableStates(const Simplex& simplex, bool print = true);

    /**
     * Checks the state of the basicvariables.
     * It passes if the state of the variables kept in the basishead is Simplex::BASIC.
     * @param simplex provides the variable states and basicvariables
     * @param print can be turned on to print information on screen
     * @return true if the basicvariable's states are correct.
     */
    static bool checkBasicVariableStates(const Simplex& simplex, bool print = true);

    /**
     * Checks the feasibility sets M,F,P of the basicvariables.
     * @param simplex provides the variables and the feasibility sets
     * @param print can be turned on to print information on the screen
     * @param tolerance feasibility tolerance
     * @return true if the feasibility sets are correct.
     */
    static bool checkBasicVariableFeasibilityStates(const Simplex& simplex, bool print = true, Numerical::Double tolerance = 0);
};

#endif // CHECKER_H
