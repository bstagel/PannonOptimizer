/**
 * @file vector.h This file contains the API of the Vector class.
 * @author smidla
 */

#ifndef VECTOR_H
#define VECTOR_H

#include <globals.h>

#include <iostream>
#include <cmath>
#include <debug.h>
#include <vector>

#include <utils/numerical.h>

/* Forward declaration */
class Matrix;

/**
 * This is a reference to the run-time parameter "elbowroom".
 * See LinalgParameterHandler for details.
 */
extern int ELBOWROOM;

/**
 * This is a reference to the run-time parameter "sparsity_ratio".
 * See LinalgParameterHandler for details.
 */
extern double SPARSITY_RATIO;


//#define CHECK check(__FUNCTION__);
#define CHECK

#ifdef ANALYSE_DOT_PRODUCT
    extern std::vector<int> diffs;
#endif

/**
* Class for storing dense and sparse vectors.
* The class can switch between vector representation methods.
*
* @class Vector
*/

class Vector
{
#define IN_VECTOR

    friend class ModelIO;
    friend class MpsModelBuilder;
    friend class PfiBasis;
    friend class LuBasis;
    friend class VectorTestSuite;
public:

    /**
     * Displays the Vector in a human-readable format
     */
    void show() const;

    /**
     * Checks the vector. You can implement arbitrary checking code for debug.
     */
    void check(const char *) const
    {

    }

    /**
     * Indicates the storing method of vector.
     */
    enum VECTOR_TYPE
    {
        SPARSE_VECTOR, DENSE_VECTOR, DEFAULT_VECTOR_TYPE
    };

    class CommonIterator;
    class Iterator;
    class NonzeroIterator;

#include <linalg/vector_iterator.h>

    /**
     * This constructor initializes a vector, with dimension given by parameter.
     * When the sparsity threshold is higher than the number of nonzeros, the
     * vector will be sparse vector, and m_data array is initialized for size
     * ELBOWROOM. Logically, the vector contains dimension piece of zeros.
     * Increases sm_fullLenghtReferenceCounter for garbage collector mechanism.
     *
     * Complexity: O(1)
     *
     * @constructor
     * @param dimension Describes the dimension of vector
     * @param type The representation type of the vector to be initialized.
     */
    explicit Vector(unsigned int dimension = 0, VECTOR_TYPE type = DEFAULT_VECTOR_TYPE);

    /**
     * This constructor is used to initialize a vector very fast.
     * The vector is not prepared for data after initializing with this constructor.
     *
     * Complexity: O(1)
     *
     * @constructor
     */
    explicit Vector(void *, void *, void *);
    /**
     * Releases the vector.
     * Decreases sm_fullLenghtReferenceCounter, and when this variable reaches zero,
     * the descructor releases the sm_fullLengthVector.
     *
     * Complexity: O(1)
     *
     * @destructor
     */
    virtual ~Vector();
    /**
     * Copy constructor of Vector class. Copies the vector given by parameter, and
     * increases sm_fullLenghtReferenceCounter.
     *
     * Complexity: O(n)
     *
     * @constructor
     * @param original The original vector, the function creates a copy from this
     * object
     */
    Vector(const Vector & original);

    /**
     * Constructor of Vector class. Copies the vector given by parameter, and
     * increases sm_fullLenghtReferenceCounter. The elements of result vector
     * are multiplied by lambda.
     *
     * Complexity: O(n)
     *
     * @constructor
     * @param original The original vector, the function creates a copy from this
     * object
     * @param lambda The elements of result vector are multiplied by lambda.
     */
    Vector(const Vector & original, Numerical::Double lambda);

    /**
     * Sets each nonzero to zero in the vector.
     */
    void clear();

    /**
     * Fills the vector with the value.
     *
     * @param value The new value of every components in the vector.
     */
    void fill(Numerical::Double value);

    /**
     * Resizes the vector.
     * If the new length is greater than the old dimension size, the old elements will remain.
     *
     * @param length The new length of the vector.
     */
    void resize(unsigned int length);

    /**
     * Returns the maximal absolute value of the elements.
     *
     * @return The maximal absolute value of the elements.
     */
    Numerical::Double absMaxElement() const;

    /**
     * Calculates the sum of the absolute values of the elements.
     * Also gives a pointer to the squares of the absolute values of the elements.
     *
     * @param squareSumPtr Pointer to the variable storing the sum of the squares of
     * the absolute values of the elements.
     * @return The sum of the absolute values of the elements.
     */
    Numerical::Double absMaxSums(Numerical::Double * squareSumPtr) const;

