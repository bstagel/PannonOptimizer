#ifndef PRIMALRATIOTEST_H
#define PRIMALRATIOTEST_H

#include <vector>
#include <linalg/vector.h>
#include <linalg/matrix.h>
#include <utils/indexlist.h>
#include <simplex/primalfeasibilitychecker.h>
#include <utils/breakpointhandler.h>

class Model;

class PrimalRatiotest{

public:

    enum PRIMAL_RATIOTEST_METHOD{
        ONE_STEP = 0,
        PIECEWISE_LINEAR_FUNCTION,
        STABLE_PIVOT
    };

    PrimalRatiotest(const SimplexModel& model,
                    const Vector& basicVariableValues,
                    const std::vector<int>& basishead,
                    const IndexList<>& basicVariableFeasibilities,
                    const IndexList<const Numerical::Double*>& variableStates);

    inline unsigned int getOutgoingVariableIndex()const{return m_outgoingVariableIndex;}

    inline Numerical::Double getPrimalSteplength()const{return m_primalSteplength;}

    inline Numerical::Double getPhaseIObjectiveValue()const{return m_phaseIObjectiveValue;}

    inline Numerical::Double getPhaseIIObjectiveValue()const{return m_phaseIIObjectiveValue;}

    inline const std::vector <unsigned int>& getBoundflips()const{return m_boundflips;}

    void performRatiotestPhase1(int incomingVariableIndex,
                                const Vector& alpha,
                                Numerical::Double phaseIReducedCost,
                                Numerical::Double phaseIObjectiveValue);
    void performRatiotestPhase2(int incomingVariableIndex,
                                const Vector& alpha,
                                Numerical::Double reducedCost);

    inline bool outgoingAtUpperBound(){return m_outgoingAtUpperBound;}

private:
    const SimplexModel& m_model;
    const Vector& m_basicVariableValues;
    const std::vector<int>& m_basishead;
    const IndexList<>& m_basicVariableFeasibilities;
    const IndexList<const Numerical::Double*>& m_variableStates;

    bool m_outgoingAtUpperBound;

    int m_outgoingVariableIndex;
    Numerical::Double m_primalSteplength;
    Numerical::Double m_phaseIObjectiveValue;
    Numerical::Double m_phaseIIObjectiveValue;
    std::vector <unsigned int> m_boundflips;
    Numerical::Double m_initialPhaseIObjectiveValue;
    Numerical::Double m_initialPhaseIIObjectiveValue;
    BreakpointHandler m_breakpointHandler;
    bool m_tPositive;

    //Ratiotest research measures
    int m_stablePivotActivationPhase1;
    int m_stablePivotBackwardStepsPhase1;
    int m_stablePivotForwardStepsPhase1;
    int m_fakeFeasibilityActivationPhase1;
    int m_fakeFeasibilityCounterPhase1;

    int m_stablePivotNotFoundPhase2;
    int m_fakeFeasibilityActivationPhase2;
    int m_fakeFeasibilityCounterPhase2;

    //Parameter references
    const PRIMAL_RATIOTEST_METHOD m_nonlinearPrimalPhaseIFunction;
    const double & m_pivotTolerance;
    const int & m_enableFakeFeasibility;

    void generateBreakpointsPhase1(const Vector& alpha,
                                   int incomingVariableIndex);
    void computeFunctionPhase1(const Vector& alpha,
                               unsigned int &iterationCounter,
                               Numerical::Double functionSlope);
    void useNumericalThresholdPhase1(unsigned int iterationCounter,
                                     const Vector& alpha,
                                     Numerical::Double& functionSlope);
    void generateBreakpointsPhase2(const Vector& alpha);
};

#endif // PRIMALRATIOTEST_H
