/**
 * @file presolvermodule.h This file contains the API of the PresolverModule class, and every
 * individual child class executing different presolving methods.
 * @author Peter Borocz
 */


#ifndef PRESOLVERMODULE_H
#define PRESOLVERMODULE_H

class Presolver;
#include <lp/presolver.h>
#include <utils/indexlist.h>


/**
 * This pure virtual class is the base class for modules executing different presolving methods.
 * A Presolver object is needed to apply these techniques to a model.
 *
 * @class PresolverModule
 */
class PresolverModule {
public:

    PresolverModule();

    /**
     * Default constructor of the PresolverModule class.
     * A parent Presolver object is needed to construct an object from this class.
     *
     * @constructor
     * @param parent Pointer to the parent Presolver containing this module.
     */
    PresolverModule(Presolver * parent);

    /**
     * Destructor of the PresolverModule class.
     *
     * @destructor
     */
    virtual ~PresolverModule() = 0;

    /**
     * This function executes the presolving methods implemented by the current module.
     * The model to be work with is specified by the parent Presolver object.
     */
    virtual void executeMethod() = 0;

    /**
     * Returns the number of successfully removed constraints by this module.
     *
     * @return  The number of successfully removed constraints by this module.
     */
    inline unsigned int getRemovedConstraintCount() { return m_removedConstraints; }

    /**
     * Returns the number of successfully removed variables by this module.
     *
     * @return  The number of successfully removed variables by this module.
     */
    inline unsigned int getRemovedVariableCount() { return m_removedVariables; }

    /**
     * Returns the name of the module.
     *
     * @return The name of the module.
     */
    inline const std::string & getName() { return m_name; }

    /**
     * Prints the statistics of the module to the output.
     * This contains the number of the eliminated variales, constaints and nonzero elements of the model.
     */
    virtual void printStatistics();

protected:

    /**
     * Pointer to the parent Presolver of the module.
     */
    Presolver * m_parent;

    /**
     * The name of the module.
     */
    std::string m_name;

    /**
     * The number of successfully removed constraints by this module.
     */
    unsigned int m_removedConstraints;

    /**
     * The number of successfully removed variables by this module.
     */
    unsigned int m_removedVariables;
};


//this module checks the model for empty rows and singleton rows
//if infeasibility is detected, it throws a PresolverException
//if a column is eliminated, the rows are checked again for new singletons

/**
 * Presolver module checking the model for empty and singleton rows.
 * If a column is eliminated, the rows are checked again for new singletons.
 *
 * @throws InfeasibilityException if infeasibility is detected.
 *
 * @class SingletonRowsModule
 */
class SingletonRowsModule : public PresolverModule {
public:

    /**
     * Default constructor of the SingletonRowsModule class.
     *
     * @constructor
     * @param parent Pointer to the parent Presolver containing this module.
     */
    SingletonRowsModule(Presolver * parent);

    /**
     * Destructor of the SingletonRowsModule class.
     *
     * @destructor
     */
    ~SingletonRowsModule();

    /**
     * Executes the presolving techniques implemented by SingletonRowsModule.
     * These are found in the literature by the names "Empty Row" and "Singleton Row".
     */
    void executeMethod();
};

//this module checks the model for fixed variables, empty columns and singleton columns
//if infeasibility or unboundedness is detected, it throws a PresolverException

/**
 * Presolver module checking the model for fixed variables, empty and singleton columns.
 * If a row is eliminated, the columns are checked again for new singletons.
 *
 * @throws InfeasibilityException if infeasibility is detected.
 * @throws UnboundedException if unboundedness is detected.
 *
 * @class SingletonColumnsModule
 */
class SingletonColumnsModule : public PresolverModule {
public:

    /**
     * Default constructor of the SingletonColumnsModule class.
     *
     * @constructor
     * @param parent Pointer to the parent Presolver containing this module.
     */
    SingletonColumnsModule(Presolver * parent);

    /**
     * Destructor of the SingletonRowsModule class.
     *
     * @destructor
     */
    ~SingletonColumnsModule();

    /**
     * Executes the presolving techniques implemented by SingletonColumnsModule.
     * These are found in the literature by the names "Fixed Variable",
     * "Empty Column" and "Singleton Column".
     */
    void executeMethod();
};

//this module checks the model for forcing and redundant constraints
//in case of not forcing nor redundant constraints, it tightens the specified variables' bounds
//variables with tightened upper and lower bounds are set to implied free
//if infeasibility or unboundedness is detected, it throws a PresolverException

/**
 * Presolver module checking the implied bounds on the variables.
 * If a constraint is not forcing nor redundant, there is a chance of tightening the bounds
 * on its variables. Variables with tightened upper and lower bounds can be set to implied
 * free, which is advantageous in the case of simplex based solvers.
 * This module checks constraints in an iterative way. If there are new constraints to be
 * checked due to tightened variable bounds, their indices are placed into another stack,
 * which replaces the actual one after the iteration.
 *
 * @throws InfeasibilityException if infeasibility is detected.
 * @throws UnboundedException if unboundedness is detected.
 *
 * @class ImpliedBoundsModule
 */
