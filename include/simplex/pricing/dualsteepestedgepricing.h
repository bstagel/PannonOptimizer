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

#ifndef DUALSTEEPESTEDGEPRICING_H
#define DUALSTEEPESTEDGEPRICING_H

#include <simplex/dualpricing.h>
#include <vector>

class DualSteepestEdgePricing: public DualPricing {
public:
    DualSteepestEdgePricing(const DenseVector &basicVariableValues,
                            IndexList<> *basicVariableFeasibilities,
                            const IndexList<> &reducedCostFeasibilities,
                            const std::vector<int> & basisHead,
                            const SimplexModel & simplexModel,
                            const Basis &basis);

    virtual ~DualSteepestEdgePricing();

    int performPricingPhase1();
    int performPricingPhase2();

    void update(int incomingIndex,
                int outgoingIndex,
                const DenseVector &incomingAlpha,
                const DenseVector &pivotRow,
                const DenseVector &pivotRowOfBasisInverse);

    void checkAndFix();

    bool weightsAreReady() const {
        return m_weightsReady;
    }

    void initWeights();
private:

    std::vector<Numerical::Double> m_weights;
    std::vector<unsigned int> m_updateCounters;
    std::vector<unsigned int> m_recomuteIndices;
    std::vector<char> m_isBasicVariable;

    std::vector<int> m_shadowBasisHead;
    std::vector<unsigned int> m_basisPositions;
    unsigned int m_previousPhase;

    bool m_weightsReady;

};


#endif // DUALSTEEPESTEDGEPRICING_H
