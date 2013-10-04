/* 
 * File:   dualdantzigpricingupdater.cpp
 * Author: smidla
 * 
 * Created on 2013. augusztus 26., 1:00
 */

#include <simplex/pricing/dualdantzigpricingupdater.h>

DualDantzigPricingUpdater::DualDantzigPricingUpdater(const Vector &basicVariableValues,
                                                     IndexList<> variableFeasibilities,
                                                     std::vector<int> basisHead):
    DualPricingUpdater(basicVariableValues, variableFeasibilities, basisHead)
{
}

DualDantzigPricingUpdater::DualDantzigPricingUpdater(const DualDantzigPricingUpdater& orig)
{
    __UNUSED(orig);
}

DualDantzigPricingUpdater::~DualDantzigPricingUpdater()
{
}

