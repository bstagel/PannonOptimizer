/* 
 * File:   dualpricingfactory.h
 * Author: smidla
 *
 * Created on 2013. augusztus 26., 0:58
 */

#ifndef DUALPRICINGFACTORY_H
#define	DUALPRICINGFACTORY_H

#include <globals.h>

#include <simplex/dualpricing.h>
#include <simplex/dualpricingupdater.h>

class SimplexModel;

class DualPricingFactory {
public:
    virtual ~DualPricingFactory(){}

    virtual DualPricingUpdater * createDualPricingUpdater() const = 0;
    virtual DualPricing * createDualPricing(const SimplexModel & model,
                                            const DualPricingUpdater & updater) const = 0;
};

#endif	/* DUALPRICINGFACTORY_H */

