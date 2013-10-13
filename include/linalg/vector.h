/**
 * @file vector.h
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

#define COPY_DOUBLES(dest, src, count) memcpy(dest, src, sizeof(Numerical::Double) * count);

/**
 *
 */
extern unsigned int ELBOWROOM;

/**
 *
 */
extern Numerical::Double SPARSITY_RATIO;


/**
 * Class for storing dense and sparse vectors. The class can switch between
 * vector representation methods.
 */

//#define CHECK check(__FUNCTION__);
#define CHECK

class Vector
{
#define IN_VECTOR

    friend class ModelIO;
    friend class MpsModelBuilder;
    friend class PfiBasis;
    friend class VectorTestSuite;
public:

    /**
     * Displays the Vector in a human-readable format
     */
    void show() const;

    /**
     * Checks the vector. You can implement arbitrary checking code
     * for debug.
     */
    void check(const char *) const
    {

    }

    /**
     * Indicates the storing method of vector.
     */
    enum VECTOR_TYPE
    {
        SPARSE_VECTOR, DENSE_VECTOR
    };

    class CommonIterator;
    class Iterator;
    class ConstIterator;
    class NonzeroIterator;
    class ConstNonzeroIterator;

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
     * @param dimension Describes the dimension of vector
     */
    explicit Vector(unsigned int dimension = 0);

    explicit Vector(void *, void *, void *);
    /**
     * Releases the vector. Decreases sm_fullLenghtReferenceCounter, and when this
     * variable reaches zero, the descructor releases the sm_fullLengthVector.
     *
     * Complexity: O(1)
     */
    virtual ~Vector();
    /**
     * Copy constructor of Vector class. Copies the vector given by parameter, and
     * increases sm_fullLenghtReferenceCounter.
     *
     * Complexity: O(n)
     *
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
     * Resizes the vector. When the new length is greater than the
     * old dimension size, the old elements will remain.
     * 
     * @param length
     */
    void resize(unsigned int length);

    /**
     * Returns the maximal absolute value of the elements.
     */
    Numerical::Double absMaxElement();

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

    /*
     * Similar to the set, but it assumes that the index-th element of vector
     * is not zero.
     * 
     * @param index The function changes the index'th element of vector
     * @param value The new value of the index'th element of vector
     */
    void change(unsigned int index, Numerical::Double value);
    /**
     * 
     * @param index
     * @param lambda
     */
    void scaleElementBy(unsigned int index, Numerical::Double lambda);
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
    Numerical::Double operator[](unsigned int index) const;

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
     * Returns the difference vector yield by subtracting another vector from a
     * vector. TODO what if dimensions are not correct
     * @param vector The vector subtracted from this vector
     * @return a vector containing the result of subtraction
     */
    Vector operator-(const Vector& vector) const;

    /**
     * Returns the sum vector yield by adding another vector to a
     * vector. TODO what if dimensions are not correct
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
     * Returns with the logical lenth of vector.
     *
     * Complexity: O(1)
     *
     * @return Logical lenght of vector, which equals with m_dimension
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
     * The function muliplies the vector by lambda. When lambda is zero, the function
     * converts the vector to an empty sparse vector.
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
    Numerical::Double dotProduct(const Vector & vector) const;

    Numerical::Double euclidNorm() const;

    /**
     * The function performs the next operation:
     * a = a + lambda * b,
     * whele a, and b are Vector type objects.
     * 
     * @param lambda Multiplier of parameter vector
     * @param vector The function adds this vector to the other vector
     * @return The function returns with the reference of modified vector
     */
    Vector & addVector(Numerical::Double lambda, const Vector & vector);

    /**
     * @param eta
     * @param pivot
     */
    Vector & elementaryFtran(const Vector & eta, unsigned int pivot);

    /**
     * @param eta
     * @param pivot
     */
    Vector & elementaryBtran(const Vector & eta, unsigned int pivot);

    void removeElement(unsigned int index);

    /**
     * @param index
     * @param value
     */
    void insertElement(unsigned int index, Numerical::Double value);

