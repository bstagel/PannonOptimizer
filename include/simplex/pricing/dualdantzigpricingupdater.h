/* 
 * File:   dualdantzigreducedcostupdater.h
 * Author: smidla
 *
 * Created on 2013. augusztus 26., 1:00
 */

#ifndef DUALDANTZIGREDUCEDCOSTUPDATER_H
#define	DUALDANTZIGREDUCEDCOSTUPDATER_H

#include <simplex/dualpricingupdater.h>

class DualDantzigPricingUpdater: public DualPricingUpdater
{
public:
    DualDantzigPricingUpdater();
    DualDantzigPricingUpdater(const DualDantzigPricingUpdater& orig);
    virtual ~DualDantzigPricingUpdater();
private:

};

#endif	/* DUALDANTZIGREDUCEDCOSTUPDATER_H */