class ImpliedBoundsModule : public PresolverModule {
public:

    /**
     * Default constructor of the ImpliedBoundsModule class.
     *
     * @constructor
     * @param parent Pointer to the parent Presolver containing this module.
     */
    ImpliedBoundsModule(Presolver * parent);

    /**
     * Destructor of the ImpliedBoundsModule class.
     *
     * @destructor
     */
    ~ImpliedBoundsModule();

    /**
     * Executes the presolving techniques implemented by the ImpliedBoundsModule.
     * These are found in the literature by the names "Forcing and Redundant Constraints",
     * "Tightening Variable Bounds" and "Implied Free Variable".
     */
    void executeMethod();

private:
    //int m_impliedFreeVariables;

    /**
     * The list of constraint indices to be checked in the actual iteration of the module.
     */
    Vector * m_constraintsToCheck;

    /**
     * The list of constraint indices to be checked in the next iteration of the module.
     */
    Vector * m_constraintStack;

    /**
     * The list of variable indices to be fixed due to the actual forcing constaint.
     */
    Vector * m_variablesToFix;

    /**
     * Pointer to the vector containing the tightest calculated lower bound of each variable in the model.
     */
    Vector * m_impliedLower;

    /**
     * Pointer to the vector containing the tightest calculated upper bound of each variable in the model.
     */
    Vector * m_impliedUpper;
};

/**
 * Presolver module checking the variables and constraints in the dual of the problem described by the model.
 * If a dual constraint does not dominate a primal variable, its dual variables' bounds can be tightened.
 * This module checks the dual constraints in an iterative way. If there are new dual constraints to be
 * checked due to tightened dual variable bounds, their indices are placed into another stack, which replaces
 * the actual one after the iteration.
 *
 * @throws InfeasibilityException if infeasibility is detected.
 * @throws UnboundedException if unboundedness is detected.
 *
 * @class DualBoundsModule
 */
class DualBoundsModule : public PresolverModule {
public:

    /**
     * Default constructor of the DualBoundsModule class.
     *
     * @constructor
     * @param parent Pointer to the parent Presolver containing this module.
     */
    DualBoundsModule(Presolver * parent);

    /**
     * Destructor of the DualBoundsModule class.
     *
     * @destructor
     */
    ~DualBoundsModule();

    /**
     * Executes the presolving techniques implemented by the ImpliedBoundsModule.
     * These are found in the literature by the names "Dominated Variable", and
     * "Improving Bounds on Shadow Prices".
     */
    void executeMethod();

private:

    /**
     * Pointer to the vector containing the tightest calculated lower bound of each dual variable in the model.
     */
    Vector * m_impliedDualLower;

    /**
     * Pointer to the vector containing the tightest calculated upper bound of each dual variable in the model.
     */
    Vector * m_impliedDualUpper;

    /**
     * Pointer to the vector containing the sum of lower bounds of the extra dual variables in the model.
     * Extra dual variables are created by bounded variables or ranged constraints of the model.
     */
    Vector * m_extraDualLowerSum;

    /**
     * Pointer to the vector containing the sum of upper bounds of the extra dual variables in the model.
     * Extra dual variables are created by bounded variables or ranged constraints of the model.
     */
    Vector * m_extraDualUpperSum;

    /**
     * The list of dual constraint indices to be checked in the actual iteration of the module.
     */
    Vector * m_variablesToCheck;

    /**
     * The list of dual constraint indices to be checked in the next iteration of the module.
     */
    Vector * m_variableStack;
};

/**
 * Presolver module checking the model for duplicate rows and columns and making the coefficient matrix sparser.
 * Duplicate vectors are identified via the hashing of nonzero pattern. The improvement of sparsity in the
 * coefficient matrix is ensured by linear algebraic operation between its vectors. Linear dependency of multiple
 * vectors is not checked nor used due to the heavy resource usage of such operations.
 *
 * @throws InfeasibilityException if infeasibility is detected.
 * @throws UnboundedException if unboundedness is detected.
 *
 * @class LinearAlgebraicModule
 */
class LinearAlgebraicModule : public PresolverModule {
public:

    /**
     * Default constructor of the LinearAlgebraicModule class.
     *
     * @constructor
     * @param parent Pointer to the parent Presolver containing this module.
     */
    LinearAlgebraicModule(Presolver * parent);

    /**
     * Destructor of the LinearAlgebraicModule class.
     *
     * @destructor
     */
    ~LinearAlgebraicModule();

    /**
     * Executes the presolving techniques implemented by the LinearAlgebraicModule.
     * These are found in the literature by the names "Duplicate Rows", "Duplicate Columns" and
     * "Making A Sparser".
     */
    void executeMethod();

    /**
     * Prints the statistics of the module to the output.
     * This contains the number of the eliminated variales, constaints and nonzero elements of the model.
     */
    void printStatistics();

private:

    /**
     * The number of elimiinated nonzero elements.
     */
    unsigned int m_removedNzr;
};

#endif // PRESOLVERMODULE_H