    /**
     *
     * @param value
     */
    void append(Numerical::Double value);

    /**
     *
     * @param dimension
     * @param capacity
     */
    void reInit(unsigned int dimension);

    /**
     * @param ratio
     */
    void setSparsityRatio(Numerical::Double ratio);

    /**
     * @return
     */
    Numerical::Double getSparsityRatio() const;

    /**
     * @param index
     * @param value
     */
    void setNewNonzero(unsigned int index, Numerical::Double value);

    /**
     * @param vector
     */
    Vector & operator=(const Vector & vector);

    /**
     * @param vector
     */
    bool operator==(const Vector & vector) const;

    inline bool operator!=(const Vector & vector) const
    {
        return !(*this == vector);
    }

    inline VECTOR_TYPE getType() const
    {
        return m_vectorType;
    }

    inline unsigned int nonZeros() const
    {
        return m_nonZeros;
    }

    /**
     * @param os
     * @param vector
     * @return
     */
    friend std::ostream & operator<<(std::ostream & os, const Vector & vector);

    inline Iterator begin() const
    {
        return Iterator(m_data, m_dataEnd, m_data, m_index);
    }

    inline NonzeroIterator beginNonzero() const
    {
        CHECK;
        if (m_size == 0) {
            return endNonzero();
        }
        return NonzeroIterator(m_data, m_dataEnd, m_data, m_index,
            m_data < m_dataEnd);
    }

    inline Iterator end() const
    {
        return Iterator(m_data, m_dataEnd, m_dataEnd,
            m_index ? m_index + m_size : 0);
    }

    inline NonzeroIterator endNonzero() const
    {
        return NonzeroIterator(m_data, m_dataEnd, m_dataEnd,
            m_index ? m_index + m_size : 0, false);
    }

    /**
     * @param nonZeros
     * @param dimension
     */
    inline void prepareForData(const unsigned int nonZeros, const unsigned int dimension, Numerical::Double ratio = -1)
    {
        delete [] m_data;
        delete [] m_index;
        if (ratio == -1) {
            m_sparsityRatio = SPARSITY_RATIO;
        } else {
            m_sparsityRatio = ratio;
        }

        m_dimension = dimension;
        m_sparsityThreshold = (unsigned int) Numerical::round(m_dimension * m_sparsityRatio);
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
            register Numerical::Double * ptr = m_data;
            while (ptr < m_dataEnd) {
                *ptr = 0;
                ptr++;
            }
        }
    }

    /**
     * @param value
     * @param index
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

    inline const Numerical::Double * getData() const
    {
        return m_data;
    }

    inline const unsigned int * getIndex() const
    {
        return m_index;
    }

    void sortElements() const;

    void scaleByLambdas(const std::vector<Numerical::Double> & lambdas);

    /**
     * Swaps the content of vector1 and vector2. The complexity is O(1).
     *
     * @param vector1
     * @param vector2
     */
    static void swap(Vector * vector1, Vector * vector2);

