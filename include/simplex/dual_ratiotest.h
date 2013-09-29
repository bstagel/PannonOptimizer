#ifndef DUAL_RATIOTEST_H
#define DUAL_RATIOTEST_H

#include <iostream>
#include <vector>
#include <linalg/vector.h>
#include <linalg/matrix.h>
#include <utils/indexlist.h>
#include <simplex/dualfeasibilitychecker.h>

class Model;


class Dual_Ratiotest{

    typedef struct BreakPoints{
        unsigned int index;
        Numerical::Double value;
        Numerical::Double functionValue;
    }BreakPoints;

public:
    Dual_Ratiotest(const Model & model);
    virtual ~Dual_Ratiotest();

    inline unsigned int getIncomingVariableIndex()const{return m_incomingVariableIndex;}
    inline Numerical::Double getPrimalSteplength()const{return m_primalSteplength;}
    inline Numerical::Double getDualSteplength()const{return m_dualSteplength;}
    inline std::vector <unsigned int> getBoundflip()const{return m_boundflips;}
    void PerformRatioTestPhase1(unsigned int outgoing,
                                Vector alpha,
                                Vector reducedCosts,
                                const IndexList<>& reducedCostFeasibilities,
                                const IndexList<Numerical::Double>& variableStates,
                                const DualFeasibilityChecker& feasibilityChecker,
                                Numerical::Double* phaseIObjectiveValue
                                );
    void PerformRatiotestPhase2(unsigned int outgoing,
                                Vector alpha,
                                Vector reducedCosts,
                                Numerical::Double objVal,
                                const IndexList<Numerical::Double>& variableStates);

private:
    Model const& m_model;
    unsigned int m_incomingVariableIndex;
    Numerical::Double m_dualSteplength;
    Numerical::Double m_primalSteplength;
    std::vector <unsigned int> m_boundflips;
    std::vector <BreakPoints> breakpoints;

    void Shift(std::vector<BreakPoints>* breakpts, unsigned int startid, unsigned int stopid);
    void getNextElement(std::vector<BreakPoints>* breakpts, unsigned int length);

};
#endif // DUAL_RATIOTEST_H
