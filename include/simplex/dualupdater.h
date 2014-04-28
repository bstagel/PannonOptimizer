/**
 * @file dualapdater.h
 */

#ifndef DUALUPDATER_H
#define	DUALUPDATER_H

#include <globals.h>

#include <simplex/dualpricingupdater.h>

class DualUpdater {
public:
    DualUpdater();
    ~DualUpdater();

    void setPricingUpdater(DualPricingUpdater * pricingUpdater);

    void update(int phase);

protected:
    DualPricingUpdater * m_pricingUpdater;
};

#endif	/* DUALUPDATER_H */

