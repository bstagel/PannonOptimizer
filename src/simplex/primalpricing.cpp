
/**
 * @file primalpricing.cpp
 */

#include <simplex/primalpricing.h>
#include <simplex/simplexparameterhandler.h>
#include <utils/thirdparty/prettyprint.h>

PrimalPricing::PrimalPricing(const DenseVector &basicVariableValues,
                             const IndexList<> & basicVariableFeasibilities,
                             IndexList<> * reducedCostFeasibilities,
                             const IndexList<const Numerical::Double *> & variableStates,
                             const std::vector<int> & basisHead,
                             const SimplexModel & model,
                             const Basis & basis,
                             const DenseVector &reducedCosts):
    m_basicVariableValues(basicVariableValues),
    m_basicVariableFeasibilities(basicVariableFeasibilities),
    m_reducedCostFeasibilities(reducedCostFeasibilities),
    m_variableStates(variableStates),
    m_basisHead(basisHead),
    m_simplexModel(model),
    m_basis(basis),
    m_reducedCosts(reducedCosts),
    m_dualInfeasibility(0),
    m_feasibilityTolerance(SimplexParameterHandler::getInstance().getDoubleParameterValue("Tolerances.e_feasibility")),
    m_optimalityTolerance(SimplexParameterHandler::getInstance().getDoubleParameterValue("Tolerances.e_optimality")),
    m_phaseIClusters(SimplexParameterHandler::getInstance().getIntegerParameterValue("Pricing.Simpri.phaseI_clusters")),
    m_phaseIVisitClusters(SimplexParameterHandler::getInstance().getIntegerParameterValue("Pricing.Simpri.phaseI_visit_clusters")),
    m_phaseIImprovingCandidates(SimplexParameterHandler::getInstance().getIntegerParameterValue("Pricing.Simpri.phaseI_improving_candidates")),
    m_phaseIIClusters(SimplexParameterHandler::getInstance().getIntegerParameterValue("Pricing.Simpri.phaseII_clusters")),
    m_phaseIIVisitClusters(SimplexParameterHandler::getInstance().getIntegerParameterValue("Pricing.Simpri.phaseII_visit_clusters")),
    m_phaseIIImprovingCandidates(SimplexParameterHandler::getInstance().getIntegerParameterValue("Pricing.Simpri.phaseII_improving_candidates"))

{
    m_reducedCost = 0.0;

    m_phase2ReducedCost = 0;
    m_phase2Index = -1;
    m_used.clear();
    unsigned int size = m_simplexModel.getRowCount() + m_simplexModel.getColumnCount();
    m_used.resize( size, false );

    m_used.clear();

    m_phase1Simpri.init(m_phaseIClusters,
                        m_phaseIVisitClusters,
                        m_phaseIImprovingCandidates,
                        size);
    unsigned int index;
    for (index = 0; index < size; index++) {
        m_phase1Simpri.insertCandidate(index);
    }

    m_phase2Simpri.init(m_phaseIIClusters,
                        m_phaseIIVisitClusters,
                        m_phaseIIImprovingCandidates,
                        size);
    for (index = 0; index < size; index++) {
        m_phase2Simpri.insertCandidate(index);
    }

}

void PrimalPricing::init() {
    std::vector<int>::const_iterator headIter = m_basisHead.begin();
    std::vector<int>::const_iterator headIterEnd = m_basisHead.end();
    for (; headIter != headIterEnd; headIter++) {
        m_phase1Simpri.removeCandidate(*headIter);
        m_phase2Simpri.removeCandidate(*headIter);
    }
}

PrimalPricing::~PrimalPricing() {
}

bool PrimalPricing::hasLockedVariable() const
{
    for(unsigned index = 0; index < m_used.size(); index++){
        if(m_used[index] == true){
            return true;
        }
    }
    return false;
}

void PrimalPricing::releaseUsed() {
    unsigned int size = m_used.size();
    m_used.clear();
    m_used.resize( size, 0 );
}

void PrimalPricing::lockLastIndex() {
    if (m_incomingIndex != -1) {
        m_used[m_incomingIndex] = true;
    } else {
        throw NumericalException(std::string("Invalid column lock index!"));
    }
}

void PrimalPricing::initPhase1() {
    m_negativeSums.clear();
    m_negativeSums.resize( m_simplexModel.getColumnCount() + m_simplexModel.getRowCount(), 0.0 );
    m_positiveSums.clear();
    m_positiveSums.resize( m_simplexModel.getColumnCount() + m_simplexModel.getRowCount(), 0.0 );
    m_phase1ReducedCosts.clear();
    m_phase1ReducedCosts.resize( m_simplexModel.getColumnCount() + m_simplexModel.getRowCount(), 0.0);

    unsigned int rowCount = m_basisHead.size();
    // get the h vector
    DenseVector auxVector( rowCount);
    const IndexList<> & basicVariableFeasibilities = m_basicVariableFeasibilities;
    IndexList<>::Iterator iter, iterEnd;
    basicVariableFeasibilities.getIterators(&iter, &iterEnd, Simplex::MINUS);
    for (; iter != iterEnd; ++iter) {
        auxVector.set(iter.getData(), 1.0);
    }

    basicVariableFeasibilities.getIterators(&iter, &iterEnd, Simplex::PLUS);
    for (; iter != iterEnd; ++iter) {
        auxVector.set(iter.getData(), -1.0);
    }
    // compute the simplex multiplier
    m_basis.Btran(auxVector);

    // compute the reduced costs
    const Matrix & matrix = m_simplexModel.getMatrix();
    DenseVector::NonzeroIterator auxIter = auxVector.beginNonzero();
    DenseVector::NonzeroIterator auxIterEnd = auxVector.endNonzero();
    for (; auxIter != auxIterEnd; ++auxIter) {
        const unsigned int rowIndex = auxIter.getIndex();
        const Numerical::Double lambda = *auxIter;
        SparseVector::NonzeroIterator nonzIter = matrix.row(rowIndex).beginNonzero();
        SparseVector::NonzeroIterator nonzIterEnd = matrix.row(rowIndex).endNonzero();
        for (; nonzIter != nonzIterEnd; ++nonzIter) {
            Numerical::Double product = lambda * *nonzIter;
            if (product > 0.0) {
                m_positiveSums[nonzIter.getIndex()] += product;
            } else {
                m_negativeSums[nonzIter.getIndex()] += product;
            }
        }
        if(lambda > 0.0){
            m_positiveSums[m_simplexModel.getColumnCount() + auxIter.getIndex()] = lambda;
        } else {
            m_negativeSums[m_simplexModel.getColumnCount() + auxIter.getIndex()] = lambda;
        }
    }
    unsigned int index;
    for (index = 0; index < m_phase1ReducedCosts.size(); index++) {
        //Stable addition is NOT appropriate here!
        m_phase1ReducedCosts[index] = m_positiveSums[index] + m_negativeSums[index];
    }
}
