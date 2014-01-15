/**
 * @file primalsimplex.h
 */

#ifndef PRIMALSIMPLEX_H
#define	PRIMALSIMPLEX_H

#include <globals.h>

#include <simplex/simplex.h>
#include <simplex/primalpricing.h>
#include <simplex/primalupdater.h>
#include <simplex/primalratiotest.h>

class PrimalSimplex: public Simplex
{
public:
    PrimalSimplex();

private:
    PrimalPricing * m_pricing;

    PrimalUpdater * m_updater;

    PrimalFeasibilityChecker * m_feasibilityChecker;

    PrimalRatiotest * m_ratiotest;

    bool m_feasible;

    void initModules();
    void releaseModules();

    virtual void computeFeasibility();
    virtual void checkFeasibility();
    virtual void price();
    virtual void selectPivot();
    virtual void update();

    virtual void initWorkingTolerance();
    virtual void computeWorkingTolerance();

    virtual void releaseLocks();
};

#endif /* PRIMALSIMPLEX_H */
