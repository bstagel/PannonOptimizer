#ifndef DUALSTEEPESTEDGEPRICING_H
#define DUALSTEEPESTEDGEPRICING_H

#include <simplex/dualpricing.h>
#include <vector>

class DualSteepestEdgePricing: public DualPricing {
public:
    DualSteepestEdgePricing(const Vector &basicVariableValues,
                            IndexList<> *basicVariableFeasibilities,
                            const IndexList<> &reducedCostFeasibilities,
                            const std::vector<int> & basisHead,
                            const SimplexModel & simplexModel,
                            const Basis &basis);

    virtual ~DualSteepestEdgePricing();

    int performPricingPhase1();
    int performPricingPhase2();

    void update(int incomingIndex,
                int outgoingIndex,
                const Vector & incomingAlpha,
                const Vector & pivotRow,
                const Vector & pivotRowOfBasisInverse);

    void checkAndFix();
private:

    std::vector<Numerical::Double> m_weights;
    std::vector<unsigned int> m_updateCounters;
    std::vector<unsigned int> m_recomuteIndices;

    unsigned int m_previousPhase;

    void initWeights();
};


#endif // DUALSTEEPESTEDGEPRICING_H
