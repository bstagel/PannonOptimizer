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

    void addColumn(const Numerical::Double * data,
                   const unsigned int * indices,
                   unsigned int count);

    void finalize();

    void removeColumn(unsigned int columnIndex);

    void removeRow(unsigned int rowIndex);

private:

    struct Column {
        Numerical::Double * m_dataStart;
        unsigned int * m_indexStart;
    };

    // row structure:
    // first number: number of nonzeros in the current block
    //

    struct RowBlock {

    };

    struct Vector {
        Numerical::Double * m_data;
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
