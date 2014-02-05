/**
 * @file   primaldantzigpricing.cpp
 */

#include <simplex/pricing/primaldantzigpricing.h>
#include <simplex/simplexparameterhandler.h>
#include <simplex/simplex.h>


PrimalDantzigPricing::PrimalDantzigPricing(const PrimalDantzigPricing& orig):
    PrimalPricing(orig),
    m_updater(orig.m_updater),
    m_basisHead(orig.m_basisHead)
{
    copy(orig);
}

PrimalDantzigPricing::PrimalDantzigPricing(const SimplexModel & model,
                                           PrimalPricingUpdater * updater,
                                           const std::vector<int> & basisHead):
    PrimalPricing(model),
    m_updater(dynamic_cast<PrimalDantzigPricingUpdater *>(updater)),
    m_basisHead(basisHead)
{

}
PrimalDantzigPricing::~PrimalDantzigPricing()
{
    release();
}

void PrimalDantzigPricing::copy(const PrimalDantzigPricing &orig)
{
    __UNUSED(orig);
}

void PrimalDantzigPricing::release()
{

}

int PrimalDantzigPricing::performPricingPhase1()
{

}

int PrimalDantzigPricing::performPricingPhase2()
{

}

void PrimalDantzigPricing::releaseUsed()
{

}

void PrimalDantzigPricing::lockLastIndex()
{

}
