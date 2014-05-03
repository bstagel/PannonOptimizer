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

static const unsigned int dualPhase1ClusterCount = 1;
static const unsigned int dualPhase1VisitedClusterCount = 1;
static const unsigned int dualPhase1ImprovingVariableCount = 0;

static const unsigned int dualPhase2ClusterCount = 1;
static const unsigned int dualPhase2VisitedClusterCount = 1;
static const unsigned int dualPhase2ImprovingVariableCount = 0;


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

    void releaseUsed();
    void lockLastIndex();

    virtual void update(int incomingIndex,
                        int outgoingIndex,
                        const Vector & incomingAlpha,
                        const Vector & pivotRow) = 0;
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

    void clearPhase1ReducedCosts();

    void initPhase1();
    void initPhase2();

};

#endif	/* DUALPRICING_H */

