#ifndef DUAL_RATIOTEST_H
#define DUAL_RATIOTEST_H

#include <iostream>
#include <vector>
#include <linalg/vector.h>
#include <linalg/matrix.h>
#include <utils/indexlist.h>
#include <simplex/dualfeasibilitychecker.h>

class Model;


class DualRatiotest{

    struct BreakPoints{
        unsigned int index;
        Numerical::Double value;
        Numerical::Double functionValue;
    };

public:
    DualRatiotest(const Model & model);
    virtual ~DualRatiotest();

    inline unsigned int getIncomingVariableIndex()const{return m_incomingVariableIndex;}
    inline unsigned int getOugoingVariableIndex()const{return m_outgoingVariableIndex;}
    inline Numerical::Double getPrimalSteplength()const{return m_primalSteplength;}
    inline Numerical::Double getDualSteplength()const{return m_dualSteplength;}
    inline const std::vector <unsigned int>& getBoundflip()const{return m_boundflips;}
    inline Numerical::Double getObjectiveFunctionPhase1()const{return m_objectiveFunctionPhase1;}
    inline Numerical::Double getObjectiveFunctionPhase2()const{return m_objectiveFunctionPhase2;}
    inline const std::vector<int>& getUpdateVector()const{return m_updateVector;}

    void performRatioTestPhase1(unsigned int outgoing,
                                const Vector& alpha,
                                const Vector& reducedCosts,
                                const IndexList<>& reducedCostFeasibilities,
                                const IndexList<const Numerical::Double*>& variableStates,
                                const DualFeasibilityChecker& feasibilityChecker,
                                Numerical::Double phaseIObjectiveValue
                                );
    void performRatiotestPhase2(unsigned int outgoing,
                                const Vector& alpha,
                                const Vector& reducedCosts,
                                Numerical::Double objVal,
                                const IndexList<const Numerical::Double*>& variableStates
                                );
private:
    Model const& m_model;
    unsigned int m_incomingVariableIndex;
    unsigned int m_outgoingVariableIndex;
    Numerical::Double m_dualSteplength;
    Numerical::Double m_primalSteplength;
    Numerical::Double m_objectiveFunctionPhase1;
    Numerical::Double m_objectiveFunctionPhase2;
    std::vector <unsigned int> m_boundflips;
    std::vector <BreakPoints> breakpoints;
    std::vector<int> m_updateVector;

    void shift(std::vector<BreakPoints>* breakpts, unsigned int startid, unsigned int stopid);
    void getNextElement(std::vector<BreakPoints>* breakpts, unsigned int length);

};
#endif // DUAL_RATIOTEST_H
