/**
 * @file dualratiotest.h This file contains the API of the DualRatiotest class.
 */

#ifndef DUALRATIOTEST_H
#define DUALRATIOTEST_H

#include <vector>
#include <linalg/vector.h>
#include <linalg/matrix.h>
#include <utils/indexlist.h>
#include <simplex/dualfeasibilitychecker.h>
#include <utils/breakpointhandler.h>

class Model;

/**
 * The dual ratiotest determines the incoming variable, and the dual (primal) steplength.
 */
class DualRatiotest{
    friend class DualRatiotestTestSuite;

public:
    /**
     * This indicates the ratio choosing methods that are implemented in the class.
     */
    enum DUAL_RATIOTEST_METHOD{
        ONE_STEP = 0,
        PIECEWISE_LINEAR_FUNCTION,
        STABLE_PIVOT
    };

    /**
     * Initializing constructor of the class.
     * @param model provides information about the varaibles, bounds etc.
     * @param reducedCosts vector of the reduced costs so that the ratios can be defined
     * @param reducedCostFeasibilities defined by the DualFeasibilityChecker
     * @param variableStates to be able to check whether a variable is on its lower or upper bound
     *
     * @constructor
     */
    DualRatiotest(const SimplexModel & model,
                  const Vector& reducedCosts,
                  const IndexList<>& reducedCostFeasibilities,
                  const IndexList<const Numerical::Double*>& variableStates);

    /**
     * Default destructor of the class.
     *
     * @destructor
     */
    virtual ~DualRatiotest();

    /**
     * Getter of the updateFeasibilitySets member.
     * @return DualRatiotest::m_updateFeasibilitySets.
     */
    inline const std::vector<std::pair<int,char> >& getUpdateFeasibilitySets(){return m_updateFeasibilitySets;}

    /**
     * Getter of the m_becomesFeasible member.
     * @return DualRatiotest::m_becomesFeasible.
     */
    inline const std::vector<int>& getBecomesFeasible(){return m_becomesFeasible;}

    /**
     * Getter that returns the incoming variable's index.
     */
    inline int getIncomingVariableIndex()const{return m_incomingVariableIndex;}

    /**
     * Getter that returns the dual steplength.
     */
    inline Numerical::Double getDualSteplength()const{return m_dualSteplength;}

    /**
     * Getter that returns the phase 1 objective value.
     */
    inline Numerical::Double getObjectiveFunctionPhase1()const{return m_phaseIObjectiveValue;}

    /**
     * Getter that returns the phase 2 objective value.
     */
    inline Numerical::Double getObjectiveFunctionPhase2()const{return m_phaseIIObjectiveValue;}

    /**
     * Getter that returns with a vector containing the variables on which a boundflip operation shall be performed.
     */
    inline const std::vector <unsigned int>& getBoundflips()const{return m_boundflips;}

    /**
     * This function performs and controls the phase 1 dual ratiotest.
     * @param alpha needed to define ratios
     * @param phaseIReducedCost needed to define t > 0 and t < 0 cases
     * @param phaseIObjectiveValue will be computed to the ratios
     */
    void performRatiotestPhase1(const Vector& alpha,
                                Numerical::Double phaseIReducedCost,
                                Numerical::Double phaseIObjectiveValue);

    /**
     * This function performs and controls the phase 2 dual ratiotest.
     * @param outgoingVariableIndex shows the variable choosen by the pricing to leave the basis
     * @param alpha needed to define ratios
     * @param phaseIIObjectiveValue will be computed to the ratios
     * @param workingTolerance shows the currently used optimality tolerance
     *
     * @throws FallbackException if there is any infeasible variable
     * @throws DualUnboundedException if no breakpoint was found
     */
    void performRatiotestPhase2(unsigned int outgoingVariableIndex,
                                const Vector& alpha,
                                Numerical::Double phaseIIObjectiveValue,
                                Numerical::Double workingTolerance);

    //Ratiotest study
    /**
     * Getter for the ratiotest studies.
     * @return DualRatiotest::m_stablePivotActivationPhase1
     */
    inline int getStablePivotActivationPhase1() const {return m_stablePivotActivationPhase1;}

