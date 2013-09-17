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
    void iterate();
};

#endif /* PRIMALSIMPLEX_H */
