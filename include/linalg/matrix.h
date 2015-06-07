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
 * @file matrix.h This file contains the API of the Matrix class.
 * @author Jozsef Smidla
 */

#ifndef MATRIX_H
#define MATRIX_H

#include <globals.h>

#include <vector>
#include <linalg/sparsevector.h>
#include <linalg/densevector.h>
#include <linalg/matrixbuilder.h>
#include <utils/numerical.h>

/**
 * Stores a matrix, with columnwise and rowwise representation method.
 *
 * @class Matrix
 */
class Matrix
{
    friend class MatrixTestSuite;
    friend class ModelIO;
    friend class MpsModelBuilder;
public:
    /**
     * Constructor of the Matrix class.
     * This constructor creates a matrix filled with zeros with appropriate dimensions.
     * When rowCount is given, and columnCount not, the matrix will be square matrix.
     *
     * @constructor
     * @param rowCount Number of rows in matrix.
     * @param columnCount Number of columns in matrix.
     */
    Matrix(unsigned int rowCount = 0, unsigned int columnCount = 0);

    /**
     * Copy constructor.
     *
     * @constructor
     * @param original The constructor copies this matrix object.
     */
    Matrix(const Matrix & original);

    /**
     * Destructor, releases the matrix.
     *
     * @destructor
     */
    ~Matrix();

    /**
     * Releases the matrix, sets number of rows and columns to zero.
     * This is a helper function of destructor and assignment operator also.
     */
    void clear();

    /**
     * Builds the matrix using the builder object.
     * @see MatrixBuilder
     *
     * @param builder This object contains the matrix data.
     */
    void build(const MatrixBuilder & builder);

    /**
     * Resizes the matrix.
     * When a dimension increases, the old elements are preserved. The new elements will be zeros.
     * When rowCount is given, and columnCount is not, the matrix will be square matrix.
     *
     * @param rowCount The new row number.
     * @param columnCount The new column number.
     */
    void resize(unsigned int rowCount = 0, unsigned int columnCount = 0);

    /**
     * Releases the matrix, and creates a new matrix with size given by the parameters.
     * When initVectors is true, the vectors will be initialized, otherwise not.
     * This function do not preserve any data of the matrix everything is overwritten.
     *
     * @param rowCount Number of rows.
     * @param columnCount Number of columns.
     * @param initVectors Specifies that, whether the vectors will be initialized or not.
     */
    void reInit(unsigned int rowCount = 0,
                unsigned int columnCount = 0);

    /**
     * Releases the actual matrix, and copies another matrix.
     *
     * @param original The matrix to be copied.
     * @return Reference to the copied matrix.
     */
    Matrix & operator=(const Matrix & original);

    /**
     * Checks whether two matrices are equal.
     * Two matrices are equals when their dimensions, and their elements are the same.
     *
     * @param other The other matrix.
     * @return True, when the matrices are equal, otherwise false.
     */
    bool operator==(const Matrix & other) const;

    /**
     * Checks whether two matrices are not equal.
     * Two matrices are equals when their dimensions, and their elements are the same.
     *
     * @param other The other matrix.
     * @return False, when the matrices are equal, otheriwe true.
     */
    inline bool operator!=(const Matrix & other) const
    {
        return !(*this == other);
    }

    /**
     * Returns the number of rows in matrix.
     *
     * @return Number of rows.
     */
    unsigned int rowCount() const;

    /**
     * Returns the number of columns in matrix.
     *
     * @return Number of columns.
     */
    unsigned int columnCount() const;

    /**
     * Get a row of the matrix.
     * It provides the row of the matrix as a constant vector.
     *
     * @param index The index of row vector.
     * @return Constant reference for the index'th row vector.
     */
    const SparseVector & row(unsigned int index) const;

    /**
     * Get a column of the matrix.
     * It provides the column of the matrix as a constant vector.
     *
     * @param index The index of column vector.
     * @return Constant reference for the index'th column vector.
     */
    const SparseVector & column(unsigned int index) const;

    /**
     * Transposes the matrix.
     * Transpose operation swap the rowwise and the columnwise representation.
     * This operation is very quick.
     *
     * @return Reference to the matrix.
     */
    Matrix & transpose();

    /**
     * Creates a transposed copy of the matrix.
     * This operation copies the whole matrix and do the transpose operation
     * on the copied matrix. Computatinally heavy operation.
     *
     * @return The transposed matrix.
     */
    Matrix transposed() const;