    /**
     * Getter for the ratiotest studies.
     * @return DualRatiotest::m_stablePivotBackwardStepsPhase1
     */
    inline int getStablePivotBackwardStepsPhase1() const {return m_stablePivotBackwardStepsPhase1;}

    /**
     * Getter for the ratiotest studies.
     * @return DualRatiotest::m_stablePivotForwardStepsPhase1
     */
    inline int getStablePivotForwardStepsPhase1() const {return m_stablePivotForwardStepsPhase1;}

    /**
     * Getter for the ratiotest studies.
     * @return DualRatiotest::m_fakeFeasibilityActivationPhase1
     */
    inline int getFakeFeasibilityActivationPhase1() const {return m_fakeFeasibilityActivationPhase1;}

    /**
     * Getter for the ratiotest studies.
     * @return DualRatiotest::m_fakeFeasibilityCounterPhase1
     */
    inline int getFakeFeasibilityCounterPhase1() const {return m_fakeFeasibilityCounterPhase1;}

    /**
     * Getter for the ratiotest studies.
     * @return DualRatiotest::m_stablePivotNotFoundPhase2
     */
    inline int getStablePivotNotFoundPhase2() const {return m_stablePivotNotFoundPhase2;}

    /**
     * Getter for the ratiotest studies.
     * @return DualRatiotest::m_fakeFeasibilityActivationPhase2
     */
    inline int getFakeFeasibilityActivationPhase2() const {return m_fakeFeasibilityActivationPhase2;}

    /**
     * Getter for the ratiotest studies.
     * @return DualRatiotest::m_fakeFeasibilityCounterPhase2
     */
    inline int getFakeFeasibilityCounterPhase2() const {return m_fakeFeasibilityCounterPhase2;}

    /**
     * Return whether the last iteration was degenerate or not.
     */
    inline int isDegenerate() const {return m_degenerate;}

private:
    /**
     * The model provides information about the varaibles, bounds etc.
     */
    const SimplexModel& m_model;

    /**
     * Vector of the reduced costs so that the ratios can be defined.
     */
    const Vector& m_reducedCosts;

    /**
     * Indexlist storing the feasibilities of the reduced costs defined by the DualFeasibilityChecker.
     */
    const IndexList<>& m_reducedCostFeasibilities;

    /**
     * Indexlist stroing variable states, to be able to check whether a variable is on its lower or upper bound.
     */
    const IndexList<const Numerical::Double*>& m_variableStates;

    /**
     * With this the t > 0, and t < 0 cases can easly handeled in the ratiotest.
     * At defining the ratios we compute with sigma*alpha values.
     */
    int m_sigma;

    /**
      * Age vector to record transformation counts.
      */
    std::vector<Numerical::Double> m_variableAge;

    /**
     * The variable index of the incoming variable.
     */
    int m_incomingVariableIndex;

    /**
     * The value of the dual steplength defined in the ratiotest.
     */
    Numerical::Double m_dualSteplength;

    /**
     * The dual phase 1 objective value, needed in the piecewise linear method.
     */
    Numerical::Double m_phaseIObjectiveValue;

    /**
     * The dual phase 2 objective value, needed in the piecewise linear method.
     */
    Numerical::Double m_phaseIIObjectiveValue;

    /**
     * Vector storing the variables (variable indices) on which a boundflip operation
     * shall be performed.
     */
    std::vector <unsigned int> m_boundflips;

    /**
     * The phase 1 objective value in the current iteration before the ratiotest is performed.
     */
    Numerical::Double m_initialPhaseIObjectiveValue;

    /**
     * The phase 2 objective value in the current iteration before the ratiotest is performed.
     */
    Numerical::Double m_initialPhaseIIObjectiveValue;

    /**
     * This class stores and provides the ratios and performs the sorting if necessary.
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
     * Reference of the run-time parameter "nonlinear_dual_phaseI_function",
     * see SimplexParameterHandler for details.
     */
    const DUAL_RATIOTEST_METHOD m_nonlinearDualPhaseIFunction;

