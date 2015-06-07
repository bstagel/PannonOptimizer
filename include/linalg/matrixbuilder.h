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
 * @file matrixbuilder.h This file contains the API of the MatrixBuilder pure virtual class.
 * @author Jozsef Smidla
 */

#ifndef MATRIXBUILDER_H
#define	MATRIXBUILDER_H

#include <globals.h>

#include <linalg/sparsevector.h>

/**
 * This pure virtual class is able to build a Matrix.
 *
 * @class MatrixBuilder
 */
class MatrixBuilder
{
public:

    /**
     * Returns the number of columns in the matrix.
     *
     * @return The number of columns in the matrix.
     */
    virtual unsigned int getColumnCount() const = 0;

    /**
     * Returns the number of rows in the matrix.
     *
     * @return The number of rows in the matrix.
     */
    virtual unsigned int getRowCount() const = 0;

    /**
     * Builds a row of the matrix and stores it in a Vector.
     * 
     * @param index The index of the row to be built.
     * @param rowVector Pointer to the vector the row is stored in.
     * @param nonzeros A vector of nonzeros to be stored.
     */
    virtual void buildRow(unsigned int index,
                          SparseVector * rowVector,
                          std::vector<unsigned int> * nonzeros) const = 0;

    /**
     * Builds a column of the matrix and stores it is a Vector.
     * 
     * @param index The index of the column to be built.
     * @param columnVector Pointer to the vector the column is stored in.
     * @param nonzeros A vector of nonzeros to be stored.
     */
    virtual void buildColumn(unsigned int index,
                             SparseVector * columnVector,
                             std::vector<unsigned int> * nonzeros) const = 0;

    /**
     * Returns true if the matrix has a rowwise representation.
     *
     * @return True if the matrix is stored rowwise.
     */
    virtual bool hasRowwiseRepresentation() const = 0;
    
    /**
     * Returns true if the matrix has a columnwise representation.
     *
     * @return True if the matrix is stored columnwise.
     */
    virtual bool hasColumnwiseRepresentation() const = 0;
};

#endif	/* MATRIXBUILDER_H */

