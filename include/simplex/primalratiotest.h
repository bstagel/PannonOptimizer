/**
 * @file primalratiotest.h This file contains the API of the PrimalRatiotest class.
 */
#ifndef PRIMALRATIOTEST_H
#define PRIMALRATIOTEST_H

#include <vector>
#include <linalg/vector.h>
#include <linalg/matrix.h>
#include <utils/indexlist.h>
#include <simplex/primalfeasibilitychecker.h>
#include <utils/breakpointhandler.h>

class Model;

/**
 * The primal ratio test determines the outgoing variable, and the primal steplength.
 *
 * @class PrimalRatiotest
 */
class PrimalRatiotest{
public:

    /**
     * This indicates the ratio choosing methods that are implemented in the class.
     */
    enum PRIMAL_RATIOTEST_METHOD{
        ONE_STEP = 0,
        PIECEWISE_LINEAR_FUNCTION,
        STABLE_PIVOT
    };

    /**
     * Initialiting constructor of the class.
     * @param model provides information about the variables, bounds etc
     * @param basicVariableValues stores the X_B vector
     * @param basishead stores the basicvariable indices
     * @param basicVariableFeasibilities linked list storing primal feasibility, defined by PrimalFeasibilityChecker
     * @param variableStates to be able to check whether a variable is on its lower or upper bound
     *
     * @constructor
     */
    PrimalRatiotest(const SimplexModel& model,
                    const Vector& basicVariableValues,
                    const std::vector<int>& basishead,
                    const IndexList<>& basicVariableFeasibilities,
                    const IndexList<const Numerical::Double*>& variableStates);

    /**
     * Getter of the basis index of the outgoing variable.
     * @return PrimalRatiotest::m_outgoingVariableIndex.
     */
    inline unsigned int getOutgoingVariableIndex()const{return m_outgoingVariableIndex;}

    /**
     * Getter of the primal theta value.
     * @return PrimalRatiotest::m_primalSteplength.
     */
    inline Numerical::Double getPrimalSteplength()const{return m_primalSteplength;}

    /**
     * Getter of the phase 1 primal objective value.
     * @return PrimalRatiotest::m_phaseIObjectiveValue.
     */
    inline Numerical::Double getPhaseIObjectiveValue()const{return m_phaseIObjectiveValue;}

    /**
     * Getter of the phase 2 primal objective value.
     * @return PrimalRatiotest::m_phaseIIObjectiveValue.
     */
    inline Numerical::Double getPhaseIIObjectiveValue()const{return m_phaseIIObjectiveValue;}

    /**
     * Getter of the vector containing the boundflipping variables.
     * @return PrimalRatiotest::m_boundflips.
     */
    inline const std::vector <unsigned int>& getBoundflips()const{return m_boundflips;}

    /**
     * This function performs and controls the phase 1 primal ratiotest.
     * @param incomingVariableIndex if the incoming variable is blocked by its bound it defines a ratio
     * @param alpha needed to define ratios
     * @param phaseIReducedCost needed to define t > 0 and t < 0 cases
     * @param phaseIObjectiveValue will be computed to the ratios
     */
    void performRatiotestPhase1(int incomingVariableIndex,
                                const Vector& alpha,
                                Numerical::Double phaseIReducedCost,
                                Numerical::Double phaseIObjectiveValue);

    /**
     * This function performs and controls the phase 2 primal ratiotest.
     * @param incomingVariableIndex if the incoming variable is blocked by its bound it defines a ratio
     * @param alpha needed to define ratios
     * @param reducedCost needed to define t > 0 and t < 0 cases
     */
    void performRatiotestPhase2(int incomingVariableIndex,
                                const Vector& alpha,
                                Numerical::Double reducedCost);

    /**
     * Getter of the outgoingAtUpperBound member.
     * @return PrimalRatiotest::m_outgoingAtUpperBound
     */
    inline bool outgoingAtUpperBound(){return m_outgoingAtUpperBound;}

private:

    /**
     * The model provides information about the LP problem such as the variable bounds.
     */
    const SimplexModel& m_model;

    /**
     * This vector(X_B) stores the basicvariable values.
     */
    const Vector& m_basicVariableValues;

    /**
     * The basis head holds the variable indices of the basicvariables.
     */
    const std::vector<int>& m_basishead;

    /**
     * This indexlist contains the feasibility sets of the basicvariables.
     */
    const IndexList<>& m_basicVariableFeasibilities;

    /**
     * Indexlist containing the variable states (being at lower upper bound).
     */
    const IndexList<const Numerical::Double*>& m_variableStates;

