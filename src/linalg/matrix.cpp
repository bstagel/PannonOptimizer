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
 * @file matrix.cpp
 */

#include <debug.h>
#include <linalg/matrix.h>

#include <cassert>
#include <cstring>
#include <iostream>
#include <vector>

Matrix::Matrix(unsigned int rowCount, unsigned int columnCount)
{
    if (rowCount == 0 && columnCount == 0) {
        // uninitialised matrix
        m_isDiagonal = false;
        m_rowCount = 0;
        m_columnCount = 0;
        m_rowWise = 0;
        m_columnWise = 0;
        m_fastColumnScaling = false;
        return;
    }
    init(rowCount, columnCount == 0 ? rowCount : columnCount);
}

Matrix::Matrix(const Matrix & original)
{
    copy(original);
}

Matrix::~Matrix()
{
    clear();
}

void Matrix::clear()
{
    unsigned int index;
    if (m_rowWise) {
        for (index = 0; index < m_rowCount; index++) {
            delete m_rowWise[index];
        }
        delete [] m_rowWise;
        m_rowWise = 0;
    }
    if (m_columnWise) {
        for (index = 0; index < m_columnCount; index++) {
            delete m_columnWise[index];
        }
        delete [] m_columnWise;
        m_columnWise = 0;
    }
    m_rowCount = 0;
    m_columnCount = 0;
    m_isDiagonal = false;
    m_fastColumnScaling = false;
}

void Matrix::build(const MatrixBuilder & builder)
{
    reInit(builder.getRowCount(), builder.getColumnCount());
    unsigned int rowIndex;
    unsigned int columnIndex;
    std::vector<unsigned int> nonzerosInRows;
    std::vector<unsigned int> nonzerosInColumns;
    std::vector<unsigned int> * nonzerosInRowsPtr = 0;
    std::vector<unsigned int> * nonzerosInColumnsPtr = 0;
    bool rowToColumn = builder.hasRowwiseRepresentation() &&
        !builder.hasColumnwiseRepresentation();
    bool columnToRow = !builder.hasRowwiseRepresentation() &&
        builder.hasColumnwiseRepresentation();
    if (rowToColumn == true) {
        nonzerosInColumns.resize(builder.getColumnCount(), 0);
        nonzerosInColumnsPtr = &nonzerosInColumns;
    }
    if (columnToRow == true) {
        nonzerosInRows.resize(builder.getRowCount(), 0);
        nonzerosInRowsPtr = &nonzerosInRows;
    }
    if (builder.hasRowwiseRepresentation() == true) {
        for (rowIndex = 0; rowIndex < builder.getRowCount(); rowIndex++) {
            builder.buildRow(rowIndex, m_rowWise[rowIndex], nonzerosInColumnsPtr);
        }
    }
    if (builder.hasColumnwiseRepresentation() == true) {
        for (columnIndex = 0; columnIndex < builder.getColumnCount(); columnIndex++) {
            builder.buildColumn(columnIndex, m_columnWise[columnIndex], nonzerosInRowsPtr);
        }
    }
    if (builder.hasRowwiseRepresentation() == false) {
        for (rowIndex = 0; rowIndex < builder.getRowCount(); rowIndex++) {
            SparseVector * row = m_rowWise[rowIndex];
            row->prepareForData(nonzerosInRows[rowIndex], builder.getColumnCount());
        }
        for (columnIndex = 0; columnIndex < builder.getColumnCount(); columnIndex++) {
            SparseVector::NonzeroIterator iter = m_columnWise[columnIndex]->beginNonzero();
            SparseVector::NonzeroIterator iterEnd = m_columnWise[columnIndex]->endNonzero();
            for (; iter < iterEnd; ++iter) {
                m_rowWise[ iter.getIndex() ]->newNonZero(*iter, columnIndex);
            }
        }
    }
    if (builder.hasColumnwiseRepresentation() == false) {
        for (columnIndex = 0; columnIndex < builder.getColumnCount(); columnIndex++) {
            SparseVector * column = m_columnWise[columnIndex];
            column->prepareForData(nonzerosInColumns[columnIndex], builder.getRowCount());
        }
        for (rowIndex = 0; rowIndex < builder.getRowCount(); rowIndex++) {
            SparseVector::NonzeroIterator iter = m_rowWise[rowIndex]->beginNonzero();
            SparseVector::NonzeroIterator iterEnd = m_rowWise[rowIndex]->endNonzero();
            for (; iter < iterEnd; ++iter) {
                m_columnWise[ iter.getIndex() ]->newNonZero(*iter, rowIndex);
            }
        }
    }
}

