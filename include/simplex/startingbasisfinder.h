#ifndef STARTINGBASISFINDER_H
#define STARTINGBASISFINDER_H

#include "globals.h"

#include <vector>

#include "simplex/simplexmodel.h"
#include "utils/numerical.h"
#include "utils/indexlist.h"


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
        LOWER_LOGICAL = 0,
        UPPER_LOGICAL = 1,
        MIXED_LOGICAL = 2,
//        SYMBOLIC_CRASH = 3,
//        LTSF_CRASH = 4,
//        ADG_CRASH = 5,
//        ADG_LTSF_CRASH = 6,
//        CPLEX = 7,
        STARTING_BASIS_STRATEGY_ENUM_LENGTH
    };

    StartingBasisFinder(const SimplexModel& model, std::vector<int>& basisHead, IndexList& variableStates, const Vector& basicVariableValues);
    ~StartingBasisFinder();

    /**
     * Starts the starting basis finder algorithm.
     * It will call the corresponding private function depending on
     * the selected strategy.
     * The results can be obtained via getBasisHead and getVariableAtUpperBound
     * functions, but it will be automatically written to m_problem.
     */
    void findStartingBasis(STARTING_BASIS_STRATEGY strategy);

    /**
     * Prints the output of the startingBasisFinder algorithm.
     * (The ordered indeces.)
     */
    void print(int printLevel);

private:
    const SimplexModel & m_model;
    std::vector<int> & m_basisHead;
    IndexList & m_variableStates;
    const Vector & m_basicVariableValues;

    SbfSuper* m_algorithm;
};

#endif // STARTINGBASISFINDER_H
