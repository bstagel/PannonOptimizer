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
    static int __counter = 0;

    initPhase1();
    int maxIndex = -1;
    int minIndex = -1;
    m_reducedCost = 0.0;
    m_incomingIndex = -1;

    //TODO: What the hell is this? :O
    //    Numerical::Double maxReducedCost = m_feasibilityTolerance;
    //    Numerical::Double minReducedCost = -m_feasibilityTolerance;

    m_phase1Simpri.start();

    Numerical::Double maxReducedCost = 0;
    Numerical::Double minReducedCost = 0;

    /*if (__counter == 2371) {
        int i;
        for (i = 0; i < m_phase1ReducedCosts.size(); i++) {
            if (m_phase1ReducedCosts[i] != 0) {
                LPERROR(i << "  " << m_phase1ReducedCosts[i] << "  " << this->m_variableStates.where(i));

                std::cin.get();
            }
        }
        //exit(1);
    }*/

    unsigned int variableIndex;
    unsigned int mennyi = 0;
    std::vector<char> volt( m_simplexModel.getVariables().size(), 0 );
    while (m_phase1Simpri.getCandidateIndex(&variableIndex) ) {
        mennyi++;
        /*if (volt[variableIndex] == 1) {
            LPERROR("VOLT MAR");
            exit(1);
        }*/
        volt[variableIndex] = 1;
        if ( m_used[variableIndex] == true ) {
            continue;
        }
        /*if (__counter == 2371) {
            if (variableIndex == 45 || variableIndex == 2037 || variableIndex == 2070 || variableIndex == 2131
                    || variableIndex == 2139 || variableIndex == 2146) {
                LPINFO("MEGVAN where: " << m_variableStates.where(variableIndex) << "  " << m_phase1ReducedCosts[variableIndex]);
                LPINFO("used: " << m_used[variableIndex]  << "  " << Simplex::NONBASIC_FREE);
                std::cin.get();
            }
            LPINFO(variableIndex << "  " << m_phase1ReducedCosts[variableIndex]);
        }*/


        // LPINFO(variableIndex);
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
      //      if (__counter == 2371)
      //          LPINFO("FREE: " << m_phase1ReducedCosts[variableIndex]);
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
    /*if (__counter == 2371) {
        LPINFO("probak: " << mennyi << "  " << m_simplexModel.getMatrix().columnCount());
    }*/
    // TODO: fixed valtozokat be se rakjuk a szakaszokba

    /*IndexList<const Numerical::Double*>::Iterator iter, iterEnd;
    m_variableStates.getIterators(&iter, &iterEnd, Simplex::NONBASIC_AT_LB, 1);

    for (; iter != iterEnd; iter++) {
        unsigned int variableIndex = iter.getData();
        if (m_used[variableIndex] == true) {
            continue;
        }
        if (m_phase1ReducedCosts[variableIndex] < minReducedCost) {
            minIndex = variableIndex;
            minReducedCost = m_phase1ReducedCosts[variableIndex];
        }
    }

    m_variableStates.getIterators(&iter, &iterEnd, Simplex::NONBASIC_AT_UB, 1);

    for (; iter != iterEnd; iter++) {
        unsigned int variableIndex = iter.getData();
        if (m_used[variableIndex] == true) {
            continue;
        }
        if (m_phase1ReducedCosts[variableIndex] > maxReducedCost) {
            maxIndex = variableIndex;
            maxReducedCost = m_phase1ReducedCosts[variableIndex];
        }
    }

    m_variableStates.getIterators(&iter, &iterEnd, Simplex::NONBASIC_FREE, 1);
    for (; iter != iterEnd; iter++) {
        unsigned int variableIndex = iter.getData();
        if (m_used[variableIndex] == true) {
            continue;
        }
        if (m_phase1ReducedCosts[variableIndex] < minReducedCost) {
            minIndex = variableIndex;
            minReducedCost = m_phase1ReducedCosts[variableIndex];
        } else if (m_phase1ReducedCosts[variableIndex] > maxReducedCost) {
            maxIndex = variableIndex;
            maxReducedCost = m_phase1ReducedCosts[variableIndex];
        }
    }*/

    //LPINFO(minIndex << ", " << maxIndex << "  " << __counter);
    __counter++;
//    LPINFO("PRICING: min: "<<minReducedCost<<" max: "<<maxReducedCost);
//    LPINFO("PRICING: minId: "<<minIndex<<" maxId: "<<maxIndex);
    if (Numerical::fabs( minReducedCost ) > maxReducedCost) {
        m_reducedCost = minReducedCost;
        m_incomingIndex = minIndex;
        //LPINFO("\t\tminIndex = " << minIndex << "  " << m_reducedCost);
        return minIndex;
    }
    m_reducedCost = maxReducedCost;
    m_incomingIndex = maxIndex;
    //LPINFO("\t\tmaxIndex = " << maxIndex << "  " << m_reducedCost);
    return maxIndex;
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

    for (; iter != iterEnd; iter++) {
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

    for (; iter != iterEnd; iter++) {
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
    for (; iter != iterEnd; iter++) {
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