void Matrix::resizeVectors(SparseVector **&vectors, unsigned int vectorCount,
    unsigned int newCount, unsigned int newLengths)
{
    unsigned int idx;

    if (vectorCount == newCount) {
        for (idx = 0; idx < vectorCount; idx++) {
            vectors[idx]->resize(newLengths);
        }
        return;
    }

    if (vectorCount < newCount) {
        SparseVector ** newVectors = new SparseVector*[newCount];
        panOptMemcpy(newVectors, vectors, sizeof (SparseVector*) * vectorCount);
        for (idx = 0; idx < vectorCount; idx++) {
            newVectors[idx]->resize(newLengths);
        }
        for (idx = vectorCount; idx < newCount; idx++) {
            newVectors[idx] = new SparseVector(newLengths);
        }
        delete [] vectors;
        vectors = newVectors;
    } else {
        SparseVector ** newVectors = new SparseVector*[newCount];
        panOptMemcpy(newVectors, vectors, sizeof (SparseVector*) * newCount);
        for (idx = 0; idx < newCount; idx++) {
            newVectors[idx]->resize(newLengths);
        }
        for (idx = newCount; idx < vectorCount; idx++) {
            delete vectors[idx];
        }
        delete [] vectors;
        vectors = newVectors;
    }

}

void Matrix::resize(unsigned int rowCount, unsigned int columnCount)
{

    resizeVectors(m_rowWise, m_rowCount, rowCount, columnCount);
    resizeVectors(m_columnWise, m_columnCount, columnCount, rowCount);

    m_rowCount = rowCount;
    m_columnCount = columnCount;
}

void Matrix::reInit(unsigned int rowCount, unsigned int columnCount)
{
    clear();
    init(rowCount, columnCount);
}

Matrix & Matrix::operator=(const Matrix & original)
{
    if (this == &original) {
        return *this;
    }
    clear();
    copy(original);
    return *this;
}

bool Matrix::operator==(const Matrix& other) const
{
    if (m_rowCount != other.m_rowCount ||
        m_columnCount != other.m_columnCount) {
        return false;
    }

    for (unsigned int index = 0; index < m_rowCount; index++) {
        if (*(m_rowWise[index]) != *(other.m_rowWise[index])) {
            return false;
        }
    }
    return true;
}

unsigned int Matrix::rowCount() const
{
    return m_rowCount;
}

unsigned int Matrix::columnCount() const
{
    return m_columnCount;
}

const SparseVector & Matrix::row(unsigned int index) const
{
    return *(m_rowWise[index]);
}

const SparseVector & Matrix::column(unsigned int index) const
{
    return *m_columnWise[index];
}

Matrix & Matrix::transpose()
{
    m_fastColumnScaling = false;
    unsigned int tempSize = m_columnCount;
    m_columnCount = m_rowCount;
    m_rowCount = tempSize;

    SparseVector ** temp = m_rowWise;
    m_rowWise = m_columnWise;
    m_columnWise = temp;
    return *this;
}

Matrix Matrix::transposed() const
{
    Matrix transposed(*this);
    transposed.transpose();

    return transposed;
}

void Matrix::removeVector(SparseVector ** & columnWise, unsigned int & columnCount,
    SparseVector ** & rowWise, unsigned int & rowCount, unsigned int index)
{
    m_fastColumnScaling = false;
    // minden oszlop index-edik sorabol el kell tavolitani egy elemet
    SparseVector ** vector1 = columnWise;
    SparseVector ** vector2 = columnWise + columnCount;

    while (vector1 < vector2) {
        (*vector1)->remove(index);
        vector1++;
    }
    delete rowWise[index];
    rowWise[index] = 0;

    // el kell tavolitani az index-edik sort
    // majd ezt lehet gyorsitani itt is egy elbowroom-ozassal, de egyelore marad igy
    SparseVector ** temp = new SparseVector*[ rowCount - 1 ];
    vector1 = rowWise;
    vector2 = temp;
    unsigned int index2 = 0;
    while (index2 < rowCount) {
        //if (index2 != index) {
        if (*vector1 != 0) {
            *vector2 = *vector1;
            vector2++;
        }
        vector1++;
        index2++;
    }
    delete [] rowWise;
    rowWise = temp;
    rowCount--;
}

