/**
 * @file primalpricing.h
 */

#ifndef PRIMALPRICING_H
#define	PRIMALPRICING_H

#include <globals.h>
#include <simplex/primalpricingupdater.h>

class SimplexModel;

class PrimalPricing {
public:
    PrimalPricing(const SimplexModel & model,
                  const Vector & reducedCosts);

    virtual ~PrimalPricing();

    virtual int performPricingPhase1() = 0;
    virtual int performPricingPhase2() = 0;

    inline Numerical::Double getReducedCost() const {
        return m_reducedCost;
    }

    virtual void releaseUsed() = 0;
    virtual void lockLastIndex() = 0;
protected:
    const SimplexModel & m_model;

    Numerical::Double m_reducedCost;

    const Vector & m_reducedCosts;
};


#endif	/* PRIMALPRICING_H */

