#ifndef SIMPLEXSOLVER_H
#define SIMPLEXSOLVER_H

#include <globals.h>
#include <lp/model.h>

class SimplexSolver {
public:
    void solve(const Model & model);
private:
    const Model * m_model;
    void parallelSolve(const Model *model);
};

#endif // SIMPLEXSOLVER_H