void Matrix::removeRow(unsigned int index)
{
    if (m_rowCount == 1) {
        clear();
    } else {
        removeVector(m_columnWise, m_columnCount, m_rowWise, m_rowCount, index);
    }
}

void Matrix::removeColumn(unsigned int index)
{
    if (m_columnCount == 1) {
        clear();
    } else {
        removeVector(m_rowWise, m_rowCount, m_columnWise, m_columnCount, index);
    }
}

void Matrix::insertVector(SparseVector ** columnWise, SparseVector ** & rowWise,
    unsigned int & rowCount, unsigned int index, const SparseVector & vector)
{
    m_fastColumnScaling = false;
    // oszlopokba is be kell szurni
    std::vector<char> inserted(vector.length(), 0);
    SparseVector::NonzeroIterator it = vector.beginNonzero();
    SparseVector::NonzeroIterator itEnd = vector.endNonzero();
    for (; it < itEnd; ++it) {
        columnWise[ it.getIndex() ]->insert(index, *it);
        inserted[ it.getIndex() ] = true;
    }
    std::vector<char>::const_iterator insertIter = inserted.begin();
    const std::vector<char>::const_iterator insertIterEnd = inserted.end();
    unsigned int insertIndex = 0;
    for (; insertIter < insertIterEnd; ++insertIter, ++insertIndex) {
        if (*insertIter == false) {
            columnWise[ insertIndex ]->insert(index, 0.0);
        }
    }

    SparseVector * temp = new SparseVector(vector);
    SparseVector ** vectors = new SparseVector*[ rowCount + 1 ];
    // index elottieket
    if (index > 0) {
        panOptMemcpy(vectors, rowWise, index * sizeof (SparseVector*));
    }
    vectors[index] = temp;
    if (index < rowCount) {
        panOptMemcpy(vectors + index + 1, rowWise + index, sizeof (SparseVector*) * (rowCount - index));
    }
    delete [] rowWise;
    rowWise = vectors;
    rowCount++;
}

void Matrix::insertEmptyVector(SparseVector ** columnWise, SparseVector ** & rowWise,
    unsigned int & rowCount, unsigned int columnCount, unsigned int index)
{
    m_fastColumnScaling = false;
    // oszlopokba is be kell szurni
    unsigned int insertIndex = 0;
    for (insertIndex = 0; insertIndex < columnCount; insertIndex++) {
        columnWise[ insertIndex ]->insert(index, 0.0);
    }

    SparseVector * temp = new SparseVector(columnCount);
    SparseVector ** vectors = new SparseVector*[ rowCount + 1 ];
    // index elottieket
    if (index > 0) {
        panOptMemcpy(vectors, rowWise, index * sizeof (SparseVector*));
    }
    vectors[index] = temp;
    if (index < rowCount) {
        panOptMemcpy(vectors + index + 1, rowWise + index, sizeof (SparseVector*) * (rowCount - index));
    }
    //0 | 1 | 2
    //0 | * | 1 | 2
    delete [] rowWise;
    rowWise = vectors;
    rowCount++;
}

void Matrix::insertEmptyRow(unsigned int index)
{
    insertEmptyVector(m_columnWise, m_rowWise, m_rowCount, m_columnCount, index);
}

void Matrix::insertEmptyColumn(unsigned int index)
{
    insertEmptyVector(m_rowWise, m_columnWise, m_columnCount, m_rowCount, index);
}

void Matrix::insertRow(unsigned int index, const SparseVector & vector)
{
    if (m_rowCount == 0) {
        this->init(0, vector.length());
    }
    insertVector(m_columnWise, m_rowWise, m_rowCount, index, vector);

}

