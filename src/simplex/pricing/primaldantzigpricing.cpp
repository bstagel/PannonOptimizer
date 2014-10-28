/**
 * @file   primaldantzigpricing.cpp
 */

#include <simplex/pricing/primaldantzigpricing.h>
#include <simplex/simplex.h>
#include <prettyprint.h>


PrimalDantzigPricing::PrimalDantzigPricing(const Vector & basicVariableValues,
                                           const IndexList<> & basicVariableFeasibilities,
                                           IndexList<> * reducedCostFeasibilities,
                                           const IndexList<const Numerical::Double *> & variableStates,
                                           const std::vector<int> & basisHead,
                                           const SimplexModel & model,
                                           const Basis & basis,
                                           const Vector &reducedCosts):
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
            }
            break;
        case Simplex::NONBASIC_AT_UB:
            if (m_phase1ReducedCosts[variableIndex] > maxReducedCost) {
                maxIndex = variableIndex;
                maxReducedCost = m_phase1ReducedCosts[variableIndex];
                m_phase1Simpri.improvingIndexFound();
            }
            break;
        case Simplex::NONBASIC_FREE:
            if (m_phase1ReducedCosts[variableIndex] < minReducedCost) {
                minIndex = variableIndex;
                minReducedCost = m_phase1ReducedCosts[variableIndex];
                m_phase1Simpri.improvingIndexFound();

            } else if (m_phase1ReducedCosts[variableIndex] > maxReducedCost) {
                maxIndex = variableIndex;
                maxReducedCost = m_phase1ReducedCosts[variableIndex];
                m_phase1Simpri.improvingIndexFound();

            }
            break;
        }
    }

//    LPINFO("PRICING: minReducedCost: "<<minReducedCost<<" maxReducedCost: "<<maxReducedCost);
//    LPINFO("PRICING: minIndex: "<<minIndex<<" maxIndex: "<<maxIndex);

//    LPINFO("min: "<<minReducedCost<<" index: "<<minIndex);
//    LPINFO("max: "<<maxReducedCost<<" index: "<<maxIndex);
    if (Numerical::fabs( minReducedCost ) > maxReducedCost) {
        m_reducedCost = minReducedCost;
        m_incomingIndex = minIndex;
        return minIndex;
    } else {
        m_reducedCost = maxReducedCost;
        m_incomingIndex = maxIndex;
        return maxIndex;
    }
}

int PrimalDantzigPricing::performPricingPhase2()
{
    //OBJECTIVE_TYPE objectiveType = m_updater->m_simplexModel.getObjectiveType();

    int maxIndex = -1;
    int minIndex = -1;
    m_reducedCost = 0.0;
    m_incomingIndex = -1;
    // ????
    //    Numerical::Double maxReducedCost = m_optimalityTolerance;
    //    Numerical::Double minReducedCost = -m_optimalityTolerance;

    Numerical::Double maxReducedCost = 0;
    Numerical::Double minReducedCost = 0;
    IndexList<const Numerical::Double*>::Iterator iter, iterEnd;

    m_variableStates.getIterators(&iter, &iterEnd, Simplex::NONBASIC_AT_LB, 1);

    for (; iter != iterEnd; ++iter) {
        unsigned int variableIndex = iter.getData();
        if (m_used[variableIndex] == true) {
            continue;
        }
        const Numerical::Double reducedCost = m_reducedCosts.at(variableIndex);
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
        const Numerical::Double reducedCost = m_reducedCosts.at(variableIndex);
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
        const Numerical::Double reducedCost = m_reducedCosts.at(variableIndex);
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
                                  const Vector * incomingAlpha,
                                  const Vector * pivotRow) {
    unsigned int outgoingVariable = m_basisHead[outgoingIndex];
    /*if (outgoingVariable == 45) {
        LPINFO("most kimegy a bazisbol");
        std::cin.get();
    }
    if (incomingIndex == 45) {
        LPINFO("most bemegy a bazisba");
        std::cin.get();
    }*/

    m_phase1Simpri.removeCandidate(incomingIndex);
    m_phase2Simpri.removeCandidate(incomingIndex);
    m_phase1Simpri.insertCandidate( outgoingVariable );
    m_phase2Simpri.insertCandidate( outgoingVariable );

    __UNUSED(incomingAlpha);
    __UNUSED(pivotRow);
}