    /**
     * Removes the index'th row from the vector.
     * The dimension of the matrix changes, the number of rows is decreased.
     *
     * @param index The index of row.
     */
    void removeRow(unsigned int index);

    /**
     * Removes the index'th column from the vector.
     * The dimension of the matrix changes, the number of columns is decreased.
     *
     * @param index The index of column.
     */
    void removeColumn(unsigned int index);

    /**
     * Inserts a row vector before the index'th row.
     * The dimension of the matrix changes, the number of rows is increased.
     *
     * @param index The function inserts the vector before this row.
     * @param vector The vector which will be inserted.
     */
    void insertRow(unsigned int index, const SparseVector &vector);

    /**
     * Inserts an empty row vector before the index'th row.
     * The dimension of the matrix changes, the number of rows is increased.
     *
     * @param index The function inserts an empthy row before this row.
     */
    void insertEmptyRow(unsigned int index);

    /**
     * Inserts a column vector before the index'th column.
     * The dimension of the matrix changes, the number of columns is increased.
     *
     * @param index The function inserts the vector before this column.
     * @param vector The vector which will be inserted.
     */
    void insertColumn(unsigned int index, const SparseVector & vector);

    /**
     * Inserts an empty column vector before the index'th column.
     * The dimension of the matrix changes, the number of columns is increased.
     *
     * @param index The function inserts an empty column before this column.
     */
    void insertEmptyColumn(unsigned int index);

    /**
     * Appends the vector after the last row.
     * The dimension of the matrix changes, the number of rows is increased.
     *
     * @param vector The function appends this vector.
     */
    void appendRow(const SparseVector &vector);

    /**
     * Appends the vector after the last column.
     *
     * @param vector The function appends this vector.
     * The dimension of the matrix changes, the number of columns is increased.
     */
    void appendColumn(const SparseVector & vector);

    /**
     * Inserts the vector before the first row.
     * The dimension of the matrix changes, the number of rows is increased.
     *
     * @param vector The function inserts this vector.
     */
    void prependRow(const SparseVector &vector);

    /**
     * Inserts the vector before the first column.
     * The dimension of the matrix changes, the number of columns is increased.
     *
     * @param vector The function inserts this vector.
     */
    void prependColumn(const SparseVector & vector);

    /**
     * Replaces a zero value with a given nonzero in the matrix.
     * This function can be used only if the original element is zero.
     * Otherwise it will lead to undefined behaviour.
     *
     * @param rowindex Row index of the element.
     * @param columnindex Column index of the element.
     * @param value A nonzero value to be set.
     */
    void setNewNonzero(unsigned int rowindex, unsigned int columnindex, Numerical::Double value);

    /**
     * Sets the value of an element.
     *
     * @param rowindex Row index of the element.
     * @param columnindex Column index of the element.
     * @param value The new value of the element.
     */
    void set(unsigned int rowindex, unsigned int columnindex, Numerical::Double value);

    /**
     * Returns the value of an element.
     *
     * @param rowindex Row index of the element.
     * @param columnindex Column index of the element.
     * @return Value of element in row rowindex and column columnindex.
     */
    Numerical::Double get(unsigned int rowindex, unsigned int columnindex) const;

    /**
     * Scales the elements of the matrix by a multiplier.
     *
     * @param lambda The function multiplies the elements of matrix with this value.
     */
    void scale(Numerical::Double lambda);

    /**
     * Scales the elements of the index'th row by a multiplier.
     *
     * @param index The index of the row to be scaled.
     * @param lambda The function multiplies the row with this value.
     */
    void scaleRow(unsigned int index, Numerical::Double lambda);

    /**
     * Scales the elements of the index'th column by a multiplier.
     *
     * @param index The index of the column to be scaled.
     * @param lambda The function multiplies the column with this value.
     */
    void scaleColumn(unsigned int index, Numerical::Double lambda);

    // TODO: a kovetkezo 4 fuggvenyhez tesztelot irni
    /**
     * Scales the elements of the given row. It multiplies only in the rowwise form.
     * The columnwise form must be updated manually when this function is used.
     * Otherwise it will lead to undefined behaviour.
     *
     * @param index The index of the row to be scaled.
     * @param lambda The function scales by this value.
     */
    void scaleOnlyRowwise(unsigned int index, Numerical::Double lambda);