void Matrix::insertColumn(unsigned int index, const SparseVector &vector)
{
    if (m_columnCount == 0) {
        this->init(vector.length(), 0);
    }
    insertVector(m_rowWise, m_columnWise, m_columnCount, index, vector);
}

void Matrix::appendRow(const SparseVector & vector)
{
    if (m_rowCount == 0) {
        this->reInit(0, vector.length());
    }
    insertVector(m_columnWise, m_rowWise, m_rowCount, m_rowCount, vector);
}

void Matrix::appendColumn(const SparseVector &vector)
{
    if (m_columnCount == 0) {
        reInit(vector.length(), 0);
    }
    insertVector(m_rowWise, m_columnWise, m_columnCount, m_columnCount, vector);
    //insertColumn(m_columnCount, vector);
}

void Matrix::prependRow(const SparseVector & vector)
{
    if (m_rowCount == 0) {
        this->init(0, vector.length());
    }
    insertRow(0, vector);
}

void Matrix::prependColumn(const SparseVector &vector)
{
    if (m_columnCount == 0) {
        this->init(vector.length(), 0);
    }
    insertColumn(0, vector);
}

void Matrix::setNewNonzero(unsigned int rowindex, unsigned int columnindex, Numerical::Double value)
{
    m_fastColumnScaling = false;
    m_rowWise[rowindex]->setNewNonzero(columnindex, value);
    m_columnWise[columnindex]->setNewNonzero(rowindex, value);
}

void Matrix::set(unsigned int rowindex, unsigned int columnindex, Numerical::Double value)
{
    m_fastColumnScaling = false;
    m_rowWise[rowindex]->set(columnindex, value);
    m_columnWise[columnindex]->set(rowindex, value);
}

Numerical::Double Matrix::get(unsigned int rowindex, unsigned int columnindex) const
{
    return m_rowWise[rowindex]->at(columnindex);
    // TODO: azt hasznalja, amelyik dense
}

void Matrix::scale(Numerical::Double lambda)
{
    m_fastColumnScaling = false;
    SparseVector ** vector1 = m_columnWise;
    SparseVector ** vector2 = m_columnWise + m_columnCount;
    while (vector1 < vector2) {
        (*vector1)->scale(lambda);
        vector1++;
    }
    vector1 = m_rowWise;
    vector2 = m_rowWise + m_rowCount;
    while (vector1 < vector2) {
        (*vector1)->scale(lambda);
        vector1++;
    }
}

void Matrix::startColumnScaling()
{
    m_fastRowScaling = true;
    m_multipliers.resize(m_columnCount, 1.0);
}

void Matrix::startRowScaling()
{
    m_fastRowScaling = true;
    m_multipliers.resize(m_rowCount, 1.0);
}

void Matrix::finishColumnScaling()
{
    SparseVector ** vector1 = m_rowWise;
    SparseVector ** vector2 = m_rowWise + m_rowCount;
    while (vector1 < vector2) {
        (*vector1)->scaleByLambdas(m_multipliers);
        vector1++;
    }
    m_fastColumnScaling = false;
}

void Matrix::finishRowScaling()
{
    SparseVector ** vector1 = m_columnWise;
    SparseVector ** vector2 = m_columnWise + m_columnCount;
    while (vector1 < vector2) {
        (*vector1)->scaleByLambdas(m_multipliers);
        vector1++;
    }
    m_fastRowScaling = false;
}

void Matrix::scaleRow(unsigned int index, Numerical::Double lambda)
{
    m_rowWise[index]->scale(lambda);
    if (m_fastRowScaling == true) {
        m_multipliers[index] = lambda;
    } else {
        SparseVector ** vector1 = m_columnWise;
        SparseVector ** vector2 = m_columnWise + m_columnCount;
        while (vector1 < vector2) {
            (*vector1)->scaleElementBy(index, lambda);
            vector1++;
        }
    }
}

