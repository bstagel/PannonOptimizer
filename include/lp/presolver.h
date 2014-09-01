/**
 * @file presolver.h This file contains the API of the Presolver class.
 * @author borocz
 */


#ifndef PRESOLVER_H
#define PRESOLVER_H

class PresolverModule;
#include <lp/model.h>
#include <lp/presolvermodule.h>
#include <utils/indexlist.h>
#include <utils/exceptions.h>
#include <utils/timer.h>

/**
 * This class controls the presolving for a specific model.
 * It is able to run different presolving techniques (modules) on it, reducing its size and
 * improving its sparsity. The aim of presolving is to reduce the total
 * amount of time needed for solving the problem by simplifying it mainly
 * with the detection and elimination of redundancies.
 *
 * @class Presolver
 */
class Presolver {
    friend class PresolverTestSuite;
public:

    /**
     * This type describes the methods to be used when a Presolver object runs.
     * DEFAULT runs all the modules in the default order, CUSTOM lets the user
     * specify the used methods in custom order and NO_PRESOLVE does not run any
     * presolving method.
     */
    enum PRESOLVE_MODE
    {
        DEFAULT = 0,
        CUSTOM,
        NO_PRESOLVE,
        PRESOLVE_MODE_ENUM_LENGTH
    };

    /**
     * This type functions as a flag at the end of the substitution vectors.
     * It decides if the vector is a substitution vector of a removed variable,
     * or the coefficent vector of a removed row.
     *
     * @see m_substituteVectors
     */
    enum SUBSTITUTED_VARIABLE_FLAG
    {
        PRIMAL_VARIABLE = 0,
        DUPLICATE_VARIABLE,
        PRIMAL_COLUMN_VECTOR,
        SUBSTITUTED_VARIABLE_FLAG_ENUM_LENGTH
    };

    /**
     * This type is able to describe the infinity count of infinite sums.
     * This is needed by certain presolving methods.
     */
    enum INFINITY_COUNT
    {
        PLUS_INF_SOURCE = 1,
        MINUS_INF_SOURCE = 1000000
    };

    class PresolverException : public PanOptException {
    public:
        // Bori: ha linkeltem a libet a solverhez,
        // akkor a linker hianyolta a destruktort es a
        // default konstruktort, most beirtam ide hogy forduljon,
        // de ezt gondold at
        PresolverException() : PanOptException("") {}
        PresolverException(const std::string & message) : PanOptException::PanOptException(message) {}
        ~PresolverException() {}
    };

    /**
     * Default constructor of the Presolver class.
     * Initializes the variables needed for the presolver according to the used model.
     *
     * @constructor
     * @param model The model to be presolved.
     */
    Presolver(Model * model);

    /**
     * Destructor of the Presolver class. Releases the dynamicly allocated modules.
     * @destructor
     */
    ~Presolver();

    /**
     * Reinitializes the Presolver to a new model. Previous settings of the presolver will be lost.
     *
     * @param model The new model to be presolved.
     */
    void reinitialize(Model * model);

    //getter functions of the presolver
    //inline unsigned int getTimeLimit() { return m_timeLimit; }
    /**
     * Returns a pointer to the model used by the presolver.
     *
     * @return Pointer to the model used by the presolver.
     * @see m_model
     */
    inline Model * getModel() { return m_model; }

    /**
     * Returns the iteration limit of the presolver.
     *
     * @return The iteration limit of the presolver.
     * @see m_iterationLimit
     */
    inline unsigned int getIterationLimit() { return m_iterationLimit; }

    /**
     * Returns a pointer to the row nonzeros vector.
     *
     * @return Pointer to the vector containing the number of nonzero elements in
     * each row of the model.
     * @see m_rowNonzeros
     */
    inline Vector * getRowNonzeros() { return m_rowNonzeros; }

    /**
     * Returns a pointer to the column nonzeros vector.
     *
     * @return Pointer to the vector containing the number of nonzero elements in
     * each column of the model.
     * @see m_columnNonzeros
     */
    inline Vector * getColumnNonzeros() { return m_columnNonzeros; }

    /**
     * Returns a pointer to the implied upper bounds vector.
     *
     * @return  Pointer to the vector containing the tightest calculated upper bounds
     * for each primal variable in the model.
     * @see m_impliedUpper
     */
    inline Vector * getImpliedUpper() { return m_impliedUpper; }

    /**
     * Returns a pointer to the implied lower bounds vector.
     *
     * @return  Pointer to the vector containing the tightest calculated lower bounds
     * for each primal variable in the model.
     * @see m_impliedLower
     */
    inline Vector * getImpliedLower() { return m_impliedLower; }

    /**
     * Returns a pointer to the implied dual upper bounds vector.
     *
     * @return Pointer to the vector containing the tightest calculated upper bounds
     * for each dual variable in the model.
     * @see m_impliedDualUpper
     */
    inline Vector * getImpliedDualUpper() { return m_impliedDualUpper; }

    /**
     * Returns a pointer to the implied dual lower bounds vector.
     *
     * @return Pointer to the vector containing the tightest calculated lower bounds
     * for each dual variable in the model.
     * @see m_impliedDualLower
     */
    inline Vector * getImpliedDualLower() { return m_impliedDualLower; }

    /**
     * Returns a pointer to the extra dual lower sum vector.
     *
     * @return Pointer to the vector containing the sum of lower bounds of each extra
     * dual variable in the model.
     * @see m_extraDualLowerSum
     */
    inline Vector * getExtraDualLowerSum() { return m_extraDualLowerSum; }

    /**
     * Returns a pointer to the extra dual upper sum vector.
     *
     * @return Pointer to the vector containing the sum of upper bounds of each extra
     * dual variable in the model.
     * @see m_extraDualUpperSum
     */
    inline Vector * getExtraDualUpperSum() { return m_extraDualUpperSum; }

