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

//TODO: Az X_B is kell majd meg
PrimalDantzigPricing::PrimalDantzigPricing(const SimplexModel & model,
                                           PrimalPricingUpdater * updater,
                                           const std::vector<int> & basisHead):
    PrimalPricing(model, updater),
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
