/**
 * @file   primalupdater.h
 */

#ifndef PRIMALUPDATER_H
#define	PRIMALUPDATER_H

#include <globals.h>

#include <simplex/primalpricingupdater.h>
#include <simplex/primalratiotestupdater.h>

class PrimalUpdater {
public:
    PrimalUpdater();
    ~PrimalUpdater();

    void setPricingUpdater(PrimalPricingUpdater * pricingUpdater);
    void setRatiotestUpdater(PrimalRatiotestUpdater *ratiotestUpdater);

    void update(int phase);

protected:
    PrimalPricingUpdater * m_pricingUpdater;
    PrimalRatiotestUpdater * m_ratiotestUpdater;
};

#endif	/* PRIMALUPDATER_H */

