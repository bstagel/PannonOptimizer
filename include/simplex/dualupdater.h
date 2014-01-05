/**
 * @file dualapdater.h
 */

#ifndef DUALUPDATER_H
#define	DUALUPDATER_H

#include <globals.h>

#include <simplex/dualpricingupdater.h>
#include <simplex/dualratiotestupdater.h>

class DualUpdater {
public:
    DualUpdater();
    ~DualUpdater();

    void setPricingUpdater(DualPricingUpdater * pricingUpdater);
    void setRatiotestUpdater(DualRatiotestUpdater* ratiotestUpdater);

    void update(int phase);

protected:
    DualPricingUpdater * m_pricingUpdater;
    DualRatiotestUpdater * m_ratiotestUpdater;

};

#endif	/* DUALUPDATER_H */

