/**
 * @file presolvermodule.h
 */


#ifndef PRESOLVERMODULE_H
#define PRESOLVERMODULE_H

class Presolver;
#include <lp/presolver.h>
#include <utils/indexlist.h>


//virtual class for modules executing different presolving methods
class PresolverModule {
public:
    explicit PresolverModule(Presolver * parent);
    virtual ~PresolverModule() = 0;

    virtual void executeMethod() = 0;

    inline unsigned int getRemovedConstraintCount() { return m_removedConstraints; }
    inline unsigned int getRemovedVariableCount() { return m_removedVariables; }
    inline const std::string & getName() { return m_name; }

protected:
    Presolver * m_parent;
    std::string m_name;
    unsigned int m_removedConstraints;
    unsigned int m_removedVariables;
};


//this module checks the model for empty rows and singleton rows
//if infeasibility is detected, it throws a PresolverException
//if a column is eliminated, the rows are checked again for new singletons
class SingletonRowsModule : public PresolverModule {
public:
    SingletonRowsModule(Presolver * parent);
    ~SingletonRowsModule();

    void executeMethod();
};

//this module checks the model for fixed variables, empty columns and singleton columns
//if infeasibility or unboundedness is detected, it throws a PresolverException
class SingletonColumnsModule : public PresolverModule {
public:
    SingletonColumnsModule(Presolver * parent);
    ~SingletonColumnsModule();

    void executeMethod();
};

//this module checks the model for forcing and redundant constraints
//in case of not forcing nor redundant constraints, it tightens the specified variables' bounds
//variables with tightened upper and lower bounds are set to implied free
//if infeasibility or unboundedness is detected, it throws a PresolverException
class ImpliedBoundsModule : public PresolverModule {
public:

    ImpliedBoundsModule(Presolver * parent);
    ~ImpliedBoundsModule();

    void executeMethod();

private:
    int m_impliedFreeVariables;
    Vector * m_constraintsToCheck;
    Vector * m_constraintStack;
    Vector * m_variablesToFix;
    Vector * m_impliedLower;
    Vector * m_impliedUpper;
};

class DualBoundsModule : public PresolverModule {
public:

    DualBoundsModule(Presolver * parent);
    ~DualBoundsModule();

    void executeMethod();

private:
    Vector * m_impliedDualLower;
    Vector * m_impliedDualUpper;
    Vector * m_extraDualLowerSum;
    Vector * m_extraDualUpperSum;
    Vector * m_variablesToCheck;
    Vector * m_variableStack;
};

#endif // PRESOLVERMODULE_H
