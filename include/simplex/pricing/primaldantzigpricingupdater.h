/**
 * @file   primaldantzigpricingupdater.h
 */

#ifndef PRIMALDANTZIGPRICINGUPDATER_H
#define	PRIMALDANTZIGPRICINGUPDATER_H

#include <globals.h>

#include <simplex/primalpricingupdater.h>
#include <simplex/simplexmodel.h>
#include <simplex/basis.h>

class PrimalDantzigPricingUpdater: public PrimalPricingUpdater
{
    friend class PrimalDantzigPricing;
public:
    PrimalDantzigPricingUpdater(const Vector &basicVariableValues,
                                IndexList<> * variableFeasibilities,
                                const IndexList<> & reducedCostFeasibilities,
                                const std::vector<int> & basisHead,
                                const SimplexModel & simplexModel,
                                const Basis & basis);
    PrimalDantzigPricingUpdater(const PrimalDantzigPricingUpdater& orig);

    virtual ~PrimalDantzigPricingUpdater();

    void updatePhase1();

    void updatePhase2();
private:

    int m_phase2Index;

    Numerical::Double m_phase2ReducedCost;

    std::vector<char> m_used;

};

#endif	/* PRIMALDANTZIGPRICINGUPDATER_H */

