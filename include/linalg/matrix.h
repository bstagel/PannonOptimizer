/**
 * @file matrix.h
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
 */
class Matrix
{
    friend class MatrixTestSuite;
    friend class ModelIO;
    friend class MpsModelBuilder;
public:
    /**
     * Constructor of the Matrix class. Creates a matrix filled with
     * zeros. When rowCount is given, and columnCount not, the matrix will
     * be square matrix.
     * 
     * @param rowCount Number of rows in matrix.
     * @param columnCount Number of columns in matrix.
     */
    Matrix(unsigned int rowCount = 0, unsigned int columnCount = 0);

    /**
     * Copy constructor.
     * 
     * @param original The constructor copies this matrix object.
     */
    Matrix(const Matrix & original);

    /**
     * Destructor, releases the matrix.
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
     * Resizes the matrix. When a dimension increases, the old elements
     * remains. The new elements will be zeros. When rowCount is given, and 
     * columnCount not, the matrix will be square matrix.
     * 
     * @param rowCount The new row number.
     * @param columnCount The new column number.
     */
    void resize(unsigned int rowCount = 0, unsigned int columnCount = 0);

    /**
     * Releases the matrix, and creates a new matrix with sizes given by
     * rowCount and columnCount. When initVectors is true, the vectors
     * will be initialized, otherwise not. 
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
     * Checks whether the two matrices are equals or not. Two matrices 
     * are equals when its dimensions, and the elements are the same.
     * 
     * @param other The other matrix.
     * @return True, when the matrices are equal, otherwise false.
     */
    bool operator==(const Matrix & other) const;

    /**
     * Checks whether the two matrices are equals or not. Two matrices 
     * are equals when its dimensions, and the elements are the same.
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
     * Removes the index'th column from the vector. The number of columns
     * decreases.
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
    void set(unsigned int y, unsigned int x, Numerical::Double value); // TODO: indexek cserejenek atgondolasa

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
     * Calculates the Matrix's determinant. Result is 0.0 if matrix is not a
     * sqaure-matrix.
     *
     * @return determinant
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
     * TODO: constructor?
     *
     * @param diagonal
     */
    static Matrix createDiagonalMatrix(const Vector& diagonal);

    /**
     * Creates a matrix from the row vector.
     * 
     * TODO: constructor?
     * 
     * @param row A row vector.
     * @return A row matrix.
     */
    static Matrix createRowMatrix(const Vector& row);

    /**
     * Creates a matrix from the column vector.
     * 
     * TODO: constructor?
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
     * Subtracts the other matrix from the actual matrix object
     * and returns the result.
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
     * Usuallay in sparse vectors the elements are not sorted ascending
     * order by index. This function sorts the elements of sparse vectors 
     * in matrix by index.
     * 
     */
    void sortVectors() const;

    void startRowScaling();

    void finishRowScaling();

    void startColumnScaling();

    void finishColumnScaling();
    /**
     * 
     */
    int gaussianElimination(bool gaussianJordan = false);


private:

    /**
     * Returns the determinant of the subMatrix.
     * 
     * @param subMatrix The function computes the determinant of this matrix.
     * @return Determinant of subMatrix.
     */
    Numerical::Double determinant(const Matrix& subMatrix) const;

    bool m_fastRowScaling;

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
     * Copies the matrix. This is a helper function of copy constructor
     * and assignement operator.
     * 
     * @param matrix The function copies this matrix.
     */
    void copy(const Matrix & matrix);

    /**
     * Initializes the matrix. Creates rowCount rows and columnCount 
     * columns, and when the initVectors is true, initialises the vectors.
     * 
     * @param rowCount Number of rows.
     * @param columnCount Number of columns.
     * @param initVectors When true, the function initializes the vectors.
     */
    void init(unsigned int rowCount, unsigned int columnCount, bool
        initVectors = true);


    static void resizeVectors(Vector ** & vectors, unsigned int vectorCount,
        unsigned int newCount, unsigned int newLengths);

    /**
     * Helper function of removeRow and removeColumn. Removes a vector
     * from m_rowWise or m_columnWise and decreases m_rowCount or
     * m_columnCount.
     * 
     * @param columnWise
     * @param columnCount
     * @param rowWise
     * @param rowCount
     * @param index
     */
    void removeVector(Vector ** & columnWise, unsigned int & columnCount,
        Vector ** & rowWise, unsigned int & rowCount, unsigned int index);

    /**
     * 
     * @param columnWise
     * @param rowWise
     * @param rowCount
     * @param index
     * @param vector
     */
    void insertVector(Vector ** columnWise, Vector ** & rowWise,
        unsigned int & rowCount, unsigned int index, const Vector & vector);

    /**
     * 
     * @param columnWise
     * @param rowWise
     * @param rowCount
     * @param columnCount
     * @param index
     */
    void insertEmptyVector(Vector ** columnWise, Vector ** & rowWise,
        unsigned int & rowCount, unsigned int columnCount, unsigned int index);

    /**
     * Returns the result of d times right matrix.
     * 
     * @param d 
     * @param right
     * @return 
     */
    friend Matrix operator*(double d, const Matrix& right);

    /**
     * Adds d to each element of right matrix and returns the result matrix.
     * 
     * @param d
     * @param right
     * @return 
     */
    friend Matrix operator+(double d, const Matrix& right);

};

#endif // MATRIX_H
