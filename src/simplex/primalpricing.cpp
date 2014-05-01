
/**
 * @file primalpricing.cpp
 */

#include <simplex/primalpricing.h>

PrimalPricing::PrimalPricing(const Vector & basicVariableValues,
                             const IndexList<> & basicVariableFeasibilities,
                             IndexList<> * reducedCostFeasibilities,
                             const IndexList<const Numerical::Double *> & variableStates,
                             const std::vector<int> & basisHead,
                             const SimplexModel & model,
                             const Basis & basis,
                             const Vector & reducedCosts):
    m_basicVariableValues(basicVariableValues),
    m_basicVariableFeasibilities(basicVariableFeasibilities),
    m_reducedCostFeasibilities(reducedCostFeasibilities),
    m_variableStates(variableStates),
    m_basisHead(basisHead),
    m_simplexModel(model),
    m_basis(basis),
    m_reducedCosts(reducedCosts)
{
    m_reducedCost = 0.0;

    m_phase2ReducedCost = 0;
    m_phase2Index = -1;
    m_used.clear();
    m_used.resize( m_simplexModel.getRowCount() + m_simplexModel.getColumnCount(), false );

}

PrimalPricing::~PrimalPricing() {

}
