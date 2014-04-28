/**
 * @file dualratiotest.h
 */

#ifndef DUALRATIOTEST_H
#define DUALRATIOTEST_H

#include <vector>
#include <linalg/vector.h>
#include <linalg/matrix.h>
#include <utils/indexlist.h>
#include <simplex/dualfeasibilitychecker.h>
#include <simplex/dualratiotestupdater.h>
#include <utils/breakpointhandler.h>

class Model;

class DualRatiotest{
    friend class DualRatiotestTestSuite;

public:
    enum DUAL_RATIOTEST_METHOD{
        ONE_STEP = 0,
        PIECEWISE_LINEAR_FUNCTION,
        STABLE_PIVOT
    };

    DualRatiotest(const SimplexModel & model,
                  const Vector& reducedCosts,
                  const IndexList<>& reducedCostFeasibilities,
                  const IndexList<const Numerical::Double*>& variableStates,
                  DualRatiotestUpdater& dualRatiotestUpdater);
    virtual ~DualRatiotest();

    inline int getIncomingVariableIndex()const{return m_incomingVariableIndex;}

    inline Numerical::Double getDualSteplength()const{return m_dualSteplength;}

    inline Numerical::Double getObjectiveFunctionPhase1()const{return m_phaseIObjectiveValue;}

    inline Numerical::Double getObjectiveFunctionPhase2()const{return m_phaseIIObjectiveValue;}

    inline const std::vector <unsigned int>& getBoundflips()const{return m_boundflips;}

    void performRatiotestPhase1(const Vector& alpha,
                                Numerical::Double phaseIReducedCost,
                                Numerical::Double phaseIObjectiveValue);

    void performRatiotestPhase2(unsigned int outgoingVariableIndex,
                                const Vector& alpha,
                                Numerical::Double phaseIIObjectiveValue);

    inline int getStablePivotActivationPhase1() const {return m_stablePivotActivationPhase1;}
    inline int getStablePivotBackwardStepsPhase1() const {return m_stablePivotBackwardStepsPhase1;}
    inline int getStablePivotForwardStepsPhase1() const {return m_stablePivotForwardStepsPhase1;}
    inline int getFakeFeasibilityActivationPhase1() const {return m_fakeFeasibilityActivationPhase1;}
    inline int getFakeFeasibilityCounterPhase1() const {return m_fakeFeasibilityCounterPhase1;}
    inline int getStablePivotNotFoundPhase2() const {return m_stablePivotNotFoundPhase2;}
    inline int getFakeFeasibilityActivationPhase2() const {return m_fakeFeasibilityActivationPhase2;}
    inline int getFakeFeasibilityCounterPhase2() const {return m_fakeFeasibilityCounterPhase2;}

private:
    const SimplexModel& m_model;
    const Vector& m_reducedCosts;
    const IndexList<>& m_reducedCostFeasibilities;
    const IndexList<const Numerical::Double*>& m_variableStates;
    bool m_tPositive;

    //Age vector to record transformation counts
    std::vector<Numerical::Double> m_variableAge;
    Numerical::Double m_ageStep;

    DualRatiotestUpdater& m_dualRatiotestUpdater;
    int m_incomingVariableIndex;
    Numerical::Double m_dualSteplength;
    Numerical::Double m_phaseIObjectiveValue;
    Numerical::Double m_phaseIIObjectiveValue;
    std::vector <unsigned int> m_boundflips;
    Numerical::Double m_initialPhaseIObjectiveValue;
    Numerical::Double m_initialPhaseIIObjectiveValue;
    BreakpointHandler m_breakpointHandler;

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
    const DUAL_RATIOTEST_METHOD m_nonlinearDualPhaseIFunction;
    const DUAL_RATIOTEST_METHOD m_nonlinearDualPhaseIIFunction;
    const double & m_optimalityTolerance;
    const double & m_pivotTolerance;
    const int & m_enableFakeFeasibility;

    void generateSignedBreakpointsPhase1(const Vector& alpha);
    void computeFunctionPhase1(const Vector& alpha,
                               unsigned int& iterationCounter,
                               Numerical::Double& functionSlope);
    void useNumericalThresholdPhase1(unsigned int iterationCounter,
                               const Vector& alpha,
                               Numerical::Double& functionSlope);
    bool numericalCheck(const Vector& alpha, unsigned int alphaId);

    void generateSignedBreakpointsPhase2(const Vector& alpha);
    void computeFunctionPhase2(const Vector& alpha,
                               unsigned int& iterationCounter,
                               Numerical::Double& functionSlope);
    void useNumericalThresholdPhase2(unsigned int iterationCounter,
                               const Vector& alpha);
};
#endif // DUALRATIOTEST_H
