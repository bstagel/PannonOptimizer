#ifndef DUALDEVEXPRICING_H
#define DUALDEVEXPRICING_H

#include <simplex/dualpricing.h>
#include <vector>

class DualDevexPricing: public DualPricing {
public:
    DualDevexPricing(const Vector &basicVariableValues,
                       IndexList<> *basicVariableFeasibilities,
                       const IndexList<> &reducedCostFeasibilities,
                       const std::vector<int> & basisHead,
                       const SimplexModel & simplexModel,
                       const Basis &basis);

    virtual ~DualDevexPricing();

    int performPricingPhase1();
    int performPricingPhase2();

    void update(int incomingIndex,
                            int outgoingIndex,
                            const Vector & incomingAlpha,
                            const Vector & pivotRow);

private:
    std::vector<char> m_referenceFramework;

    std::vector<Numerical::Double> m_weights;

    unsigned int m_previousPhase;

    void setReferenceFramework();
};

#endif // DUALDEVEXPRICING_H
