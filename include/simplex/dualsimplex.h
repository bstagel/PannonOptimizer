/**
 * @file dualsimplex.h
 */


#ifndef DUALSIMPLEX_H
#define	DUALSIMPLEX_H

#include <globals.h>

#include <simplex/simplex.h>
#include <simplex/dualpricing.h>
#include <simplex/dualupdater.h>
#include <simplex/dualfeasibilitychecker.h>
#include <simplex/dualratiotest.h>
#include <simplex/dualratiotestupdater.h>

class DualSimplex: public Simplex
{
public:
    DualSimplex();

private:
    DualPricing * m_pricing;

    DualUpdater * m_updater;

    DualFeasibilityChecker * m_feasibilityChecker;

    DualRatiotest * m_ratiotest;

    bool m_feasible;
    unsigned int m_incomingIndex;
    unsigned int m_outgoingIndex;

    void initModules();
    void releaseModules();

    virtual void computeFeasibility() throw (NumericalException);
    virtual void checkFeasibility() throw (OptimizationResultException, NumericalException);
    virtual void price() throw (OptimizationResultException, NumericalException);
    virtual void selectPivot() throw (OptimizationResultException, NumericalException);
    virtual void update()throw (NumericalException);

    void computeTransformedRow(Vector* alpha, unsigned int rowIndex) throw (NumericalException);
};

#endif /* DUALSIMPLEX_H */
