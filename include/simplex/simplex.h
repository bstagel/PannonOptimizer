/**
 * @file simplex.h
 */


#ifndef SIMPLEX_H
#define	SIMPLEX_H

#include "globals.h"

#include "lp/method.h"
#include "simplex/simplexmodel.h"
#include "utils/numerical.h"
#include "utils/indexlist.h"

class Simplex : public Method
{
public:

    enum FEASIBILITY
    {
        FEASIBLE,
        MINUS,
        PLUS
    };

    enum VARIABLE_STATE
    {
        BASIC,
        NONBASIC_AT_LB,
        NONBASIC_AT_UB
    };

    Simplex();
    virtual ~Simplex();

    inline const Numerical::Double & getObjectiveValue() const {return m_objectiveValue;}
    inline const std::vector<unsigned int> & getBasisHead() const {return m_basisHead;}
    void getVariablesByState(VARIABLE_STATE state, IndexList::Iterator * begin, IndexList::Iterator * end) const;
    void getVariablesByFeasibility(VARIABLE_STATE state, IndexList::Iterator * begin, IndexList::Iterator * end) const;
    void getReducedCostsByFeasibility(VARIABLE_STATE state, IndexList::Iterator * begin, IndexList::Iterator * end) const;

    void setModel(const Model & model);

private:
    Numerical::Double m_objectiveValue;
    SimplexModel * m_simplexModel;

    std::vector<unsigned int> m_basisHead;
    IndexList m_variableStates;
    IndexList m_variableFeasibilities;
    IndexList m_reducedCostFeasibilities;

    void constraintAdded();
    void variableAdded();
};

#endif /* SIMPLEX_H */
