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
 * @file simplexstate.h This file contains the API of the SimplexState class.
 * @author Peter Borocz
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
 * Objects of this class can be stored hierarchically, in parent-child correlations. Children objects contain
 * only the difference from their parent objects.
 *
 * @class SimplexState
 */
class SimplexState {
public:

    /**
     * Describes the type of the SimplexState object.
     * - SIMPLEXSTATE_FULL means that the object can explicitly describe a simplex state.
     * - SIMPLEXSTATE_DIFF means that the object stores difference vectors of the described simplex state. The simplex
     * state can be calculated with these difference vectors and the parent object.
     * - SIMPLEXSTATE_NULL means that the object does not contain any valid information about simplex state.
     */
    enum SIMPLEXSTATE_TYPE
    {
        SIMPLEXSTATE_FULL,
        SIMPLEXSTATE_DIFF,
        SIMPLEXSTATE_NULL
    };

    /**
     * Default constructor of the SimplexState class.
     *
     * @constructor
     */
    SimplexState();

    /**
     * Constructor of the SimplexState class. Also sets the represented simplex state.
     *
     * @param basisHead The basis head of the simplex state.
     * @param variableStates The variable states of the simplex state.
     * @param basicVariableValues The values of the basic variables of the simplex state.
     * @param parent The parent object of the simplex state. By default it is a null pointer. If a null pointer is
     * specified, the parent object will be the last full object created if applicable.
     * @constructor
     */
    SimplexState(const std::vector<int>& basisHead, const IndexList<const Numerical::Double*>& variableStates,
                 const DenseVector& basicVariableValues, SimplexState* parent = nullptr);

    /**
     * Copy constructor of the SimplexState class.
     *
     * @param other The SimplexState object to be copied.
     * @constructor
     */
    SimplexState(const SimplexState& other);

    /**
     * Destructor of the SimplexState class. The object removes itself from its parent if available.
     * If it has children, the first of them will be converted to full state and will be the new parent
     * object of the others.
     *
     * @destructor
     */
    ~SimplexState();

    /**
     * Assignment operator of the SimplexState class.
     *
     * @param other The SimplexState object to be assigned.
     * @return Reference of the this.
     */
    SimplexState& operator=(const SimplexState& other);

    /**
     * Sets the represented simplex state data.
     *
     * @param basisHead The basis head of the simplex state.
     * @param variableStates The variable states of the simplex state.
     * @param basicVariableValues The values of the basic variables of the simplex state.
     * @param parent The parent object of the simplex state. By default it is a null pointer. If a null pointer is
     * specified, the parent object will be the last full object created if applicable.
     */
    void setSimplexState(const std::vector<int>& basisHead, const IndexList<const Numerical::Double*>& variableStates,
                         const DenseVector& basicVariableValues, SimplexState* parent = nullptr);

    /**
     * Sets another SimplexState object as parent if applicable. If there is already a parent registered, this
     * function removes it first.
     *
     * @param parent The new parent of the SimplexState object.
     */
    void setParent(SimplexState* parent = nullptr);

    /**
     * Returns the type of the SimplexState object.
     *
     * @see SIMPLEXSTATE_TYPE
     * @return The type of the SimplexState object.
     */
    inline SIMPLEXSTATE_TYPE getType() const { return m_type; }

    /**
     * Returns the basis head of the represented simplex state.
     *
     * @return The basis head of the represented simplex state.
     */
    std::vector<int> getBasisHead() const;

    /**
     * Returns the variable states of the represented simplex state.
     *
     * @return The variable states of the represented simplex state.
     */
    IndexList<const Numerical::Double*> getVariableStates() const;

    /**
     * Returns the basis head of the represented simplex state.
     *
     * @return The basis head of the represented simplex state.
     */
    DenseVector getBasicVariableValues() const;

    /**
     * Prints the object data to the output.
     */
    void print() const;

private:

    /**
     * Registers another SimplexState object as child.
     *
     * @param child The new child of the SimplexState object.
     */
    void registerChild(SimplexState* child);

    /**
     * Removes a child of the SimplexState object.
     *
     * @param child The child to be removed.
     */
    void removeChild(SimplexState* child);

    /**
     * Sets the represented simplex state.
     *
     * @param basisHead The basis head of the simplex state.
     * @param variableStates The variable states of the simplex state.
     * @param basicVariableValues The values of the basic variables of the simplex state.
     * @param parent The parent object of the simplex state. By default it is a null pointer. If a null pointer is
     * specified, the parent object will be the last full object created if applicable.
     */
    void init(const std::vector<int>& basisHead, const IndexList<const Numerical::Double*>& variableStates,
              const DenseVector& basicVariableValues, SimplexState* parent = nullptr);

    /**
     * Copies the SimplexState object.
     *
     * @param other The SimplexState object to be copied.
     */
    void copy(const SimplexState& other);

    /**
     * Clears the SimplexState object.
     */
    void clear();

    /**
     * Contains the type of the SimplexState object.
     */
    SIMPLEXSTATE_TYPE m_type;

    /**
     * Stores a pointer to the lastly exported full simplex state.
     */
    static SimplexState* sm_lastSimplexState;

    /**
     * Stores the basis head of the represented full simplex state.
     */
    std::vector<int>* m_basisHead;

    /**
     * Stores the variable states of the represented full simplex state.
     */
    IndexList<const Numerical::Double*>* m_variableStates;

    /**
     * Stores the values of the basic variables of the represented full simplex state.
     */
    DenseVector* m_basicVariableValues;

    /**
     * Stores the parent of the SimplexState object if this is a child.
     */
    SimplexState* m_parent;

    /**
     * Stores the basis head difference from the parent simplex state if this is a child.
     */
    SparseVector* m_basisHeadDiff;

    /**
     * Stores the variable states difference from the parent simplex state if this is a child.
     */
    SparseVector* m_variableStatesDiff;

    /**
     * Stores the basic variable values difference from the parent simplex state if this is a child.
     */
    SparseVector* m_basicVariableValuesDiff;

    /**
     * Stores the children of the SimplexState object.
     */
    std::list<SimplexState*>* m_children;
};

#endif // SIMPLEXSTATE_H
