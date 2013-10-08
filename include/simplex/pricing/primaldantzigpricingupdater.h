/**
 * @file   primaldantzigpricingupdater.h
 */

#ifndef PRIMALDANTZIGPRICINGUPDATER_H
#define	PRIMALDANTZIGPRICINGUPDATER_H

#include <globals.h>

#include <simplex/primalpricingupdater.h>

class PrimalDantzigPricingUpdater: public PrimalPricingUpdater
{
public:
    PrimalDantzigPricingUpdater();
    PrimalDantzigPricingUpdater(const PrimalDantzigPricingUpdater& orig);
    virtual ~PrimalDantzigPricingUpdater();
private:

};

#endif	/* PRIMALDANTZIGPRICINGUPDATER_H */

