#include <simplex/dualpricingupdater.h>

DualPricingUpdater::DualPricingUpdater(const Vector & basicVariableValues,
                                       IndexList<> variableFeasibilities,
                                       std::vector<int> basisHead):
    m_basicVariableValues(basicVariableValues),
    m_variableFeasibilities(variableFeasibilities),
    m_basisHead(basisHead)
{

}

DualPricingUpdater::~DualPricingUpdater() {

}
