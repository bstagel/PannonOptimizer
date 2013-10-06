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

    virtual unsigned int performPricingPhase1() = 0;
    virtual unsigned int performPricingPhase2() = 0;

    inline Numerical::Double getReducedCost() const {
        return m_reducedCost;
    }
protected:
    const SimplexModel & m_model;

    const DualPricingUpdater & m_updater;

    Numerical::Double m_reducedCost;
};

#endif	/* DUALPRICING_H */

