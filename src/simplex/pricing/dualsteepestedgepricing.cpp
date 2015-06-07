//=================================================================================================
/*!
//  This file is part of the Pannon Optimizer library. 
//  This library is free software; you can redistribute it and/or modify it under the 
//  terms of the GNU Lesser General Public License as published by the Free Software 
//  Foundation; either version 3.0, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
//  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//  See the GNU General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License; see the file 
//  COPYING. If not, see http://www.gnu.org/licenses/.
*/
//=================================================================================================

/**
 * @file   dualdantzigpricing.cpp
 */

#include <simplex/pricing/dualsteepestedgepricing.h>
#include <simplex/simplex.h>

DualSteepestEdgePricing::DualSteepestEdgePricing(const DenseVector &basicVariableValues,
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
    m_previousPhase = 0;
    m_weightsReady = false;
}

DualSteepestEdgePricing::~DualSteepestEdgePricing()
{

}

int DualSteepestEdgePricing::performPricingPhase1() {
    initPhase1();
    if (unlikely(m_weightsReady == false)) {
        initWeights();
    }

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

    /*Numerical::Double sum = 0.0;
    Numerical::Double squareSum = 0.0;

    Numerical::Double sum2 = 0.0;
    Numerical::Double squareSum2 = 0.0;
    for (auto weight: m_weights) {
        sum += weight;
        squareSum += weight * weight;
    }*/


    while (m_phase1Simpri.getCandidateIndex(&index) ) {
        if ( m_used[index] == true ) {
            continue;
        }
        //LPINFO("SIMPRI: " << index);
        unsigned int variableIndex = m_basisHead[index];
        Variable::VARIABLE_TYPE variableType = m_simplexModel.getVariable(variableIndex).getType();
        Numerical::Double weightedReducedCost = m_phase1ReducedCosts[index] / Numerical::sqrt(m_weights[index]);


        if ( variableType == Variable::FIXED ||
             variableType == Variable::BOUNDED ||
             (variableType == Variable::PLUS && weightedReducedCost > 0) ||
             (variableType == Variable::MINUS && weightedReducedCost < 0)) {

            /*sum += fabs(weightedReducedCost);
            squareSum += weightedReducedCost * weightedReducedCost;
            sum2 += fabs(m_phase1ReducedCosts[index]);
            squareSum2 += m_phase1ReducedCosts[index] * m_phase1ReducedCosts[index];*/

            if ( Numerical::fabs(weightedReducedCost) > max ) {
                max = Numerical::fabs(weightedReducedCost);
                m_outgoingIndex = index;
                m_phase1Simpri.improvingIndexFound();
            }
        }
    }

    /*unsigned int size = m_weights.size();
    LPINFO("VARIANCE: " << squareSum / size - ((sum / size) * (sum / size)) );
    LPINFO("VARIANCE 2: " << squareSum2 / size - ((sum2 / size) * (sum2 / size)) );
    LPINFO("------------------");*/

    if (m_outgoingIndex != -1) {
        m_reducedCost = m_phase1ReducedCosts[m_outgoingIndex];
    }
    return m_outgoingIndex;
}

