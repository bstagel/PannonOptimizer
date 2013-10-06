/* 
 * File:   dualdantzigpricingfactory.h
 * Author: smidla
 *
 * Created on 2013. augusztus 26., 1:03
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
                                                  const IndexList<> & variableFeasibilities,
                                                  const IndexList<> & reducedCostFeasibilities,
                                                  const std::vector<int> & basisHead,
                                                  const SimplexModel & simplexModel,
                                                  const Basis & basis) const;
    DualPricing * createDualPricing(const SimplexModel & model,
                                    const DualPricingUpdater & updater) const;
private:

};

#endif	/* DUALDANTZIGPRICINGFACTORY_H */