    /**
     * Calculates the sum of the absolute values of the elements.
     * Gives pointers to the squares of the absolute values of the elements, as well as the
     * maximal and minimal absolute values of the elements.
     *
     * @see absMaxSums()
     * @param squareSumPtr Pointer to the variable storing the sum of the squares of
     * the absolute values of the elements.
     * @param minPtr Pointer to the variable storing the minimal absolute value of the
     * elements.
     * @param maxPtr Pointer to the variable storing the maximal absolute value of the
     * elements.
     * @return The sum of the absolute values of the elements.
     */
    Numerical::Double absMaxSumsAndMinMax(Numerical::Double * squareSumPtr,
                                          Numerical::Double * minPtr,
                                          Numerical::Double * maxPtr) const;

    /**
     * Scales the vector with the given multipliers.
     * Returns the sum of the absolute values of the elements, the squares of the absolute
     * values of the elements, and the maximal and minimal absolute values of the elements.
     *
     * @see absMaxSumsAndMinMax()
     * @param multipliers Variable storing the scaling multipliers. Its dimension has to be
     * equal to the dimension of the vector.
     * @param lambda Base scaling multiplier which is applied to every element of the vector.
     * @param squareSumPtr Pointer to the variable storing the sum of the squares of
     * the absolute values of the elements.
     * @param minPtr Pointer to the variable storing the minimal absolute value of the
     * elements.
     * @param maxPtr Pointer to the variable storing the maximal absolute value of the
     * elements.
     * @return The sum of the absolute values of the elements.
     */
    Numerical::Double scaleAndGetResults(const std::vector<Numerical::Double> & multipliers,
                                         Numerical::Double lambda,
                                         Numerical::Double * squareSumPtr,
                                         Numerical::Double * minPtr,
                                         Numerical::Double * maxPtr);

    /**
     * The function set logically the index'th element of vector for value.
     * Example:
     * <pre>
     * ELBOWROOM = 5
     * m_vectorType = DENSE_VECTOR
     * m_capacity = 10
     * m_size = 5
     * m_dimension = 5
     * m_nonZeros = 4
     * m_sparsityRatio = 0.5
     * m_sparsityThreshold = 3
     * m_data = {1.0, 2.0, 5.0, 0.0, 4.0, | X, X, X, X, X}
     * m_index = NULL
     *
     * set(2, 3)
     * m_data = {1.0, 0.0, 3.0, 0.0, 4.0, | X, X, X, X, X}
     * set(1, 0)
     * m_data = {1.0, 0.0, 5.0, 0.0, 4.0, | X, X, X, X, X}
     * m_nonZeros = 3

     * set(0, 0)
     * // switching from dense to sparse
     * m_vectorType = SPARSE_VECTOR
     * m_capacity = 7
     * m_size = 2
     * m_dimension = 5
     * m_nonZeros = 2
     * m_sparsityRatio = 0.5
     * m_sparsityThreshold = 3
     * m_data = {5.0, 4.0 | X, X, X, X, X}
     * m_index = {2, 4, | X, X, X, X, X}
     *
     * set(2, 0)
     * m_data = {4.0 | X, X, X, X, X, X}
     * m_index = {4, | X, X, X, X, X, X}
     * m_size = 1
     * m_nonZeros = 1
     * -----------------------
     * </pre>
     *
     * <table>
     * <tr align="center">
     *   <th>vector type</th>
     *   <th>value</th>
     *   <th>index'th element</th>
     *   <th>number of non zeros</th>
     *   <th>complexity</th>
     * </tr>
     * <tr align="center">
     *   <td> dense </td>
     *   <td> nonzero </td>
     *   <td> - </td>
     *   <td> - </td>
     *   <td> O(1) </td>
     * </tr>
     * <tr align="center">
     *   <td> dense </td>
     *   <td> zero </td>
     *   <td> zero </td>
     *   <td> - </td>
     *   <td> O(1) </td>
     * </tr>
     * <tr align="center">
     *   <td> dense </td>
     *   <td> zero </td>
     *   <td> nonzero </td>
     *   <td> <pre> > m_sparsityThreshold </pre> </td>
     *   <td> O(1) </td>
     * </tr>
     * <tr align="center">
     *   <td> dense </td>
     *   <td> zero </td>
     *   <td> nonzero </td>
     *   <td> <pre> >= m_sparsityThreshold </pre> </td>
     *   <td> O(n) </td>
     * </tr>
     * <tr align="center">
     *   <td> sparse </td>
     *   <td> - </td>
     *   <td> - </td>
     *   <td> - </td>
     *   <td> O(n) </td>
     * </tr>
     * </table>
     *
     * @param index The function changes the index'th element of vector
     * @param value The new value of the index'th element of vector
     */
    void set(unsigned int index, Numerical::Double value);

