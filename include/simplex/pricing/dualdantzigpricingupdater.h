/* 
 * File:   dualdantzigpricingupdater.h
 * Author: smidla
 *
 * Created on 2013. augusztus 26., 1:00
 */

#ifndef DUALDANTZIGREPRICINGUPDATER_H
#define	DUALDANTZIGREPRICINGUPDATER_H

#include <globals.h>

#include <simplex/dualpricingupdater.h>

class DualDantzigPricingUpdater: public DualPricingUpdater
{
public:
    DualDantzigPricingUpdater(const Vector & basicVariableValues,
                              IndexList<> variableFeasibilities,
                              std::vector<int> basisHead);
    DualDantzigPricingUpdater(const DualDantzigPricingUpdater& orig);
    virtual ~DualDantzigPricingUpdater();
private:

};

#endif	/* DUALDANTZIGREPRICINGUPDATER_H */

