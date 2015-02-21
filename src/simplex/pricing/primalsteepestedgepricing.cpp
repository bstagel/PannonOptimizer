/**
 * @file   primaldantzigpricing.cpp
 */

#include <simplex/pricing/primalsteepestedgepricing.h>
#include <simplex/simplex.h>
#include <prettyprint.h>


PrimalSteepestEdgePricing::PrimalSteepestEdgePricing(const DenseVector &basicVariableValues,
                                                     const IndexList<> & basicVariableFeasibilities,
                                                     IndexList<> * reducedCostFeasibilities,
                                                     const IndexList<const Numerical::Double *> & variableStates,
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
    m_weightsReady = false;
}


PrimalSteepestEdgePricing::~PrimalSteepestEdgePricing()
{
    release();
}

void PrimalSteepestEdgePricing::release()
{

}

int PrimalSteepestEdgePricing::performPricingPhase1()
{
    if (m_weightsReady == false) {
        recomputeSteepestEdgeWeights();
    }
    initPhase1();
    m_reducedCost = 0.0;
    m_incomingIndex = -1;

    m_phase1Simpri.start();

    int maxIndex = -1;
    int minIndex = -1;
    Numerical::Double maxWeightedReducedCost = 0;
    Numerical::Double minWeightedReducedCost = 0;
    Numerical::Double maxReducedCost = 0;
    Numerical::Double minReducedCost = 0;

    unsigned int variableIndex;
    while (m_phase1Simpri.getCandidateIndex(&variableIndex) ) {
        if ( m_used[variableIndex] == true ) {
            continue;
        }

        Numerical::Double weight = Numerical::sqrt(m_weights[variableIndex]);
        switch ( m_variableStates.where(variableIndex) ) {
        case Simplex::NONBASIC_AT_LB:
            if (m_phase1ReducedCosts[variableIndex] / weight < minWeightedReducedCost) {
                minIndex = variableIndex;
                minReducedCost = m_phase1ReducedCosts[variableIndex];
                minWeightedReducedCost = m_phase1ReducedCosts[variableIndex]  / weight;
                m_phase1Simpri.improvingIndexFound();
            }
            break;
        case Simplex::NONBASIC_AT_UB:
            if (m_phase1ReducedCosts[variableIndex] / weight > maxWeightedReducedCost) {
                maxIndex = variableIndex;
                maxReducedCost = m_phase1ReducedCosts[variableIndex];
                maxWeightedReducedCost = m_phase1ReducedCosts[variableIndex] / weight;
                m_phase1Simpri.improvingIndexFound();
            }
            break;
        case Simplex::NONBASIC_FREE:
            if (m_phase1ReducedCosts[variableIndex] / weight < minWeightedReducedCost) {
                minIndex = variableIndex;
                minReducedCost = m_phase1ReducedCosts[variableIndex];
                minWeightedReducedCost = m_phase1ReducedCosts[variableIndex] / weight;
                m_phase1Simpri.improvingIndexFound();

            } else if (m_phase1ReducedCosts[variableIndex] / weight > maxWeightedReducedCost) {
                maxIndex = variableIndex;
                maxReducedCost = m_phase1ReducedCosts[variableIndex];
                maxWeightedReducedCost = m_phase1ReducedCosts[variableIndex] / weight;
                m_phase1Simpri.improvingIndexFound();

            }
            break;
        }
    }

    if (Numerical::fabs( minWeightedReducedCost ) > maxWeightedReducedCost) {
        m_reducedCost = minReducedCost;
        m_incomingIndex = minIndex;
        return minIndex;
    } else {
        m_reducedCost = maxReducedCost;
        m_incomingIndex = maxIndex;
        return maxIndex;
    }
}