    /**
     * Similar to the set, but it assumes that the index-th element of vector is not zero.
     *
     * @see set()
     * @param index The function changes the index'th element of vector
     * @param value The new value of the index'th element of vector
     */
    void change(unsigned int index, Numerical::Double value);

    /**
     * Multiplies and element of the vector by a given value.
     *
     * @param index The index of the element to be scaled.
     * @param lambda The multiplier which is applied to the element.
     */
    void scaleElementBy(unsigned int index, Numerical::Double lambda);

    /**
     * Returns a row vector yield by multiplying vector with a matrix.
     * If the given right matrix's row count (m) is not the same as the length of
     * the vector, then this vector will be interpreted as an m-length row vector,
     * filled up by zeros if needed.
     *
     * @param matrix The matrix used to multiply this vector
     * @return a row vector containing the result of multiplication
     */
    Vector operator*(const Matrix& matrix) const;

    /**
     * Returns a vector multiplied by the given number
     *
     * @see scaleBy()
     * @param m The multiplicator
     * @return a vector containing the result of multiplication
     */
    Vector operator*(Numerical::Double m) const;

    /**
     * Returns the difference vector yield by subtracting another vector from a vector.
     *
     * @param vector The vector subtracted from this vector
     * @return a vector containing the result of subtraction
     */
    Vector operator-(const Vector& vector) const;

    /**
     * Returns the sum vector yield by adding another vector to a vector.
     *
     * @param vector The vector added to this vector
     * @return a vector containing the result of adding
     */
    Vector operator+(const Vector& vector) const;

    /**
     * Returns with the value of index'th element of vector.
     *
     * <table>
     * <tr align="center">
     *   <th>vector type</th>
     *   <th>complexity</th>
     * </tr>
     * <tr align="center">
     *   <td> dense </td>
     *   <td> O(1) </td>
     * </tr>
     * <tr align="center">
     *   <td> sparse </td>
     *   <td> O(n) </td>
     * </tr>
     * </table>
     *
     * @param index The index of the element of we need
     * @return The index'th element of vector
     */
    const Numerical::Double &at(unsigned int index) const;

    /**
     * Returns with the pointer of the index'th dense element.
     * Do not use this function on a sparse vector!
     * Complexity: O(1)
     *
     * @param index Index of a dense element.
     * @return Pointer of the index'th dense element.
     */
    const Numerical::Double * getDenseElementPointer(unsigned int index) const;

    /**
     * Returns with the logical length of vector.
     *
     * Complexity: O(1)
     *
     * @return Logical lenght of vector, which equals to m_dimension.
     */
    unsigned int length() const;
    /**
     * Returns with the capacity of vector.
     *
     * Complexity: O(1)
     *
     * @return Capacity of vector
     */
    unsigned int capacity() const;
    /**
     * The function returns with the index of last stored element.
     *
     * Complexity: O(n)
     *
     * @return The index of last non zero element
     */
    unsigned int maxIndex() const;
    /**
     * The function muliplies the vector by lambda.
     * When lambda is zero, the function converts the vector to an empty sparse vector.
     * Example:
     * <pre>
     * ELBOWROOM = 5
     * m_vectorType = DENSE_VECTOR
     * m_capacity = 10
     * m_size = 5
     * m_dimension = 5
     * m_nonZeros = 4
     * m_sparsityRatio = 0.5
     * m_sparsityThreshold = 3
     * m_data = {1.0, 2.0, 5.0, 0.0, 4.0, | X, X, X, X, X}
     * m_index = NULL
     *
     * scaleBy(2)
     * m_data = {2.0, 4.0, 10.0, 0.0, 8.0, | X, X, X, X, X}
     *
     * scaleBy(0)
     * m_vectorType = SPARSE_VECTOR
     * m_capacity = 5
     * m_size = 0
     * m_dimension = 5
     * m_nonZeros = 0
     * m_sparsityRatio = 0.5
     * m_sparsityThreshold = 3
     * m_data = { | X, X, X, X, X}
     * m_index = { | X, X, X, X, X}
     * </pre>
     *
     * <table>
     * <tr align="center">
     *   <th>lambda</th>
     *   <th>complexity</th>
     * </tr>
     * <tr align="center">
     *   <td> nonzero </td>
     *   <td> O(n) </td>
     * </tr>
     * <tr align="center">
     *   <td> zero </td>
     *   <td> O(1) </td>
     * </tr>
     * </table>
     *
     * @param lambda The value which with the elements of vector are multiplied
     * @return Reference of the vector
     */
    Vector & scaleBy(Numerical::Double lambda);
    /**
     * Returns with value of dot product between 2 vectors.
     *
     * <table>
     * <tr align="center">
     *   <th> first vector </th>
     *   <th> second vector </th>
     *   <th> complexity </th>
     * </tr>
     * <tr align="center">
     *   <td> null vector </td>
     *   <td> null vector </td>
     *   <td> O(1) </td>
     * </tr>
     * <tr align="center">
     *   <td> null vector </td>
     *   <td> not null vector </td>
     *   <td> O(1) </td>
     * </tr>
     * <tr align="center">
     *   <td> not null vector </td>
     *   <td> null vector </td>
     *   <td> O(1) </td>
     * </tr>
     * <tr align="center">
     *   <td> not null vector </td>
     *   <td> not null vector </td>
     *   <td> O(n) </td>
     * </tr>
     * </table>
     *
     * @param vector The vector which with the operation have to be performed
     * @return Value of dot product
     */
    Numerical::Double dotProduct(const Vector & vector, bool stableAddAbs = true, bool stableAddRel = true) const;

