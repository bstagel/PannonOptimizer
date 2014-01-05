/**
 * @file dualpricingfactory.h
 */

#ifndef DUALPRICINGFACTORY_H
#define	DUALPRICINGFACTORY_H

#include <globals.h>

#include <simplex/dualpricing.h>
#include <simplex/dualpricingupdater.h>
#include <simplex/simplex.h>

class SimplexModel;

class DualPricingFactory {
public:
    virtual ~DualPricingFactory(){}

    virtual DualPricingUpdater * createDualPricingUpdater(const Vector &basicVariableValues,
                                                          IndexList<> * variableFeasibilities,
                                                          const IndexList<> & reducedCostFeasibilities,
                                                          const std::vector<int> & basisHead,
                                                          const SimplexModel & simplexModel,
                                                          const Basis & basis) const = 0;

    virtual DualPricing * createDualPricing(const SimplexModel & model,
                                            DualPricingUpdater * updater,
                                            const Vector& reducedCosts,
                                            const std::vector<int>& basisHead) const = 0;
};

#endif	/* DUALPRICINGFACTORY_H */

