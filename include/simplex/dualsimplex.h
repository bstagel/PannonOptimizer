/**
 * @file dualsimplex.h
 */


#ifndef DUALSIMPLEX_H
#define	DUALSIMPLEX_H

#include <globals.h>

#include <simplex/simplex.h>
#include <simplex/dualpricing.h>
#include <simplex/dualupdater.h>

class DualSimplex: public Simplex
{
public:
    DualSimplex();

private:
    DualPricing * m_pricing;

    DualUpdater * m_updater;

    void initModules();
    void releaseModules();

    virtual void reinvert() throw (NumericalException);
    virtual void computeBasicSolution() throw (NumericalException);
    virtual void computeFeasibility() throw (NumericalException);
    virtual void checkFeasibility() throw (OptimizationResultException, NumericalException);
    virtual void price() throw (OptimizationResultException, NumericalException);
    virtual void selectPivot() throw (OptimizationResultException, NumericalException);
    virtual void update()throw (NumericalException);
};

#endif /* DUALSIMPLEX_H */