    /**
     * Returns a pointer to the vector of variables in the model.
     *
     * @return Pointer to the vector of variables in the model.
     * @see m_variables
     */
    inline std::vector<Variable> * getVariables() { return m_variables; }

    /**
     * Returns a pointer to the vector of constraints in the model.
     *
     * @return Pointer to the vector of constraints in the model.
     * @see m_constraints
     */
    inline std::vector<Constraint> * getConstraints() { return m_constraints; }

    /**
     * Returns a pointer to the substitution vector array.
     *
     * @return Pointer to the vector of substitution vectors at the current stage
     * of the presolved model.
     * @see m_substituteVectors
     */
    inline std::vector<Vector*> * getSubstituteVectors() { return m_substituteVectors; }

    //setter functions of the presolver

    //inline void setTimeLimit(unsigned int time) { m_timeLimit = time; }
    /**
     * Sets the iteration limit of the presolver to a given value.
     *
     * @param maxIterations The new iteration limit of the presolver.
     * @see m_iterationLimit
     */
    inline void setIterationLimit(unsigned int maxIterations) { m_iterationLimit = maxIterations; }

    /**
     * Sets the mode of the presolver to a given value.
     *
     * @param mode The new mode of the presolver.
     * @see m_mode
     */
    inline void setPresolveMode(PRESOLVE_MODE mode) { m_mode = mode; }

    //functions to handle the nesseccary modules
    /**
     * Adds a new module to the presolver.
     * If the presolver mode is set to CUSTOM, the presolver will execute each added
     * module in the order of their addition during the presolve.
     *
     * @param newModule Pointer to the new module to be added.
     */
    inline void addModule(PresolverModule * newModule) { m_modules.push_back(newModule); }

    /**
     * Clears the module queue of the presolver.
     */
    inline void clearModules() { m_modules.clear(); }

    //this function fixes the variable specified by index to a certain value
    //stores the modifications in the presolver instead of modifying the model itself
    /**
     * Sets a variable in the model to the given value, then removes it from the model.
     * Several different presolving methods use this operation.
     *
     * @param index The index of the variable to be removed.
     * @param value The value of the variable.
     */
    void fixVariable(int index, Numerical::Double value);

    /**
     * Executes the currently queued modules on the model.
     */
    void presolve();

    /**
     * Prints the statistics of the presolver to the output.
     * This contains the number of the eliminated variales, constaints and nonzero elements of the model, as
     * well as the elapsed time during the presolving process.
     */
    void printStatistics();

private:
    /**
     * Pointer to the Model to be presolved.
     * Every Presolver class can only presolve one model at a time.
     */
    Model * m_model;

    /**
     * Pointer to the vector of variables of the model.
     * The size of this vector equals to the number of variables in the model and each element of the
     * vector is a Variable object.
     */
    std::vector<Variable> * m_variables;

    /**
     * Pointer to the vector of constraints of the model.
     * The size of this vector equals to the number of constraints in the model and each element of the
     * vector is a Constraint object.
     */
    std::vector<Constraint> * m_constraints;

    //unsigned int m_timeLimit;
    /**
     * The maximum number of module iterations the presolver can run before finishing.
     * The process is considered finished if the last module iteration was not able
     * to remove and variable, constraint or nonzero element from the model.
     */
    unsigned int m_iterationLimit;

    /**
     * The current mode of the presolver.
     *
     * @see PRESOLVE_MODE
     */
    PRESOLVE_MODE m_mode;

    /**
     * Pointer to the vector of queued modules in the presolver.
     * If the mode is set to CUSTOM, this queue is specified by the used, otherwise it is
     * a pre-defined vector of presolver modules.
     */
    std::vector<PresolverModule*> m_modules;

    /**
     * Pointer to the vector of substitution vectors of the model.
     * During the presolving for every removed variable and constraint a substitution vector
     * is added to make the solver able to postsolve the model after calculating an optimal solution for
     * the presolved model, thus retrieving the solution for the original model.
     */
    std::vector<Vector*> * m_substituteVectors;

    //Vectors storing the rows and columns by nonzero count
    /**
     * Pointer to the vector containing the number of nonzero elements in each row of the coefficient matrix in the model.
     * This vector is used and updated by several different presolving method.
     */
    Vector * m_rowNonzeros;

    /**
     * Pointer to the vector containing the number of nonzero elements in each column of the coefficient matrix in the model.
     * This vector is used and updated by several different presolving method.
     */
    Vector * m_columnNonzeros;

    //Vectors storing the implied bounds of the variables
    /**
     * Pointer to the vector containing the tightest calculated lower bound of each variable in the model.
     * This vector is used and updated by several different presolving method.
     */
    Vector * m_impliedLower;

    /**
     * Pointer to the vector containing the tightest calculated upper bound of each variable in the model.
     * This vector is used and updated by several different presolving method.
     */
    Vector * m_impliedUpper;

    /**
     * Pointer to the vector containing the tightest calculated lower bound of each dual variable in the model.
     * This vector is used and updated by several different presolving method.
     */
    Vector * m_impliedDualLower;

    /**
     * Pointer to the vector containing the tightest calculated upper bound of each dual variable in the model.
     * This vector is used and updated by several different presolving method.
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
     * The number of implied free variables made during the presolving.
     * Implied free variables have their bounds removed due to redundancy in the model which can be
     * advantageous in the case of a simplex based solver.
     */
    unsigned int m_impliedFreeCount;

    /**
     * The timer counting the elapsed time from the start of the presolving.
     */
    Timer * m_timer;
};

#endif // PRESOLVER_H
