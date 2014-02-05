/**
 * @file   primaldantzigpricingfactory.h
 */

#ifndef PRIMALDANTZIGPRICINGFACTORY_H
#define	PRIMALDANTZIGPRICINGFACTORY_H

#include <globals.h>

#include <simplex/primalpricingfactory.h>

class PrimalDantzigPricingFactory : public PrimalPricingFactory
{
public:
    PrimalDantzigPricingFactory();
    PrimalDantzigPricingFactory(const PrimalDantzigPricingFactory& orig);
    virtual ~PrimalDantzigPricingFactory();

    PrimalPricingUpdater * createPrimalPricingUpdater(const Vector &basicVariableValues,
                                                      IndexList<> * variableFeasibilities,
                                                      const IndexList<> & reducedCostFeasibilities,
                                                      const std::vector<int> & basisHead,
                                                      const SimplexModel & simplexModel,
                                                      const Basis & basis) const;

    PrimalPricing * createPrimalPricing(const SimplexModel & model,
                                        PrimalPricingUpdater * updater,
                                        const std::vector<int>& basisHead) const;
private:

};

#endif	/* PRIMALDANTZIGPRICINGFACTORY_H */

