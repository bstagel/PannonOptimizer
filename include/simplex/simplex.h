/**
 * @file simplex.h
 */


#ifndef SIMPLEX_H
#define	SIMPLEX_H

#include <globals.h>

#include <lp/method.h>
#include <simplex/simplexmodel.h>
#include <utils/numerical.h>
#include <utils/indexlist.h>

class StartingBasisFinder;

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
    inline const std::vector<int> & getBasisHead() const {return m_basisHead;}
    void getVariablesByState(VARIABLE_STATE state, IndexList::Iterator * begin, IndexList::Iterator * end) const;
    void getVariablesByFeasibility(VARIABLE_STATE state, IndexList::Iterator * begin, IndexList::Iterator * end) const;
    void getReducedCostsByFeasibility(VARIABLE_STATE state, IndexList::Iterator * begin, IndexList::Iterator * end) const;

    void setModel(const Model & model);

    void solve();
    void findStartingBasis();

protected:
    SimplexModel * m_simplexModel;

    std::vector<int> m_basisHead;
    IndexList m_variableStates;
    IndexList m_variableFeasibilities;
    IndexList m_reducedCostFeasibilities;
    Vector m_basicVariableValues;
    Vector m_reducedCosts;
    Numerical::Double m_objectiveValue;
    Numerical::Double m_phaseIObjectiveValue;

    //Modules
    StartingBasisFinder* m_startingBasisFinder;

    void constraintAdded();
    void variableAdded();

    virtual void initModules();
    virtual void releaseModules();
    virtual void iterate() = 0;
};

#endif /* SIMPLEX_H */