void Matrix::scaleColumn(unsigned int index, Numerical::Double lambda)
{
    m_columnWise[index]->scale(lambda);
    if (m_fastColumnScaling == true) {
        if (index >= m_multipliers.size()) {
            LPERROR("hiba\n");
        }
        m_multipliers[index] = lambda;
    } else {
        SparseVector ** vector1 = m_rowWise;
        SparseVector ** vector2 = m_rowWise + m_rowCount;
        while (vector1 < vector2) {
            (*vector1)->scaleElementBy(index, lambda);
            vector1++;
        }
    }
}

void Matrix::scaleOnlyRowwise(unsigned int index, Numerical::Double lambda) {
    m_rowWise[index]->scale(lambda);
}

void Matrix::scaleOnlyColumnwise(unsigned int index, Numerical::Double lambda) {
    m_columnWise[index]->scale(lambda);
}

void Matrix::scaleOnlyRowwiseLambdas(unsigned int index, const std::vector<Numerical::Double> & lambdas) {
    m_rowWise[index]->scaleByLambdas(lambdas);
}

void Matrix::scaleOnlyColumnwiseLambdas(unsigned int index, const std::vector<Numerical::Double> & lambdas) {
    m_columnWise[index]->scaleByLambdas(lambdas);
}

void Matrix::invert()
{
    if (m_isDiagonal == true) {
        unsigned int i;
        for (i = 0; i < m_rowCount; i++) {
            m_rowWise[i]->set(i, 1.0 / m_rowWise[i]->at(i));
            m_columnWise[i]->set(i, 1.0 / m_columnWise[i]->at(i));
        }
    } else {
        Matrix copyMatrix = *this;
        unsigned int i;
        SparseVector e(m_rowCount);
        for (i = 0; i < m_rowCount; i++) {
            e.set(i, 1.0);
            copyMatrix.appendColumn(e);
            e.set(i, 0.0);
        }
        copyMatrix.gaussianElimination(true);
        for (i = 0; i < m_columnCount; i++) {
            m_columnWise[i]->clear();
        }
        for (i = 0; i < m_rowCount; i++) {
            m_rowWise[i]->prepareForData(copyMatrix.m_rowWise[i]->nonZeros() - 1, m_rowCount);
            SparseVector::NonzeroIterator iter = copyMatrix.m_rowWise[i]->beginNonzero();
            SparseVector::NonzeroIterator iterEnd = copyMatrix.m_rowWise[i]->endNonzero();
            for (; iter != iterEnd; ++iter) {
                if (iter.getIndex() >= m_rowCount) {
                    unsigned int columnIndex = iter.getIndex() - m_rowCount;
                    m_rowWise[i]->newNonZero(*iter, columnIndex);
                    m_columnWise[columnIndex]->set(i, *iter);
                }
            }
        }
    }
}

Matrix Matrix::inverse() const
{
    if (m_isDiagonal == true) {
        Matrix inverted(m_rowCount, m_columnCount);
        unsigned int i;
        for (i = 0; i < m_rowCount; i++) {
            inverted.m_rowWise[i]->set(i, 1.0 / m_rowWise[i]->at(i));
            inverted.m_columnWise[i]->set(i, 1.0 / m_columnWise[i]->at(i));
        }
        inverted.m_isDiagonal = true;
        return inverted;
    } else {
        Matrix inverted = *this;
        inverted.invert();
        return inverted;
    }
}

Numerical::Double Matrix::determinant() const
{
    if (rowCount() != columnCount()) {
        //LPWARNING("Determinant cannot be calculated as Matrix is not squared. "
        //        << "(row count: " << rowCount() << ", column count: " << columnCount());
        return 0.0;
    }

    return determinant(*this);
}

Numerical::Double Matrix::determinant(const Matrix& matrix) const
{
    /* Preconditions */
    /* Assert that given matrix is a square matrix */
    assert(matrix.rowCount() == matrix.columnCount());

    /* DEBUG */
    //matrix.show();

    if (matrix.rowCount() == 1) {
        return matrix.get(0, 0);
    }

    Matrix u = matrix;
    int swapCount = u.gaussianElimination();
    if (swapCount == -1) {
        return 0.0;
    }
    Numerical::Double result = 1;
    unsigned int i;
    for (i = 0; i < matrix.m_rowCount; i++) {
        Numerical::Double value = u.get(i, i);
        result *= value;
    }
    if (swapCount % 2 == 1) {
        result = -result;
    }
    return result;
}

