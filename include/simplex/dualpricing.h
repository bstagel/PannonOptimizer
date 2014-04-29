/**
 * @file dualpricing.h
 */

#ifndef DUALPRICING_H
#define	DUALPRICING_H

#include <globals.h>
#include <linalg/vector.h>
#include <utils/indexlist.h>
#include <simplex/basis.h>
#include <simplex/pricing/simpri.h>

class SimplexModel;

class DualPricing {
public:
    DualPricing(const Vector &basicVariableValues,
                IndexList<> * basicVariableFeasibilities,
                const IndexList<> & reducedCostFeasibilities,
                const std::vector<int> & basisHead,
                const SimplexModel & simplexModel,
                const Basis & basis);

    virtual ~DualPricing();

    virtual int performPricingPhase1() = 0;
    virtual int performPricingPhase2() = 0;

    inline Numerical::Double getReducedCost() const {
        return m_reducedCost;
    }

    virtual void releaseUsed() = 0;
    virtual void lockLastIndex() = 0;
protected:
    const Vector & m_basicVariableValues;

    IndexList<> * m_basicVariableFeasibilities;

    const IndexList<> & m_reducedCostFeasibilities;

    const std::vector<int> & m_basisHead;

    const SimplexModel & m_simplexModel;

    const Basis & m_basis;

    Numerical::Double m_reducedCost;
};

#endif	/* DUALPRICING_H */

