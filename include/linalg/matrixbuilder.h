/**
 * @file matrixbuilder.h This file contains the API of the MatrixBuilder pure virtual class.
 * @author smidla
 */

#ifndef MATRIXBUILDER_H
#define	MATRIXBUILDER_H

#include <globals.h>

#include <linalg/vector.h>

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
     */
    virtual void buildRow(unsigned int index, Vector * rowVector,
        std::vector<unsigned int> * nonzeros) const = 0;

    /**
     * Builds a column of the matrix and stores it is a Vector.
     * 
     * @param index The index of the column to be built.
     * @param columnVector Pointer to the vector the column is stored in.
     */
    virtual void buildColumn(unsigned int index, Vector * columnVector,
        std::vector<unsigned int> * nonzeros) const = 0;

    /**
     * Returns true if the matrix is stored row-wise.
     *
     * @return True if the matrix is stored row-wise.
     */
    virtual bool hasRowwiseRepresentation() const = 0;
    
    /**
     * Returns true if the matrix is stored column-wise.
     *
     * @return True if the matrix is stored column-wise.
     */
    virtual bool hasColumnwiseRepresentation() const = 0;
};

#endif	/* MATRIXBUILDER_H */