void Matrix::show() const
{
    unsigned int i, j;
    std::cout << "row wise: " << std::endl;
    for (i = 0; i < m_rowCount; i++) {
        for (j = 0; j < m_columnCount; j++) {
            std::cout.width(10);
            std::cout << m_rowWise[i]->at(j);
        }
        std::cout << std::endl;
    }
    std::cout << "column wise: " << std::endl;
    for (i = 0; i < m_rowCount; i++) {
        for (j = 0; j < m_columnCount; j++) {
            std::cout.width(10);
            std::cout << m_columnWise[j]->at(i);
        }
        std::cout << std::endl;
    }
}

unsigned int Matrix::nonZeros() const
{
    unsigned int nonZeros = 0;

    for (unsigned int i = 0; i < m_rowCount; i++) {
        nonZeros += m_rowWise[i]->nonZeros();
    }
    return nonZeros;
}

Numerical::Double Matrix::density() const
{
    return (Numerical::Double) nonZeros() / (Numerical::Double) (m_rowCount * m_columnCount);
}

DenseVector Matrix::operator *(const DenseVector& rightVector) const
{
    DenseVector result(this->rowCount());

    DenseVector::NonzeroIterator iter = rightVector.beginNonzero();
    DenseVector::NonzeroIterator iterEnd = rightVector.endNonzero();
    for (; iter < iterEnd; ++iter) {
        result.addVector(*iter, *(m_columnWise[ iter.getIndex() ]));
    }

    return result;
}

void Matrix::copy(const Matrix & matrix)
{
    if (this == &matrix) {
        return;
    }
    unsigned int index;
    m_rowCount = matrix.m_rowCount;
    m_columnCount = matrix.m_columnCount;
    m_isDiagonal = matrix.m_isDiagonal;
    m_rowWise = 0;
    m_columnWise = 0;

    m_fastRowScaling = matrix.m_fastRowScaling;
    m_multipliers = matrix.m_multipliers;
    m_rowWise = new SparseVector*[ m_rowCount ];
    for (index = 0; index < m_rowCount; index++) {
        m_rowWise[index] = new SparseVector(*matrix.m_rowWise[index]);
    }
    m_columnWise = new SparseVector*[ m_columnCount ];
    for (index = 0; index < m_columnCount; index++) {
        m_columnWise[index] = new SparseVector(*matrix.m_columnWise[index]);
    }
}

void Matrix::init(unsigned int rowCount, unsigned int columnCount)
{
    unsigned int index;
    m_isDiagonal = false;
    m_rowCount = rowCount;
    m_columnCount = columnCount;
    m_rowWise = 0;
    m_columnWise = 0;
    m_fastRowScaling = false;
    m_fastColumnScaling = false;
    if (rowCount > 0) {
        m_rowWise = new SparseVector*[ rowCount ];
    } else {
        m_rowWise = 0;
    }
    for (index = 0; index < m_rowCount; index++) {
//        if (initVectors) {
            m_rowWise[index] = new SparseVector(m_columnCount);
//        } else {
//            m_rowWise[index] = new SparseVector(NULL, NULL, NULL);
//        }
    }
    if (m_columnCount > 0) {
        m_columnWise = new SparseVector*[ m_columnCount ];
    } else {
        m_columnWise = 0;
    }

    for (index = 0; index < m_columnCount; index++) {
//        if (initVectors) {
            m_columnWise[index] = new SparseVector(m_rowCount);
//        } else {
//            m_columnWise[index] = new SparseVector(NULL, NULL, NULL);
//        }
    }

}

Matrix Matrix::createDiagonalMatrix(const DenseVector &diagonal)
{
    unsigned int m = diagonal.length();
    Matrix matrix(m, m);
    matrix.m_isDiagonal = true;

    for (unsigned int i = 0; i < m; i++) {
        matrix.set(i, i, diagonal.at(i));
    }
    return matrix;
}

Matrix Matrix::createRowMatrix(const SparseVector &row)
{
    Matrix ret(0, row.length());
    ret.appendRow(row);
    return ret;
}

