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
                              IndexList<> * variableFeasibilities,
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

    std::vector<char> m_used;

};

#endif	/* DUALDANTZIGREPRICINGUPDATER_H */

