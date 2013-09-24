/**
 * @file matrixbuilder.h
 */

#ifndef MATRIXBUILDER_H
#define	MATRIXBUILDER_H

#include <globals.h>

#include <linalg/vector.h>

class MatrixBuilder
{
public:
    /**
     * 
     * @return 
     */
    virtual unsigned int getColumnCount() const = 0;

    /**
     * 
     * @return 
     */
    virtual unsigned int getRowCount() const = 0;

    /**
     * 
     * @param index
     * @param rowVector
     */
    virtual void buildRow(unsigned int index, Vector * rowVector,
        std::vector<unsigned int> * nonzeros) const = 0;

    /**
     * 
     * @param index
     * @param columnVector
     */
    virtual void buildColumn(unsigned int index, Vector * columnVector,
        std::vector<unsigned int> * nonzeros) const = 0;

    /**
     * 
     * @return 
     */
    virtual bool hasRowwiseRepresentation() const = 0;
    
    /**
     * 
     * @return 
     */
    virtual bool hasColumnwiseRepresentation() const = 0;
};

#endif	/* MATRIXBUILDER_H */

