/**
 * @file   primaldantzigpricingupdater.cpp
 */

#include <simplex/pricing/primaldantzigpricingupdater.h>
#include <simplex/simplex.h>

PrimalDantzigPricingUpdater::PrimalDantzigPricingUpdater(const Vector &basicVariableValues,
                                                         IndexList<> * variableFeasibilities,
                                                         const IndexList<> & reducedCostFeasibilities,
                                                         const std::vector<int> & basisHead,
                                                         const SimplexModel & simplexModel,
                                                         const Basis & basis):
    PrimalPricingUpdater(basicVariableValues, variableFeasibilities,
                         reducedCostFeasibilities, basisHead, simplexModel,
                         basis)
{

}

PrimalDantzigPricingUpdater::PrimalDantzigPricingUpdater(const PrimalDantzigPricingUpdater& orig):
    PrimalPricingUpdater(orig)
{
}

PrimalDantzigPricingUpdater::~PrimalDantzigPricingUpdater() {
}

void PrimalDantzigPricingUpdater::updatePhase1() {
//    m_used.clear();
//    m_used.resize( m_basisHead.size(), false );
}

void PrimalDantzigPricingUpdater::updatePhase2() {
//    m_used.clear();
//    m_used.resize( m_basisHead.size(), false );
}
