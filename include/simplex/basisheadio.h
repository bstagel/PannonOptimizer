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
 * @file basisheadio.h This file contains the BasisHeadIO class.
 */
#ifndef BASISHEADIO_H
#define BASISHEADIO_H

#include <globals.h>
#include <simplex/simplex.h>
#include <lp/variable.h>
#include <lp/constraint.h>
#include <lp/panoptstate.h>
#include <utils/indexlist.h>
#include <vector>

/**
 * This abstract class describes a general basishead input/output unit.
 * Derived classes from this are able to write and read the basis into / from a file.
 *
 * @class BasisHeadIO
 */
class BasisHeadIO: public PanOptState {
public:

    /**
     * Default constructor of the class.
     *
     * @constructor
     */
    BasisHeadIO():m_basisHeadPtr(0), m_variableStatesPtr(0) {}

    /**
     * Default destructor of the class.
     *
     * @destructor
     */
    virtual ~BasisHeadIO() {}

    /**
     * Pure virtual function, adding a new basic variable to the input/output unit.
     *
     * @param variable Pointer to the variable to be added.
     * @param basisPosition The basis position on which the variable will be added.
     * @param variableIndex The index of the variable to be added.
     * @param value The current value of the variable to be added.
     */
    virtual void addBasicVariable(const Variable * variable,
                                  unsigned int basisPosition,
                                  unsigned int variableIndex,
                                  Numerical::Double value) = 0;

    /**
     * Pure virtual function, adding a new nonbasic variable to the input/output unit.
     *
     * @param variable Pointer to the variable to be added.
     * @param variableIndex The index of the variable to be added.
     * @param state Describes whether the nonbasic variable is on its lower or upper bound.
     * @param value The current value of the variable to be added.
     */
    virtual void addNonbasicVariable(const Variable * variable,
                                     unsigned int variableIndex,
                                     Simplex::VARIABLE_STATE state,
                                     Numerical::Double value) = 0;

    /**
     * Adds a specific variable to the input/output unit.
     *
     * @param variable The variable to be added.
     */
    virtual void addVariable(const Variable & variable) = 0;

    /**
     * Setter for the basis head pointer.
     *
     * @param basisHead Pointer to the basis head to be set.
     */
    void setBasisHead(std::vector<int> * basisHead) {
        m_basisHeadPtr = basisHead;
    }

    /**
     * Setter for the list containing the variable states.
     *
     * @param variableStates Pointer to the indexlist to be set.
     */
    void setVariableStateList(IndexList<const Numerical::Double*> * variableStates) {
        m_variableStatesPtr = variableStates;
    }

protected:

    /**
     * Pointer to the basis head vector (containing variable indices of basic variables)
     */
    std::vector<int> * m_basisHeadPtr;

    /**
     * Pointer of indexlist containing the variable states.
     */
    IndexList<const Numerical::Double*> * m_variableStatesPtr;
};

#endif // BASISHEADIO_H
