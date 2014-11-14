/**
 *  @file simplexsolver.h
 */

#ifndef SIMPLEXSOLVER_H
#define SIMPLEXSOLVER_H

#include <globals.h>
#include <lp/model.h>

/**
 * @class SimplexSolver
 */
class SimplexSolver {
public:
    void solve(const Model & model);
private:
    void parallelSolve(const Model *model);
};

#endif // SIMPLEXSOLVER_H
