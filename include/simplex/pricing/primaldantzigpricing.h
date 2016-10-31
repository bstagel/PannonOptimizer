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
 * @file   primaldantzigpricing.h
 */

#ifndef PRIMALDANTZIGPRICING_H
#define	PRIMALDANTZIGPRICING_H

#include <globals.h>

#include <simplex/primalpricing.h>
#include <utils/exceptions.h>
#include <utils/indexlist.h>
#include <vector>

class PrimalDantzigPricing: public PrimalPricing
{
public:
    PrimalDantzigPricing(const DenseVector & basicVariableValues,
                         const IndexList<> & basicVariableFeasibilities,
                         IndexList<> * reducedCostFeasibilities,
                         const IndexList<const Numerical::Double*> & variableStates,
                         const std::vector<int> & basisHead,
                         const SimplexModel & model,
                         const Basis & basis,
                         const DenseVector & reducedCosts);
    virtual ~PrimalDantzigPricing();

    int performPricingPhase1();
    int performPricingPhase2();

    void update(int incomingIndex,
                int outgoingIndex,
                const DenseVector *incomingAlpha,
                const DenseVector *pivotRow);
private:
    void release();

};

#endif	/* PRIMALDANTZIGPRICING_H */

