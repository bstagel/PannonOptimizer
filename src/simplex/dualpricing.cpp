/**
 * @file dualpricing.cpp
 */

#include <simplex/dualpricing.h>

DualPricing::DualPricing(const Vector & basicVariableValues,
                         IndexList<> * basicVariableFeasibilities,
                         const IndexList<> & reducedCostFeasibilities,
                         const std::vector<int> & basisHead,
                         const SimplexModel & simplexModel,
                         const Basis & basis):
    m_basicVariableValues(basicVariableValues),
    m_basicVariableFeasibilities(basicVariableFeasibilities),
    m_reducedCostFeasibilities(reducedCostFeasibilities),
    m_basisHead(basisHead),
    m_simplexModel(simplexModel),
    m_basis(basis),
    m_reducedCost(0.0)
{

}

DualPricing::~DualPricing() {

}
