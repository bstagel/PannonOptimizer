/**
 * @file   dualdantzigpricing.cpp
 */

#include <simplex/pricing/dualdantzigpricing.h>
#include <simplex/simplex.h>

DualDantzigPricing::DualDantzigPricing(const Vector & basicVariableValues,
                                       IndexList<> * basicVariableFeasibilities,
                                       const IndexList<> & reducedCostFeasibilities,
                                       const std::vector<int> & basisHead,
                                       const SimplexModel & simplexModel,
                                       const Basis & basis):
    DualPricing(basicVariableValues,
                basicVariableFeasibilities,
                reducedCostFeasibilities,
                basisHead,
                simplexModel,
                basis)
{

}

DualDantzigPricing::~DualDantzigPricing()
{

}

int DualDantzigPricing::performPricingPhase1() {
    initPhase1();
    //TODO: A sorok szamat hivjuk mindenutt rowCountnak, az oszlopokat meg columnCount-nak, ne keverjunk
//    const unsigned int variableCount = m_simplexModel.getMatrix().rowCount();
    Numerical::Double max = 0;
    m_outgoingIndex = -1;
    unsigned int index;
    m_phase1Simpri.start();

    while (m_phase1Simpri.getCandidateIndex(&index) ) {
        if ( m_used[index] == true ) {
            continue;
        }
        //LPINFO("SIMPRI: " << index);
        unsigned int variableIndex = m_basisHead.at(index);
        Variable::VARIABLE_TYPE variableType = m_simplexModel.getVariable(variableIndex).getType();
        if ( variableType == Variable::FIXED ||
             variableType == Variable::BOUNDED ||
             (variableType == Variable::PLUS && m_phase1ReducedCosts[index] > 0) ||
             (variableType == Variable::MINUS && m_phase1ReducedCosts[index] < 0)) {
            if ( Numerical::fabs(m_phase1ReducedCosts[index]) > max ) {
                max = Numerical::fabs(m_phase1ReducedCosts[index]);
                m_outgoingIndex = index;
                m_phase1Simpri.improvingIndexFound();
            }
        }
    }

    if (m_outgoingIndex != -1) {
        m_reducedCost = m_phase1ReducedCosts[m_outgoingIndex];
    }
    return m_outgoingIndex;
}

int DualDantzigPricing::performPricingPhase2() {
    initPhase2();

    IndexList<>::Iterator iter, iterEnd;
    Numerical::Double max = -1.0;
    unsigned int rowIndex;
    m_outgoingIndex = -1;
    //m_updater->m_simplexModel.getVariable(0);
    m_phase2Simpri.start();
    Numerical::Double difference;
    while ( m_phase2Simpri.getCandidateIndex(&rowIndex) ) {
        if ( m_used[rowIndex] == true ) {
            continue;
        }
        int variableIndex = m_basisHead[rowIndex];
        switch ( m_basicVariableFeasibilities->where( rowIndex ) ) {
        case Simplex::MINUS:
            difference =  m_simplexModel.getVariable(variableIndex).getLowerBound() -
                    m_basicVariableValues.at(rowIndex);
            if (difference > max) {
                max = difference;
                m_outgoingIndex = rowIndex;
                m_phase2Simpri.improvingIndexFound();
            }
            break;
        case Simplex::PLUS:
            difference = m_basicVariableValues.at(rowIndex) -
                    m_simplexModel.getVariable(variableIndex).getUpperBound();
            if (difference > max) {
                max = difference;
                m_outgoingIndex = rowIndex;
                m_phase2Simpri.improvingIndexFound();
            }
            break;
        }
    }

    m_reducedCost = max;
    return m_outgoingIndex;

}

void DualDantzigPricing::update(int incomingIndex,
                                int outgoingIndex,
                                const Vector & incomingAlpha,
                                const Vector & pivotRow)
{
    __UNUSED(incomingIndex);
    __UNUSED(outgoingIndex);
    __UNUSED(incomingAlpha);
    __UNUSED(pivotRow);
}
