/* 
 * File:   matrixbuilder.h
 * Author: smidla
 *
 * Created on 2013. march 6., 13:22
 */

#ifndef MATRIXBUILDER_H
#define	MATRIXBUILDER_H

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
    virtual void buildRow(unsigned int index, Vector * rowVector) const = 0;

    /**
     * 
     * @param index
     * @param columnVector
     */
    virtual void buildColumn(unsigned int index, Vector * columnVector) const = 0;
    
};

#endif	/* MATRIXBUILDER_H */

