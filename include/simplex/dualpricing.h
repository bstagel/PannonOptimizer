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
                const DualPricingUpdater & updater);

    virtual ~DualPricing();

    //TODO: EXCEPTIONS
    //TODO: Joco a visszateres sima int lett az exceptionok helyett
    virtual int performPricingPhase1() = 0;
    virtual int performPricingPhase2() = 0;

    inline Numerical::Double getReducedCost() const {
        return m_reducedCost;
    }
protected:
    const SimplexModel & m_model;

    const DualPricingUpdater & m_updater;

    Numerical::Double m_reducedCost;
};

#endif	/* DUALPRICING_H */