    /**
     * Calculates the Euclidean norm of the vector.
     *
     * @return Value of the Euclidean norm of the vector.
     */
    Numerical::Double euclidNorm() const;

    Numerical::Double l1Norm() const;

    /**
     * The function performs the next operation: a = a + lambda * b.
     * a and b are Vector type objects.
     *
     * @param lambda Multiplier of parameter vector
     * @param vector The function adds this vector to the other vector
     * @return The function returns with the reference of modified vector
     */
    Vector & addVector(Numerical::Double lambda, const Vector & vector,
                       Numerical::ADD_TYPE addType = Numerical::ADD_ABS_REL);

    /**
     * Performs an elementary forward transformation on the vector.
     * This operation is the multiplication of the vector with a special matrix.
     *
     * @param eta The eta vector of the elementary transformation matrix.
     * @param pivot The index of the eta vector in the elementary transformation
     * matrix.
     * @return Reference of the vector.
     */
    Vector & elementaryFtran(const Vector & eta, unsigned int pivot);

    /**
     * Performs an elementary backward transformation on the vector.
     * This operation is the multiplication of the vector with a special matrix.
     *
     * @param eta The eta vector of the elementary transformation matrix.
     * @param pivot The index of the eta vector in the elementary transformation
     * matrix
     * @return Reference of the vector.
     */
    Vector & elementaryBtran(const Vector & eta, unsigned int pivot);

    /**
     * Removes an element from the vector.
     * This operation decreases the dimension of the vector by 1.
     *
     * @param index The index of the element to be removed.
     */
    void removeElement(unsigned int index);

    /**
     * Inserts a new element to the vector.
     * This operation increases the dimension of the vector by 1.
     *
     * @param index The index of the element to be added.
     * @param value The value of the element to be added.
     */
    void insertElement(unsigned int index, Numerical::Double value);

    /**
     * Appends a new element to the end of the vector.
     * This operation increases the dimension of the vector by 1.
     *
     * @see insertElement()
     * @param value The value of the element to be appended.
     */
    void append(Numerical::Double value);

    /**
     * Appends a new vector to the end of the vector.
     * This operation increases the dimension of the vector by the dimension of the new vector.
     *
     * @param vector The new vector to be appended.
     */
    void appendVector(const Vector & vector);

    /**
     * Reinitializes the vector to the given dimension.
     * This operation also reallocates the data storage of the vector.
     *
     * @param dimension The dimension of the reinitialized vector.
     */
    void reInit(unsigned int dimension);

    /**
     * Changes the sparsity ratio of the vector.
     *
     * @see SPARSITY_RATIO
     * @param ratio The value of the new sparsity ratio.
     */
    void setSparsityRatio(Numerical::Double ratio);

    /**
     * @see SPARSITY_RATIO
     * @return The value of the sparsity ratio of the vector.
     */
    Numerical::Double getSparsityRatio() const;

