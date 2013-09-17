/* 
 * File:   dualupdater.h
 * Author: smidla
 *
 * Created on 2013. augusztus 26., 0:54
 */

#ifndef DUALUPDATER_H
#define	DUALUPDATER_H

#include <globals.h>

#include <simplex/dualpricingupdater.h>

class DualUpdater {
public:
    DualUpdater();

    void setPricingUpdater(DualPricingUpdater * pricingUpdater);

    void update();

protected:
    DualPricingUpdater * m_pricingUpdater;
};

#endif	/* DUALUPDATER_H */

