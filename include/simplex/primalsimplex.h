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

    PrimalRatiotest * m_ratiotest;

    PrimalUpdater * m_updater;

    void initModules();
    void releaseModules();

    virtual void computeFeasibility() throw (NumericalException);
    virtual void checkFeasibility() throw (OptimizationResultException, NumericalException);
    virtual void price() throw (OptimizationResultException, NumericalException);
    virtual void selectPivot() throw (OptimizationResultException, NumericalException);
    virtual void update()throw (NumericalException);
};

#endif /* PRIMALSIMPLEX_H */
