#ifndef PRICINGCONTROLLER_H
#define PRICINGCONTROLLER_H

#include <simplex/pricing/dualdantzigpricing.h>
#include <simplex/pricing/dualdevexpricing.h>

class DualPricingController {
public:
    DualPricingController(const Vector &basicVariableValues,
                          IndexList<> *basicVariableFeasibilities,
                          const IndexList<> &reducedCostFeasibilities,
                          const std::vector<int> & basisHead,
                          const SimplexModel & simplexModel,
                          const Basis &basis);

    ~DualPricingController();

    int performPricingPhase1();
    int performPricingPhase2();

    void update(int incomingIndex,
                int outgoingIndex,
                const Vector & incomingAlpha,
                const Vector & pivotRow,
                const Vector & pivotRowOfBasisInverse);

    Numerical::Double getReducedCost() const {
        return m_reducedCost;
    }

    void releaseUsed() {
        m_dantzig->releaseUsed();
        m_devex->releaseUsed();
    }

    void lockLastIndex() {
        m_dantzig->lockLastIndex();
        m_devex->lockLastIndex();
    }

private:
    DualDantzigPricing * m_dantzig;

    DualDevexPricing * m_devex;

    //DualPricing * m_last;

    Numerical::Double m_reducedCost;
};

#endif // PRICINGCONTROLLER_H
