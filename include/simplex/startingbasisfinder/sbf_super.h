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
 * @file sbf_super.h
 */

#ifndef SBF_SUPER_H_
#define SBF_SUPER_H_

#include <globals.h>

#include <vector>
#include <set>
#include <assert.h>

#include <simplex/simplexmodel.h>
#include <simplex/simplex.h>
#include <simplex/startingbasisfinder.h>
#include <utils/numerical.h>
#include <utils/indexlist.h>

class SbfSuper
{
public:
    SbfSuper(const SimplexModel& model,
             std::vector<int>* basisHead,
             IndexList<const Numerical::Double*>* variableStates,
             StartingBasisFinder::STARTING_NONBASIC_STATES nonbasicStates);
	virtual ~SbfSuper();

    virtual void run() = 0;

protected:
    const SimplexModel & m_model;
    std::vector<int> * m_basisHead;
    IndexList<const Numerical::Double*> * m_variableStates;
    StartingBasisFinder::STARTING_NONBASIC_STATES m_nonbasicStates;

    void adjustVariableByType(unsigned int variableIndex, Simplex::VARIABLE_STATE state);
};


#endif /* SBF_SUPER_H_ */
