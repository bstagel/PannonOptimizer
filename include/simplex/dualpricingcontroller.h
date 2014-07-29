/**
 * @file dualpricingcontroller.h File containing the DualPricingController class.
 */
#ifndef PRICINGCONTROLLER_H
#define PRICINGCONTROLLER_H

#include <simplex/pricing/dualdantzigpricing.h>
#include <simplex/pricing/dualdevexpricing.h>

/**
 * The dual pricing controller is needed because of the paralell pricing framework.
 * @class DualPricingController
 */
class DualPricingController {
public:

    /**
     * Initializing constructor of the class.
     * It calls the constructors of the DualDantzigPricing and the DualDevexPricing.
     * @param basicVariableValues X_B vector
     * @param basicVariableFeasibilities the feasibility of the X_B vector
     * @param reducedCostFeasibilities feasibility sets of the reduced costs
     * @param basisHead basicvariable indices
     * @param simplexModel model of the LP problem
     * @param basis the B basis
     * @constructor
     */
    DualPricingController(const Vector &basicVariableValues,
                          IndexList<> *basicVariableFeasibilities,
                          const IndexList<> &reducedCostFeasibilities,
                          const std::vector<int> & basisHead,
                          const SimplexModel & simplexModel,
                          const Basis &basis);

    /**
     * Default destructor of the class.
     * @destructor
     */
    ~DualPricingController();

    /**
     * This function performs the parallel pricing in phase 1.
     * It starts to threads one with a dantzig and one with a devex strategy.
     * @return the variable index of the selected variable.
     */
    int performPricingPhase1();

    /**
     * This function performs the parallel pricing in phase 2.
     * It starts to threads one with a dantzig and one with a devex strategy.
     * @return the variable index of the selected variable.
     */
    int performPricingPhase2();

    /**
     * This function starts two threads and calles the update functions for dantzig and devex modules.
     * See DualDevexPricing for details.
     */
    void update(int incomingIndex,
                int outgoingIndex,
                const Vector & incomingAlpha,
                const Vector & pivotRow,
                const Vector & pivotRowOfBasisInverse);

    /**
     * Returnst the reduced cost selected by the pricing.
     * @return
     */
    Numerical::Double getReducedCost() const {return m_reducedCost;}

    /**
     * This calls the releaseUsed function for the dantzig and the devex modules.
     */
    void releaseUsed() {
        m_dantzig->releaseUsed();
        m_devex->releaseUsed();
    }

    /**
     * This calls the lockLastIndex function for the dantzig and the devex modules.
     */
    void lockLastIndex() {
        m_dantzig->lockLastIndex();
        m_devex->lockLastIndex();
    }

private:

    /**
     * Pointer to the dantzig pricing module.
     */
    DualDantzigPricing * m_dantzig;

    /**
     * Pointer to the devex pricing module.
     */
    DualDevexPricing * m_devex;

    //DualPricing * m_last;

    /**
     * The reduced cost selected by the pricing.
     */
    Numerical::Double m_reducedCost;
};

#endif // PRICINGCONTROLLER_H
