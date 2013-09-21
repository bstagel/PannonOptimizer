/* 
 * File:   primalpricingfactory.h
 * Author: smidla
 *
 * Created on 2013. augusztus 26., 0:57
 */

#ifndef PRIMALPRICINGFACTORY_H
#define	PRIMALPRICINGFACTORY_H

#include <globals.h>

#include <simplex/primalpricingupdater.h>
#include <simplex/primalpricing.h>

class PrimalPricingFactory {
public:
    virtual ~PrimalPricingFactory(){}

    virtual PrimalPricingUpdater * createPrimalPricingUpdater() const = 0;
    virtual PrimalPricing * createPrimalPricing() const = 0;
};

#endif	/* PRIMALPRICINGFACTORY_H */

