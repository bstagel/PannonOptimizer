/**
 * @file primalsimplex.h
 */

#ifndef PRIMALSIMPLEX_H
#define	PRIMALSIMPLEX_H

#include <globals.h>

#include <simplex/simplex.h>
#include <simplex/primalpricing.h>
#include <simplex/primalupdater.h>

class PrimalSimplex: public Simplex
{
public:
    PrimalSimplex();

private:
    PrimalPricing * m_pricing;

    PrimalUpdater * m_updater;

    void initModules();
    void releaseModules();
    void reinvert() throw(NumericalException);
    void iterate() throw(OptimizationResultException, NumericalException);
};

#endif /* PRIMALSIMPLEX_H */
