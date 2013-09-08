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
    void iterate();
};

#endif /* DUALSIMPLEX_H */
