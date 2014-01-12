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

class Model;

class DualRatiotest{
    friend class DualRatiotestTestSuite;

    struct BreakPoint{
        int index;
        Numerical::Double value;
        Numerical::Double functionValue;

        friend ostream & operator<<(ostream & os, const BreakPoint & breakpoint)
        {
            os << "(" << breakpoint.index << "; " << breakpoint.value << "; " << breakpoint.functionValue << ")";
            return os;
        }
        bool operator !=(const BreakPoint& other){
            return !( (other.index==index)&&(other.value==value)&&(other.functionValue==functionValue) );
        }
    };

public:
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
                                Numerical::Double phaseIObjectiveValue,
                                Numerical::Double expandingTolerance)
                                throw (NumericalException);

    void performRatiotestPhase2(unsigned int outgoingVariableIndex,
                                const Vector& alpha,
                                Numerical::Double phaseIIObjectiveValue)
                                throw (NumericalException, DualUnboundedException);

private:
    const SimplexModel& m_model;
    const Vector& m_reducedCosts;
    const IndexList<>& m_reducedCostFeasibilities;
    const IndexList<const Numerical::Double*>& m_variableStates;
    bool m_tPositive;
    bool m_transform;

    DualRatiotestUpdater& m_dualRatiotestUpdater;
    int m_incomingVariableIndex;
    Numerical::Double m_dualSteplength;
    Numerical::Double m_phaseIObjectiveValue;
    Numerical::Double m_phaseIIObjectiveValue;
    std::vector <unsigned int> m_boundflips;
    std::vector <BreakPoint> m_breakpoints;
    Numerical::Double m_pivotThreshold;

    //EXPAND
//    void generateExpandedBreakpointsPhase1(const Vector& alpha,
//                                           Numerical::Double phaseIReducedCost,
//                                           Numerical::Double phaseIObjectiveValue,
//                                           Numerical::Double expandingTolerance);

    void shift(std::vector<BreakPoint>* breakpoints, unsigned int startid, unsigned int stopid);
    void getNextElement(std::vector<BreakPoint>* breakpoints, unsigned int length);
    bool numericalCheckPhase1(const Vector& alpha, unsigned int alphaId);
    void generateBreakpointsPhase1(const Vector& alpha,
                                   Numerical::Double phaseIReducedCost,
                                   Numerical::Double phaseIObjectiveValue);
    void computeFunctionPhase1(const Vector& alpha,
                               unsigned int& iterationCounter,
                               Numerical::Double& functionSlope);
    void useNumericalThresholdPhase1(unsigned int iterationCounter,
                               const Vector& alpha,
                               Numerical::Double& functionSlope);
    void generateBreakpointsPhase2(unsigned int outgoingVariableIndex,
                                   const Vector& alpha,
                                   Numerical::Double phaseIIObjectiveValue);
    void computeFunctionPhase2(const Vector& alpha,
                               unsigned int& iterationCounter,
                               Numerical::Double& functionSlope,
                               Numerical::Double& primalSteplength);
    void useNumericalThresholdPhase2(unsigned int iterationCounter,
                               const Vector& alpha,
                               const Numerical::Double primalSteplength);
};
#endif // DUALRATIOTEST_H
