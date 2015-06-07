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
 * @file sbf_structural.cpp
 */

#include <simplex/startingbasisfinder/sbf_structural.h>

#include <simplex/simplex.h>

SbfStructural::SbfStructural(const SimplexModel& model,
                       std::vector<int>* basisHead,
                       IndexList<const Numerical::Double *> *variableStates,
                       StartingBasisFinder::STARTING_NONBASIC_STATES nonbasicStates):
    SbfSuper(model, basisHead, variableStates, nonbasicStates)
{

}

SbfStructural::~SbfStructural()
{

}

void SbfStructural::run()
{
    unsigned int i;
    unsigned int j;

    unsigned int rowCount = m_model.getRowCount();
    unsigned int columnCount = m_model.getColumnCount();

    m_basisHead->clear();

    /* Basic variables: set state to BASIC */
    for (i=0, j=0; i < rowCount && i < columnCount; i++, j++) {
        m_basisHead->push_back(j);
        m_variableStates->insert(Simplex::BASIC, j, &ZERO);
    }
    unsigned int last = i;
    /* Nonbasic variables: set state to NONBASIC_AT_UB/LB depending on the strategy used */
    switch (m_nonbasicStates) {

    case StartingBasisFinder::LOWER: {
        for (i=last; i<columnCount + rowCount; i++) {
            adjustVariableByType(i, Simplex::NONBASIC_AT_LB);
        }
        break;
    }

    case StartingBasisFinder::UPPER: {
        for (i=last; i<columnCount + rowCount; i++) {
            adjustVariableByType(i, Simplex::NONBASIC_AT_UB);
        }
        break;
    }

    case StartingBasisFinder::MIXED: {
        const DenseVector & costs = m_model.getCostVector();
        //Always minimize
        for (i=last; i<columnCount + rowCount; i++) {
            if (costs.at(i) < 0) {
                adjustVariableByType(i, Simplex::NONBASIC_AT_LB);
            } else {
                adjustVariableByType(i, Simplex::NONBASIC_AT_UB);
            }
        }
        break;
    }

    default: {
        throw ParameterException("Invalid starting nonbasic state given!");
        break;
    }

    }
}