Matrix Matrix::createColumnMatrix(const SparseVector& column)
{
    Matrix ret(column.length(), 1);
    for (unsigned int i = 0; i < column.length(); i++) {
        ret.set(i, 0, column.at(i));
    }
    return ret;
}

Matrix Matrix::operator*(const Matrix& other) const
{
    // TODO: check whether dimensions are correct or not
    Matrix result(m_rowCount, other.m_columnCount);
    if (m_isDiagonal == true) {
        unsigned int columnIndex;
        for (columnIndex = 0; columnIndex < other.m_columnCount; columnIndex++) {
            // a      | x1 |   | a*x1 |
            //  b     | x2 |   | b*x2 |
            //   c    | x3 | = | c*x3 |
            //    d   | x4 |   | d*x4 |
            SparseVector::NonzeroIterator iter = other.column(columnIndex).beginNonzero();
            SparseVector::NonzeroIterator iterEnd = other.column(columnIndex).endNonzero();
            for (; iter < iterEnd; ++iter) {
                const unsigned int index = iter.getIndex();
                if (m_rowWise[index]->nonZeros() > 0) {
                    result.set(index, columnIndex, *iter * m_rowWise[index]->at(index));
                }
            }
        }
    } else if (other.m_isDiagonal == true) {
        //               | a    |
        //               |  b   |
        //               |   c  | = | a*x1 b*x2 c*x3 d*x4 |
        // | x1 x2 x3 x4 |    d |

        unsigned int rowIndex;
        for (rowIndex = 0; rowIndex < m_rowCount; rowIndex++) {
            SparseVector::NonzeroIterator iter = m_rowWise[rowIndex]->beginNonzero();
            SparseVector::NonzeroIterator iterEnd = m_rowWise[rowIndex]->endNonzero();
            for (; iter < iterEnd; ++iter) {
                const unsigned int index = iter.getIndex();
                if (other.m_rowWise[index]->nonZeros() > 0) {
                    result.set(rowIndex, index, *iter * other.m_rowWise[index]->at(index));
                }
            }
        }
    } else {
        for (unsigned int i = 0; i < m_rowCount; i++) {
            for (unsigned int j = 0; j < other.m_columnCount; j++) {
                Numerical::Double product = m_rowWise[i]->dotProduct(*other.m_columnWise[j]);
                result.m_rowWise[i]->set(j, product);
                result.m_columnWise[j]->set(i, product);
            }
        }
    }
    return result;
}

Matrix Matrix::operator-(const Matrix& other) const
{
    // TODO: check whether dimensions are correct or not
    Matrix result(*this);
    unsigned int row, column;
    for (row = 0; row < m_rowCount; row++) {
        result.m_rowWise[row]->addVector(-1.0, *other.m_rowWise[row]);
    }
    for (column = 0; column < m_columnCount; column++) {
        result.m_columnWise[column]->addVector(-1.0, *other.m_columnWise[column]);
    }

    return result;
}

void Matrix::operator-=(const Matrix& other)
{
    // TODO: check whether dimensions are correct or not
    unsigned int row, column;
    for (row = 0; row < m_rowCount; row++) {
        m_rowWise[row]->addVector(-1.0, *other.m_rowWise[row]);
    }
    for (column = 0; column < m_columnCount; column++) {
        m_columnWise[column]->addVector(-1.0, *other.m_columnWise[column]);
    }
}

Matrix Matrix::operator+(const Matrix& other) const
{
    // TODO: check whether dimensions are correct or not
    Matrix result(*this);
    unsigned int row, column;
    for (row = 0; row < m_rowCount; row++) {
        result.m_rowWise[row]->addVector(1.0, *other.m_rowWise[row]);
    }
    for (column = 0; column < m_columnCount; column++) {
        result.m_columnWise[column]->addVector(1.0, *other.m_columnWise[column]);
    }

    return result;
}

void Matrix::operator+=(const Matrix& other)
{
    // TODO: check whether dimensions are correct or not
    unsigned int row, column;
    for (row = 0; row < m_rowCount; row++) {
        m_rowWise[row]->addVector(1.0, *other.m_rowWise[row]);
    }
    for (column = 0; column < m_columnCount; column++) {
        m_columnWise[column]->addVector(1.0, *other.m_columnWise[column]);
    }
}

