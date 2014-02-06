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
                                                      const IndexList<> &basicVariableFeasibilities,
                                                      IndexList<> *reducedCostFeasibilities,
                                                      const IndexList<const Numerical::Double*> & variableStates,
                                                      const std::vector<int> & basisHead,
                                                      const SimplexModel & simplexModel,
                                                      const Basis & basis) const;

    PrimalPricing * createPrimalPricing(const SimplexModel & model,
                                        PrimalPricingUpdater * updater,
                                        const std::vector<int>& basisHead,
                                        const Vector & reducedCosts) const;
private:

};

#endif	/* PRIMALDANTZIGPRICINGFACTORY_H */

