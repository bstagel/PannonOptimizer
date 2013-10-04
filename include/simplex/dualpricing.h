/* 
 * File:   dualpricing.h
 * Author: smidla
 *
 * Created on 2013. augusztus 26., 0:55
 */

#ifndef DUALPRICING_H
#define	DUALPRICING_H

#include <globals.h>
#include <simplex/dualpricingupdater.h>

class SimplexModel;

class DualPricing {
public:
    DualPricing(const SimplexModel & model,
                const DualPricingUpdater & updater);

    virtual ~DualPricing();

    unsigned int performPricingPhase1();
    unsigned int performPricingPhase2();
private:
    const SimplexModel & m_model;

    const DualPricingUpdater & m_updater;
};

#endif	/* DUALPRICING_H */