//Matrix operator*(Numerical::Double d, const Matrix& right)
//{
//    Matrix m(0, 0);
//    unsigned int index;
//    m.m_rowCount = right.m_rowCount;
//    m.m_columnCount = right.m_columnCount;
//    m.m_isDiagonal = right.m_isDiagonal;
//    m.m_rowWise = 0;
//    m.m_columnWise = 0;

//    m.m_rowWise = new Vector*[ m.m_rowCount ];
//    for (index = 0; index < m.m_rowCount; index++) {
//        m.m_rowWise[index] = new Vector(*right.m_rowWise[index], d);
//    }
//    m.m_columnWise = new Vector*[ m.m_columnCount ];
//    for (index = 0; index < m.m_columnCount; index++) {
//        m.m_columnWise[index] = new Vector(*right.m_columnWise[index], d);
//    }
//    return m;
//}

//Matrix operator+(Numerical::Double d, const Matrix& right)
//{
//    Matrix m = right;
//    for (unsigned int i = 0; i < right.rowCount(); i++) {
//        for (unsigned int j = 0; j < right.columnCount(); j++) {
//            m.set(i, j, d + right.get(i, j));
//        }
//    }
//    return m;
//}

void Matrix::sortVectors() const
{
    unsigned int index;
    for (index = 0; index < rowCount(); index++) {
        (*m_rowWise[index]).sortIndices();
    }
    for (index = 0; index < columnCount(); index++) {
        (*m_columnWise[index]).sortIndices();
    }
}

int Matrix::gaussianElimination(bool gaussianJordan)
{
    unsigned int row;
    int swapCount = 0;
    for (row = 0; row < m_rowCount; row++) {
        if (m_rowWise[row]->at(row) == 0.0) {
            unsigned int pivot = row + 1;
            while (pivot < m_rowCount && m_rowWise[pivot]->at(row) == 0.0) {
                pivot++;
            }
            if (pivot >= m_rowCount) {
                //LPWARNING("Gaussian elimination terminated");
                return 0;
            }

            SparseVector temp = *m_rowWise[row];
            *m_rowWise[row] = *m_rowWise[pivot];
            *m_rowWise[pivot] = temp;
            swapCount++;
        }

        Numerical::Double divider = m_rowWise[row]->at(row);
        unsigned int row2;
        if (gaussianJordan) {
            for (row2 = 0; row2 < m_rowCount; row2++) {
                if (row2 != row) {
                    m_rowWise[row2]->addVector(-m_rowWise[row2]->at(row) / divider, *m_rowWise[row]);
                }
            }
        } else {
            for (row2 = row + 1; row2 < m_rowCount; row2++) {
                m_rowWise[row2]->addVector(-m_rowWise[row2]->at(row) / divider, *m_rowWise[row]);
            }
        }

    }
    if (gaussianJordan) {
        for (row = 0; row < m_rowCount; row++) {
            m_rowWise[row]->scale(1.0 / m_rowWise[row]->at(row));
        }
    }

    return swapCount;
}

Numerical::Double Matrix::scaleRowAndGetResults(unsigned int rowIndex,
                                        const std::vector<Numerical::Double> & multipliers,
                                        Numerical::Double lambda,
                                        Numerical::Double * squareSumPtr,
                                        Numerical::Double * minPtr,
                                        Numerical::Double * maxPtr) {
    return m_rowWise[rowIndex]->scaleAndGetResults(multipliers, lambda, squareSumPtr, minPtr, maxPtr);
}

Numerical::Double Matrix::scaleColumnAndGetResults(unsigned int columnIndex,
                                           const std::vector<Numerical::Double> &multipliers,
                                           Numerical::Double lambda,
                                           Numerical::Double * squareSumPtr,
                                           Numerical::Double * minPtr,
                                           Numerical::Double * maxPtr) {
    return m_columnWise[columnIndex]->scaleAndGetResults(multipliers, lambda, squareSumPtr, minPtr, maxPtr);
}
