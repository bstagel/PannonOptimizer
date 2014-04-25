/**
 * @file   dualdantzigpricing.cpp
 */

#include <simplex/pricing/dualdantzigpricing.h>
#include <simplex/simplexparameterhandler.h>
#include <simplex/simplex.h>

static const unsigned int dualPhase1ClusterCount = 1;
static const unsigned int dualPhase1VisitedClusterCount = 1;
static const unsigned int dualPhase1ImprovingVariableCount = 0;

static const unsigned int dualPhase2ClusterCount = 1;
static const unsigned int dualPhase2VisitedClusterCount = 1;
static const unsigned int dualPhase2ImprovingVariableCount = 0;

static const Numerical::Double feasibilityTolerance =
        SimplexParameterHandler::getInstance().getDoubleParameterValue("e_feasibility");

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
    m_phase1ReducedCosts = new Numerical::Double[ m_simplexModel.getRowCount() ];
    clearPhase1ReducedCosts();
    m_phase2ReducedCost = 0;
    m_phase2Index = -1;
    m_used.clear();
    m_used.resize( m_basisHead.size(), false );

    m_phase1Simpri.init(dualPhase1ClusterCount,
                        dualPhase1VisitedClusterCount,
                        dualPhase1ImprovingVariableCount,
                        simplexModel.getMatrix().rowCount());
    unsigned int index;
    for (index = 0; index < simplexModel.getMatrix().rowCount(); index++) {
        m_phase1Simpri.insertCandidate(index);
    }

    m_phase2Simpri.init(dualPhase2ClusterCount,
                        dualPhase2VisitedClusterCount,
                        dualPhase2ImprovingVariableCount,
                        simplexModel.getMatrix().rowCount());
    for (index = 0; index < simplexModel.getMatrix().rowCount(); index++) {
        m_phase2Simpri.insertCandidate(index);
    }
}

DualDantzigPricing::DualDantzigPricing(const DualDantzigPricing& orig):
    DualPricing(orig)
{
    copy(orig);
}

DualDantzigPricing::~DualDantzigPricing()
{
    release();
}

void DualDantzigPricing::copy(const DualDantzigPricing & orig) {
    m_phase1ReducedCosts = new Numerical::Double[ m_simplexModel.getRowCount() ];
    COPY_DOUBLES(m_phase1ReducedCosts, orig.m_phase1ReducedCosts, m_simplexModel.getRowCount());
    //memcpy(m_phase1ReducedCosts, orig.m_phase1ReducedCosts,
    //       sizeof(Numerical::Double) * m_updater->m_simplexModel.getRowCount());
}

void DualDantzigPricing::release() {
    delete [] m_phase1ReducedCosts;
    m_phase1ReducedCosts = 0;
}


void DualDantzigPricing::clearPhase1ReducedCosts() {
    unsigned int index;
    for (index = 0; index < m_simplexModel.getRowCount(); index++) {
        m_phase1ReducedCosts[index] = 0;
    }
    // DO NOT USE MEMSET! Sometimes Numerical::Double can be a special object.
}

void DualDantzigPricing::initPhase1() {
    const Matrix & matrix = m_simplexModel.getMatrix();
    const unsigned int variableCount = matrix.columnCount();
    clearPhase1ReducedCosts();
    IndexList<>::Iterator iter, iterEnd;
    m_reducedCostFeasibilities.getIterators(&iter, &iterEnd, Simplex::MINUS);
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

    m_reducedCostFeasibilities.getIterators(&iter, &iterEnd, Simplex::PLUS);

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
    m_basis.Ftran(temp);
    clearPhase1ReducedCosts();
    Vector::NonzeroIterator vectorIter = temp.beginNonzero();
    Vector::NonzeroIterator vectorIterEnd = temp.endNonzero();
    for (; vectorIter < vectorIterEnd; vectorIter++) {
        m_phase1ReducedCosts[ vectorIter.getIndex() ] = *vectorIter;
    }

}

void DualDantzigPricing::initPhase2() {

    m_basicVariableFeasibilities->clearPartition(Simplex::FEASIBLE);
    m_basicVariableFeasibilities->clearPartition(Simplex::MINUS);
    m_basicVariableFeasibilities->clearPartition(Simplex::PLUS);

    Vector::Iterator iter = m_basicVariableValues.begin();
    Vector::Iterator iterEnd = m_basicVariableValues.end();
    for(; iter < iterEnd ; iter++){
        const Variable & variable = m_simplexModel.getVariable(m_basisHead[iter.getIndex()]);
        if(Numerical::lessthan(*iter, variable.getLowerBound(), feasibilityTolerance)) {
            m_basicVariableFeasibilities->insert(Simplex::MINUS, iter.getIndex());
        } else if(Numerical::lessthan(variable.getUpperBound(), *iter, feasibilityTolerance)) {
            m_basicVariableFeasibilities->insert(Simplex::PLUS, iter.getIndex());
        } else {
            m_basicVariableFeasibilities->insert(Simplex::FEASIBLE, iter.getIndex());
        }
    }
}

int DualDantzigPricing::performPricingPhase1() {
    initPhase1();
    //TODO: A sorok szamat hivjuk mindenutt rowCountnak, az oszlopokat meg columnCount-nak, ne keverjunk
    const unsigned int variableCount = m_simplexModel.getMatrix().rowCount();
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

    /*for (index = 0; index < variableCount; index++) {
        if ( m_updater->m_used[index] == true ) {
            continue;
        }
        unsigned int variableIndex = m_basisHead.at(index);
        Variable::VARIABLE_TYPE variableType = m_model.getVariable(variableIndex).getType();
        if ( variableType == Variable::FIXED ||
             variableType == Variable::BOUNDED ||
             (variableType == Variable::PLUS && m_phase1ReducedCosts[index] > 0) ||
             (variableType == Variable::MINUS && m_phase1ReducedCosts[index] < 0)) {
            if ( Numerical::fabs(m_phase1ReducedCosts[index]) > max ) {
                max = Numerical::fabs(m_phase1ReducedCosts[index]);
                m_outgoingIndex = index;
            }
        }
    }*/
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

    /*m_updater->m_basicVariableFeasibilities->getIterators(&iter, &iterEnd, Simplex::MINUS);
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
            m_outgoingIndex = rowIndex;
        }
    }

    m_updater->m_basicVariableFeasibilities->getIterators(&iter, &iterEnd, Simplex::PLUS);
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
            m_outgoingIndex = rowIndex;
        }
    }*/

    m_reducedCost = max;
    return m_outgoingIndex;

}

void DualDantzigPricing::releaseUsed() {
    unsigned int size = m_used.size();
    m_used.clear();
    m_used.resize( size, false );
}

void DualDantzigPricing::lockLastIndex() {
    if (m_outgoingIndex != -1) {
        m_used[m_outgoingIndex] = true;
    } else {
        throw NumericalException(std::string("Invalid row lock index!"));
    }
}
