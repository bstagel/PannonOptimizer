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
 * @file   primaldantzigpricing.cpp
 */

#include <simplex/pricing/primaldantzigpricing.h>
#include <simplex/simplex.h>
#include <simplex/simplexparameterhandler.h>

PrimalDantzigPricing::PrimalDantzigPricing(const DenseVector &basicVariableValues,
                                           const IndexList<> & basicVariableFeasibilities,
                                           IndexList<> * reducedCostFeasibilities,
                                           const IndexList<const Numerical::Double *> &variableStates,
                                           const std::vector<int> & basisHead,
                                           const SimplexModel & model,
                                           const Basis & basis,
                                           const DenseVector &reducedCosts):
    PrimalPricing(basicVariableValues,
                  basicVariableFeasibilities,
                  reducedCostFeasibilities,
                  variableStates,
                  basisHead,
                  model,
                  basis,
                  reducedCosts)
{
    m_reducedCost = 0.0;
    m_alternateCandidateCounter = 0;
}


PrimalDantzigPricing::~PrimalDantzigPricing()
{
    release();
}

void PrimalDantzigPricing::release()
{

}

int PrimalDantzigPricing::performPricingPhase1()
{
    initPhase1();
    m_reducedCost = 0.0;
    m_incomingIndex = -1;

    m_phase1Simpri.start();

    int maxIndex = -1;
    int minIndex = -1;
    Numerical::Double maxReducedCost = 0;
    Numerical::Double minReducedCost = 0;

    std::vector<unsigned> phase2Improving;
    bool considerPhase2Improvement = SimplexParameterHandler::getInstance().getBoolParameterValue("Pricing.combined_objective");

    unsigned int variableIndex;
    while (m_phase1Simpri.getCandidateIndex(&variableIndex) ) {
        if ( m_used[variableIndex] == true ) {
            continue;
        }
        switch ( m_variableStates.where(variableIndex) ) {
        case Simplex::NONBASIC_AT_LB:
            if (m_phase1ReducedCosts[variableIndex] < minReducedCost) {
                minIndex = variableIndex;
                minReducedCost = m_phase1ReducedCosts[variableIndex];
                m_phase1Simpri.improvingIndexFound();
                if (m_reducedCosts[variableIndex] <= 0) phase2Improving.push_back(variableIndex);
            }
            break;
        case Simplex::NONBASIC_AT_UB:
            if (m_phase1ReducedCosts[variableIndex] > maxReducedCost) {
                maxIndex = variableIndex;
                maxReducedCost = m_phase1ReducedCosts[variableIndex];
                m_phase1Simpri.improvingIndexFound();
                if (m_reducedCosts[variableIndex] >= 0) phase2Improving.push_back(variableIndex);
            }
            break;
        case Simplex::NONBASIC_FREE:
            if (m_phase1ReducedCosts[variableIndex] < minReducedCost) {
                minIndex = variableIndex;
                minReducedCost = m_phase1ReducedCosts[variableIndex];
                m_phase1Simpri.improvingIndexFound();
                if (m_reducedCosts[variableIndex] <= 0) phase2Improving.push_back(variableIndex);
            } else if (m_phase1ReducedCosts[variableIndex] > maxReducedCost) {
                maxIndex = variableIndex;
                maxReducedCost = m_phase1ReducedCosts[variableIndex];
                m_phase1Simpri.improvingIndexFound();
                if (m_reducedCosts[variableIndex] >= 0) phase2Improving.push_back(variableIndex);
            }
            break;
        }
    }

    unsigned candidate = Numerical::fabs( minReducedCost ) > maxReducedCost ? minIndex : maxIndex;
    m_incomingIndex = candidate;
    m_reducedCost = m_phase1ReducedCosts[candidate];

    if (considerPhase2Improvement && !phase2Improving.empty()) {
        bool improving = false;
        for (unsigned i = 0; i < phase2Improving.size(); ++i) {
            if (candidate == phase2Improving[i]) {
                improving = true;
                break;
            }
        }
        if (!improving) {
            unsigned biggestPhase2Improving = 0;
            for (unsigned i = 1; i < phase2Improving.size(); ++i) {
                if (Numerical::fabs(m_phase1ReducedCosts[phase2Improving[i]]) >
                        Numerical::fabs(m_phase1ReducedCosts[phase2Improving[biggestPhase2Improving]])) {
                    biggestPhase2Improving = i;
                }
            }
            ++m_alternateCandidateCounter;
            m_incomingIndex = phase2Improving[biggestPhase2Improving];
            m_reducedCost = m_phase1ReducedCosts[phase2Improving[biggestPhase2Improving]];
        }
    }
    return m_incomingIndex;
}