    /**
     * Scales the elements of the given row. It multiplies only in the rowwise form.
     * The columnwise form must be updated manually when this function is used.
     * Otherwise it will lead to undefined behaviour.
     *
     * @param index The index of the row to be scaled.
     * @param lambdas The function scales with the elements of this vector.
     */
    void scaleOnlyRowwiseLambdas(unsigned int index, const std::vector<Numerical::Double> &lambdas);

    /**
     * Scales the elements of the given column. It multiplies only in the columnwise form.
     * The rowwise form must be updated manually when this function is used.
     * Otherwise it will lead to undefined behaviour.
     *
     * @param index The index of the column to be scaled.
     * @param lambda The function scales by this value.
     */
    void scaleOnlyColumnwise(unsigned int index, Numerical::Double lambda);

    /**
     * Scales the elements of the given column. It multiplies only in the columnwise form.
     * The rowwise form must be updated manually when this function is used.
     * Otherwise it will lead to undefined behaviour.
     *
     * @param index The index of the column to be scaled.
     * @param lambdas The function scales with the elements of this vector.
     */
    void scaleOnlyColumnwiseLambdas(unsigned int index, const std::vector<Numerical::Double> &lambdas);

    /**
     * Inverts the matrix.
     * The invers is created using Gauss-Jordan elimination.
     */
    void invert();

    /**
     * Creates and returns the inverse of the matrix.
     *
     * @return The inverse of the matrix.
     */
    Matrix inverse() const;

    /**
     * Calculates the determinant of the matrix.
     * Result is 0.0 if the matrix is not a sqaure-matrix.
     *
     * @return The determinant of the matrix.
     */
    Numerical::Double determinant() const;

    /**
     * Displays the Matrix in a human-readable format.
     */
    void show() const;

    /**
     * Counts the nonzero elements in the matrix.
     *
     * @return The number of nonzeros in the matrix.
     */
    unsigned int nonZeros() const;

    /**
     * Gives the density of the matrix.
     * Density is a value between 0 and 1 computed as the ratio of nonzero elements
     * and the total elements in the matrix.
     *
     * @return The density of the matrix.
     */
    Numerical::Double density() const;

    /**
     * Returns a product yielded by multiplying the matrix with a vector from the right.
     * If the given right vector's dimension is not the same as column count of
     * the matrix (n), then it will be interpreted as an n-length column vector,
     * filled up by zeros if needed.
     *
     * @param rightVector The column vector used to multiply this matrix.
     * @return A column vector containing the product.
     */
    DenseVector operator*(const DenseVector &rightVector) const;

    /**
     * Creates a diagonal matrix from given vector.
     * The dimension of the matrix will be same as the vector.
     *
     * @param diagonal A vector which provides the elements for the diagonal matrix.
     * @return The created diagonal matrix.
     */
    static Matrix createDiagonalMatrix(const DenseVector &diagonal);

    /**
     * Creates a matrix from the row vector.
     * The resulted matrix will have only one row.
     * The column dimension is set from the given row vector.
     *
     * @param row A row vector.
     * @return A row matrix.
     */
    static Matrix createRowMatrix(const SparseVector &row);

    /**
     * Creates a matrix from the column vector.
     * The resulted matrix will have only one column.
     * The column dimension is set from the given column vector.
     *
     * @param column A column vector.
     * @return A column matrix.
     */
    static Matrix createColumnMatrix(const SparseVector& column);

    /**
     * Multiplies two matrices.
     * The matrix is multiplied with another matrix from the right.
     * The dimensions are not checked against each other, so use this function carefully.
     *
     * @param other The right member of matrix product.
     * @return Product of the two matrices.
     */
    Matrix operator*(const Matrix& other) const;

    /**
     * Computes the differencee of a matrix and the actual matrix.
     * The result will be given in a new matrix.
     * The dimensions are not checked against each other, so use this function carefully.
     *
     * @param other The right member of subtraction.
     * @return The result matrix.
     */
    Matrix operator-(const Matrix& other) const;

    /**
     * Subtracts a matrix from the actual one.
     * The dimensions are not checked against each other, so use this function carefully.
     *
     * @param other The function subtracts this matrix.
     */
    void operator-=(const Matrix& other);

