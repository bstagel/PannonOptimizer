#ifndef DUALSTEEPESTEDGEPRICING_H
#define DUALSTEEPESTEDGEPRICING_H

#include <simplex/dualpricing.h>
#include <vector>

class DualSteepestEdgePricing: public DualPricing {
public:
    DualSteepestEdgePricing(const DenseVector &basicVariableValues,
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
                const DenseVector &incomingAlpha,
                const DenseVector &pivotRow,
                const DenseVector &pivotRowOfBasisInverse);

    void checkAndFix();

    bool weightsAreReady() const {
        return m_weightsReady;
    }

    void initWeights();
private:

    std::vector<Numerical::Double> m_weights;
    std::vector<unsigned int> m_updateCounters;
    std::vector<unsigned int> m_recomuteIndices;
    std::vector<char> m_isBasicVariable;

    std::vector<int> m_shadowBasisHead;
    std::vector<unsigned int> m_basisPositions;
    unsigned int m_previousPhase;

    bool m_weightsReady;

};


#endif // DUALSTEEPESTEDGEPRICING_H