    /**
     * This variable is true if the outgoing variable is leaving at its upper bound.
     */
    bool m_outgoingAtUpperBound;

    /**
     * Basis index of the outgoing variable.
     */
    int m_outgoingVariableIndex;

    /**
     * Value of the primal theta(steplength).
     */
    Numerical::Double m_primalSteplength;

    /**
     * The phase 1 objective value is the sum of primal infeasibilities.
     */
    Numerical::Double m_phaseIObjectiveValue;

    /**
     * The phase 2 objective is the objective function of the LP problem.
     */
    Numerical::Double m_phaseIIObjectiveValue;

    /**
     * Vector containing the boundflipping variables.
     */
    std::vector <unsigned int> m_boundflips;

    /**
     * The initial value of the phase 1 objective function is needed if it was perturbed.
     */
    Numerical::Double m_initialPhaseIObjectiveValue;

    /**
     * The initial value of the phase 2 objective function is needed if it was perturbed.
     */
    Numerical::Double m_initialPhaseIIObjectiveValue;

    /**
     * The breakpointHandler provides the ratios to the ratiotest.
     */
    BreakpointHandler m_breakpointHandler;

    /**
     * Bool representing the t > 0 and the t < 0 cases.
     */
    bool m_tPositive;

    //Ratiotest research measures
    /**
     * Counter for the ratiotest studies.
     * It counts the number of activating the stable pivot procedure in phase1.
     */
    int m_stablePivotActivationPhase1;

    /**
     * Counter for the ratiotest studies.
     * It counts the number of steps taken left from the top of the piecewise linear function.
     */
    int m_stablePivotBackwardStepsPhase1;

    /**
     * Counter for the ratiotest studies.
     * It counts the number of steps taken right from the top of the piecewise linear function.
     */
    int m_stablePivotForwardStepsPhase1;

    /**
     * Counter for the ratiotest studies.
     * It's incremented if fake feasible variables are found in phase 1.
     */
    int m_fakeFeasibilityActivationPhase1;

    /**
     * Counter for the ratiotest studies.
     * It computes the number of fake feasible variables.
     */
    int m_fakeFeasibilityCounterPhase1;

    /**
     * Counter for the ratiotest studies.
     * It's incremented if the stable pivot procedure hasn't find a good pivot in phase 2.
     */
    int m_stablePivotNotFoundPhase2;

    /**
     * Counter for the ratiotest studies.
     * It's incremented if fake feasible variables are found in phase 2.
     */
    int m_fakeFeasibilityActivationPhase2;

    /**
     * Counter for the ratiotest studies.
     * It computes the number of fake feasible variables.
     */
    int m_fakeFeasibilityCounterPhase2;

    //Parameter references
    /**
     * Reference of the run-time parameter "nonlinear_primal_phaseI_function",
     * see SimplexParameterHandler for details.
     */
    const PRIMAL_RATIOTEST_METHOD m_nonlinearPrimalPhaseIFunction;

    /**
     * Reference of the "e_pivot" tolerance run-time parameter,
     * see SimplexParameterHandler for details.
     */
    const double & m_pivotTolerance;

    /**
     * Reference of the "enable_fake_feasibility" run-time parameter,
     * see SimplexParameterHandler for details.
     */
    const int & m_enableFakeFeasibility;

    /**
     * This function computes the ratios in primal phase 1.
     * @param alpha alpha values are the denumerator of the ratios
     * @param incomingVariableIndex in case if the incoming variable is blocked by its bound it defines a ratio
     */
    void generateBreakpointsPhase1(const Vector& alpha, int incomingVariableIndex);

    /**
     * This function computes the piecewise linear concave function in primal phase 1.
     * @param alpha the basicvariable values are devided with the alpha values
     * @param iterationCounter shows how many breakpoints are computed
     * @param functionSlope is the value of the objective function's slope
     */
    void computeFunctionPhase1(const Vector& alpha, unsigned int &iterationCounter, Numerical::Double functionSlope);

    /**
     * This is the phase 1 stable pivot procedure that provides the solver a numerically stable pivot candidate.
     * @param iterationCounter shows the maximum of the piecewise linear function
     * @param alpha the basicvariable values are devided with the alpha values
     * @param functionSlope is the value of the objective function's slope
     */
    void useNumericalThresholdPhase1(unsigned int iterationCounter, const Vector& alpha, Numerical::Double& functionSlope);

    /**
     * This function generates the ratios in phase 2.
     * @param alpha alpha values are the denumerator of the ratios
     */
    void generateBreakpointsPhase2(const Vector& alpha);
};

#endif // PRIMALRATIOTEST_H