    /**
     * Returns the sum of two matrices.
     * The result will be given in a new matrix.
     * The dimensions are not checked against each other, so use this function carefully.
     *
     * @param other The right member of addition.
     * @return Sum of two matrices.
     */
    Matrix operator+(const Matrix& other) const;

    /**
     * Adds a matrix to the actual one.
     * The dimensions are not checked against each other, so use this function carefully.
     *
     * @param other The function adds this matrix to the actual one.
     */
    void operator+=(const Matrix& other);

    /**
     * This function sorts the elements of sparse vectors in matrix.
     * Sorting is done on the rowwise and the columnwise representation too.
     * A sorted sparse vector contains the elements using increasing index order.
     *
     * @see Vector::sortElements()
     */
    void sortVectors() const;

    /**
     * Prepares the matrix for row scaling.
     */
    void startRowScaling();

    /**
     * Executes row scaling on the matrix with the current multipliers.
     *
     * @see m_multipliers
     */
    void finishRowScaling();

    /**
     * Prepares the matrix for column scaling
     */
    void startColumnScaling();

    /**
     * Executes column scaling on the matrix with the current multipliers.
     *
     * @see m_multipliers
     */
    void finishColumnScaling();

    /**
     * This function performs a Gaussian elimination on the matrix.
     *
     * @param gaussianJordan If this parameter is true, a Gauss-Jordan elimination is performed.
     * @return The number of row swaps throughout the Gaussian elimination.
     */
    int gaussianElimination(bool gaussianJordan = false);

    /**
     * Scales a row vector with the given multipliers and return detailed information.
     * Returns the sum of the absolute values of the elements, the squares of the
     * absolute values of the elements, and the maximal and minimal absolute values
     * of the elements.
     *
     * @param rowIndex The index of the row to be scaled.
     * @param multipliers Variable storing the scaling multipliers. Its dimension has to be
     * equal to the number of columns in the matrix.
     * @param lambda Scaling multiplier which is applied to every element of the vector.
     * @param squareSumPtr Pointer to the variable storing the sum of the squares of
     * the absolute values of the elements.
     * @param minPtr Pointer to the variable storing the minimal absolute value of the
     * elements.
     * @param maxPtr Pointer to the variable storing the maximal absolute value of the
     * elements.
     * @return The sum of the absolute values of the elements.
     */
    Numerical::Double scaleRowAndGetResults(unsigned int rowIndex,
                                            const std::vector<Numerical::Double> &multipliers,
                                            Numerical::Double lambda,
                                            Numerical::Double * squareSumPtr,
                                            Numerical::Double * minPtr,
                                            Numerical::Double * maxPtr);

    /**
     * Scales a column vector with the given multipliers.
     * Returns the sum of the absolute values of the elements, the squares of the absolute values
     * of the elements, and the maximal and minimal absolute values of the elements.
     *
     * @param columnIndex The index of the column to be scaled.
     * @param multipliers Variable storing the scaling multipliers. Its dimension has to be
     * equal to the number of rows in the matrix.
     * @param lambda Scaling multiplier which is applied to every element of the vector.
     * @param squareSumPtr Pointer to the variable storing the sum of the squares of
     * the absolute values of the elements.
     * @param minPtr Pointer to the variable storing the minimal absolute value of the
     * elements.
     * @param maxPtr Pointer to the variable storing the maximal absolute value of the
     * elements.
     * @return The sum of the absolute values of the elements.
     */
    Numerical::Double scaleColumnAndGetResults(unsigned int columnIndex,
                                               const std::vector<Numerical::Double> &multipliers,
                                               Numerical::Double lambda,
                                               Numerical::Double * squareSumPtr,
                                               Numerical::Double * minPtr,
                                               Numerical::Double * maxPtr);

private:

    /**
     * Returns the determinant of a given matrix.
     *
     * @param subMatrix The function computes the determinant of this matrix.
     * @return Determinant of subMatrix.
     */
    Numerical::Double determinant(const Matrix& subMatrix) const;

    /**
     * True if the matrix is ready for row scaling.
     */
    bool m_fastRowScaling;

    /**
     * True if the matrix is ready for column scaling.
     */
    bool m_fastColumnScaling;

    /**
     * Scale factors for the fast scaling
     */
    std::vector<Numerical::Double> m_multipliers;

    /**
     * The rowwise representation of the matrix.
     * Array of Vector pointers, stores pointers to the row vectors.
     * It has m_rowCount elements.
     */
    SparseVector ** m_rowWise;

