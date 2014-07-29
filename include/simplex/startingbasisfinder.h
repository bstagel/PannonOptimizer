/**
 * @file startingbasisfinder.h This file contains the StartingBasisFinder class.
 */

#ifndef STARTINGBASISFINDER_H
#define STARTINGBASISFINDER_H

#include <globals.h>

#include <vector>

#include <simplex/simplexmodel.h>
#include <utils/numerical.h>
#include <utils/indexlist.h>


class SbfSuper;

/**
 * This class describes the startingbasis finder module.
 * Various techniques can be implemented.
 * @class StartingBasisFinder
 */
class StartingBasisFinder
{
public:

    /**
     * Describes the different output statistics levels.
     */
    enum SbfPrintLevel {
        PRINT_STATISTIC = 1,
        PRINT_DETAILED = 2,
        PRINT_STRUCTURALS = 4
    };


    /**
     * Enumeration type of available starting basis finder strategies.
     */
    enum STARTING_BASIS_STRATEGY {
        LOGICAL = 0,
        SYMBOLIC_CRASH = 1,
        STARTING_BASIS_STRATEGY_ENUM_LENGTH
    };

    /**
     * Enumeration type of available starting nonbasic states.
     */
    enum STARTING_NONBASIC_STATES {
        LOWER = 0,
        UPPER = 1,
        MIXED = 2,
        STARTING_NONBASIC_STATES_ENUM_LENGTH
    };

    /**
     * Initializing constructor of the class.
     * @param model the model of the LP problem
     * @param basisHead vector containing basicvariable indices
     * @param variableStates indexlist containing the states of the variables
     * @constructor
     */
    StartingBasisFinder(const SimplexModel& model,
                        std::vector<int>* basisHead,
                        IndexList<const Numerical::Double*>* variableStates);

    /**
     * Default destructor of the class.
     * @destructor
     */
    ~StartingBasisFinder();

    /**
     * Starts the starting basis finder algorithm.
     * It will call the corresponding private function depending on the selected strategy.
     * The results can be obtained via getBasisHead and getVariableAtUpperBound
     * functions, but it will be automatically written to m_problem.
     */
    void findStartingBasis(STARTING_BASIS_STRATEGY strategy, STARTING_NONBASIC_STATES nonbasicStates);

    /**
     * Prints the output of the startingBasisFinder algorithm.
     * (The ordered indeces.) The possible output detail types can be found at StartingBasisFinder::SbfPrintLevel.
     */
    void print(int printLevel);

private:

    /**
     * Model of the LP problem.
     */
    const SimplexModel & m_model;

    /**
     * Pointer to the basicvariable's indices.
     */
    std::vector<int>* m_basisHead;

    /**
     * Pointer of indexlist contains the variable states (being at lower / upper bound)
     */
    IndexList<const Numerical::Double*>* m_variableStates;

    /**
     * Pointer to the basicvariable values (X_B vector).
     */
    Vector* m_basicVariableValues;

    /**
     * The used startingbasis finder algorithm possible values at StartingBasisFinder::STARTING_BASIS_STRATEGY.
     */
    SbfSuper* m_algorithm;
};

#endif // STARTINGBASISFINDER_H
