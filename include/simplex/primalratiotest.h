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
     *
     * @param model The simplex model of the LP problem.
     * @param basicVariableValues The X_B vector.
     * @param basishead The basis head storing the basic variable indices.
     * @param basicVariableFeasibilities Index list storing primal feasibilities, defined by PrimalFeasibilityChecker.
     * @param variableStates Index list containing variable states to check whether a variable is on its lower or upper bound.
     *
     * @constructor
     */
    PrimalRatiotest(const SimplexModel& model,
                    const Vector& basicVariableValues,
                    const std::vector<int>& basishead,
                    const IndexList<>& basicVariableFeasibilities,
                    const IndexList<const Numerical::Double*>& variableStates);

    /**
     * Default destructor of the class.
     *
     * @destructor
     */
    virtual ~PrimalRatiotest(){}

    /**
     * Getter of the basis index of the outgoing variable.
     *
     * @return PrimalRatiotest::m_outgoingVariableIndex.
     */
    inline unsigned int getOutgoingVariableIndex()const{return m_outgoingVariableIndex;}

    /**
     * Getter of the primal theta value.
     *
     * @return PrimalRatiotest::m_primalSteplength.
     */
    inline Numerical::Double getPrimalSteplength()const{return m_primalSteplength;}

    /**
     * Getter of the phase 1 primal objective value.
     *
     * @return PrimalRatiotest::m_phaseIObjectiveValue.
     */
    inline Numerical::Double getPhaseIObjectiveValue()const{return m_phaseIObjectiveValue;}

    /**
     * Getter of the vector containing the boundflipping variables.
     *
     * @return PrimalRatiotest::m_boundflips.
     */
    inline const std::vector <unsigned int>& getBoundflips()const{return m_boundflips;}

    /**
     * Performs and controls the phase 1 primal ratiotest.
     *
     * @param incomingVariableIndex The index of the incoming variable.
     * @param alpha The alpha value needed to define ratios.
     * @param phaseIReducedCost The phase 1 reduced cost needed to define t > 0 and t < 0 cases.
     * @param phaseIObjectiveValue The current phase 1 objective value.
     */
    void performRatiotestPhase1(int incomingVariableIndex, const Vector& alpha, Numerical::Double phaseIReducedCost, Numerical::Double phaseIObjectiveValue);

    /**
     * Performs and controls the phase 2 primal ratiotest.
     *
     * @param incomingVariableIndex The index of the incoming variable.
     * @param alpha The alpha value needed to define ratios.
     * @param reducedCost The reduces cost needed to define t > 0 and t < 0 cases.
     */
    void performRatiotestPhase2(int incomingVariableIndex, const Vector& alpha, Numerical::Double reducedCost, Numerical::Double workingTolerance);

    /**
     * Getter of the outgoingAtUpperBound member.
     *
     * @return PrimalRatiotest::m_outgoingAtUpperBound
     */
    inline bool outgoingAtUpperBound(){return m_outgoingAtUpperBound;}

    /**
     * Returns whether the last iteration was degenerate or not.
     *
     * @return 0 if the last iteration was not degenerate, otherwise not 0.
     */
    inline int isDegenerate() const {return m_degenerate;}

private:

    /**
     * The simplex model of the problem.
     */
    const SimplexModel& m_model;

    /**
     * This vector(X_B) stores the basic variable values.
     */
    const Vector& m_basicVariableValues;

    /**
     * The basis head holds the variable indices of the basic variables.
     */
    const std::vector<int>& m_basishead;

    /**
     * Index list contains the feasibility sets of the basic variables.
     */
    const IndexList<>& m_basicVariableFeasibilities;

    /**
     * Index list containing the variable states (being at lower upper bound).
     */
    const IndexList<const Numerical::Double*>& m_variableStates;

    /**
     * With this the t > 0, and t < 0 cases can easly handeled in the ratiotest.
     * At defining the ratios we compute with sigma*alpha values.
     */
    int m_sigma;

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
     * Reference of the run-time parameter "nonlinear_primal_phaseI_function".
     *
     * @see SimplexParameterHandler
     */
    const PRIMAL_RATIOTEST_METHOD m_nonlinearPrimalPhaseIFunction;

    /**
     * Reference of the "e_feasibility" tolerance run-time parameter,
     * see SimplexParameterHandler for details.
     */
    const double & m_feasibilityTolerance;

    /**
     * Reference of the "e_pivot" tolerance run-time parameter.
     *
     * @see SimplexParameterHandler
     */
    const double & m_pivotTolerance;

    /**
     * Reference of the "enable_fake_feasibility" run-time parameter.
     *
     * @see SimplexParameterHandler
     */
    const bool & m_enableFakeFeasibility;

    /**
     * Parameter reference of run-time parameter "expand",
     * see SimplexParameterHandler for details.
     */
    const std::string & m_expand;

    /**
     * This is the value of the tolerance increment in the expand procedure.
     */
    Numerical::Double m_toleranceStep;

    /**
     * Describes whether the last iteration was degenerate or not.
     */
    bool m_degenerate;

    /**
     * This function computes the ratios in primal phase 1.
     *
     * @param alpha The alpha values which are the denumerator of the ratios.
     * @param incomingVariableIndex The index of the incoming variable.
     */
    void generateSignedBreakpointsPhase1(const Vector& alpha, int incomingVariableIndex);

    /**
     * This function computes the piecewise linear concave function in primal phase 1.
     *
     * @param alpha The alpha values.
     * @param iterationCounter The counter for how many breakpoints are computed.
     * @param functionSlope The value of the objective function's slope.
     */
    void computeFunctionPhase1(const Vector& alpha, unsigned int &iterationCounter, Numerical::Double functionSlope);

    /**
     * This is the phase 1 stable pivot procedure that provides the solver a numerically stable pivot candidate.
     *
     * @param iterationCounter The maximum of the piecewise linear function.
     * @param alpha The alpha values.
     * @param functionSlope The value of the objective function's slope.
     */
    void useNumericalThresholdPhase1(unsigned int iterationCounter, const Vector& alpha, Numerical::Double& functionSlope);

    /**
     * This function generates the ratios in phase 2.
     *
     * @param alpha The alpha values which are the denumerator of the ratios
     */
    void generateSignedBreakpointsPhase2(const Vector& alpha);

    /**
     * With this function we can define ratios corresponding to the expanding tolerance.
     *
     * @param alpha The alpha vector.
     * @param workingTolerance The value of the expanding tolerance in the current iteration.
     */
    void generateExpandedBreakpointsPhase2(const Vector& alpha, Numerical::Double workingTolerance);

    /**
     * Converts a string describing a primal ratiotest method to the method describing enum.
     *
     * @param method The string to be converted.
     * @return The dual ratiotest method in PrimalRatiotest::Primal_RATIOTEST_METHOD enum.
     */
    static PRIMAL_RATIOTEST_METHOD getPrimalRatiotestMethod(const std::string & method) {
        if (method == "TRADITIONAL") {
            return ONE_STEP;
        }
        if (method == "PIECEWISE") {
            return PIECEWISE_LINEAR_FUNCTION;
        }
        if (method == "PIECEWISE_THRESHOLD") {
            return STABLE_PIVOT;
        }
        // TODO: exception
        return ONE_STEP;
    }
};

#endif // PRIMALRATIOTEST_H
