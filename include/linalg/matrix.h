/**
 * @file matrix.h This file contains the API of the Matrix class.
 * @author smidla
 */

#ifndef MATRIX_H
#define MATRIX_H

#include <globals.h>

#include <vector>
#include <linalg/vector.h>
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
     * Constructor of the Matrix class. Creates a matrix filled with zeros.
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
     * This is a helper function of destructor and assignment operator
     * also.
     */
    void clear();

    /**
     * Builds the matrix using the builder object.
     *
     * @param builder This object contains the matrix data.
     */
    void build(const MatrixBuilder & builder);

    /**
     * Resizes the matrix.
     * When a dimension increases, the old elements remain. The new elements will be zeros.
     * When rowCount is given, and columnCount not, the matrix will be square matrix.
     *
     * @param rowCount The new row number.
     * @param columnCount The new column number.
     */
    void resize(unsigned int rowCount = 0, unsigned int columnCount = 0);

    /**
     * Releases the matrix, and creates a new matrix with sizes given by rowCount and columnCount.
     * When initVectors is true, the vectors will be initialized, otherwise not.
     *
     * @param rowCount Number of rows.
     * @param columnCount Number of columns.
     * @param initVectors Specifies that, whether the vectors will be
     * initialized or not.
     */
    void reInit(unsigned int rowCount = 0, unsigned int columnCount = 0, bool
                initVectors = true);

    /**
     * Releases the matrix, and copies the original.
     *
     * @param original The original matrix.
     * @return Reference to the copy matrix.
     */
    Matrix & operator=(const Matrix & original);

    /**
     * Checks whether the two matrices are equals or not.
     * Two matrices are equals when its dimensions, and the elements are the same.
     *
     * @param other The other matrix.
     * @return True, when the matrices are equal, otherwise false.
     */
    bool operator==(const Matrix & other) const;

    /**
     * Checks whether the two matrices are equals or not.
     * Two matrices are equals when its dimensions, and the elements are the same.
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
     * Returns a const reference to the index'th row vector.
     *
     * @param index The index of row vector.
     * @return Const reference for the index'th row vector.
     */
    const Vector & row(unsigned int index) const;

    /**
     * Retruns a const reference to the index'th column vector.
     *
     * @param index The index of column vector.
     * @return Const reference for the index'th column vector.
     */
    const Vector & column(unsigned int index) const;

    /**
     * Transposes the matrix.
     *
     * @return Reference to the matrix.
     */
    Matrix & transpose();

    /**
     * Creates a transposed copy of the matrix.
     *
     * @return The transpsed matrix.
     */
    Matrix transposed() const;

    /**
     * Removes the index'th row from the vector. The number of rows decreases.
     *
     * @param index The index of row.
     */
    void removeRow(unsigned int index);

    /**
     * Removes the index'th column from the vector. The number of columns decreases.
     *
     * @param index The index of column.
     */
    void removeColumn(unsigned int index);

    /**
     * Inserts a row vector before the index'th row.
     *
     * @param index The function inserts the vector before this row.
     * @param vector The vector which will be inserted.
     */
    void insertRow(unsigned int index, const Vector & vector);

    /**
     * Inserts an empty row vector before the index'th row.
     *
     * @param index The function inserts an empthy row before this row.
     */
    void insertEmptyRow(unsigned int index);

    /**
     * Inserts an empty column vector before the index'th column.
     *
     * @param index The function inserts an empty column before this column.
     */
    void insertEmptyColumn(unsigned int index);

    /**
     * Inserts a column vector before the index'th column.
     *
     * @param index The function inserts the vector before this column.
     * @param vector The vector which will be inserted.
     */
    void insertColumn(unsigned int index, const Vector & vector);

    /**
     * Appends the vector after the last row.
     *
     * @param vector The function appends this vector.
     */
    void appendRow(const Vector & vector);

    /**
     * Appends the vector after the last column.
     *
     * @param vector The function appends this vector.
     */
    void appendColumn(const Vector & vector);

    /**
     * Inserts the vector before the first row.
     *
     * @param vector The function inserts this vector.
     */
    void prependRow(const Vector & vector);

    /**
     * Inserts the vector before the first column.
     *
     * @param vector The function inserts this vector.
     */
    void prependColumn(const Vector & vector);

    /**
     * Sets the element in row y and column x to value.
     *
     * @param y Row index.
     * @param x Column index.
     * @param value The new value of the element.
     */
    void set(unsigned int y, unsigned int x, Numerical::Double value);

    /**
     * Returns the value of the element in row y and column x.
     *
     * @param y Row index.
     * @param x Column index.
     * @return Value of element in row y and column x.
     */
    Numerical::Double get(unsigned int y, unsigned int x) const;

    /**
     * Scales the elements of matrix by lambda.
     *
     * @param lambda The function multiplies the elements of matrix with this
     * value.
     */
    void scale(Numerical::Double lambda);

    /**
     * Scales the elements of the index'th row.
     *
     * @param index This row will be scaled.
     * @param lambda The function scales by this value.
     */
    void scaleRow(unsigned int index, Numerical::Double lambda);

    /**
     * Scales the elements of the index'th column.
     *
     * @param index This column will be scaled.
     * @param lambda The function scales by this value.
     */
    void scaleColumn(unsigned int index, Numerical::Double lambda);

    // TODO: a kovetkezo 4 fuggvenyhez tesztelot irni
    /**
     * Scales the elements of the intex'th row. It multiplies only in the rowwise form.
     *
     * @param lambda The function scales by this value.
     */
    void scaleOnlyRowwise(unsigned int index, Numerical::Double lambda);

    /**
     * Scales the elements of the intex'th row. It multiplies only in the rowwise form.
     *
     * @param lambdas The function scales with the elements of this vector.
     */
    void scaleOnlyRowwiseLambdas(unsigned int index, const std::vector<Numerical::Double> & lambdas);

    /**
     * Scales the elements of the intex'th column. It multiplies only in the columnwise form.
     *
     * @param lambda The function scales by this value.
     */
    void scaleOnlyColumnwise(unsigned int index, Numerical::Double lambda);

    /**
     * Scales the elements of the intex'th column. It multiplies only in the columnwise form.
     *
     * @param lambdas The function scales with the elements of this vector.
     */
    void scaleOnlyColumnwiseLambdas(unsigned int index, const std::vector<Numerical::Double> & lambdas);

    /**
     * Replaces a zero value in row y and column x in the matrix.
     * Assumes that the original element is zero.
     *
     * @param y Row index.
     * @param x Column index.
     * @param value A nonzero value.
     */
    void setNewNonzero(unsigned int y, unsigned int x, Numerical::Double value);

    /**
     * Inverts the matrix.
     */
    void invert();

    /**
     * Returns the inverse of the matrix.
     *
     * @return The inverse of the matrix.
     */
    Matrix inverse() const;

    /**
     * Calculates the Matrix's determinant.
     * Result is 0.0 if matrix is not a sqaure-matrix.
     *
     * @return The determinant of the matrix.
     */
    Numerical::Double determinant() const;

    /**
     * Displays the Matrix in a human-readable format
     */
    void show() const;

    /**
     * Counts nonzero elements in the matrix
     *
     * @return number of counted nonzeros
     */
    unsigned int nonZeros() const;

    /**
     * Gives a value between 0 and 1 about the ratio of nonzero elements
     *
     * @return The ratio of nonzeros in the matrix
     */
    Numerical::Double density() const;

    /**
     * Returns a column vector yield by multiplying matrix with a column vector.
     * If the given right vector's length is not the same as column count of
     * the matrix (n), then it will be interpreted as an n-length column vector,
     * filled up by zeros if needed.
     *
     * @param rightVector The column vector used to multiply this matrix
     * @return a column vector  containing the result of multiplication
     */
    Vector operator*(const Vector& rightVector) const;

    /**
     * Creates a diagonal matrix from given vector.
     *
     * @param diagonal
     */
    static Matrix createDiagonalMatrix(const Vector& diagonal);

    /**
     * Creates a matrix from the row vector.
     *
     * @param row A row vector.
     * @return A row matrix.
     */
    static Matrix createRowMatrix(const Vector& row);

    /**
     * Creates a matrix from the column vector.
     *
     * @param column A column vector.
     * @return A column matrix.
     */
    static Matrix createColumnMatrix(const Vector& column);

    /**
     * Multiplies two matrices.
     *
     * @param other The right member of matrix product.
     * @return Product of two matrices.
     */
    Matrix operator*(const Matrix& other) const;

    /**
     * Subtracts the other matrix from the actual matrix object and returns the result.
     *
     * @param other The right member of subtraction.
     * @return The result matrix.
     */
    Matrix operator-(const Matrix& other) const;

    /**
     * Subtracts the other matrix from the actual one.
     *
     * @param other The function subtracts this matrix.
     */
    void operator-=(const Matrix& other);

    /**
     * Returns the sum of two matrices.
     *
     * @param other The right member of addition.
     * @return Sum of two matrices.
     */
    Matrix operator+(const Matrix& other) const;

    /**
     * Adds the other matrix to the actual one.
     *
     * @param other The function adds this matrix to the actual one.
     */
    void operator+=(const Matrix& other);

    /**
     * This function sorts the elements of sparse vectors in matrix by index.
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
     * @param gaussianJordan If this parameter is true, a Gaussian-Jordan
     * elimination is performed.
     * @return The number of row spaws throughout the Gaussian elimination.
     */
    int gaussianElimination(bool gaussianJordan = false);

    /**
     * Scales a row vector with the given multipliers.
     * Returns the sum of the absolute values of the elements, the squares of the
     * absolute values of the elements, and the maximal and minimal absolute values
     * of the elements.
     *
     * @param rowIndex The index of the row to be scaled.
     * @param multipliers Variable storing the scaling multipliers. Its dimension has to be
     * equal to the number of columns in the matrix.
     * @param lambda Base scaling multiplier which is applied to every element of the vector.
     * @param squareSumPtr Pointer to the variable storing the sum of the squares of
     * the absolute values of the elements.
     * @param minPtr Pointer to the variable storing the minimal absolute value of the
     * elements.
     * @param maxPtr Pointer to the variable storing the maximal absolute value of the
     * elements.
     * @return The sum of the absolute values of the elements.
     */
    Numerical::Double scaleRowAndGetResults(unsigned int rowIndex,
                                            const std::vector<Numerical::Double> & multipliers,
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
     * @param lambda Base scaling multiplier which is applied to every element of the vector.
     * @param squareSumPtr Pointer to the variable storing the sum of the squares of
     * the absolute values of the elements.
     * @param minPtr Pointer to the variable storing the minimal absolute value of the
     * elements.
     * @param maxPtr Pointer to the variable storing the maximal absolute value of the
     * elements.
     * @return The sum of the absolute values of the elements.
     */
    Numerical::Double scaleColumnAndGetResults(unsigned int columnIndex,
                                               const std::vector<Numerical::Double> & multipliers,
                                               Numerical::Double lambda,
                                               Numerical::Double * squareSumPtr,
                                               Numerical::Double * minPtr,
                                               Numerical::Double * maxPtr);

private:

    /**
     * Returns the determinant of the subMatrix.
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
     * Array of Vector pointers, stores pointers to the row vectors.
     * It has m_rowCount elements.
     */
    Vector ** m_rowWise;

    /**
     * Array of Vector pointers, stores pointers to the column vectors.
     * It has m_columncount elements.
     */
    Vector ** m_columnWise;

    /**
     * Number of elements of m_rowWise array.
     */
    unsigned int m_rowCount;

    /**
     * Number of elements of m_columnWise array.
     */
    unsigned int m_columnCount;

    /**
     * When the matrix is diagonal, it is true, otherwise false.
     */
    bool m_isDiagonal;

    /**
     * Copies the matrix.
     * This is a helper function of copy constructor and assignement operator.
     *
     * @param matrix The function copies this matrix.
     */
    void copy(const Matrix & matrix);

    /**
     * Initializes the matrix.
     * Creates rowCount rows and columnCount columns, and when the initVectors is true, initialises the vectors.
     *
     * @param rowCount Number of rows.
     * @param columnCount Number of columns.
     * @param initVectors When true, the function initializes the vectors.
     */
    void init(unsigned int rowCount, unsigned int columnCount, bool
              initVectors = true);


    /**
     * Resizes the vector array of the matrix, allowing to change the number of rows and columns in it.
     *
     * @param vectors Pointer to the vector array of the matrix.
     * @param vectorCount The number of vectors in the vector array.
     * @param newCount The new number of vectors in the vector array.
     * @param newLengths The new length of the vectors in the vector array.
     */
    static void resizeVectors(Vector ** & vectors, unsigned int vectorCount,
                              unsigned int newCount, unsigned int newLengths);

    /**
     * Removes a vector from rowWise and decreases rowCount.
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
    void removeVector(Vector ** & columnWise, unsigned int & columnCount,
                      Vector ** & rowWise, unsigned int & rowCount, unsigned int index);

    /**
     * Inserts a vector to rowWise, and increases rowCount.
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
    void insertVector(Vector ** columnWise, Vector ** & rowWise,
                      unsigned int & rowCount, unsigned int index, const Vector & vector);

    /**
     * Inserts an empty vector to rowWise, and increases rowCount.
     * Parameter references can be swapped for column insertion.
     *
     * @param columnWise The column-wise representation of the target matrix.
     * @param rowWise The row-wise representation of the target matrix.
     * @param rowCount The number of rows in the matrix.
     * @param columnCount The number of columns in the matrix.
     * @param index The index of the vector to be added.
     */
    void insertEmptyVector(Vector ** columnWise, Vector ** & rowWise,
                           unsigned int & rowCount, unsigned int columnCount, unsigned int index);

    /**
     * Returns the result of d times right matrix.
     *
     * @param d The scalar the matrix is to be multiplied with.
     * @param right The matrix to be multiplied.
     * @return The result matrix of the multiplication.
     */
    friend Matrix operator*(Numerical::Double d, const Matrix& right);

    /**
     * Adds d to each element of right matrix and returns the result matrix.
     *
     * @param d The scalar to be added to all elements of the matrix.
     * @param right The original matrix the operation is performed with.
     * @return The result matrix of the mass addition.
     */
    friend Matrix operator+(Numerical::Double d, const Matrix& right);

};

#endif // MATRIX_H
