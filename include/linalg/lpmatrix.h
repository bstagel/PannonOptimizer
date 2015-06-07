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

#ifndef LPMATRIX_H
#define LPMATRIX_H

#include <globals.h>
#include <utils/numerical.h>
#include <vector>

class LPMatrix {
public:
    LPMatrix();

    LPMatrix(const LPMatrix & orig);

    LPMatrix & operator=(const LPMatrix & orig);

    ~LPMatrix();

    void addColumn(const double * data,
                   const unsigned int * indices,
                   unsigned int count);

    void finalize();

    void removeColumn(unsigned int columnIndex);

    void removeRow(unsigned int rowIndex);

private:

    struct Column {
        double * m_dataStart;
        unsigned int * m_indexStart;
    };

    // row structure:
    // first number: number of nonzeros in the current block
    //

    struct RowBlock {

    };

    struct Vector {
        double * m_data;
        unsigned int * m_index;
        unsigned int * m_transposedIndex;
        unsigned int m_count;
    };

//    unsigned int m_rowCount;

//    unsigned int m_columnCount;

    //std::vector<Column> m_columns;

    std::vector<Vector> m_columns;

    std::vector<Vector> m_rows;

    std::vector<unsigned int> m_rowCounter;

};

#endif // LPMATRIX_H
