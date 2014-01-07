/**
 * @file   dualdantzigpricing.cpp
 */

#include <simplex/pricing/dualdantzigpricing.h>
#include <simplex/simplexparameterhandler.h>
#include <simplex/simplex.h>

static const Numerical::Double feasibilityTolerance =
        SimplexParameterHandler::getInstance().getDoubleParameterValue("e_feasibility");

DualDantzigPricing::DualDantzigPricing(const SimplexModel & model,
                                       DualPricingUpdater * updater,
                                       const Vector & reducedCosts,
                                       const std::vector<int> & basisHead):
    DualPricing(model, updater),
    m_updater(dynamic_cast<DualDantzigPricingUpdater *>(updater)),
    m_reducedCosts(reducedCosts),
    m_basisHead(basisHead)
{
    m_phase1ReducedCosts = new Numerical::Double[ m_updater->m_simplexModel.getRowCount() ];
    clearPhase1ReducedCosts();
}

DualDantzigPricing::DualDantzigPricing(const DualDantzigPricing& orig):
    DualPricing(orig),
    m_updater(orig.m_updater),
    m_reducedCosts(orig.m_reducedCosts),
    m_basisHead(orig.m_basisHead)
{
    copy(orig);
}

DualDantzigPricing::~DualDantzigPricing()
{
    release();
}

void DualDantzigPricing::copy(const DualDantzigPricing & orig) {
    m_phase1ReducedCosts = new Numerical::Double[ m_updater->m_simplexModel.getRowCount() ];
    memcpy(m_phase1ReducedCosts, orig.m_phase1ReducedCosts,
           sizeof(Numerical::Double) * m_updater->m_simplexModel.getRowCount());
}

void DualDantzigPricing::release() {
    delete [] m_phase1ReducedCosts;
    m_phase1ReducedCosts = 0;
}


void DualDantzigPricing::clearPhase1ReducedCosts() {
    memset(m_phase1ReducedCosts, 0, sizeof(Numerical::Double) * m_updater->m_simplexModel.getRowCount());
}

