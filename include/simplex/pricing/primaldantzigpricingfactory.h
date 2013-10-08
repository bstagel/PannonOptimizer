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

    PrimalPricingUpdater * createPrimalPricingUpdater() const;
    PrimalPricing * createPrimalPricing() const;
private:

};

#endif	/* PRIMALDANTZIGPRICINGFACTORY_H */