int PrimalSteepestEdgePricing::performPricingPhase2()
{
    //OBJECTIVE_TYPE objectiveType = m_updater->m_simplexModel.getObjectiveType();
    if (m_weightsReady == false) {
        recomputeSteepestEdgeWeights();
    }

    int maxIndex = -1;
    int minIndex = -1;
    m_reducedCost = 0.0;
    m_incomingIndex = -1;
    // ????
    //    Numerical::Double maxReducedCost = m_optimalityTolerance;
    //    Numerical::Double minReducedCost = -m_optimalityTolerance;

    Numerical::Double maxReducedCost = 0;
    Numerical::Double minReducedCost = 0;
    Numerical::Double maxWeightedReducedCost = 0;
    Numerical::Double minWeightedReducedCost = 0;
    IndexList<const Numerical::Double*>::Iterator iter, iterEnd;

    m_variableStates.getIterators(&iter, &iterEnd, Simplex::NONBASIC_AT_LB, 1);

    for (; iter != iterEnd; ++iter) {
        unsigned int variableIndex = iter.getData();
        if (m_used[variableIndex] == true) {
            continue;
        }
        const Numerical::Double weight = Numerical::sqrt(m_weights[variableIndex]);
        const Numerical::Double reducedCost = m_reducedCosts.at(variableIndex);
        if (reducedCost / weight < minWeightedReducedCost) {
            minIndex = variableIndex;
            minReducedCost = reducedCost;
            minWeightedReducedCost = reducedCost / weight;
        }
    }

    m_variableStates.getIterators(&iter, &iterEnd, Simplex::NONBASIC_AT_UB, 1);

    for (; iter != iterEnd; ++iter) {
        unsigned int variableIndex = iter.getData();
        if (m_used[variableIndex] == true) {
            continue;
        }
        const Numerical::Double weight = Numerical::sqrt(m_weights[variableIndex]);
        const Numerical::Double reducedCost = m_reducedCosts.at(variableIndex);
        if (reducedCost / weight > maxWeightedReducedCost) {
            maxIndex = variableIndex;
            maxReducedCost = reducedCost;
            maxWeightedReducedCost = reducedCost / weight;
        }
    }

    m_variableStates.getIterators(&iter, &iterEnd, Simplex::NONBASIC_FREE, 1);
    for (; iter != iterEnd; ++iter) {
        unsigned int variableIndex = iter.getData();
        if (m_used[variableIndex] == true) {
            continue;
        }
        const Numerical::Double weight = Numerical::sqrt(m_weights[variableIndex]);
        const Numerical::Double reducedCost = m_reducedCosts.at(variableIndex);
        if (reducedCost / weight < minWeightedReducedCost) {
            minIndex = variableIndex;
            minReducedCost = reducedCost;
            minWeightedReducedCost = reducedCost / weight;
        } else if (reducedCost / weight > maxWeightedReducedCost) {
            maxIndex = variableIndex;
            maxReducedCost = reducedCost;
            maxWeightedReducedCost = reducedCost / weight;
        }
    }
    //    LPINFO("PRICING: min: "<<minReducedCost<<" max: "<<maxReducedCost);
    //    LPINFO("PRICING: minId: "<<minIndex<<" maxId: "<<maxIndex);
    if (Numerical::fabs( minWeightedReducedCost ) > maxWeightedReducedCost) {
        m_reducedCost = minReducedCost;
        m_incomingIndex = minIndex;
        return minIndex;
    }
    m_reducedCost = maxReducedCost;
    m_incomingIndex = maxIndex;
    return maxIndex;
}

