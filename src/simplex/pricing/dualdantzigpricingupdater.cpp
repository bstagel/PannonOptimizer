/**
 * @file   dualdantzigpricingupdater.cpp
 */

#include <simplex/pricing/dualdantzigpricingupdater.h>
#include <simplex/simplex.h>

static const unsigned int dualPhase1ClusterCount = 1;
static const unsigned int dualPhase1VisitedClusterCount = 1;
static const unsigned int dualPhase1ImprovingVariableCount = 0;

static const unsigned int dualPhase2ClusterCount = 1;
static const unsigned int dualPhase2VisitedClusterCount = 1;
static const unsigned int dualPhase2ImprovingVariableCount = 0;

DualDantzigPricingUpdater::DualDantzigPricingUpdater(const Vector &basicVariableValues,
                                                     IndexList<> * basicVariableFeasibilities,
                                                     const IndexList<> & reducedCostFeasibilities,
                                                     const std::vector<int> & basisHead,
                                                     const SimplexModel & simplexModel,
                                                     const Basis & basis):
    DualPricingUpdater(basicVariableValues, basicVariableFeasibilities,
                       reducedCostFeasibilities, basisHead, simplexModel,
                       basis)
{
    m_phase2ReducedCost = 0;
    m_phase2Index = -1;
    m_used.clear();
    m_used.resize( m_basisHead.size(), false );

    m_phase1Simpri.init(dualPhase1ClusterCount,
                        dualPhase1VisitedClusterCount,
                        dualPhase1ImprovingVariableCount,
                        simplexModel.getMatrix().rowCount());
    unsigned int index;
    for (index = 0; index < simplexModel.getMatrix().rowCount(); index++) {
        m_phase1Simpri.insertCandidate(index);
    }

    m_phase2Simpri.init(dualPhase2ClusterCount,
                        dualPhase2VisitedClusterCount,
                        dualPhase2ImprovingVariableCount,
                        simplexModel.getMatrix().rowCount());
    for (index = 0; index < simplexModel.getMatrix().rowCount(); index++) {
        m_phase2Simpri.insertCandidate(index);
    }
}

DualDantzigPricingUpdater::DualDantzigPricingUpdater(const DualDantzigPricingUpdater& orig):
    DualPricingUpdater(orig)
{
}

DualDantzigPricingUpdater::~DualDantzigPricingUpdater() {
}

void DualDantzigPricingUpdater::updatePhase1() {
//    m_used.clear();
//    m_used.resize( m_basisHead.size(), false );
}

void DualDantzigPricingUpdater::updatePhase2() {
//    m_used.clear();
//    m_used.resize( m_basisHead.size(), false );
}