void DualDantzigPricing::initPhase1() {
    const Matrix & matrix = m_updater->m_simplexModel.getMatrix();
    const unsigned int variableCount = matrix.columnCount();
    clearPhase1ReducedCosts();
    IndexList<>::Iterator iter, iterEnd;
    m_updater->m_reducedCostFeasibilities.getIterators(&iter, &iterEnd, Simplex::MINUS);
    unsigned int index;
    for (; iter != iterEnd; iter++) {
        index = iter.getData();
        if (index >= variableCount) {
            Numerical::stableAddTo( m_phase1ReducedCosts[ index - variableCount ], 1 );
        } else {
            //TODO: Joco, ezt mert nem addVector-ral csinaltad?
            Vector::NonzeroIterator columnIter = matrix.column(index).beginNonzero();
            Vector::NonzeroIterator columnIterEnd = matrix.column(index).endNonzero();
            for (; columnIter < columnIterEnd; columnIter++) {
                Numerical::stableAddTo( m_phase1ReducedCosts[columnIter.getIndex()], *columnIter );
            }
        }
    }

    m_updater->m_reducedCostFeasibilities.getIterators(&iter, &iterEnd, Simplex::PLUS);

    for (; iter != iterEnd; iter++) {
        index = iter.getData();
        if (index >= variableCount) {
            Numerical::stableSubFrom( m_phase1ReducedCosts[ index - variableCount ], 1 );
        } else {
            //TODO: Joco, ezt mert nem addVector-ral csinaltad?
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

    //TODO: A temp mire kell kulon?
    Vector temp;
    temp.prepareForData( nonzeros, matrix.rowCount(), 0.0);
    for (index = 0; index < matrix.rowCount(); index++) {
        if (m_phase1ReducedCosts[index] != 0.0) {
            temp.newNonZero( m_phase1ReducedCosts[index], index );
        }
    }
    m_updater->m_basis.Ftran(temp);
    clearPhase1ReducedCosts();
    Vector::NonzeroIterator vectorIter = temp.beginNonzero();
    Vector::NonzeroIterator vectorIterEnd = temp.endNonzero();
    for (; vectorIter < vectorIterEnd; vectorIter++) {
        m_phase1ReducedCosts[ vectorIter.getIndex() ] = *vectorIter;
    }

}

void DualDantzigPricing::initPhase2() {

    m_updater->m_variableFeasibilities->clearPartition(Simplex::FEASIBLE);
    m_updater->m_variableFeasibilities->clearPartition(Simplex::MINUS);
    m_updater->m_variableFeasibilities->clearPartition(Simplex::PLUS);

    Vector::NonzeroIterator iter = m_updater->m_basicVariableValues.beginNonzero();
    Vector::NonzeroIterator iterEnd = m_updater->m_basicVariableValues.endNonzero();
    for(; iter < iterEnd ; iter++){
        const Variable & variable = m_model.getVariable(m_updater->m_basisHead[iter.getIndex()]);
        if(Numerical::lessthan(*iter, variable.getLowerBound(), feasibilityTolerance)) {
            m_updater->m_variableFeasibilities->insert(Simplex::MINUS, iter.getIndex());
        } else if(Numerical::lessthan(variable.getUpperBound(), *iter, feasibilityTolerance)) {
            m_updater->m_variableFeasibilities->insert(Simplex::PLUS, iter.getIndex());
        } else {
            m_updater->m_variableFeasibilities->insert(Simplex::FEASIBLE, iter.getIndex());
        }
    }

}

int DualDantzigPricing::performPricingPhase1() {
    initPhase1();
    //TODO: A sorok szamat hivjuk mindenutt rowCountnak, az oszlopokat meg columnCount-nak, ne keverjunk
    const unsigned int variableCount = m_model.getMatrix().rowCount();
    Numerical::Double max = 0;
    int maxIndex = -1;
    unsigned int index;
    for (index = 0; index < variableCount; index++) {
        if ( m_updater->m_used[index] == true ) {
            continue;
        }
        unsigned int variableIndex = m_basisHead.at(index);
        Variable::VARIABLE_TYPE variableType = m_model.getVariable(variableIndex).getType();
        //TODO: Min // Max feladat!
        if ( variableType == Variable::FIXED ||
             variableType == Variable::BOUNDED ||
             (variableType == Variable::PLUS && m_phase1ReducedCosts[index] > 0) ||
             (variableType == Variable::MINUS && m_phase1ReducedCosts[index] < 0)) {
            if ( Numerical::fabs(m_phase1ReducedCosts[index]) > max ) {
                max = Numerical::fabs(m_phase1ReducedCosts[index]);
                maxIndex = index;
            }
        }
    }
    if (maxIndex != -1) {
        m_reducedCost = m_phase1ReducedCosts[maxIndex];
        m_updater->m_used[maxIndex] = true;
    }
    return maxIndex;
}

int DualDantzigPricing::performPricingPhase2() {
    initPhase2();

    IndexList<>::Iterator iter, iterEnd;
    Numerical::Double max = -1.0;
    int rowIndex;
    int phase2Index = -1;
    m_updater->m_simplexModel.getVariable(0);
    m_updater->m_variableFeasibilities->getIterators(&iter, &iterEnd, Simplex::MINUS);
    for (; iter != iterEnd; iter++) {
        rowIndex = (int)iter.getData();
        if ( m_updater->m_used[rowIndex] == true ) {
            continue;
        }
        int variableIndex = m_updater->m_basisHead[rowIndex];
        Numerical::Double difference =  m_updater->m_simplexModel.getVariable(variableIndex).getLowerBound() -
                m_updater->m_basicVariableValues.at(rowIndex);
        if (difference > max) {
            max = difference;
            phase2Index = rowIndex;
        }
//        LPINFO("M difference: "<<difference << " - rowindex: "<< rowIndex);
    }

    m_updater->m_variableFeasibilities->getIterators(&iter, &iterEnd, Simplex::PLUS);
    for (; iter != iterEnd; iter++) {
        rowIndex = (int)iter.getData();
        if ( m_updater->m_used[rowIndex] == true ) {
            continue;
        }
        int variableIndex = m_updater->m_basisHead[rowIndex];
        Numerical::Double difference = m_updater->m_basicVariableValues.at(rowIndex) -
                m_updater->m_simplexModel.getVariable(variableIndex).getUpperBound();
        if (difference > max) {
            max = difference;
            phase2Index = rowIndex;
        }
//        LPINFO("P difference: "<<difference << " - rowindex: "<< rowIndex);
    }

    m_reducedCost = max;
    if (phase2Index != -1) {
        m_updater->m_used[phase2Index] = true;
    }
    return phase2Index;

}

void DualDantzigPricing::releaseUsed() {
    unsigned int size = m_updater->m_used.size();
    m_updater->m_used.clear();
    m_updater->m_used.resize( size, false );
}