int DualSteepestEdgePricing::performPricingPhase2() {
    initPhase2();
    if (unlikely(m_weightsReady == false)) {
        initWeights();
    }

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
                           m_basicVariableValues.at(rowIndex)) / Numerical::sqrt(m_weights[rowIndex]);
            if (difference > max) {
                max = difference;
                m_reducedCost = difference * Numerical::sqrt(m_weights[rowIndex]);
                m_outgoingIndex = rowIndex;
                m_phase2Simpri.improvingIndexFound();
            }
            break;
        case Simplex::PLUS:
            difference = (m_basicVariableValues.at(rowIndex) -
                          m_simplexModel.getVariable(variableIndex).getUpperBound()) / Numerical::sqrt(m_weights[rowIndex]);
            if (difference > max) {
                max = difference;
                m_reducedCost = difference * Numerical::sqrt(m_weights[rowIndex]);
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
                                     const DenseVector &incomingAlpha,
                                     const DenseVector &pivotRow,
                                     const DenseVector &pivotRowOfBasisInverse)
{
    __UNUSED(pivotRow);

    unsigned int rowCount = m_simplexModel.getRowCount();

    if (unlikely(m_basis.isFresh() )) {
        std::vector<Numerical::Double> newWeights = m_weights;
        unsigned int index;
        for (index = 0; index < rowCount; index++) {
            if (m_shadowBasisHead[index] != m_basisHead[index]) {
                unsigned int variableIndex = m_basisHead[index];
                if (m_isBasicVariable[variableIndex] != 1) {
                    m_recomuteIndices.push_back(index);
                } else {
                    newWeights[index] = m_weights[ m_basisPositions[variableIndex] ];
                }
                m_basisPositions[variableIndex] = index;
            }
        }
        m_weights = newWeights;
        m_shadowBasisHead = m_basisHead;
        checkAndFix();
    }

    unsigned int index;
    DenseVector tau = pivotRowOfBasisInverse;
    m_basis.Ftran(tau);


    Numerical::Double alpha_p_q = incomingAlpha.at(outgoingIndex);

    Numerical::Double pivotWeight = pivotRowOfBasisInverse.euclidNorm2();

    DenseVector::NonzeroIterator iter = incomingAlpha.beginNonzero();
    DenseVector::NonzeroIterator iterEnd = incomingAlpha.endNonzero();
    for (; iter != iterEnd; ++iter) {
        index = iter.getIndex();
        if (unlikely((int)index == outgoingIndex)) {
            continue;
        }


        Numerical::Double ratio = *iter / alpha_p_q;

        Numerical::Double newWeight = m_weights[index] - 2 * ratio * tau.at(index) + ratio * ratio * pivotWeight;

        m_weights[index] = newWeight;
        m_updateCounters[index]++;
        if (newWeight <= 0) {
            m_recomuteIndices.push_back(index);
//            LPWARNING("Recompute the weight of row " << index << ", because the weight = " << newWeight);

        }
    }


    m_weights[outgoingIndex] = pivotWeight / (alpha_p_q * alpha_p_q);

    m_basisPositions[incomingIndex] = outgoingIndex;
    m_shadowBasisHead[outgoingIndex] = incomingIndex;

    // this variable leaves the basis
    m_isBasicVariable[ m_basisHead[outgoingIndex] ] = 0;
    // this variable enters the basis
    m_isBasicVariable[ incomingIndex ] = 1;

}

void DualSteepestEdgePricing::checkAndFix()
{
    unsigned int rowCount = m_simplexModel.getRowCount();

    for (auto index: m_recomuteIndices) {
        DenseVector row(rowCount);
        row.clear();
        row.set(index, 1.0);
        m_basis.Btran(row);
        Numerical::Double norm = row.euclidNorm2();
        m_weights[ index ] = norm;
        m_updateCounters[ index ] = 0;
    }

    m_recomuteIndices.clear();

}

void DualSteepestEdgePricing::initWeights() {  
    // LPINFO("init weights");
    unsigned int rowCount = m_simplexModel.getRowCount();
    unsigned int columnCount = m_simplexModel.getColumnCount();
    m_weights.resize(rowCount, 1);
    m_shadowBasisHead = m_basisHead;
    m_basisPositions.resize(rowCount + columnCount);
    m_updateCounters.resize(rowCount + columnCount, 0);
    m_isBasicVariable.resize(rowCount + columnCount, 0);
    unsigned int index;
    DenseVector row(rowCount);
    for (index = 0; index < rowCount; index++) {
        row.clear();
        row.set(index, 1.0);
        m_basis.Btran(row);

        m_weights[index] = row.euclidNorm2();

        unsigned int variableIndex = m_basisHead[index];
        m_basisPositions[variableIndex] = index;
        m_isBasicVariable[variableIndex] = 1;

    }
    m_weightsReady = true;
    // LPINFO("weights initialized");
}

