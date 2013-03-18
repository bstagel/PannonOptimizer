#include <debug.h>
#include <linalg/matrix.h>

#include <cassert>
#include <cstring>
#include <iostream>
#include <vector>

#ifdef WIN32
typedef unsigned int uint;
#endif

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
    reInit(builder.getRowCount(), builder.getColumnCount(), false);
    unsigned int rowIndex;
    for (rowIndex = 0; rowIndex < builder.getRowCount(); rowIndex++) {
        m_rowWise[rowIndex] = new Vector;
        builder.buildRow(rowIndex, m_rowWise[rowIndex]);
    }
    unsigned int columnIndex;
    for (columnIndex = 0; columnIndex < builder.getColumnCount(); columnIndex++) {
        m_columnWise[columnIndex] = new Vector;
        builder.buildColumn(columnIndex, m_columnWise[columnIndex]);
    }    
}

void Matrix::resizeVectors(Vector ** & vectors, unsigned int vectorCount,
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
        Vector ** newVectors = new Vector*[newCount];
        memcpy(newVectors, vectors, sizeof (Vector*) * vectorCount);
        for (idx = 0; idx < vectorCount; idx++) {
            newVectors[idx]->resize(newLengths);
        }
        for (idx = vectorCount; idx < newCount; idx++) {
            newVectors[idx] = new Vector(newLengths);
        }
        delete [] vectors;
        vectors = newVectors;
    } else {
        Vector ** newVectors = new Vector*[newCount];
        memcpy(newVectors, vectors, sizeof (Vector*) * newCount);
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

void Matrix::reInit(unsigned int rowCount, unsigned int columnCount, bool
    initVectors)
{
    clear();
    init(rowCount, columnCount, initVectors);
}

Matrix & Matrix::operator=(const Matrix & original)
{
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

    for (uint index = 0; index < m_rowCount; index++) {
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

const Vector & Matrix::row(unsigned int index) const
{
    return *(m_rowWise[index]);
}

const Vector & Matrix::column(unsigned int index) const
{
    return *m_columnWise[index];
}

Matrix & Matrix::transpose()
{
    m_fastColumnScaling = false;
    unsigned int tempSize = m_columnCount;
    m_columnCount = m_rowCount;
    m_rowCount = tempSize;

    Vector ** temp = m_rowWise;
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

void Matrix::removeVector(Vector ** & columnWise, unsigned int & columnCount,
    Vector ** & rowWise, unsigned int & rowCount, unsigned int index)
{
    m_fastColumnScaling = false;
    // minden oszlop index-edik sorabol el kell tavolitani egy elemet
    register Vector ** vector1 = columnWise;
    register Vector ** vector2 = columnWise + columnCount;

    while (vector1 < vector2) {
        (*vector1)->removeElement(index);
        vector1++;
    }
    delete rowWise[index];
    rowWise[index] = 0;

    // el kell tavolitani az index-edik sort
    // majd ezt lehet gyorsitani itt is egy elbowroom-ozassal, de egyelore marad igy
    Vector ** temp = new Vector*[ rowCount - 1 ];
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

// TODO: ha a vegere szurunk be, akkor insertElement helyett append-et kell meghivni!

void Matrix::insertVector(Vector ** columnWise, Vector ** & rowWise,
    unsigned int & rowCount, unsigned int index, const Vector & vector)
{
    m_fastColumnScaling = false;
    // oszlopokba is be kell szurni
    std::vector<bool> inserted(vector.length(), false);
    Vector::ConstNonzeroIterator iterator = vector.beginNonzero();
    Vector::ConstNonzeroIterator iteratorEnd = vector.endNonzero();
    for (; iterator < iteratorEnd; iterator++) {
        columnWise[ iterator.getIndex() ]->insertElement(index, *iterator);
        inserted[ iterator.getIndex() ] = true;
    }
    std::vector<bool>::const_iterator insertIter = inserted.begin();
    const std::vector<bool>::const_iterator insertIterEnd = inserted.end();
    unsigned int insertIndex = 0;
    for (; insertIter < insertIterEnd; insertIter++, insertIndex++) {
        if (*insertIter == false) {
            columnWise[ insertIndex ]->insertElement(index, 0.0);
        }
    }

    Vector * temp = new Vector(vector);
    Vector ** vectors = new Vector*[ rowCount + 1 ];
    // index elottieket
    if (index > 0) {
        memcpy(vectors, rowWise, index * sizeof (Vector*));
    }
    vectors[index] = temp;
    if (index < rowCount) {
        memcpy(vectors + index + 1, rowWise + index, sizeof (Vector*) * (rowCount - index));
    }
    delete [] rowWise;
    rowWise = vectors;
    rowCount++;
}

void Matrix::insertEmptyVector(Vector ** columnWise, Vector ** & rowWise,
    unsigned int & rowCount, unsigned int columnCount, unsigned int index)
{
    m_fastColumnScaling = false;
    // oszlopokba is be kell szurni
    unsigned int insertIndex = 0;
    for (insertIndex = 0; insertIndex < columnCount; insertIndex++) {
        columnWise[ insertIndex ]->insertElement(index, 0.0);
    }

    Vector * temp = new Vector(columnCount);
    Vector ** vectors = new Vector*[ rowCount + 1 ];
    // index elottieket
    if (index > 0) {
        memcpy(vectors, rowWise, index * sizeof (Vector*));
    }
    vectors[index] = temp;
    if (index < rowCount) {
        memcpy(vectors + index + 1, rowWise + index, sizeof (Vector*) * (rowCount - index));
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

void Matrix::insertRow(unsigned int index, const Vector & vector)
{
    if (m_rowCount == 0) {
        this->init(0, vector.length());
    }
    insertVector(m_columnWise, m_rowWise, m_rowCount, index, vector);

}

void Matrix::insertColumn(unsigned int index, const Vector & vector)
{
    if (m_columnCount == 0) {
        this->init(vector.length(), 0);
    }
    insertVector(m_rowWise, m_columnWise, m_columnCount, index, vector);
}

void Matrix::appendRow(const Vector & vector)
{
    if (m_rowCount == 0) {
        this->reInit(0, vector.length());
    }
    insertVector(m_columnWise, m_rowWise, m_rowCount, m_rowCount, vector);
}

void Matrix::appendColumn(const Vector & vector)
{
    if (m_columnCount == 0) {
        reInit(vector.length(), 0);
    }
    insertVector(m_rowWise, m_columnWise, m_columnCount, m_columnCount, vector);
    //insertColumn(m_columnCount, vector);
}

void Matrix::prependRow(const Vector & vector)
{
    if (m_rowCount == 0) {
        this->init(0, vector.length());
    }
    insertRow(0, vector);
}

void Matrix::prependColumn(const Vector & vector)
{
    if (m_columnCount == 0) {
        this->init(vector.length(), 0);
    }
    insertColumn(0, vector);
}

void Matrix::setNewNonzero(unsigned int y, unsigned int x, Numerical::Double value)
{
    m_fastColumnScaling = false;
    m_rowWise[y]->setNewNonzero(x, value);
    m_columnWise[x]->setNewNonzero(y, value);
}

void Matrix::set(unsigned int y, unsigned int x, Numerical::Double value)
{
    m_fastColumnScaling = false;
    m_rowWise[y]->set(x, value);
    m_columnWise[x]->set(y, value);
}

Numerical::Double Matrix::get(unsigned int y, unsigned int x) const
{
    return m_rowWise[y]->at(x);
    // TODO: azt hasznalja, amelyik dense
}

void Matrix::scale(Numerical::Double lambda)
{
    m_fastColumnScaling = false;
    register Vector ** vector1 = m_columnWise;
    register Vector ** vector2 = m_columnWise + m_columnCount;
    while (vector1 < vector2) {
        (*vector1)->scaleBy(lambda);
        vector1++;
    }
    vector1 = m_rowWise;
    vector2 = m_rowWise + m_rowCount;
    while (vector1 < vector2) {
        (*vector1)->scaleBy(lambda);
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
    register Vector ** vector1 = m_rowWise;
    register Vector ** vector2 = m_rowWise + m_rowCount;
    while (vector1 < vector2) {
        (*vector1)->scaleByLambdas(m_multipliers);
        vector1++;
    }
    m_fastColumnScaling = false;
}

void Matrix::finishRowScaling()
{
    register Vector ** vector1 = m_columnWise;
    register Vector ** vector2 = m_columnWise + m_columnCount;
    while (vector1 < vector2) {
        (*vector1)->scaleByLambdas(m_multipliers);
        vector1++;
    }
    m_fastRowScaling = false;
}

void Matrix::scaleRow(unsigned int index, Numerical::Double lambda)
{
    m_rowWise[index]->scaleBy(lambda);
    if (m_fastRowScaling == true) {
        m_multipliers[index] = lambda;
    } else {
        register Vector ** vector1 = m_columnWise;
        register Vector ** vector2 = m_columnWise + m_columnCount;
        while (vector1 < vector2) {
            (*vector1)->scaleElementBy(index, lambda);
            vector1++;
        }
    }
}

void Matrix::scaleColumn(unsigned int index, Numerical::Double lambda)
{
    m_columnWise[index]->scaleBy(lambda);
    if (m_fastColumnScaling == true) {
        if (index >= m_multipliers.size()) {
            LPERROR("hiba\n");
        }
        m_multipliers[index] = lambda;
    } else {
        register Vector ** vector1 = m_rowWise;
        register Vector ** vector2 = m_rowWise + m_rowCount;
        while (vector1 < vector2) {
            (*vector1)->scaleElementBy(index, lambda);
            vector1++;
        }
    }
}

void Matrix::invert()
{
    if (m_isDiagonal == true) {
        unsigned int i;
        for (i = 0; i < m_rowCount; i++) {
            m_rowWise[i]->set(i, 1 / m_rowWise[i]->at(i));
            m_columnWise[i]->set(i, 1 / m_columnWise[i]->at(i));
        }
    } else {
        Matrix copyMatrix = *this;
        unsigned int i;
        Vector e(m_rowCount);
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
            Vector::NonzeroIterator iter = copyMatrix.m_rowWise[i]->beginNonzero();
            Vector::NonzeroIterator iterEnd = copyMatrix.m_rowWise[i]->endNonzero();
            for (; iter != iterEnd; iter++) {
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
            inverted.m_rowWise[i]->set(i, 1 / m_rowWise[i]->at(i));
            inverted.m_columnWise[i]->set(i, 1 / m_columnWise[i]->at(i));
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

    /* Allocate new subMatrix */
    /*Numerical::Double det = 0;

    for (unsigned int i = 0; i < matrix.rowCount(); i++) {
        Matrix subMatrix(matrix);
        Numerical::Double pivot = subMatrix.get(i, 0);
        subMatrix.removeRow(i);
        subMatrix.removeColumn(0);

        Numerical::Double subDet = determinant(subMatrix);
        det += (i % 2 == 1 ? -1.0 : 1.0) * pivot * subDet;
    }*/

    //return det;
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

Vector Matrix::operator *(const Vector& rightVector) const
{
    Vector result(this->rowCount());
    Numerical::Double ratio = result.getSparsityRatio();
    result.setSparsityRatio(0.0);

    Vector::ConstNonzeroIterator iter = rightVector.beginNonzero();
    Vector::ConstNonzeroIterator iterEnd = rightVector.endNonzero();
    for (; iter < iterEnd; iter++) {
        result.addVector(*iter, *(m_columnWise[ iter.getIndex() ]));
    }
    result.setSparsityRatio(ratio);

    return result;
}

void Matrix::copy(const Matrix & matrix)
{
    unsigned int index;
    m_rowCount = matrix.m_rowCount;
    m_columnCount = matrix.m_columnCount;
    m_isDiagonal = matrix.m_isDiagonal;
    m_rowWise = 0;
    m_columnWise = 0;

    m_fastRowScaling = matrix.m_fastRowScaling;
    m_multipliers = matrix.m_multipliers;
    m_rowWise = new Vector*[ m_rowCount ];
    for (index = 0; index < m_rowCount; index++) {
        m_rowWise[index] = new Vector(*matrix.m_rowWise[index]);
    }
    m_columnWise = new Vector*[ m_columnCount ];
    for (index = 0; index < m_columnCount; index++) {
        m_columnWise[index] = new Vector(*matrix.m_columnWise[index]);
    }
}

void Matrix::init(unsigned int rowCount, unsigned int columnCount, bool
    initVectors)
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
        m_rowWise = new Vector*[ rowCount ];
    } else {
        m_rowWise = 0;
    }
    for (index = 0; index < m_rowCount; index++) {
        if (initVectors) {
            m_rowWise[index] = new Vector(m_columnCount);
        } else {
            m_rowWise[index] = new Vector(NULL, NULL, NULL);
        }
    }
    if (m_columnCount > 0) {
        m_columnWise = new Vector*[ m_columnCount ];
    } else {
        m_columnWise = 0;
    }

    for (index = 0; index < m_columnCount; index++) {
        if (initVectors) {
            m_columnWise[index] = new Vector(m_rowCount);
        } else {
            m_columnWise[index] = new Vector(NULL, NULL, NULL);
        }
    }

}

Matrix Matrix::diagonalMatrix(const Vector& diagonal)
{
    unsigned int m = diagonal.length();
    Matrix matrix(m, m);
    matrix.m_isDiagonal = true;

    for (uint i = 0; i < m; i++) {
        matrix.set(i, i, diagonal.at(i));
    }
    return matrix;
}

Matrix Matrix::rowVector(const Vector& row)
{
    Matrix ret(0, row.length());
    ret.appendRow(row);
    return ret;
}

Matrix Matrix::columnVector(const Vector& column)
{
    Matrix ret(column.length(), 1);
    for (uint i = 0; i < column.length(); i++) {
        ret.set(i, 0, column.at(i));
    }
    return ret;
}

Matrix Matrix::operator*(const Matrix& other) const
{
    /* FIXME: check wether dimensions are correct */
    Matrix result(m_rowCount, other.m_columnCount);
    if (m_isDiagonal == true) {
        unsigned int columnIndex;
        for (columnIndex = 0; columnIndex < other.m_columnCount; columnIndex++) {
            // a      | x1 |   | a*x1 |
            //  b     | x2 |   | b*x2 |
            //   c    | x3 | = | c*x3 |
            //    d   | x4 |   | d*x4 |
            Vector::ConstNonzeroIterator iter = other.column(columnIndex).beginNonzero();
            Vector::ConstNonzeroIterator iterEnd = other.column(columnIndex).endNonzero();
            for (; iter < iterEnd; iter++) {
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
            Vector::NonzeroIterator iter = m_rowWise[rowIndex]->beginNonzero();
            Vector::NonzeroIterator iterEnd = m_rowWise[rowIndex]->endNonzero();
            for (; iter < iterEnd; iter++) {
                const unsigned int index = iter.getIndex();
                if (other.m_rowWise[index]->nonZeros() > 0) {
                    result.set(rowIndex, index, *iter * other.m_rowWise[index]->at(index));
                }
            }
        }
    } else {
        for (uint i = 0; i < m_rowCount; i++) {
            for (uint j = 0; j < other.m_columnCount; j++) {
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
    /* FIXME: check wether dimensions are correct */
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
    /* FIXME: check wether dimensions are correct */
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
    /* FIXME: check wether dimensions are correct */
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
    /* FIXME: check wether dimensions are correct */
    unsigned int row, column;
    for (row = 0; row < m_rowCount; row++) {
        m_rowWise[row]->addVector(1.0, *other.m_rowWise[row]);
    }
    for (column = 0; column < m_columnCount; column++) {
        m_columnWise[column]->addVector(1.0, *other.m_columnWise[column]);
    }
}

Matrix operator*(double d, const Matrix& right)
{
    Matrix m(0, 0);
    unsigned int index;
    m.m_rowCount = right.m_rowCount;
    m.m_columnCount = right.m_columnCount;
    m.m_isDiagonal = right.m_isDiagonal;
    m.m_rowWise = 0;
    m.m_columnWise = 0;

    m.m_rowWise = new Vector*[ m.m_rowCount ];
    for (index = 0; index < m.m_rowCount; index++) {
        m.m_rowWise[index] = new Vector(*right.m_rowWise[index], d);
    }
    m.m_columnWise = new Vector*[ m.m_columnCount ];
    for (index = 0; index < m.m_columnCount; index++) {
        m.m_columnWise[index] = new Vector(*right.m_columnWise[index], d);
    }
    return m;
}

Matrix operator+(double d, const Matrix& right)
{
    Matrix m = right;
    for (uint i = 0; i < right.rowCount(); i++) {
        for (uint j = 0; j < right.columnCount(); j++) {
            m.set(i, j, d + right.get(i, j));
        }
    }
    return m;
}

void Matrix::sortVectors() const
{
    ///clock_t cl_start, cl_end;
    //LPINFO("start sorting");
    //cl_start = clock();
    unsigned int index;
    for (index = 0; index < rowCount(); index++) {
        row(index).sortElements();
    }
    for (index = 0; index < columnCount(); index++) {
        column(index).sortElements();
    }
    //exit(1);
    //cl_end = clock();
    //LPINFO("sorting time: " << ((Numerical::Double) (cl_end - cl_start) / (Numerical::Double) CLOCKS_PER_SEC) << " sec");
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

            Vector temp = *m_rowWise[row];
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
            m_rowWise[row]->scaleBy(1.0 / m_rowWise[row]->at(row));
        }
    }

    return swapCount;
}
