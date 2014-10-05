/**
 * @file dualpricingcontroller.h File containing the DualPricingController class.
 */
#ifndef PRICINGCONTROLLER_H
#define PRICINGCONTROLLER_H

#include <simplex/pricing/dualdantzigpricing.h>
#include <simplex/pricing/dualdevexpricing.h>

/**
 * The dual pricing controller is needed because of the parallel pricing framework.
 *
 * @class DualPricingController
 */
class DualPricingController {
public:

    /**
     * Initializing constructor of the class.
     * It calls the constructors of the DualDantzigPricing and the DualDevexPricing.
     *
     * @param basicVariableValues The basic variable values (X_B vector).
     * @param basicVariableFeasibilities The feasibility states of the X_B values.
     * @param reducedCostFeasibilities The feasibility states of the reduced costs.
     * @param basisHead The basic variable indices.
     * @param simplexModel The model of the LP problem.
     * @param basis The B basis.
     *
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
     *
     * @destructor
     */
    ~DualPricingController();

    /**
     * This function performs the parallel pricing in phase 1.
     * It starts a thread with a dantzig and one with a devex pricing strategy.
     *
     * @return The variable index of the selected variable.
     */
    int performPricingPhase1();

    /**
     * This function performs the parallel pricing in phase 2.
     * It starts a thread with a dantzig and one with a devex pricing strategy.
     *
     * @return The variable index of the selected variable.
     */
    int performPricingPhase2();

    /**
     * This function starts two threads and calles the update functions for dantzig and devex modules.
     *
     * @param incomingIndex The index of the incoming variable.
     * @param outgoingIndex The index of the outgoing variable.
     * @param incomingAlpha The incoming Alpha vector.
     * @param pivotRow The pivot row vector.
     * @param pivotRowOfBasisInverse The pivot row vector of the basis inverse.
     */
    void update(int incomingIndex,
                int outgoingIndex,
                const Vector & incomingAlpha,
                const Vector & pivotRow,
                const Vector & pivotRowOfBasisInverse);

    /**
     * Returns the reduced cost selected by the pricing.
     *
     * @return the reduced cost selected by the pricing.
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
