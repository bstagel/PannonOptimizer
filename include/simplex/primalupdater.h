/* 
 * File:   primalupdater.h
 * Author: smidla
 *
 * Created on 2013. augusztus 26., 0:54
 */

#ifndef PRIMALUPDATER_H
#define	PRIMALUPDATER_H

#include <globals.h>
#include <simplex/primalpricingupdater.h>

class PrimalUpdater {
public:
    PrimalUpdater();

    void setPricingUpdater(PrimalPricingUpdater * pricingUpdater);

    void update();

protected:
    PrimalPricingUpdater * m_pricingUpdater;
};

#endif	/* PRIMALUPDATER_H */

