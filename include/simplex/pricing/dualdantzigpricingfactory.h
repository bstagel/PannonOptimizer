/**
 * @file   dualdantzigpricingfactory.h
 */

#ifndef DUALDANTZIGPRICINGFACTORY_H
#define	DUALDANTZIGPRICINGFACTORY_H

#include <globals.h>

#include <simplex/dualpricingfactory.h>

class DualDantzigPricingFactory: public DualPricingFactory
{
public:
    DualDantzigPricingFactory();
    DualDantzigPricingFactory(const DualDantzigPricingFactory& orig);
    virtual ~DualDantzigPricingFactory();
    
    DualPricingUpdater * createDualPricingUpdater(const Vector &basicVariableValues,
                                                  IndexList<> * variableFeasibilities,
                                                  const IndexList<> & reducedCostFeasibilities,
                                                  const std::vector<int> & basisHead,
                                                  const SimplexModel & simplexModel,
                                                  const Basis & basis) const;

    DualPricing * createDualPricing(const SimplexModel & model,
                                    DualPricingUpdater * updater,
                                    const std::vector<int>& basisHead) const;
private:

};

#endif	/* DUALDANTZIGPRICINGFACTORY_H */

