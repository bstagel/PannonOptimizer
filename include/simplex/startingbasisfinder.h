/**
 * @file startingbasisfinder.h
 */

#ifndef STARTINGBASISFINDER_H
#define STARTINGBASISFINDER_H

#include <globals.h>

#include <vector>

#include <simplex/simplexmodel.h>
#include <utils/numerical.h>
#include <utils/indexlist.h>


class SbfSuper;

class StartingBasisFinder
{
public:
    enum SbfPrintLevel {
        PRINT_STATISTIC = 1,
        PRINT_DETAILED = 2,
        PRINT_STRUCTURALS = 4
    };


    /** Enumeration type of available starting basis finder strategies. */
    enum STARTING_BASIS_STRATEGY {
        LOGICAL = 0,
        SYMBOLIC_CRASH = 1,
        STARTING_BASIS_STRATEGY_ENUM_LENGTH
    };

    /** Enumeration type of available starting basis finder strategies. */
    enum STARTING_NONBASIC_STATES {
        LOWER = 0,
        UPPER = 1,
        MIXED = 2,
        STARTING_NONBASIC_STATES_ENUM_LENGTH
    };

    StartingBasisFinder(const SimplexModel& model,
                        std::vector<int>* basisHead,
                        IndexList<const Numerical::Double*>* variableStates,
                        Vector* basicVariableValues);
    ~StartingBasisFinder();

    /**
     * Starts the starting basis finder algorithm.
     * It will call the corresponding private function depending on
     * the selected strategy.
     * The results can be obtained via getBasisHead and getVariableAtUpperBound
     * functions, but it will be automatically written to m_problem.
     */
    void findStartingBasis(STARTING_BASIS_STRATEGY strategy, STARTING_NONBASIC_STATES nonbasicStates);

    /**
     * Prints the output of the startingBasisFinder algorithm.
     * (The ordered indeces.)
     */
    void print(int printLevel);

private:
    const SimplexModel & m_model;
    std::vector<int>* m_basisHead;
    IndexList<const Numerical::Double*>* m_variableStates;
    Vector* m_basicVariableValues;

    SbfSuper* m_algorithm;
};

#endif // STARTINGBASISFINDER_H
