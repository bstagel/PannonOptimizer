#ifndef PRIMALRATIOTEST_H
#define PRIMALRATIOTEST_H

#include <vector>
#include <linalg/vector.h>
#include <linalg/matrix.h>
#include <utils/indexlist.h>
#include <simplex/primalfeasibilitychecker.h>
#include <simplex/primalratiotestupdater.h>

class Model;

class PrimalRatiotest{
public:
    PrimalRatiotest(const SimplexModel& model,
                    const IndexList<>& variableFeasibilities,
                    const IndexList<const Numerical::Double*>& variableStates);

    inline unsigned int getOutgoingVariableIndex()const{return m_outgoingVariableIndex;}

    inline Numerical::Double getPrimalSteplength()const{return m_primalSteplength;}

    inline Numerical::Double getDualSteplength()const{return m_dualSteplength;}

    inline Numerical::Double getObjectiveFunctionPhase1()const{return m_objectiveFunctionPhase1;}

    inline Numerical::Double getObjectiveFunctionPhase2()const{return m_objectiveFunctionPhase2;}

    void performRatiotestPhase1();
    void performRatiotestPhase2();

private:
    const SimplexModel& m_model;
    const IndexList<>& m_variableFeasibilities;
    const IndexList<const Numerical::Double*>& m_variableStates;

    unsigned int m_outgoingVariableIndex;
    Numerical::Double m_dualSteplength;
    Numerical::Double m_primalSteplength;
    Numerical::Double m_objectiveFunctionPhase1;
    Numerical::Double m_objectiveFunctionPhase2;
};

#endif // PRIMALRATIOTEST_H
