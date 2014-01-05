/**
 * @file   dualdantzigpricingupdater.cpp
 */

#include <simplex/pricing/dualdantzigpricingupdater.h>
#include <simplex/simplex.h>

DualDantzigPricingUpdater::DualDantzigPricingUpdater(const Vector &basicVariableValues,
                                                     IndexList<> * variableFeasibilities,
                                                     const IndexList<> & reducedCostFeasibilities,
                                                     const std::vector<int> & basisHead,
                                                     const SimplexModel & simplexModel,
                                                     const Basis & basis):
    DualPricingUpdater(basicVariableValues, variableFeasibilities,
                       reducedCostFeasibilities, basisHead, simplexModel,
                       basis)
{
    m_phase2ReducedCost = 0;
    m_phase2Index = -1;
    m_used.clear();
    m_used.resize( m_basisHead.size(), false );
}

DualDantzigPricingUpdater::DualDantzigPricingUpdater(const DualDantzigPricingUpdater& orig):
    DualPricingUpdater(orig)
{
}

DualDantzigPricingUpdater::~DualDantzigPricingUpdater() {
}

void DualDantzigPricingUpdater::updatePhase1() {
    m_used.clear();
    m_used.resize( m_basisHead.size(), false );
}

void DualDantzigPricingUpdater::updatePhase2() {
    m_used.clear();
    m_used.resize( m_basisHead.size(), false );
}
