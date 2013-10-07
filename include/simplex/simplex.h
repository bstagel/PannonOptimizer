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
class Basis;

class Simplex : public Method
{
public:

    enum FEASIBILITY
    {
        FEASIBLE = 0,
        MINUS,
        PLUS,
        FEASIBILITY_ENUM_LENGTH
    };

    enum VARIABLE_STATE
    {
        BASIC = 0,
        NONBASIC_AT_LB,
        NONBASIC_AT_UB,
        NONBASIC_FREE,
        VARIABLE_STATE_ENUM_LENGTH
    };

    Simplex();
    virtual ~Simplex();

    inline const Numerical::Double & getObjectiveValue() const {return m_objectiveValue;}
    inline const Numerical::Double & getPhaseIObjectiveValue() const {return m_phaseIObjectiveValue;}
    inline const std::vector<int> & getBasisHead() const {return m_basisHead;}

    //TODO Ezt hogyan implementaljuk?
    const std::vector<Numerical::Double> getPrimalSolution() const;
    const std::vector<Numerical::Double> getDualSolution() const;


    void setModel(const Model & model);

    void solve();
    void findStartingBasis();

protected:
    SimplexModel * m_simplexModel;

    std::vector<int> m_basisHead;
    //Template parameter holds the value of the variable
    IndexList<const Numerical::Double*> m_variableStates;
    IndexList<> m_variableFeasibilities;
    IndexList<> m_reducedCostFeasibilities;
    Vector m_basicVariableValues;
    Vector m_reducedCosts;
    Numerical::Double m_objectiveValue;
    Numerical::Double m_phaseIObjectiveValue;

    bool m_baseChanged;

    //Modules
    StartingBasisFinder* m_startingBasisFinder;
    Basis* m_basis;

    void constraintAdded();
    void variableAdded();

    virtual void initModules();
    virtual void releaseModules();

    void reinvert() throw (NumericalException);
    void computeBasicSolution() throw (NumericalException);
    void computeReducedCosts() throw (NumericalException);
    virtual void computeFeasibility() throw (NumericalException) = 0;
    virtual void checkFeasibility() throw (OptimizationResultException, NumericalException) = 0;
    virtual void price() throw (OptimizationResultException, NumericalException) = 0;
    virtual void selectPivot() throw (OptimizationResultException, NumericalException) = 0;
    virtual void update()throw (NumericalException)  = 0;

    void transform(unsigned int incomingIndex,
                   int outgoingIndex,
                   const std::vector<unsigned int>& boundflips,
                   Numerical::Double primalTheta);
};

#endif /* SIMPLEX_H */
