/**
 * @file dualpricing.h
 */

#ifndef DUALPRICING_H
#define	DUALPRICING_H

#include <globals.h>
#include <simplex/dualpricingupdater.h>

class SimplexModel;

class DualPricing {
public:
    DualPricing(const SimplexModel & model,
                DualPricingUpdater * updater);

    virtual ~DualPricing();

    virtual int performPricingPhase1() = 0;
    virtual int performPricingPhase2() = 0;

    inline Numerical::Double getReducedCost() const {
        return m_reducedCost;
    }

    virtual void releaseUsed() = 0;
protected:
    const SimplexModel & m_model;

    DualPricingUpdater * m_updater;

    Numerical::Double m_reducedCost;
};

#endif	/* DUALPRICING_H */