void PrimalSteepestEdgePricing::update(int incomingIndex,
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

    const Matrix & matrix = m_simplexModel.getMatrix();
    const unsigned int columns = matrix.columnCount();

    // gamma_q = alpha^t_q * alpha_q + 1

    m_weights[incomingIndex] = incomingAlpha->euclidNorm2() + 1.0;
    Numerical::Double incomingGamma = m_weights[incomingIndex];

    SparseVector temp = SparseVector::createVectorFromDenseArray( &incomingAlpha->at(0), incomingAlpha->length() );
    m_basis.Btran(temp);

    Numerical::Double alpha_q_p = incomingAlpha->at(outgoingIndex);
    m_weights[outgoingVariable] = (1.0 / (alpha_q_p * alpha_q_p)) * m_weights[incomingIndex];

    SparseVector multiplier = SparseVector::createUnitVector( incomingAlpha->length(), outgoingIndex );
    m_basis.Btran(multiplier);

    IndexList<const Numerical::Double*>::Iterator iter, iterEnd;
    m_variableStates.getIterators(&iter, &iterEnd,
                                  Simplex::NONBASIC_AT_LB,
                                  Simplex::VARIABLE_STATE_ENUM_LENGTH - Simplex::NONBASIC_AT_LB);
    for (; iter != iterEnd; ++iter) {
        const unsigned int variableIndex = iter.getData();
        if (variableIndex == outgoingVariable || variableIndex == incomingIndex) {
            continue;
        }

        Numerical::Double weight = m_weights[variableIndex];

        if (variableIndex < matrix.columnCount()) {
            Numerical::Double alpha_p_j = matrix.column(variableIndex).dotProduct(multiplier);

            Numerical::Double updatedAlpha_j_p = alpha_p_j / alpha_q_p;
            if (updatedAlpha_j_p == 0.0) {
                continue;
            }



            weight = Numerical::stableAdd(weight, updatedAlpha_j_p * updatedAlpha_j_p * incomingGamma);

            Numerical::Double dotp = temp.dotProduct(matrix.column(variableIndex));

            if (dotp != 0.0) {
                weight = Numerical::stableAdd(weight, -2.0 * updatedAlpha_j_p * dotp);
            }
            Numerical::Double alternativeWeight = Numerical::stableAdd(updatedAlpha_j_p * updatedAlpha_j_p, 1.0);
            if (weight < alternativeWeight) {
                weight = alternativeWeight;
            }


        } else {
            Numerical::Double alpha_p_j = multiplier.at(variableIndex - columns);
            //end = clock();
            //time += end - start;
            //LPINFO("dot product: " << alpha_p_j);
            //std::cin.get();

            Numerical::Double updatedAlpha_j_p = alpha_p_j / alpha_q_p;
            if (updatedAlpha_j_p == 0.0) {
                continue;
            }


            weight = Numerical::stableAdd(weight, updatedAlpha_j_p * updatedAlpha_j_p * incomingGamma);
            //start = clock();
            Numerical::Double dotp = temp.at(variableIndex - columns);
            //end = clock();
            //time += end - start;
            if (dotp != 0.0) {
                weight = Numerical::stableAdd(weight, -2.0 * updatedAlpha_j_p * dotp);
            }
            Numerical::Double alternativeWeight = Numerical::stableAdd(updatedAlpha_j_p * updatedAlpha_j_p, 1.0);
            if (weight < alternativeWeight) {
                weight = alternativeWeight;
            }




        }
        m_weights[variableIndex] = weight;
    }

    incomingGamma = 2.0;

}

void PrimalSteepestEdgePricing::recomputeSteepestEdgeWeights()
{
    const unsigned int columnCount = m_simplexModel.getColumnCount();
    const unsigned int rowCount = m_simplexModel.getRowCount();
    if (m_weights.size() < columnCount + rowCount ) {
        m_weights.resize(columnCount + rowCount, 1.0);
    }
    IndexList<const Numerical::Double*>::Iterator iter, iterEnd;
    m_variableStates.getIterators(&iter, &iterEnd,
                                  Simplex::NONBASIC_AT_LB,
                                  Simplex::VARIABLE_STATE_ENUM_LENGTH - Simplex::NONBASIC_AT_LB);
    for (; iter != iterEnd; ++iter) {
        const unsigned int variableIndex = iter.getData();
        if (variableIndex < columnCount) {
            SparseVector column = m_simplexModel.getMatrix().column( variableIndex );

            m_basis.Ftran(column);
            Numerical::Double weight = column.euclidNorm();
            weight = weight * weight + 1;
            m_weights[ variableIndex ] = weight;
        } else {
            SparseVector column = SparseVector::createUnitVector(rowCount, variableIndex - columnCount);
            m_basis.Ftran(column);
            Numerical::Double weight = column.euclidNorm();
            weight = weight * weight + 1;
            m_weights[ variableIndex ] = weight;
        }
    }
}

