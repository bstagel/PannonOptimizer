#include <simplex/pricing/primaldevexpricing.h>
#include <simplex/simplex.h>
#include <prettyprint.h>


PrimalDevexPricing::PrimalDevexPricing(const DenseVector &basicVariableValues,
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


PrimalDevexPricing::~PrimalDevexPricing()
{
    release();
}

void PrimalDevexPricing::release()
{

}

int PrimalDevexPricing::performPricingPhase1()
{
    if (m_weightsReady == false) {
        setReferenceFramework();
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

        auto weight = Numerical::sqrt(m_weights[variableIndex]);
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

int PrimalDevexPricing::performPricingPhase2()
{
    //OBJECTIVE_TYPE objectiveType = m_updater->m_simplexModel.getObjectiveType();
    if (m_weightsReady == false) {
        setReferenceFramework();
    }

    int maxIndex = -1;
    int minIndex = -1;
    m_reducedCost = 0.0;
    m_incomingIndex = -1;
    // ????
    //    double maxReducedCost = m_optimalityTolerance;
    //    double minReducedCost = -m_optimalityTolerance;

    Numerical::Double maxReducedCost = 0;
    Numerical::Double minReducedCost = 0;
    Numerical::Double maxWeightedReducedCost = 0;
    Numerical::Double minWeightedReducedCost = 0;

    auto iter = m_variableStates.getIterator();
    auto iterEnd = m_variableStates.getIterator();
    m_variableStates.getIterators(&iter, &iterEnd, Simplex::NONBASIC_AT_LB, 1);

    for (; iter != iterEnd; ++iter) {
        unsigned int variableIndex = iter.getData();
        if (m_used[variableIndex] == true) {
            continue;
        }
        const auto weight = Numerical::sqrt(m_weights[variableIndex]);
        const auto reducedCost = m_reducedCosts.at(variableIndex);
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
        const auto weight = Numerical::sqrt(m_weights[variableIndex]);
        const auto reducedCost = m_reducedCosts.at(variableIndex);
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
        const auto weight = Numerical::sqrt(m_weights[variableIndex]);
        const auto reducedCost = m_reducedCosts.at(variableIndex);
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

void PrimalDevexPricing::update(int incomingIndex,
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
    const unsigned int rows = matrix.rowCount();

    auto exactIncomingWeight = getExactDevexWeigth(*incomingAlpha, incomingIndex);
    auto estimatedIncomingWeight = m_weights[incomingIndex];
    auto ratio = exactIncomingWeight / estimatedIncomingWeight;
    if (ratio > 3.0 || ratio < 1.0 / 3.0) {
        setReferenceFramework();
        //LPINFO("Set reference framework");
        return;
    }

    auto alpha_q_p = (*incomingAlpha)[outgoingIndex];
    SparseVector multiplier = SparseVector::createUnitVector(rows, outgoingIndex);
    m_basis.Btran(multiplier);

    // TODO: erre a reszre gyorsitast irni, mint a regi panoptban
    // pl nem itt kell letrehozni ezt a ket vektort
    // kell egy indexvektor is majd, ami jeloli, hogy hol vannak nem nullak
    // a ket vektorban
    std::vector<Numerical::Double> positiveSums(columns, 0.0);
    std::vector<Numerical::Double> negativeSums(columns, 0.0);

    auto multIter = multiplier.beginNonzero();
    auto multIterEnd = multiplier.endNonzero();
    for (; multIter != multIterEnd; ++multIter) {
        const unsigned int rowIndex = multIter.getIndex();
        const auto mul = *multIter;
        auto rowIter = matrix.row(rowIndex).beginNonzero();
        auto rowIterEnd = matrix.row(rowIndex).endNonzero();
        for (; rowIter != rowIterEnd; ++rowIter) {
            const auto prod = mul * *rowIter;
            if (prod < 0.0) {
                negativeSums[ rowIter.getIndex() ] += prod;
            } else {
                positiveSums[ rowIter.getIndex() ] += prod;
            }
        }
    }

    auto posIter = positiveSums.begin();
    auto negIter = negativeSums.begin();
    auto negIterEnd = negativeSums.end();
    int index = 0;
    for (; negIter != negIterEnd; ++negIter, ++posIter, ++index) {
        if (unlikely(index == incomingIndex || index == (int)outgoingVariable)) {
            continue;
        }
        const auto alpha_p_j = Numerical::stableAdd( *posIter, *negIter );
        if (alpha_p_j == 0.0) {
            continue;
        }
        const auto updatedAlpha_j_p = alpha_p_j / alpha_q_p;

        auto weight = m_weights[index];
        auto weight2 = updatedAlpha_j_p * updatedAlpha_j_p * exactIncomingWeight;
        m_weights[index] = weight > weight2 ? weight : weight2;
    }

    multIter = multiplier.beginNonzero();
    index = columns;
    for (; multIter != multIterEnd; ++multIter) {
        if (unlikely(index == incomingIndex || index == (int)outgoingVariable)) {
            continue;
        }
        if (m_variableStates.where(index) == Simplex::BASIC) {
            continue;
        }
        Numerical::Double alpha_p_j = *multIter;
        Numerical::Double updatedAlpha_j_p = alpha_p_j / alpha_q_p;
        Numerical::Double weight = m_weights[index];
        Numerical::Double weight2 = updatedAlpha_j_p * updatedAlpha_j_p * exactIncomingWeight;
        m_weights[index] = weight > weight2 ? weight : weight2;
    }

    auto weight2 = exactIncomingWeight / (alpha_q_p * alpha_q_p);
    if (weight2 > 1.0) {
        m_weights[ outgoingIndex ] = weight2;
    } else {
        m_weights[ outgoingIndex ] = 1.0;
    }

    // gamma_q = alpha^t_q * alpha_q + 1

    /* m_weights[incomingIndex] = incomingAlpha->euclidNorm2() + 1.0;
    double incomingGamma = m_weights[incomingIndex];

    SparseVector temp = SparseVector::createVectorFromDenseArray( &incomingAlpha->at(0), incomingAlpha->length() );
    m_basis.Btran(temp);

    double alpha_q_p = incomingAlpha->at(outgoingIndex);
    m_weights[outgoingVariable] = (1.0 / (alpha_q_p * alpha_q_p)) * m_weights[incomingIndex];

    SparseVector multiplier = SparseVector::createUnitVector( incomingAlpha->length(), outgoingIndex );
    m_basis.Btran(multiplier);

    IndexList<const double*>::Iterator iter, iterEnd;
    m_variableStates.getIterators(&iter, &iterEnd,
                                  Simplex::NONBASIC_AT_LB,
                                  Simplex::VARIABLE_STATE_ENUM_LENGTH - Simplex::NONBASIC_AT_LB);
    for (; iter != iterEnd; ++iter) {
        const unsigned int variableIndex = iter.getData();
        if (variableIndex == outgoingVariable || variableIndex == incomingIndex) {
            continue;
        }

        double weight = m_weights[variableIndex];

        if (variableIndex < matrix.columnCount()) {
            double alpha_p_j = matrix.column(variableIndex).dotProduct(multiplier);

            double updatedAlpha_j_p = alpha_p_j / alpha_q_p;
            if (updatedAlpha_j_p == 0.0) {
                continue;
            }



            weight = Numerical::stableAdd(weight, updatedAlpha_j_p * updatedAlpha_j_p * incomingGamma);

            double dotp = temp.dotProduct(matrix.column(variableIndex));

            if (dotp != 0.0) {
                weight = Numerical::stableAdd(weight, -2.0 * updatedAlpha_j_p * dotp);
            }
            double alternativeWeight = Numerical::stableAdd(updatedAlpha_j_p * updatedAlpha_j_p, 1.0);
            if (weight < alternativeWeight) {
                weight = alternativeWeight;
            }


        } else {
            double alpha_p_j = multiplier.at(variableIndex - columns);
            //end = clock();
            //time += end - start;
            //LPINFO("dot product: " << alpha_p_j);
            //std::cin.get();

            double updatedAlpha_j_p = alpha_p_j / alpha_q_p;
            if (updatedAlpha_j_p == 0.0) {
                continue;
            }


            weight = Numerical::stableAdd(weight, updatedAlpha_j_p * updatedAlpha_j_p * incomingGamma);
            //start = clock();
            double dotp = temp.at(variableIndex - columns);
            //end = clock();
            //time += end - start;
            if (dotp != 0.0) {
                weight = Numerical::stableAdd(weight, -2.0 * updatedAlpha_j_p * dotp);
            }
            double alternativeWeight = Numerical::stableAdd(updatedAlpha_j_p * updatedAlpha_j_p, 1.0);
            if (weight < alternativeWeight) {
                weight = alternativeWeight;
            }




        }
        m_weights[variableIndex] = weight;
    }

    incomingGamma = 2.0;*/

}

void PrimalDevexPricing::setReferenceFramework()
{
    const unsigned int columnCount = m_simplexModel.getColumnCount();
    const unsigned int rowCount = m_simplexModel.getRowCount();
    m_weights.clear();
    m_referenceFrameWorkMask.clear();
    m_weights.resize(columnCount + rowCount, 1.0);
    m_referenceFrameWorkMask.resize(columnCount + rowCount, 0);

    auto iter = m_variableStates.getIterator();
    auto iterEnd = m_variableStates.getIterator();
    m_variableStates.getIterators(&iter, &iterEnd,
                                  Simplex::NONBASIC_AT_LB,
                                  (int)Simplex::VARIABLE_STATE_ENUM_LENGTH - (int)Simplex::NONBASIC_AT_LB);
    for (; iter != iterEnd; ++iter) {
        const unsigned int variableIndex = iter.getData();
        m_referenceFrameWorkMask[variableIndex] = 1;
    }
    m_weightsReady = true;
}

Numerical::Double PrimalDevexPricing::getExactDevexWeigth(const DenseVector &vector,
                                                          unsigned int index)
{
    Numerical::Double result = 0.0;
    if (m_referenceFrameWorkMask[index] == 1) {
        result += 1.0;
    }
    auto iter = vector.beginNonzero();
    auto iterEnd = vector.endNonzero();
    for (; iter != iterEnd; ++iter) {
        const unsigned int variableIndex = m_basisHead[ iter.getIndex() ];
        result += *iter * m_referenceFrameWorkMask[ variableIndex ];
    }

    return result;
}
