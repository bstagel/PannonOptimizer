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

#ifndef DUALDEVEXPRICING_H
#define DUALDEVEXPRICING_H

#include <simplex/dualpricing.h>
#include <vector>

class DualDevexPricing: public DualPricing {
public:
    DualDevexPricing(const DenseVector &basicVariableValues,
                     IndexList<> *basicVariableFeasibilities,
                     const IndexList<> &reducedCostFeasibilities,
                     const std::vector<int> & basisHead,
                     const SimplexModel & simplexModel,
                     const Basis &basis);

    virtual ~DualDevexPricing();

    int performPricingPhase1();
    int performPricingPhase2();

    void update(int incomingIndex,
                int outgoingIndex,
                const DenseVector &incomingAlpha,
                const DenseVector &pivotRow,
                const DenseVector &pivotRowOfBasisInverse);

    void checkAndFix();

private:
    std::vector<char> m_referenceFramework;

    std::vector<Numerical::Double> m_weights;

    unsigned int m_previousPhase;

    void setReferenceFramework();
};

#endif // DUALDEVEXPRICING_H
