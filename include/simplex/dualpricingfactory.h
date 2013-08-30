/* 
 * File:   dualpricingfactory.h
 * Author: smidla
 *
 * Created on 2013. augusztus 26., 0:58
 */

#ifndef DUALPRICINGFACTORY_H
#define	DUALPRICINGFACTORY_H

#include <simplex/dualpricing.h>
#include <simplex/dualreducedcostupdater.h>

class DualPricingFactory {
public:
    virtual DualReducedCostUpdater * createDualReducedCostUpdater() const = 0; 
    virtual DualPricing * createDualPricing() const = 0;
};

#endif	/* DUALPRICINGFACTORY_H */