int PrimalDantzigPricing::performPricingPhase2()
{
    //OBJECTIVE_TYPE objectiveType = m_updater->m_simplexModel.getObjectiveType();

    int maxIndex = -1;
    int minIndex = -1;
    m_reducedCost = 0.0;
    m_incomingIndex = -1;
    // ????
    //    double maxReducedCost = m_optimalityTolerance;
    //    double minReducedCost = -m_optimalityTolerance;

    Numerical::Double maxReducedCost = 0;
    Numerical::Double minReducedCost = 0;
    auto iter = m_variableStates.getIterator();
    auto iterEnd = m_variableStates.getIterator();

    m_variableStates.getIterators(&iter, &iterEnd, Simplex::NONBASIC_AT_LB, 1);

    for (; iter != iterEnd; ++iter) {
        unsigned int variableIndex = iter.getData();
        if (m_used[variableIndex] == true) {
            continue;
        }
        const auto reducedCost = m_reducedCosts.at(variableIndex);
        if (reducedCost < minReducedCost) {
            minIndex = variableIndex;
            minReducedCost = reducedCost;
        }
    }

    m_variableStates.getIterators(&iter, &iterEnd, Simplex::NONBASIC_AT_UB, 1);

    for (; iter != iterEnd; ++iter) {
        unsigned int variableIndex = iter.getData();
        if (m_used[variableIndex] == true) {
            continue;
        }
        const auto reducedCost = m_reducedCosts.at(variableIndex);
        if (reducedCost > maxReducedCost) {
            maxIndex = variableIndex;
            maxReducedCost = reducedCost;
        }
    }

    m_variableStates.getIterators(&iter, &iterEnd, Simplex::NONBASIC_FREE, 1);
    for (; iter != iterEnd; ++iter) {
        unsigned int variableIndex = iter.getData();
        if (m_used[variableIndex] == true) {
            continue;
        }
        const auto reducedCost = m_reducedCosts.at(variableIndex);
        if (reducedCost < minReducedCost) {
            minIndex = variableIndex;
            minReducedCost = reducedCost;
        } else if (reducedCost > maxReducedCost) {
            maxIndex = variableIndex;
            maxReducedCost = reducedCost;
        }
    }
//    LPINFO("PRICING: min: "<<minReducedCost<<" max: "<<maxReducedCost);
//    LPINFO("PRICING: minId: "<<minIndex<<" maxId: "<<maxIndex);
    if (Numerical::fabs( minReducedCost ) > maxReducedCost) {
        m_reducedCost = minReducedCost;
        m_incomingIndex = minIndex;
        return minIndex;
    }
    m_reducedCost = maxReducedCost;
    m_incomingIndex = maxIndex;
    return maxIndex;
}

void PrimalDantzigPricing::update(int incomingIndex,
                                  int outgoingIndex,
                                  const DenseVector * incomingAlpha,
                                  const DenseVector * pivotRow) {
    unsigned int outgoingVariable = m_basisHead[outgoingIndex];

    m_phase1Simpri.removeCandidate(incomingIndex);
    m_phase2Simpri.removeCandidate(incomingIndex);
    m_phase1Simpri.insertCandidate( outgoingVariable );
    m_phase2Simpri.insertCandidate( outgoingVariable );

    __UNUSED(incomingAlpha);
    __UNUSED(pivotRow);
}
