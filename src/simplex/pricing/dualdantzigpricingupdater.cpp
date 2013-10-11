/**
 * @file   dualdantzigpricingupdater.cpp
 */

#include <simplex/pricing/dualdantzigpricingupdater.h>
#include <simplex/simplex.h>

DualDantzigPricingUpdater::DualDantzigPricingUpdater(const Vector &basicVariableValues,
                                                     const IndexList<> & variableFeasibilities,
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
}

DualDantzigPricingUpdater::DualDantzigPricingUpdater(const DualDantzigPricingUpdater& orig):
    DualPricingUpdater(orig)
{
    copy(orig);
}

DualDantzigPricingUpdater::~DualDantzigPricingUpdater() {
    release();
}

DualDantzigPricingUpdater & DualDantzigPricingUpdater::operator = (
        const DualDantzigPricingUpdater & orig) {
    if (&orig == this) {
        return *this;
    }
    //DualPricingUpdater::operator=(orig);
    release();
    copy(orig);
    return *this;
}

void DualDantzigPricingUpdater::updatePhase1() {
}

void DualDantzigPricingUpdater::updatePhase2() {
}
