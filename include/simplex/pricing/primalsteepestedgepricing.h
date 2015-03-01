#ifndef PRIMALSTEEPESTEDGEPRICING_H
#define PRIMALSTEEPESTEDGEPRICING_H

#include <globals.h>

#include <simplex/primalpricing.h>
#include <utils/exceptions.h>
#include <utils/indexlist.h>
#include <vector>

class PrimalSteepestEdgePricing: public PrimalPricing
{
public:
    PrimalSteepestEdgePricing(const DenseVector & basicVariableValues,
                              const IndexList<> & basicVariableFeasibilities,
                              IndexList<> * reducedCostFeasibilities,
                              const IndexList<const Numerical::Double*> & variableStates,
                              const std::vector<int> & basisHead,
                              const SimplexModel & model,
                              const Basis & basis,
                              const DenseVector & reducedCosts);
    virtual ~PrimalSteepestEdgePricing();

    int performPricingPhase1();
    int performPricingPhase2();

    void update(int incomingIndex,
                int outgoingIndex,
                const DenseVector *incomingAlpha,
                const DenseVector *pivotRow);

private:

    std::vector<Numerical::Double> m_weights;

    bool m_weightsReady;

    void recomputeSteepestEdgeWeights();

    void release();

};

#endif // PRIMALSTEEPESTEDGEPRICING_H
