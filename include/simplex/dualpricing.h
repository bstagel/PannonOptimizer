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
#include <simplex/pricing.h>

class SimplexModel;

class DualPricing: public Pricing {
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

    void releaseUsed();
    void lockLastIndex();

    virtual void update(int incomingIndex,
                        int outgoingIndex,
                        const Vector & incomingAlpha,
                        const Vector & pivotRow,
                        const Vector & pivotRowOfBasisInverse) = 0;

    void init() {}
protected:
    const Vector & m_basicVariableValues;

    IndexList<> * m_basicVariableFeasibilities;

    const IndexList<> & m_reducedCostFeasibilities;

    const std::vector<int> & m_basisHead;

    const SimplexModel & m_simplexModel;

    const Basis & m_basis;

    Numerical::Double m_reducedCost;

    std::vector<char> m_used;

    int m_outgoingIndex;

    Numerical::Double m_phase2ReducedCost;

    Simpri m_phase1Simpri;

    Simpri m_phase2Simpri;

    Numerical::Double * m_phase1ReducedCosts;

    //Parameter references
    const double & m_feasibilityTolerance;
    const int & m_phaseIClusters;
    const int & m_phaseIVisitClusters;
    const int & m_phaseIImprovingCandidates;
    const int & m_phaseIIClusters;
    const int & m_phaseIIVisitClusters;
    const int & m_phaseIIImprovingCandidates;

    void clearPhase1ReducedCosts();

    void initPhase1();
    void initPhase2();

};

#endif	/* DUALPRICING_H */