    /**
     * Reference of the run-time parameter "nonlinear_dual_phaseII_function",
     * see SimplexParameterHandler for details.
     */
    const DUAL_RATIOTEST_METHOD m_nonlinearDualPhaseIIFunction;

    /**
     * Reference of the "e_optimality" tolerance run-time parameter,
     * see SimplexParameterHandler for details.
     */
    const double & m_optimalityTolerance;

    /**
     * Reference of the "e_pivot" tolerance run-time parameter,
     * see SimplexParameterHandler for details.
     */
    const double & m_pivotTolerance;

    /**
     * Reference of the "enable_fake_feasibility" run-time parameter,
     * see SimplexParameterHandler for details.
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
     * This contains the directions of the phase 1 ratios.
     * If the variable moves up from set M or F it stores 1,
     * otherwise if it moves down from set P or F it stores 0.
     */
    std::vector<char> m_ratioDirections;

    /**
     * This vector stores the changes in the feasibilities.
     * If the variable moves from set F to M it stores the variableindex and -1 as a pair.
     */
    std::vector<std::pair<int,char> > m_updateFeasibilitySets;

    /**
     * This vector contains the variable idices of explicit feasible variables.
     */
    std::vector<int> m_becomesFeasible;

    /**
     * Holds whether the last iteration was degenerate or not.
     */
    bool m_degenerate;

    /**
     * This function computes the ratios in dual phase 1.
     * @param alpha the reduced cost values are devided by the alpha values
     */
    void generateSignedBreakpointsPhase1(const Vector& alpha);

    /**
     * This function computes the piecewise linear concave function in dual phase 1.
     * If it is called the BreakpointHandler performs the necessary sorting operations.
     * @param alpha the reduced cost values are devided by the alpha values
     * @param iterationCounter shows how many breakpoints are computed
     * @param functionSlope is the value of the objective function's slope
     */
    void computeFunctionPhase1(const Vector& alpha,
                               unsigned int& iterationCounter,
                               Numerical::Double& functionSlope);

    /**
     * This is the phase 1 stable pivot procedure that provides the solver a numerically stable pivot candidate.
     * @param iterationCounter shows the maximum of the piecewise linear function
     * @param alpha the reduced cost values are devided by the alpha values
     * @param functionSlope is the value of the objective function's slope
     */
    void useNumericalThresholdPhase1(unsigned int iterationCounter,
                               const Vector& alpha,
                               Numerical::Double& functionSlope);

    /**
     * This function computes the ratios in dual phase 2.
     * @param alpha the reduced cost values are devided by the alpha values
     */
    void generateSignedBreakpointsPhase2(const Vector& alpha);

    /**
     * With this function we can define ratios corresponding to the expanding tolerance.
     * @param alpha the reduced cost values are devided by the alpha values
     * @param workingTolerance shows the value of the expanding tolerance in the current iteration
     * @param secondTurn is an optional parameter for the second pass of the expand procedure
     */
    void generateExpandedBreakpointsPhase2(const Vector& alpha,
                                           Numerical::Double workingTolerance);

    /**
     * This function computes the piecewise linear concave function in dual phase 2.
     * If it is called the BreakpointHandler performs the necessary sorting operations.
     * @param alpha the reduced cost values are devided by the alpha values
     * @param iterationCounter shows how many breakpoints are computed
     * @param functionSlope is the value of the objective function's slope
     * @param workingTolerance The actual expanding tolerance.
     */
    void computeFunctionPhase2(const Vector& alpha,
                               unsigned int& iterationCounter,
                               Numerical::Double& functionSlope,
                               Numerical::Double workingTolerance);

    /**
     * This is the phase 2 stable pivot procedure that provides the solver a numerically stable pivot candidate.
     * @param iterationCounter shows the maximum of the piecewise linear function
     * @param alpha the reduced cost values are devided by the alpha values
     * @param functionSlope is the value of the objective function's slope
     */
    void useNumericalThresholdPhase2(unsigned int iterationCounter,
                                     const Vector& alpha);

    static DUAL_RATIOTEST_METHOD getDualRatiotestMethod(const std::string & method) {
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
#endif // DUALRATIOTEST_H
