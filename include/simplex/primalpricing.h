/**
 * @file primalpricing.h
 */

#ifndef PRIMALPRICING_H
#define	PRIMALPRICING_H

#include <globals.h>
#include <utils/indexlist.h>
#include <simplex/basis.h>
#include <simplex/pricing/simpri.h>
#include <simplex/pricing.h>

class SimplexModel;

class PrimalPricing: public Pricing {
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

    void releaseUsed();
    void lockLastIndex();

    virtual void update(int incomingIndex,
                        int outgoingIndex,
                        const Vector * incomingAlpha,
                        const Vector * pivotRow) = 0;

    void init();
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

    int m_incomingIndex;

    std::vector<Numerical::Double> m_negativeSums;
    std::vector<Numerical::Double> m_positiveSums;
    std::vector<Numerical::Double> m_phase1ReducedCosts;

    Simpri m_phase1Simpri;

    Simpri m_phase2Simpri;

    //Parameter references
    const double & m_feasibilityTolerance;
    const double & m_optimalityTolerance;
    const int & m_phaseIClusters;
    const int & m_phaseIVisitClusters;
    const int & m_phaseIImprovingCandidates;
    const int & m_phaseIIClusters;
    const int & m_phaseIIVisitClusters;
    const int & m_phaseIIImprovingCandidates;

    void initPhase1();
};


#endif	/* PRIMALPRICING_H */

