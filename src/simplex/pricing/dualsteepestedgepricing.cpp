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
   // __UNUSED(pivotRowOfBasisInverse);
   // __UNUSED(incomingIndex);
    __UNUSED(pivotRow);


//    if (unlikely(m_basis.isFresh() )) {
//    }


//    unsigned int rowCount = m_simplexModel.getRowCount();
    unsigned int index;
    Vector tau = pivotRowOfBasisInverse;
    m_basis.Ftran(tau);
    //initWeights();


    Numerical::Double alpha_p_q = incomingAlpha.at(outgoingIndex);
   //LPINFO("alpha_p_q = " << alpha_p_q);
    Numerical::Double pivotWeight = m_weights[m_basisHead[outgoingIndex]];

    Vector::NonzeroIterator iter = incomingAlpha.beginNonzero();
    Vector::NonzeroIterator iterEnd = incomingAlpha.endNonzero();
    for (; iter != iterEnd; iter++) {
        index = iter.getIndex();
        if (unlikely((int)index == outgoingIndex)) {
            continue;
        }

        unsigned int variableIndex = m_basisHead[index];

        Numerical::Double ratio = *iter / alpha_p_q;

        Numerical::Double newWeight = m_weights[variableIndex];
        newWeight -= Numerical::stableAdd(2 * ratio * tau.at(index), -(ratio * ratio * pivotWeight));
//        std::cout << m_weights[variableIndex] << " -> " << newWeight << std::endl;
        /*if (newWeight <= 0) {
            LPERROR("hiba: " << newWeight);
            Vector row(m_simplexModel.getRowCount(), Vector::DENSE_VECTOR);
            row.setNewNonzero(index, 1.0);
            Vector eta = incomingAlpha;
            Numerical::Double atPivot = eta.at(outgoingIndex);
            eta.scaleBy(-1.0 / atPivot);
            eta.set(outgoingIndex, 1.0 / atPivot);
            row.elementaryBtran(eta, outgoingIndex);
            m_basis.Btran(row);
            LPINFO(row.euclidNorm() * row.euclidNorm());
            LPINFO("current index: " << index);
            LPINFO("current variable: " << variableIndex);
            LPINFO("incoming index: " << incomingIndex);
            LPINFO("outgoing index:" << outgoingIndex);
            LPINFO("outgoing variable: " << m_basisHead[outgoingIndex]);

              LPINFO(m_weights[variableIndex] << " - 2 * (" << *iter << " / " << alpha_p_q << ") * " << tau.at(index) <<
                     " + (" << *iter << " / " << alpha_p_q << ")^2 * " << pivotWeight);

             auto iter = row.beginNonzero();
             auto iterEnd = row.endNonzero();
             for (; iter != iterEnd; iter++) {
                 LPINFO("\t"<<iter.getIndex() << " " <<*iter);
             }

             LPINFO("-------------------\nBEFORE: \n");
             row.clear();
             row.setNewNonzero(index, 1.0);
             m_basis.Btran(row);
             iter = row.beginNonzero();
             iterEnd = row.endNonzero();
             for (; iter != iterEnd; iter++) {
                 LPINFO("\t"<<iter.getIndex() << " " <<*iter);
             }


            std::cin.get();
            //exit(1);
        }*/
        m_weights[variableIndex] = newWeight;
        m_updateCounters[variableIndex]++;
        if (m_updateCounters[variableIndex] >= 25) {
            m_recomuteIndices.push_back(index);
        }
        //std::cout << "beallitjuk ezt: " << index << " " << variableIndex << " erre: " << newWeight << std::endl;
    }
    //m_weights[m_basisHead[outgoingIndex]] /= (alpha_p_q * alpha_p_q);


    m_weights[incomingIndex] = pivotWeight / (alpha_p_q * alpha_p_q);
    m_updateCounters[incomingIndex]++;
    //m_weights[incomingIndex] *= m_weights[incomingIndex];
    //std::cout << "vegul ezt erre: " << incomingIndex << "  " << m_weights[incomingIndex] << std::endl;

    //LPERROR(m_basisHead[outgoingIndex]);
    //m_weights[incomingIndex] = pivotRowOfBasisInverse.euclidNorm();
    //m_weights[incomingIndex] *= m_weights[incomingIndex];
    //m_weights[incomingIndex] = m_weights[ m_basisHead[outgoingIndex] ];
}

void DualSteepestEdgePricing::checkAndFix()
{
    //return;
    unsigned int rowCount = m_simplexModel.getRowCount();
//    unsigned int index;
    /*for (index = 0; index < rowCount; index++) {
        LPINFO(index << ".: " << m_weights[index]);
    }
    LPINFO("-----------------");*/

    for (auto index: m_recomuteIndices) {
        Vector row(rowCount, Vector::DENSE_VECTOR);
        row.clear();
        row.set(index, 1.0);
        m_basis.Btran(row);
        Numerical::Double norm = row.euclidNorm();
        norm *= norm;
        m_weights[ m_basisHead[index] ] = norm;
        m_updateCounters[ m_basisHead[index] ] = 0;
    }
    m_recomuteIndices.clear();

    /*bool wrong = false;

    for (index = 0; index < rowCount; index++) {
        Vector row(rowCount, Vector::DENSE_VECTOR);

        row.clear();
        row.set(index, 1.0);
        m_basis.Btran(row);
        Numerical::Double norm = row.euclidNorm();
        norm *= norm;
        //LPINFO(index << ".: " << norm << " " << m_weights[index]);
        unsigned int variableIndex = m_basisHead[index];

        if (norm != m_weights[variableIndex] && Numerical::fabs(norm - m_weights[variableIndex]) > 1e-03 ) {
            wrong = true;
            LPERROR(norm - m_weights[variableIndex]);
            LPERROR(norm << " " << m_weights[variableIndex]);
            LPERROR("index: " << index << "   " << variableIndex);
            LPERROR("update counter: " << m_updateCounters[variableIndex]);
            //LPERROR("incoming: " << incomingIndex);
            //LPERROR("outgoing: " << outgoingIndex);
        }
        //m_weights[index] = norm;
    }
    if (wrong) {
        cin.get();
    }
    for (index = 0; index < rowCount; index++) {
        LPINFO(index << ".: " << m_weights[index]);
    }*/
    //std::cin.get();*/

}

void DualSteepestEdgePricing::initWeights() {
    // LPINFO("init weights");
    unsigned int rowCount = m_simplexModel.getRowCount();
    unsigned int columnCount = m_simplexModel.getColumnCount();
    m_weights.resize(rowCount + columnCount, 1);
    m_updateCounters.resize(rowCount + columnCount, 0);
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