    /**
     * Changes a previously zero value of the vector to a new, nonzero value.
     *
     * @param index The index of the element to be changed.
     * @param value The value of the element to be changed.
     */
    void setNewNonzero(unsigned int index, Numerical::Double value);

    /**
     * Makes the vector equal to a given vector.
     *
     * @param vector Reference to the vector.
     */
    Vector & operator=(const Vector & vector);

    /**
     * Returns true if the two vectors are equal.
     *
     * @param vector Reference to the other vector.
     * @return Logical equivalence of the two vectors.
     */
    bool operator==(const Vector & vector) const;

    /**
     * Returns false if the two vectors are equal.
     *
     * @param vector Reference to the other vector.
     * @return Complement of logical equivalence of the two vectors.
     */
    inline bool operator!=(const Vector & vector) const
    {
        return !(*this == vector);
    }

    /**
     * Returns the current representation type of the vector.
     *
     * @see m_vectorType
     * @return The current representation type of the vector.
     */
    inline VECTOR_TYPE getType() const
    {
        return m_vectorType;
    }

    /**
     * Returns the number of nonzero elements in the vector.
     *
     * @see m_nonZeros
     * @return The number of nonzero elements in the vector.
     */
    inline unsigned int nonZeros() const
    {
        return m_nonZeros;
    }

    /**
     * Prints the properties of the vector.
     *
     * @param os Reference of the output stream to be printed on.
     * @param vector Reference of the vector.
     * @return Reference of the output stream with the printed data.
     */
    friend std::ostream & operator<<(std::ostream & os, const Vector & vector);

    /**
     * Returns an Iterator pointing to the first element of the vector.
     *
     * @see Iterator
     * @return Iterator pointing to the first element of the vector.
     */
    inline Iterator begin() const
    {
        return Iterator(m_data, m_dataEnd, m_data, m_index);
    }

    /**
     * Returns a NonzeroIterator pointing to the first element of the vector.
     *
     * @see NonzeroIterator
     * @return NonzeroIterator pointing to the first element of the vector.
     */
    inline NonzeroIterator beginNonzero() const
    {
        //CHECK;
        if (m_size == 0) {
            return endNonzero();
        }
        return NonzeroIterator(m_data, m_dataEnd, m_data, m_index,
                               m_data < m_dataEnd);
    }

    /**
     * Returns an Iterator pointing to the end of the vector.
     *
     * @see Iterator
     * @return Iterator pointing to the end of the vector.
     */
    inline Iterator end() const
    {
        return Iterator(m_data, m_dataEnd, m_dataEnd,
                        m_index ? m_index + m_size : 0);
    }

    /**
     * Returns a NonzeroIterator pointing to the end of the vector.
     *
     * @see NonzeroIterator
     * @return NonzeroIterator pointing to the end of the vector.
     */
    inline NonzeroIterator endNonzero() const
    {
        return NonzeroIterator(m_data, m_dataEnd, m_dataEnd,
                               m_index ? m_index + m_size : 0, false);
    }

    /**
     * Initializes the vector.
     * The dimension, the number of nonzeros, and the sparsity ratio are customized.
     *
     * @see SPARSITY_RATIO
     * @param nonZeros The amount of nonzero elements in the vector.
     * @param dimension The dimension of the vector.
     * @param ratio The sparsity ratio of the vector.
     */
    inline void prepareForData(const unsigned int nonZeros, const unsigned int dimension, Numerical::Double ratio = -1)
    {
        freeData(m_data);
        freeIndex(m_index);
        //delete [] m_data;
        //delete [] m_index;
        /*if (ratio == -1) {
            m_sparsityRatio = SPARSITY_RATIO;
        } else {
            m_sparsityRatio = ratio;
        }*/

        if (ratio >= 0.0) {
            m_sparsityRatio = ratio;
        }

        m_dimension = dimension;
        m_sparsityThreshold = (unsigned int) Numerical::round(Numerical::Double(m_dimension) * m_sparsityRatio);
        if (nonZeros < m_sparsityThreshold) {
            m_vectorType = SPARSE_VECTOR;
            m_index = allocateIndex(nonZeros + ELBOWROOM);
            m_data = allocateData(nonZeros + ELBOWROOM);
            m_dataEnd = m_data;
            m_capacity = nonZeros + ELBOWROOM;
            m_nonZeros = nonZeros;
            m_size = nonZeros;
            m_sorted = false;
        } else {
            m_vectorType = DENSE_VECTOR;
            m_index = 0;
            m_data = allocateData(dimension + ELBOWROOM);
            m_dataEnd = m_data + dimension;
            m_capacity = dimension + ELBOWROOM;
            m_nonZeros = nonZeros;
            m_size = dimension;
            m_sorted = true;
            Numerical::Double * ptr = m_data;
            while (ptr < m_dataEnd) {
                *ptr = 0;
                ptr++;
            }
        }
    }

