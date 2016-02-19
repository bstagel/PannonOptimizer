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
 * @file simplexstate.h This file contains the SimplexState class.
 */

#ifndef SIMPLEXSTATE_H
#define SIMPLEXSTATE_H

#include <globals.h>
#include <list>
#include <vector>

#include <linalg/densevector.h>
#include <linalg/sparsevector.h>
#include <utils/indexlist.h>

/**
 * This class describes a state of a basis, without information about the model.
 * It allows exporting and importing bases with the opportunity of only recording the changes since last export.
 *
 * @class SimplexState
 */
class SimplexState {
public:

    enum SIMPLEXSTATE_TYPE
    {
        SIMPLEXSTATE_FULL,
        SIMPLEXSTATE_DIFF,
        SIMPLEXSTATE_NULL
    };

    SimplexState();
    SimplexState(std::vector<int>* basisHead, IndexList<const Numerical::Double*>* variableStates,
                 DenseVector* basicVariableValues, SimplexState* parent = nullptr);
    ~SimplexState();

    void setSimplexState(std::vector<int>* basisHead, IndexList<const Numerical::Double*>* variableStates,
                         DenseVector* basicVariableValues, SimplexState* parent = nullptr);
    void setParent(SimplexState* parent = nullptr);
    void registerChild(SimplexState* child);
    void removeChild(SimplexState* child);

    inline SIMPLEXSTATE_TYPE getType() const { return m_type; }
    std::vector<int> getBasisHead() const;
    IndexList<const Numerical::Double*> getVariableStates() const;
    DenseVector getBasicVariableValues() const;

private:

    SIMPLEXSTATE_TYPE m_type;
    static SimplexState* sm_lastSimplexState;

    std::vector<int>* m_basisHead;
    IndexList<const Numerical::Double*>* m_variableStates;
    DenseVector* m_basicVariableValues;

    SimplexState* m_parent;
    SparseVector* m_basisHeadDiff;
    SparseVector* m_variableStatesDiff;
    SparseVector* m_basicVariableValuesDiff;
    std::list<SimplexState*>* m_children;
};

#endif // SIMPLEXSTATE_H
