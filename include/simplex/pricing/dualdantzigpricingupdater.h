/**
 * @file:   dualdantzigpricingupdater.h
 */

#ifndef DUALDANTZIGREPRICINGUPDATER_H
#define	DUALDANTZIGREPRICINGUPDATER_H

#include <globals.h>

#include <simplex/dualpricingupdater.h>
#include <simplex/simplexmodel.h>
#include <simplex/basis.h>

class DualDantzigPricingUpdater: public DualPricingUpdater
{
    friend class DualDantzigPricing;
public:
    DualDantzigPricingUpdater(const Vector &basicVariableValues,
                              const IndexList<> & variableFeasibilities,
                              const IndexList<> & reducedCostFeasibilities,
                              const std::vector<int> & basisHead,
                              const SimplexModel & simplexModel,
                              const Basis & basis);
    DualDantzigPricingUpdater(const DualDantzigPricingUpdater& orig);

    virtual ~DualDantzigPricingUpdater();

    void updatePhase1();

    void updatePhase2();
private:

    int m_phase2Index;

    Numerical::Double m_phase2ReducedCost;

    void clearPhase1ReducedCosts();
};

#endif	/* DUALDANTZIGREPRICINGUPDATER_H */