    /**
     * Sets a previously zero element to a given value.
     *
     * @see setNewNonzero()
     * @param value The new value of the element.
     * @param index The index of the element.
     */
    inline void newNonZero(const Numerical::Double value, const unsigned int index)
    {
        if (m_vectorType == DENSE_VECTOR) {
            m_data[index] = value;
        } else {
            *m_dataEnd = value;
            m_index[ m_dataEnd - m_data ] = index;
            m_dataEnd++;
        }
    }

    /**
     * Returns a pointer to the data array of the vector.
     *
     * @see m_data
     * @return Pointer to the data array of the vector.
     */
    inline const Numerical::Double * getData() const
    {
        return m_data;
    }

    /**
     * Returns a pointer to the index array of the vector.
     *
     * @see m_index
     * @return Pointer to the index array of the vector.
     */
    inline const unsigned int * getIndex() const
    {
        return m_index;
    }

    /**
     * Sorts the elements in the vector by index.
     * This function uses the most efficient sorting method.
     */
    void sortElements() const;

    /**
     * Returns true if the vector is sorted.
     *
     * @see m_sorted
     * @return Boolean describing if the vector is sorted.
     */
    inline bool isSorted() {return m_sorted;}

    /**
     * Scales the vector by the given vector of lambdas.
     *
     * @param lambdas The vector of lambdas.
     */
    void scaleByLambdas(const std::vector<Numerical::Double> & lambdas);

    /**
     * Swaps the content of vector1 and vector2. The complexity is O(1).
     *
     * @param vector1 The first vector to be swapped.
     * @param vector2 The second vector to be swapped.
     */
    static void swap(Vector * vector1, Vector * vector2);

protected:
    /**
     * Describes the type of vector.
     * When the value of m_vectorType is DENSE_VECTOR
     * the vector represents a dense vector. In this case m_size = m_dimension,
     * and zeros are stored also in m_data. The m_index is NULL pointer, i.e. the
     * indices are omitted. When the value of m_vectorType is SPARSE_VECTOR,
     * the vector represents a sparse vector, and m_size describes the size of
     * used part of m_index and m_data arrays.
     */
    VECTOR_TYPE m_vectorType;

    /**
     * Describes the size of used part of m_data and m_index arrays.
     */
    unsigned int m_size; // dense esetben megegyezik az m_dimension-al

    /**
     * Describes the dimension of vector.
     * The function length() returns with value this data member.
     */
    unsigned int m_dimension;

    /**
     * This variable decsribes the real size of m_data (and at sparse vector m_index also) array(s).
     */
    unsigned int m_capacity;

    /**
     * This array stores elements of vector.
     */
    Numerical::Double * m_data;

    /**
     * This array stores indices of non zeros in sparse vector.
     */
    unsigned int * m_index;

    /**
     * Points to the end of used part of m_data array.
     */
    Numerical::Double * m_dataEnd;

    /**
     * Describes the number of non zero elements in vector.
     * This variable is updated when the elements of vector are changed.
     */
    unsigned int m_nonZeros;

    /**
     * Describes the maximum number of nonzeros in sparse representation.
     * This variable equals with (unsigned int) round(m_dimension * m_sparsityRatio),
     * and changed by insert / remove operations.
     *
     * @see m_sparsityRatio
     */
    unsigned int m_sparsityThreshold;

    /**
     * Describes the maximum sparsity ratio.
     * When the ratio of non nozeros and dimension of vector grows above this value,
     * the vectro representation switches to dense.
     *
     * @see m_sparsityThreshold
     */
    Numerical::Double m_sparsityRatio;

    /**
     * When m_sorted is true, the sparse elemnets are sorted by ascending order.
     */
    mutable bool m_sorted;

    /**
     * Temporary vector for linear time dot product and add operations.
     */
    THREAD_STATIC_DECL Numerical::Double * sm_fullLengthVector;

