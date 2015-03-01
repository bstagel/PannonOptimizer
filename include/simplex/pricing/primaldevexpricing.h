#ifndef PRIMALDEVEXPRICING_H
#define PRIMALDEVEXPRICING_H

#include <globals.h>

#include <simplex/primalpricing.h>
#include <utils/exceptions.h>
#include <utils/indexlist.h>
#include <vector>

class PrimalDevexPricing: public PrimalPricing
{
public:
    PrimalDevexPricing(const DenseVector & basicVariableValues,
                       const IndexList<> & basicVariableFeasibilities,
                       IndexList<> * reducedCostFeasibilities,
                       const IndexList<const Numerical::Double*> & variableStates,
                       const std::vector<int> & basisHead,
                       const SimplexModel & model,
                       const Basis & basis,
                       const DenseVector & reducedCosts);
    virtual ~PrimalDevexPricing();

    int performPricingPhase1();
    int performPricingPhase2();

    void update(int incomingIndex,
                int outgoingIndex,
                const DenseVector *incomingAlpha,
                const DenseVector *pivotRow);

private:

    std::vector<Numerical::Double> m_weights;
    std::vector<char> m_referenceFrameWorkMask;

    bool m_weightsReady;

    void re();

    void release();

    void setReferenceFramework();

    Numerical::Double getExactDevexWeigth(const DenseVector &vector, unsigned int index);

};

#endif // PRIMALDEVEXPRICING_H
