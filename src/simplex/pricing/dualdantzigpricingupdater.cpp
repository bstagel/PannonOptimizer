/* 
 * File:   dualdantzigpricingupdater.cpp
 * Author: smidla
 *
 * Created on 2013. augusztus 26., 1:00
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
    m_phase1ReducedCosts = new Numerical::Double[ simplexModel.getRowCount() ];
    clearPhase1ReducedCosts();
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

void DualDantzigPricingUpdater::copy(const DualDantzigPricingUpdater & orig) {
    m_phase1ReducedCosts = new Numerical::Double[ m_simplexModel.getRowCount() ];
    memcpy(m_phase1ReducedCosts, orig.m_phase1ReducedCosts,
           sizeof(Numerical::Double) * m_simplexModel.getRowCount());
    m_phase2Index = orig.m_phase2Index;
    m_phase2ReducedCost = orig.m_phase2ReducedCost;
}

void DualDantzigPricingUpdater::release() {
    delete [] m_phase1ReducedCosts;
    m_phase1ReducedCosts = 0;
}

void DualDantzigPricingUpdater::clearPhase1ReducedCosts() {
    memset(m_phase1ReducedCosts, 0, sizeof(Numerical::Double) * m_simplexModel.getRowCount());
}

void DualDantzigPricingUpdater::updatePhase1() {
    const Matrix & matrix = m_simplexModel.getMatrix();
    const unsigned int variableCount = matrix.columnCount();
    clearPhase1ReducedCosts();
    IndexList<>::Iterator iter, iterEnd;
    m_reducedCostFeasibilities.getIterators(&iter, &iterEnd, Simplex::MINUS, 1);
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

    m_reducedCostFeasibilities.getIterators(&iter, &iterEnd, Simplex::PLUS, 1);

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
    m_basis.Ftran(temp);
    clearPhase1ReducedCosts();
    Vector::NonzeroIterator vectorIter = temp.beginNonzero();
    Vector::NonzeroIterator vectorIterEnd = temp.endNonzero();
    for (; vectorIter < vectorIterEnd; vectorIter++) {
        m_phase1ReducedCosts[ vectorIter.getIndex() ] = *vectorIter;
    }
}

void DualDantzigPricingUpdater::updatePhase2() {
    IndexList<>::Iterator iter, iterEnd;
    Numerical::Double max = -1.0;
    int rowIndex;
    m_simplexModel.getVariable(0);
    m_variableFeasibilities.getIterators(&iter, &iterEnd, Simplex::MINUS, 1);
    for (; iter != iterEnd; iter++) {
        rowIndex = (int)iter.getData();
        int variableIndex = m_basisHead[rowIndex];
        Numerical::Double difference =  m_simplexModel.getVariable(variableIndex).getLowerBound() - m_basicVariableValues[rowIndex];
        if (difference > max) {
            max = difference;
            m_phase2Index = rowIndex;
        }
    }

    m_variableFeasibilities.getIterators(&iter, &iterEnd, Simplex::PLUS, 1);
    for (; iter != iterEnd; iter++) {
        rowIndex = (int)iter.getData();
        int variableIndex = m_basisHead[rowIndex];
        Numerical::Double difference =  m_basicVariableValues[rowIndex] - m_simplexModel.getVariable(variableIndex).getUpperBound();
        if (difference > max) {
            max = difference;
            m_phase2Index = rowIndex;
        }
    }
    m_phase2ReducedCost = max;
}