    /**
     * Describes the size of sm_fullLengthVector.
     * The sm_fullLengthVector is not released after dot product or add operations,
     * because memory allocation is time consuming. When an operation needs bigger
     * array, it allocates a new vector.
     *
     * @see sm_fullLengthVector
     */
    THREAD_STATIC_DECL unsigned int sm_fullLengthVectorLenght;

    /**
     * Describes how many Vector type objects exists.
     * It is important for releasing sm_fullLengthVector. When this variable is zero, the program
     * releases sm_fullLengthVector automatically.
     *
     * @see sm_fullLengthVector
     */
    THREAD_STATIC_DECL unsigned int sm_fullLenghtReferenceCounter;

    /**
     * Temporary vector for the counting sort vector sorting algorithm.
     *
     * @see countingSort
     */
    THREAD_STATIC_DECL unsigned long * sm_countingSortBitVector;

    /**
     * Describes the size of sm_countingSortBitVector.
     * The sm_countingSortBitVector is not released after dot
     * product or add operations, because memory allocation
     * is time consuming. When an operation needs bigger array, it allocates
     * a new vector.
     *
     * @see sm_countingSortBitVector
     */
    THREAD_STATIC_DECL unsigned int sm_countingSortBitVectorLength;

    /**
     * Initializes the vector with the given dimension.
     *
     * @param dimension The dimension of the vector.
     */
    void init(unsigned int dimension);

    /**
     * Allocates the data array of the vector with the given capacity.
     *
     * @param capacity The capacity of the vector.
     * @return Pointer to the allocated data array of the vector.
     */
    static Numerical::Double * allocateData(unsigned int capacity);

    /**
     * Allocates the index array of the vector with the given capacity.
     *
     * @param capacity The capacity of the vector.
     * @return  Pointer to the allocated index array of the vector.
     */
    static unsigned int * allocateIndex(unsigned int capacity);

    /**
     * Frees the given data array.
     *
     * @param data Reference of the pointer to the data array.
     */
    static void freeData(Numerical::Double * & data);

    /**
     * Frees the given index array.
     *
     * @param index Reference of the pointer to the index array.
     */
    static void freeIndex(unsigned int * & index);

    /**
     * Resizes a dense vector.
     *
     * @see ELBOWROOM
     * @param size The new dimension of the vector.
     * @param elbowroom The elbowroom of the vector.
     */
    void resizeDense(unsigned int size, unsigned int elbowroom);

    /**
     * Resizes a sparse vector.
     *
     * @param capacity The new capacity of the vector.
     */
    void resizeSparse(unsigned int capacity);

    /**
     * Switches the representation type of the vector from dense to sparse.
     *
     * @see VECTOR_TYPE
     */
    void denseToSparse();

    /**
     * Switches the representation type of the vector from sparse to dense.
     *
     * @see VECTOR_TYPE
     */
    void sparseToDense();

    /**
     * The function copies the original vector.
     * It is supposed that the vector is uninitialized or
     * the m_data and m_index are released.
     *
     * Complexity: O(n)
     *
     * @param original The original vector, the function creates a copy from this
     * object
     */
    void copy(const Vector & original, bool reallocate = true);

    /**
     * Helper function for operator+ and addVector, executing the dense vector-dense vector addition.
     *
     * @param lambda Multiplier of parameter vector
     * @param vector The function adds this vector to the other vector
     */
    void addDenseToDense(Numerical::Double lambda, const Vector & vector,
                         Numerical::ADD_TYPE addType);

    /**
     * Helper function for operator+ and addVector, executing the dense vector-sparse vector addition.
     *
     * @param lambda Multiplier of parameter vector
     * @param vector The function adds this vector to the other vector
     */
    void addDenseToSparse(Numerical::Double lambda,
                          const Vector & vector,
                          Numerical::ADD_TYPE addType);

    /**
     * Helper function for operator+ and addVector, executing the sparse vector-dense vector addition.
     *
     * @param lambda Multiplier of parameter vector
     * @param vector The function adds this vector to the other vector
     */
    void addSparseToDense(Numerical::Double lambda,
                          const Vector & vector,
                          Numerical::ADD_TYPE addType);

    /**
     * Helper function for operator+ and addVector, executing the sparse vector-sparse vector addition.
     *
     * @param lambda Multiplier of parameter vector
     * @param vector The function adds this vector to the other vector
     */
    void addSparseToSparse(Numerical::Double lambda,
                           const Vector & vector,
                           Numerical::ADD_TYPE addType);