    /**
     * The columnwise representation of the matrix.
     * Array of Vector pointers, stores pointers to the column vectors.
     * It has m_columncount elements.
     */
    SparseVector ** m_columnWise;

    /**
     * Stores the number of rows of the matrix.
     * It is the number of elements of m_rowWise array.
     */
    unsigned int m_rowCount;

    /**
     * Stores the number of columns of the matrix.
     * Number of elements of m_columnWise array.
     */
    unsigned int m_columnCount;

    /**
     * When the matrix is diagonal, it is true, otherwise false.
     */
    bool m_isDiagonal;

    /**
     * Copies the matrix.
     * This is a helper function of the copy constructor and the assignement operator.
     *
     * @param matrix The function copies this matrix.
     */
    void copy(const Matrix & matrix);

    /**
     * Initializes the matrix with the size given by the parameters.
     * When initVectors is true, the vectors will be initialized, otherwise not.
     *
     * @param rowCount Number of rows.
     * @param columnCount Number of columns.
     */
    void init(unsigned int rowCount,
              unsigned int columnCount);

    /**
     * Resizes a representation of a matrix, allowing to change the number of rows and columns in it.
     *
     * @param vectors Pointer to the vector array of the matrix, which stores a representation.
     * @param vectorCount The number of vectors in the original vector array.
     * @param newCount The number of vectors in the new vector array.
     * @param newLengths The length of the vectors in the new vector array.
     */
    static void resizeVectors(SparseVector ** & vectors, unsigned int vectorCount,
                              unsigned int newCount, unsigned int newLengths);

    /**
     * Removes a vector from the matrix and decreases the dimension.
     * Default behaviour removes a row from the matrix.
     * Parameter references can be swapped for column removal.
     *
     * @see removeRow()
     * @see removeColumn()
     *
     * @param columnWise The column-wise representation of the target matrix.
     * @param columnCount The number of columns in the taret matrix.
     * @param rowWise The row-wise representation of the target matrix.
     * @param rowCount The row count to be decreased.
     * @param index The index of the row to be removed.
     */
    void removeVector(SparseVector **&columnWise, unsigned int & columnCount,
                      SparseVector **&rowWise, unsigned int & rowCount, unsigned int index);

    /**
     * Inserts a vector to the matrix and increases the dimension.
     * Default behaviour inserts a row to the matrix.
     * Parameter references can be swapped for column insertion.
     *
     * @see insertRow()
     * @see insertColumn()
     *
     * @param columnWise The column-wise representation of the target matrix.
     * @param rowWise The row-wise representation of the target matrix.
     * @param rowCount The number of rows in the matrix.
     * @param index The index of the vector to be added.
     * @param vector The vector to be added.
     */
    void insertVector(SparseVector **columnWise, SparseVector **&rowWise,
                      unsigned int & rowCount, unsigned int index, const SparseVector &vector);

    /**
     * Inserts an empty vector to the matrix and increases the dimension.
     * Default behaviour inserts a row to the matrix.
     * Parameter references can be swapped for column insertion.
     *
     * @param columnWise The column-wise representation of the target matrix.
     * @param rowWise The row-wise representation of the target matrix.
     * @param rowCount The number of rows in the matrix.
     * @param columnCount The number of columns in the matrix.
     * @param index The index of the vector to be added.
     */
    void insertEmptyVector(SparseVector **columnWise, SparseVector **&rowWise,
                           unsigned int & rowCount, unsigned int columnCount, unsigned int index);

//TODO: Ez a ket fuggveny feltehetoleg torolheto, sehol nincs hasznalva es nincs is jol implementalva!
//    /**
//     * Returns the result of d times right matrix.
//     *
//     * @param d The scalar the matrix is to be multiplied with.
//     * @param right The matrix to be multiplied.
//     * @return The result matrix of the multiplication.
//     */
//    friend Matrix operator*(Numerical::Double d, const Matrix& right);

//    /**
//     * Adds d to each element of right matrix and returns the result matrix.
//     *
//     * @param d The scalar to be added to all elements of the matrix.
//     * @param right The original matrix the operation is performed with.
//     * @return The result matrix of the mass addition.
//     */
//    friend Matrix operator+(Numerical::Double d, const Matrix& right);

};

#endif // MATRIX_H
