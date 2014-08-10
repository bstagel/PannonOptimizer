#include <linalg/lpmatrix.h>

void LPMatrix::addColumn(const Numerical::Double * data,
                         const unsigned int * indices,
                         unsigned int count) {
    Vector newVector;
    newVector.m_data = new Numerical::Double[count];
    newVector.m_index = new unsigned int[count];
    newVector.m_transposedIndex = new unsigned int[count];
    newVector.m_count = count;
    unsigned int index;
    for (index = 0; index < count; index++) {
        newVector.m_data[index] = data[index];
        newVector.m_index[index] = indices[index];
        if (m_rowCounter.size() < indices[index]) {
            m_rowCounter.resize(indices[index] + 1);
        }
        newVector.m_transposedIndex[index] = newVector.m_transposedIndex[index];
        m_rowCounter[indices[index]]++;
    }
    m_columns.push_back(newVector);
}

void LPMatrix::finalize() {
    unsigned int rowIndex;
    unsigned int columnIndex;

    unsigned int rowCount = m_rowCounter.size();
    unsigned int columnCount = m_columns.size();
    for (rowIndex = 0; rowIndex < rowCount; rowIndex++) {
        Vector newVector;
        unsigned int count = m_rowCounter[rowIndex];
        newVector.m_data = new Numerical::Double[ count ];
        newVector.m_index = new unsigned int[ count ];
        newVector.m_transposedIndex = new unsigned int[ count ];
        newVector.m_count = 0;
        m_rows.push_back(newVector);
    }
    for (columnIndex = 0; columnIndex < columnCount; columnIndex++) {
        unsigned int index;
        for (index = 0; index < m_columns[columnIndex].m_count; index++) {
            unsigned int rowIndex = m_columns[columnIndex].m_index[index];
            Vector & row = m_rows[rowIndex];
            row.m_data[ row.m_count ] = m_columns[columnIndex].m_data[index];
            row.m_index[ row.m_count ] = columnIndex;
            row.m_transposedIndex[ row.m_count ] = index;
        }
    }
}

void LPMatrix::removeColumn(unsigned int columnIndex) {
    Vector & column = m_columns[columnIndex];
    unsigned int index;
    for (index = 0; index < column.m_count; index++) {
        Vector & row = m_rows[ column.m_index[index] ];
        unsigned int transposedIndex = column.m_transposedIndex[index];
        row.m_data[ transposedIndex ] = row.m_data[ row.m_count - 1 ];
        row.m_index[ transposedIndex ] = row.m_index[ row.m_count - 1 ];
        row.m_transposedIndex[ transposedIndex ] = row.m_transposedIndex[ row.m_count - 1 ];

        //
        unsigned int transposedIndexColumn = row.m_transposedIndex[ row.m_count - 1 ]; // other column
        unsigned int otherColumn = row.m_index[ row.m_count - 1 ];
        Vector & column = m_columns[otherColumn];
        column.m_transposedIndex[ transposedIndexColumn ] = transposedIndex;
        row.m_count--;
    }
    m_columns.erase( m_columns.begin() + columnIndex );
}

void LPMatrix::removeRow(unsigned int rowIndex) {
    Vector & row = m_rows[rowIndex];
    unsigned int index;
    for (index = 0; index < row.m_count; index++) {
        Vector & column = m_columns[ row.m_index[index] ];
        unsigned int transposedIndex = row.m_transposedIndex[index];
        column.m_data[ transposedIndex ] = column.m_data[ column.m_count - 1 ];
        column.m_index[ transposedIndex ] = column.m_index[ column.m_count - 1 ];
        column.m_transposedIndex[ transposedIndex ] = column.m_transposedIndex[ column.m_count - 1 ];

        //
        unsigned int transposedIndexRow = column.m_transposedIndex[ column.m_count - 1 ]; // other column
        unsigned int otherRow = column.m_index[ column.m_count - 1 ];
        Vector & row = m_rows[otherRow];
        row.m_transposedIndex[ transposedIndexRow ] = transposedIndex;
        column.m_count--;
    }
    m_rows.erase( m_rows.begin() + rowIndex );
}