    /**
     * Helper function returning the index'th element of a sparse vector using linear search.
     *
     * @param index The index of the element to be returned.
     * @return The index'th element of the vector.
     */
    Numerical::Double * getElementSparseLinear(unsigned int index) const;

    /**
     * Helper function returning the index'th element of a sparse vector using binary search.
     *
     * @param index The index of the element to be returned.
     * @return The index'th element of the vector.
     */
    Numerical::Double * getElementSparseBinary(unsigned int index) const;

    /**
     * Helper function returning the index'th element of a sparse vector using the more efficient method.
     *
     * @param index The index of the element to be returned.
     * @return The index'th element of the vector.
     */
    Numerical::Double * getElementSparse(unsigned int index) const;

    /**
     * Helper function adding a new element to a sparse vector.
     *
     * @param index The index of the new element to be added.
     * @param value The value of the new element to be added.
     */
    void addNewElementSparse(unsigned int index, Numerical::Double value);

    /**
     * Helper function removing an element from a sparse vector.
     *
     * @param index The index of the element to be removed.
     */
    void removeElementSparse(unsigned int index);

    /**
     * Sorts the elements of the vector by index, using counting sort.
     *
     * @see sortElements()
     */
    void countingSort() const;

    /**
     * Sorts the elements of the vector by index, using heap sort.
     *
     * @see sortElements()
     */
    void heapSort() const;

    /**
     * Sorts the elements of the vector by index, using selection sort.
     *
     * @see sortElements()
     */
    void selectionSort() const;

    /**
     * Sorts the elements of the vector by index, using insertion sort.
     *
     * @see sortElements()
     */
    void insertionSort() const;

    // dense -> sparse
    /**
     * Helper function converting a dense vector data array to sparse data and index arrays.
     *
     * @param denseVector Pointer to the dense data array.
     * @param sparseVector Pointer to the sparse data array.
     * @param indexVector Pointer to the index array.
     * @param denseLength Length of the dense data array.
     * @return Pointer to the end of the index array.
     */
    static unsigned int gather(Numerical::Double * denseVector, Numerical::Double * sparseVector,
                               unsigned int * indexVector, unsigned int denseLength);


    // sparse -> dense
    /**
     * Helper function creating a dense vector data array from a given sparse vector.
     *
     * @param denseVector Pointer to the dense data array.
     * @param denseLength Length of the dense array.
     * @param sparseVector Reference of the sparse vector to be converted.
     */
    static void scatter(Numerical::Double * & denseVector, unsigned int & denseLength,
                        const Vector& sparseVector);

    //    // sparse -> dense
    //    /**
    //     * @param denseVector
    //     * @param denseLength
    //     * @param sparseVector
    //     * @param index
    //     * @param sparseLength
    //     * @param sparseMaxIndex
    //     */
    //    static void scatter(Numerical::Double * & denseVector, unsigned int & denseLength,
    //        Numerical::Double * sparseVector, unsigned int * index,
    //        unsigned int sparseLength, unsigned int sparseMaxIndex);

    // sparse -> dense
    /**
     * Helper function scattering a dense data array into sparse data and index arrays.
     * It gives a pointer to a specific element.
     *
     * @param denseVector Reference of the pointer to the dense data array.
     * @param denseLength The length of the dense data array.
     * @param sparseVector Pointer to the sparse data array.
     * @param index Pointer to the sparse index array.
     * @param sparseLength The length of the sparse data and index arrays.
     * @param sparseMaxIndex The maximal index the vector contains.
     * @param pivot The index of the element to be returned.
     * @return Pointer to the pivot'th element in the sparse data array.
     */
    static Numerical::Double * scatterWithPivot(Numerical::Double * & denseVector, unsigned int & denseLength,
                                                Numerical::Double * sparseVector, unsigned int * index,
                                                unsigned int sparseLength, unsigned int sparseMaxIndex, unsigned int pivot);


    /**
     * Clears the specified temporary full length vector of a sparse vector.
     *
     * @param denseVector Pointer to the full length vector.
     * @param sparseIndex Pointer to the index array of the vector.
     * @param sparseLength The length of the index array.
     */
    static void clearFullLenghtVector(Numerical::Double * denseVector,
                                      unsigned int * sparseIndex, unsigned int sparseLength);

#undef IN_VECTOR
};

/**
 * Multiplies the specified vector by a scalar value.
 *
 * @param m The scalar value.
 * @param v The vector to be multiplied.
 * @return Instance of the result vector.
 */
Vector operator*(Numerical::Double m, const Vector& v);

#endif // VECTOR_H
