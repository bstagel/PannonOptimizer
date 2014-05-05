/**
 * @file dualpricing.cpp
 */

#include <simplex/dualpricing.h>
#include <simplex/simplexparameterhandler.h>

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
    m_reducedCost(0.0),
    m_feasibilityTolerance(SimplexParameterHandler::getInstance().getDoubleParameterValue("e_feasibility")),
    m_phaseIClusters(SimplexParameterHandler::getInstance().getIntegerParameterValue("simpri_phaseI_clusters")),
    m_phaseIVisitClusters(SimplexParameterHandler::getInstance().getIntegerParameterValue("simpri_phaseI_visit_clusters")),
    m_phaseIImprovingCandidates(SimplexParameterHandler::getInstance().getIntegerParameterValue("simpri_phaseI_improving_candidates")),
    m_phaseIIClusters(SimplexParameterHandler::getInstance().getIntegerParameterValue("simpri_phaseII_clusters")),
    m_phaseIIVisitClusters(SimplexParameterHandler::getInstance().getIntegerParameterValue("simpri_phaseII_visit_clusters")),
    m_phaseIIImprovingCandidates(SimplexParameterHandler::getInstance().getIntegerParameterValue("simpri_phaseII_improving_candidates"))

{
    m_phase1ReducedCosts = new Numerical::Double[ m_simplexModel.getRowCount() ];
    clearPhase1ReducedCosts();
    m_phase2ReducedCost = 0;
    m_used.clear();
    m_used.resize( m_basisHead.size(), false );

    m_phase1Simpri.init(m_phaseIClusters,
                        m_phaseIVisitClusters,
                        m_phaseIImprovingCandidates,
                        simplexModel.getMatrix().rowCount());
    unsigned int index;
    for (index = 0; index < simplexModel.getMatrix().rowCount(); index++) {
        m_phase1Simpri.insertCandidate(index);
    }

    m_phase2Simpri.init(m_phaseIIClusters,
                        m_phaseIIVisitClusters,
                        m_phaseIIImprovingCandidates,
                        simplexModel.getMatrix().rowCount());
    for (index = 0; index < simplexModel.getMatrix().rowCount(); index++) {
        m_phase2Simpri.insertCandidate(index);
    }
}

DualPricing::~DualPricing() {
    delete [] m_phase1ReducedCosts;
    m_phase1ReducedCosts = 0;
}

void DualPricing::clearPhase1ReducedCosts() {
    unsigned int index;
    for (index = 0; index < m_simplexModel.getRowCount(); index++) {
        m_phase1ReducedCosts[index] = 0;
    }
    // DO NOT USE MEMSET! Sometimes Numerical::Double can be a special object.
}

void DualPricing::initPhase1() {
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
            Numerical::stableAddTo( m_phase1ReducedCosts[ index - variableCount ], - 1 );
        } else {
            //TODO: Joco, ezt mert nem addVector-ral csinaltad?
            Vector::NonzeroIterator columnIter = matrix.column(index).beginNonzero();
            Vector::NonzeroIterator columnIterEnd = matrix.column(index).endNonzero();
            for (; columnIter < columnIterEnd; columnIter++) {
                Numerical::stableAddTo( m_phase1ReducedCosts[columnIter.getIndex()], - *columnIter );
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

void DualPricing::initPhase2() {

    m_basicVariableFeasibilities->clearPartition(Simplex::FEASIBLE);
    m_basicVariableFeasibilities->clearPartition(Simplex::MINUS);
    m_basicVariableFeasibilities->clearPartition(Simplex::PLUS);

    Vector::Iterator iter = m_basicVariableValues.begin();
    Vector::Iterator iterEnd = m_basicVariableValues.end();
    for(; iter < iterEnd ; iter++){
        const Variable & variable = m_simplexModel.getVariable(m_basisHead[iter.getIndex()]);
        if(Numerical::lessthan(*iter, variable.getLowerBound(), m_feasibilityTolerance)) {
            m_basicVariableFeasibilities->insert(Simplex::MINUS, iter.getIndex());
        } else if(Numerical::lessthan(variable.getUpperBound(), *iter, m_feasibilityTolerance)) {
            m_basicVariableFeasibilities->insert(Simplex::PLUS, iter.getIndex());
        } else {
            m_basicVariableFeasibilities->insert(Simplex::FEASIBLE, iter.getIndex());
        }
    }
}

void DualPricing::releaseUsed() {
    unsigned int size = m_used.size();
    m_used.clear();
    m_used.resize( size, false );
}

void DualPricing::lockLastIndex() {
    if (m_outgoingIndex != -1) {
        m_used[m_outgoingIndex] = true;
    } else {
        throw NumericalException(std::string("Invalid row lock index!"));
    }
}
