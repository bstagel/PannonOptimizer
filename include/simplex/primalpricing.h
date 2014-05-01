/**
 * @file primalpricing.h
 */

#ifndef PRIMALPRICING_H
#define	PRIMALPRICING_H

#include <globals.h>
#include <utils/indexlist.h>
#include <simplex/basis.h>

class SimplexModel;

class PrimalPricing {
public:
    PrimalPricing(const Vector & basicVariableValues,
                  const IndexList<> & basicVariableFeasibilities,
                  IndexList<> * reducedCostFeasibilities,
                  const IndexList<const Numerical::Double*> & variableStates,
                  const std::vector<int> & basisHead,
                  const SimplexModel & model,
                  const Basis & basis,
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
    const Vector & m_basicVariableValues;

    const IndexList<> & m_basicVariableFeasibilities;

    IndexList<> * m_reducedCostFeasibilities;

    const IndexList<const Numerical::Double*> & m_variableStates;

    const std::vector<int> & m_basisHead;

    const SimplexModel & m_simplexModel;

    const Basis & m_basis;

    Numerical::Double m_reducedCost;

    const Vector & m_reducedCosts;

    int m_phase2Index;

    Numerical::Double m_phase2ReducedCost;

    std::vector<char> m_used;
};


#endif	/* PRIMALPRICING_H */

