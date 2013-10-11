/**
 * @file   dualdantzigpricing.cpp
 */

#include <simplex/pricing/dualdantzigpricing.h>
#include <simplex/simplex.h>

DualDantzigPricing::DualDantzigPricing(const SimplexModel & model,
                                       const DualPricingUpdater &updater):
    DualPricing(model, updater),
    m_updater(dynamic_cast<const DualDantzigPricingUpdater &>(updater))
{
    m_phase1ReducedCosts = new Numerical::Double[ m_updater.m_simplexModel.getRowCount() ];
    clearPhase1ReducedCosts();
}

DualDantzigPricing::DualDantzigPricing(const DualDantzigPricing& orig):
    DualPricing(orig),
    m_updater(orig.m_updater)
{
    copy(orig);
}

DualDantzigPricing::~DualDantzigPricing()
{
    release();
}

void DualDantzigPricing::copy(const DualDantzigPricing & orig) {
    m_phase1ReducedCosts = new Numerical::Double[ m_updater.m_simplexModel.getRowCount() ];
    memcpy(m_phase1ReducedCosts, orig.m_phase1ReducedCosts,
           sizeof(Numerical::Double) * m_updater.m_simplexModel.getRowCount());
}

void DualDantzigPricing::release() {
    delete [] m_phase1ReducedCosts;
    m_phase1ReducedCosts = 0;
}


void DualDantzigPricing::clearPhase1ReducedCosts() {
    memset(m_phase1ReducedCosts, 0, sizeof(Numerical::Double) * m_updater.m_simplexModel.getRowCount());
}

void DualDantzigPricing::initPhase1() {
    const Matrix & matrix = m_updater.m_simplexModel.getMatrix();
    const unsigned int variableCount = matrix.columnCount();
    clearPhase1ReducedCosts();
    IndexList<>::Iterator iter, iterEnd;
    m_updater.m_reducedCostFeasibilities.getIterators(&iter, &iterEnd, Simplex::MINUS);
    unsigned int index;
    for (; iter != iterEnd; iter++) {
        index = iter.getData();
        if (index >= variableCount) {
            Numerical::stableAddTo( m_phase1ReducedCosts[ variableCount - index ], 1 );
        } else {
            Vector::NonzeroIterator columnIter = matrix.column(index).beginNonzero();
            Vector::NonzeroIterator columnIterEnd = matrix.column(index).endNonzero();
            for (; columnIter < columnIterEnd; columnIter++) {
                Numerical::stableAddTo( m_phase1ReducedCosts[columnIter.getIndex()], *columnIter );
            }
        }
    }

    m_updater.m_reducedCostFeasibilities.getIterators(&iter, &iterEnd, Simplex::PLUS);

    for (; iter != iterEnd; iter++) {
        index = iter.getData();
        if (index >= variableCount) {
            Numerical::stableSubFrom( m_phase1ReducedCosts[ variableCount - index ], 1 );
        } else {
            Vector::NonzeroIterator columnIter = matrix.column(index).beginNonzero();
            Vector::NonzeroIterator columnIterEnd = matrix.column(index).endNonzero();
            for (; columnIter < columnIterEnd; columnIter++) {
                Numerical::stableSubFrom( m_phase1ReducedCosts[columnIter.getIndex()], *columnIter );
            }
        }
    }
    unsigned int nonzeros = 0;
    for (index = 0; index < matrix.rowCount(); index++) {
        nonzeros += m_phase1ReducedCosts[index] != 0.0;
    }

    Vector temp;
    temp.prepareForData( nonzeros, matrix.rowCount(), 0.0);
    for (index = 0; index < matrix.rowCount(); index++) {
        if (m_phase1ReducedCosts[index] != 0.0) {
            temp.newNonZero( m_phase1ReducedCosts[index], index );
        }
    }
    m_updater.m_basis.Ftran(temp);
    clearPhase1ReducedCosts();
    Vector::NonzeroIterator vectorIter = temp.beginNonzero();
    Vector::NonzeroIterator vectorIterEnd = temp.endNonzero();
    for (; vectorIter < vectorIterEnd; vectorIter++) {
        m_phase1ReducedCosts[ vectorIter.getIndex() ] = *vectorIter;
    }
}

unsigned int DualDantzigPricing::performPricingPhase1() {
    initPhase1();
    const unsigned int variableCount = m_model.getMatrix().rowCount();
    Numerical::Double max = m_phase1ReducedCosts[0];
    unsigned int maxIndex = 0;
    unsigned int index;
    for (index = 1; index < variableCount; index++) {
        if ( Numerical::fabs(m_phase1ReducedCosts[index]) > max ) {
            max = Numerical::fabs(m_phase1ReducedCosts[index]);
            maxIndex = index;
        }
    }
    m_reducedCost = m_phase1ReducedCosts[maxIndex];
    return maxIndex;
}

unsigned int DualDantzigPricing::performPricingPhase2() throw (OptimalException) {
    /*if ( m_updater.m_phase2Index == -1 ) {
        throw OptimalException("Optimal solution found.");
    }
    m_reducedCost = m_updater.m_phase2ReducedCost;
    return m_updater.m_phase2Index;*/

    IndexList<>::Iterator iter, iterEnd;
    Numerical::Double max = -1.0;
    int rowIndex;
    unsigned int phase2Index = 0;
    m_updater.m_simplexModel.getVariable(0);
    m_updater.m_variableFeasibilities.getIterators(&iter, &iterEnd, Simplex::MINUS);
    for (; iter != iterEnd; iter++) {
        rowIndex = (int)iter.getData();
        int variableIndex = m_updater.m_basisHead[rowIndex];
        Numerical::Double difference =  m_updater.m_simplexModel.getVariable(variableIndex).getLowerBound() -
                m_updater.m_basicVariableValues[rowIndex];
        if (difference > max) {
            max = difference;
            phase2Index = rowIndex;
        }
    }

    m_updater.m_variableFeasibilities.getIterators(&iter, &iterEnd, Simplex::PLUS);
    for (; iter != iterEnd; iter++) {
        rowIndex = (int)iter.getData();
        int variableIndex = m_updater.m_basisHead[rowIndex];
        Numerical::Double difference = m_updater.m_basicVariableValues[rowIndex] -
                m_updater.m_simplexModel.getVariable(variableIndex).getUpperBound();
        if (difference > max) {
            max = difference;
            phase2Index = rowIndex;
        }
    }
    if (max == -1) {
        throw OptimalException("Optimal solution found.");
    }
    return phase2Index;

}