protected:
    /**
     * Describes the type of vector. When the value of m_vectorType is DENSE_VECTOR
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
     * Describes the dimension of vector. The function length() returns with
     * value this data member.
     */
    unsigned int m_dimension;

    /**
     * This variable decsribes the real size of m_data (and at sparse vector
     * m_index also) array(s).
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
     * Describes the number of non zero elements in vector. This variable is
     * updated when the elements of vector are changed.
     */
    unsigned int m_nonZeros;

    /**
     * Describes the maximum sparsity ratio. When the ratio of non nozeros and
     * dimension of vector grows above this value, the vectro representation
     * switches to dense.
     *
     * @see m_sparsityThreshold
     */
    Numerical::Double m_sparsityRatio;
    /**
     * Describes the maximum number of non zeros in sparse representation.
     * This variable equals with (unsigned int) round(m_dimension * m_sparsityRatio),
     * and changed by insert / remove operations.
     *
     * @see m_sparsityRatio
     */
    unsigned int m_sparsityThreshold;

    /**
     * When m_sorted is true, the sparse elemnets are sorted
     * by ascending order.
     */
    mutable bool m_sorted;

    /**
     * Temporary vector for linear time dot product and add operations.
     */
    static Numerical::Double * sm_fullLengthVector;
    /**
     * Describes the size of sm_fullLengthVector. The sm_fullLengthVector is
     * not released after dot product or add operations, because memory allocation
     * is time consuming. When an operation needs biger array, it allocates
     * a new vector.
     */
    static unsigned int sm_fullLengthVectorLenght;
    /**
     * Describes how many Vector type objects exists. It is important for
     * releasing sm_fullLengthVector. When this variable is zero, the program
     * releases sm_fullLengthVector automatically.
     */
    static unsigned int sm_fullLenghtReferenceCounter;

    static unsigned long * sm_countingSortBitVector;

    static unsigned int sm_countingSortBitVectorLength;

    void init(unsigned int dimension);

    static Numerical::Double * allocateData(unsigned int capacity);

    static unsigned int * allocateIndex(unsigned int capacity);

    static void freeData(Numerical::Double * & data);

    static void freeIndex(unsigned int * & index);

    /**
     * @param size
     * @param elbowroom
     */
    void resizeDense(unsigned int size, unsigned int elbowroom);

    /**
     * @param size
     * @param capacity
     */
    void resizeSparse(unsigned int capacity);

    void denseToSparse();

    void sparseToDense();

    /**
     * Helper function for operator= and copy constructor. The function copies
     * the original vector. It is supposed that the vector is uninitialized or
     * the m_data and m_index are released.
     *
     * Complexity: O(n)
     *
     * @param original The original vector, the function creates a copy from this
     * object
     */
    void copy(const Vector & original, bool reallocate = true);

    /**
     * @param lambda
     * @param vector
     */
    void addDenseToDense(Numerical::Double lambda, const Vector & vector);

    /**
     * @param lambda
     * @param vector
     */
    void addDenseToSparse(Numerical::Double lambda, const Vector & vector);

    /**
     * @param lambda
     * @param vector
     */
    void addSparseToDense(Numerical::Double lambda, const Vector & vector);

    /**
     * @param lambda
     * @param vector
     */
    void addSparseToSparse(Numerical::Double lambda, const Vector & vector);

    /**
     * @param index
     * @return
     */
    Numerical::Double * getElementSparseLinear(unsigned int index) const;

    /**
     * @param index
     * @return
     */
    Numerical::Double * getElementSparseBinary(unsigned int index) const;

    /**
     * @param index
     * @return
     */
    Numerical::Double * getElementSparse(unsigned int index) const;

    /**
     * @param index
     * @param value
     */
    void addNewElementSparse(unsigned int index, Numerical::Double value);

    /**
     * @param index
     */
    void removeElementSparse(unsigned int index);

    void countingSort() const;

    void heapSort() const;

    void selectionSort() const;

    void insertionSort() const;

    // dense -> sparse
    /**
     * @param denseVector
     * @param sparseVector
     * @param indexVector
     * @param denseLength
     * @param setZero
     */
    static unsigned int gather(Numerical::Double * denseVector, Numerical::Double * sparseVector,
        unsigned int * index, unsigned int denseLength, bool setZero);


    // sparse -> dense
    /**
     * @param denseVector
     * @param denseLength
     * @param sparseVector
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
     * @param denseVector
     * @param denseLength
     * @param sparseVector
     * @param index
     * @param sparseLength
     * @param sparseMaxIndex
     * @param pivot
     */
    static Numerical::Double * scatterWithPivot(Numerical::Double * & denseVector, unsigned int & denseLength,
        Numerical::Double * sparseVector, unsigned int * index,
        unsigned int sparseLength, unsigned int sparseMaxIndex, unsigned int pivot);


    /**
     * @param denseVector
     * @param sparseIndex
     * @param sparseLength
     */
    static void clearFullLenghtVector(Numerical::Double * denseVector,
        unsigned int * sparseIndex, unsigned int sparseLength);

#undef IN_VECTOR
};

Vector operator*(Numerical::Double m, const Vector& v);

#endif // VECTOR_H
