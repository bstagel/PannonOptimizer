/* 
 * File:   dualdantzigpricing.cpp
 * Author: smidla
 * 
 * Created on 2013. augusztus 26., 0:59
 */

#include <simplex/pricing/dualdantzigpricing.h>

DualDantzigPricing::DualDantzigPricing(const SimplexModel & model,
                                       const DualPricingUpdater &updater):
    DualPricing(model, updater),
    m_updater(dynamic_cast<const DualDantzigPricingUpdater &>(updater))
{

}

DualDantzigPricing::DualDantzigPricing(const DualDantzigPricing& orig):
    DualPricing(orig),
    m_updater(orig.m_updater)
{
    __UNUSED(orig);
}

DualDantzigPricing::~DualDantzigPricing()
{
}

unsigned int DualDantzigPricing::performPricingPhase1() {
    const unsigned int variableCount = m_model.getMatrix().columnCount();
    Numerical::Double max = m_updater.m_phase1ReducedCosts[0];
    unsigned int maxIndex = 0;
    unsigned int index;
    for (index = 1; index < variableCount; index++) {
        if ( Numerical::fabs(m_updater.m_phase1ReducedCosts[index]) > max ) {
            max = Numerical::fabs(m_updater.m_phase1ReducedCosts[index]);
            maxIndex = index;
        }
    }
    m_reducedCost = m_updater.m_phase1ReducedCosts[maxIndex];
    return maxIndex;
}

unsigned int DualDantzigPricing::performPricingPhase2() throw (OptimalException) {
    if ( m_updater.m_phase2Index == -1 ) {
        throw OptimalException("Optimal solution found.");
    }
    return m_updater.m_phase2Index;
}
