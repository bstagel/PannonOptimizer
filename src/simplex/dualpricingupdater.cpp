/**
 * @file dualpricingupdater.cpp
 */

#include <simplex/dualpricingupdater.h>

DualPricingUpdater::DualPricingUpdater(const Vector &basicVariableValues,
                                       const IndexList<> & variableFeasibilities,
                                       const IndexList<> & reducedCostFeasibilities,
                                       const std::vector<int> & basisHead,
                                       const SimplexModel &simplexModel,
                                       const Basis & basis):
    m_basicVariableValues(basicVariableValues),
    m_variableFeasibilities(variableFeasibilities),
    m_reducedCostFeasibilities(reducedCostFeasibilities),
    m_basisHead(basisHead),
    m_simplexModel(simplexModel),
    m_basis(basis)
{

}

DualPricingUpdater::~DualPricingUpdater() {

}
