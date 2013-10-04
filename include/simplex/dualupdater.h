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
#include <simplex/dualratiotestupdater.h>

class DualUpdater {
public:
    DualUpdater();

    void setPricingUpdater(DualPricingUpdater * pricingUpdater);
    void setRatiotestUpdater(DualRatiotestUpdater* ratiotestUpdater);

    void update();

protected:
    DualPricingUpdater * m_pricingUpdater;
    DualRatiotestUpdater * m_ratiotestUpdater;
};

#endif	/* DUALUPDATER_H */

