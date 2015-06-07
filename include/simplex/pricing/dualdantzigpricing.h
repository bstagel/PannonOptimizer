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
 * @file   dualdantzigpricing.h This file contains the API of the DualDantzigPricing class.
 */

#ifndef DUALDANTZIGPRICING_H
#define	DUALDANTZIGPRICING_H

#include <globals.h>

#include <simplex/dualpricing.h>
#include <utils/exceptions.h>
#include <vector>

#include <simplex/pricing/dualsteepestedgepricing.h>

class DualDantzigPricing: public DualPricing
{
public:
    DualDantzigPricing(const DenseVector &basicVariableValues,
                       IndexList<> *basicVariableFeasibilities,
                       const IndexList<> &reducedCostFeasibilities,
                       const std::vector<int> & basisHead,
                       const SimplexModel & simplexModel,
                       const Basis &basis,
                       bool shadowSteepestEdgeWeights);
    virtual ~DualDantzigPricing();

    int performPricingPhase1();
    int performPricingPhase2();

    void update(int incomingIndex,
                int outgoingIndex,
                const DenseVector &incomingAlpha,
                const DenseVector &pivotRow,
                const DenseVector &pivotRowOfBasisInverse);

    void checkAndFix() {

    }

    DualSteepestEdgePricing * getShadowSteepestEdge() {
        return m_shadowSteepestEdge;
    }

private:

    DualSteepestEdgePricing * m_shadowSteepestEdge;
};

#endif	/* DUALDANTZIGPRICING_H */

