/**
 * @file   primalpricingfactory.h
 */

#ifndef PRIMALPRICINGFACTORY_H
#define	PRIMALPRICINGFACTORY_H

#include <globals.h>

#include <simplex/primalpricingupdater.h>
#include <simplex/primalpricing.h>
#include <simplex/simplex.h>

class SimplexModel;

class PrimalPricingFactory {
public:
    virtual ~PrimalPricingFactory(){}

    virtual PrimalPricingUpdater * createPrimalPricingUpdater(const Vector &basicVariableValues,
                                                              const IndexList<> & basicVariableFeasibilities,
                                                              IndexList<> * reducedCostFeasibilities,
                                                              const std::vector<int> & basisHead,
                                                              const SimplexModel & simplexModel,
                                                              const Basis & basis) const = 0;

    virtual PrimalPricing * createPrimalPricing(const SimplexModel & model,
                                                PrimalPricingUpdater * updater,
                                                const std::vector<int>& basisHead) const = 0;
};

#endif	/* PRIMALPRICINGFACTORY_H */

