/* 
 * File:   dualdantzigpricingfactory.h
 * Author: smidla
 *
 * Created on 2013. augusztus 26., 1:03
 */

#ifndef DUALDANTZIGPRICINGFACTORY_H
#define	DUALDANTZIGPRICINGFACTORY_H

#include <simplex/dualpricingfactory.h>

class DualDantzigPricingFactory: public DualPricingFactory
{
public:
    DualDantzigPricingFactory();
    DualDantzigPricingFactory(const DualDantzigPricingFactory& orig);
    virtual ~DualDantzigPricingFactory();
    
    DualReducedCostUpdater * createDualReducedCostUpdater() const;
    DualPricing * createDualPricing() const;    
private:

};

#endif	/* DUALDANTZIGPRICINGFACTORY_H */

