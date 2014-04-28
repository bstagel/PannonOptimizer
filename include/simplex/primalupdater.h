/**
 * @file   primalupdater.h
 */

#ifndef PRIMALUPDATER_H
#define	PRIMALUPDATER_H

#include <globals.h>

#include <simplex/primalpricingupdater.h>

class PrimalUpdater {
public:
    PrimalUpdater();
    ~PrimalUpdater();

    void setPricingUpdater(PrimalPricingUpdater * pricingUpdater);

    void update(int phase);

protected:
    PrimalPricingUpdater * m_pricingUpdater;
};

#endif	/* PRIMALUPDATER_H */

