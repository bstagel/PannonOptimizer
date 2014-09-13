/**
 * @file   dualdantzigpricing.cpp
 */

#include <simplex/pricing/dualsteepestedgepricing.h>
#include <simplex/simplex.h>

DualSteepestEdgePricing::DualSteepestEdgePricing(const Vector & basicVariableValues,
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
    initWeights();
    m_previousPhase = 0;
}

DualSteepestEdgePricing::~DualSteepestEdgePricing()
{

}

int DualSteepestEdgePricing::performPricingPhase1() {
    initPhase1();

    if (m_previousPhase != 1) {
        //setReferenceFramework();
        m_previousPhase = 1;
    }
    if (m_basis.isFresh()) {
    }

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
        unsigned int variableIndex = m_basisHead[index];
        Variable::VARIABLE_TYPE variableType = m_simplexModel.getVariable(variableIndex).getType();
        Numerical::Double weightedReducedCost = m_phase1ReducedCosts[index] / Numerical::sqrt(m_weights[variableIndex]);
        if ( variableType == Variable::FIXED ||
             variableType == Variable::BOUNDED ||
             (variableType == Variable::PLUS && weightedReducedCost > 0) ||
             (variableType == Variable::MINUS && weightedReducedCost < 0)) {
            if ( Numerical::fabs(weightedReducedCost) > max ) {
                max = Numerical::fabs(weightedReducedCost);
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

int DualSteepestEdgePricing::performPricingPhase2() {
    initPhase2();

    if (m_previousPhase != 2) {
        //    setReferenceFramework();
        m_previousPhase = 2;
    }

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
            difference =  (m_simplexModel.getVariable(variableIndex).getLowerBound() -
                           m_basicVariableValues.at(rowIndex)) / Numerical::sqrt(m_weights[variableIndex]);
            if (difference > max) {
                max = difference;
                m_reducedCost = difference * Numerical::sqrt(m_weights[rowIndex]);
                m_outgoingIndex = rowIndex;
                m_phase2Simpri.improvingIndexFound();
            }
            break;
        case Simplex::PLUS:
            difference = (m_basicVariableValues.at(rowIndex) -
                          m_simplexModel.getVariable(variableIndex).getUpperBound()) / Numerical::sqrt(m_weights[variableIndex]);
            if (difference > max) {
                max = difference;
                m_reducedCost = difference * Numerical::sqrt(m_weights[variableIndex]);
                m_outgoingIndex = rowIndex;
                m_phase2Simpri.improvingIndexFound();
            }
            break;
        }
    }

    return m_outgoingIndex;

}

void DualSteepestEdgePricing::update(int incomingIndex,
                                     int outgoingIndex,
                                     const Vector & incomingAlpha,
                                     const Vector & pivotRow,
                                     const Vector & pivotRowOfBasisInverse)
{
    __UNUSED(pivotRowOfBasisInverse);
    __UNUSED(incomingIndex);
    __UNUSED(pivotRow);


    if (unlikely(m_basis.isFresh() )) {
        //LPERROR("UJ BAZIS");
        //initWeights();
    }

    unsigned int rowCount = m_simplexModel.getRowCount();
    unsigned int index;
    Vector tau(rowCount, Vector::DENSE_VECTOR);

    tau = pivotRowOfBasisInverse;
    m_basis.Ftran(tau);
    //initWeights();

    Numerical::Double pivotWeight = m_weights[m_basisHead[outgoingIndex]];
    Numerical::Double alpha_p_q = incomingAlpha.at(outgoingIndex);
   //LPINFO("alpha_p_q = " << alpha_p_q);

    Vector::NonzeroIterator iter = incomingAlpha.beginNonzero();
    Vector::NonzeroIterator iterEnd = incomingAlpha.endNonzero();
    for (; iter != iterEnd; iter++) {
        index = iter.getIndex();
        if (unlikely((int)index == outgoingIndex)) {
            continue;
        }
        unsigned int variableIndex = m_basisHead[index];
        // LPINFO("index: " << index);
        Numerical::Double ratio = *iter / alpha_p_q;
      //  LPINFO(m_weights[index] << " - 2 * (" << *iter << " / " << alpha_p_q << ") * " << tau.at(index) <<
      //         " + (" << *iter << " / " << alpha_p_q << ")^2 * " << pivotWeight);

        Numerical::Double newWeight = m_weights[variableIndex] - 2 * ratio * tau.at(index) + ratio * ratio * pivotWeight;
        m_weights[variableIndex] = newWeight;
    }
    m_weights[m_basisHead[outgoingIndex]] /= (alpha_p_q * alpha_p_q);
    //LPERROR(m_basisHead[outgoingIndex]);
    //m_weights[incomingIndex] = pivotRowOfBasisInverse.euclidNorm();
    //m_weights[incomingIndex] *= m_weights[incomingIndex];
    m_weights[incomingIndex] = m_weights[ m_basisHead[outgoingIndex] ];
}

void DualSteepestEdgePricing::check() const
{
    //return;
    unsigned int rowCount = m_simplexModel.getRowCount();
    unsigned int index;
    Vector row(rowCount, Vector::DENSE_VECTOR);
    /*for (index = 0; index < rowCount; index++) {
        LPINFO(index << ".: " << m_weights[index]);
    }
    LPINFO("-----------------");*/

    for (index = 0; index < rowCount; index++) {
        row.clear();
        row.set(index, 1.0);
        m_basis.Btran(row);
        Numerical::Double norm = row.euclidNorm();
        norm *= norm;
        //LPINFO(index << ".: " << norm << " " << m_weights[index]);
        unsigned int variableIndex = m_basisHead[index];
        if (norm != m_weights[variableIndex] && Numerical::fabs(norm - m_weights[variableIndex]) > 1e-10 ) {
            LPERROR(norm - m_weights[variableIndex]);
            LPERROR(norm << " " << m_weights[variableIndex]);
            LPERROR("index: " << variableIndex);
            //LPERROR("incoming: " << incomingIndex);
            //LPERROR("outgoing: " << outgoingIndex);
            cin.get();
        }
        //m_weights[index] = norm;
    }
    /*for (index = 0; index < rowCount; index++) {
        LPINFO(index << ".: " << m_weights[index]);
    }*/
    //std::cin.get();

}

void DualSteepestEdgePricing::initWeights() {
    // LPINFO("init weights");
    unsigned int rowCount = m_simplexModel.getRowCount();
    unsigned int columnCount = m_simplexModel.getColumnCount();
    m_weights.resize(rowCount + columnCount, 1);
    unsigned int index;
    Vector row(rowCount, Vector::DENSE_VECTOR);
    for (index = 0; index < rowCount; index++) {
        row.clear();
        row.set(index, 1.0);
        m_basis.Btran(row);
        m_weights[index] = row.euclidNorm();
        m_weights[index] *= m_weights[index];
    }
    // LPINFO("weights initialized");
}

