/* 
 * File:   primaldantzigpricingfactory.h
 * Author: smidla
 *
 * Created on 2013. augusztus 26., 1:02
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

    PrimalReducedCostUpdater * createPrimalReducedCostUpdater() const;
    PrimalPricing * createPrimalPricing() const;
private:

};

#endif	/* PRIMALDANTZIGPRICINGFACTORY_H */

