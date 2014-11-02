/**
 * @file vector.h This file contains the API of the Vector class.
 * @author Jozsef Smidla
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
extern Numerical::Double SPARSITY_RATIO;


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
    friend class InitPanOpt;
public:

    /**
     * Displays the Vector in a human-readable format.
     */
    void show() const;

    /**
     * Checks the vector. You can implement arbitrary checking code for debugging purposes.
     */
    void check(const char *) const
    {

    }

    /**
     * Indicates the storing method of vector.
     * - SPARSE_VECTOR is a vector storing (index, value) pairs. It is effective
     * if the number of nonzeros is low in the vector.
     * - DENSE_VECTOR is a traditional vector implementation using array.
     * - DEFAULT_VECTOR_TYPE specifies that the vector is adjusted dynamically
     * according to the actual sparsity ratio.
     */
    enum VECTOR_TYPE
    {
        SPARSE_VECTOR, DENSE_VECTOR, DEFAULT_VECTOR_TYPE
    };

#include <linalg/vector_iterator.h>

    /**
     * This constructor initializes a vector, with a given dimension.
     * When the sparsity threshold is higher than the number of nonzeros, the
     * vector will be sparse vector, and the m_data array is initialized for size
     * ELBOWROOM. Logically, the vector is a zero vector containing zeros.
     * Increases the sm_fullLenghtReferenceCounter for garbage collector mechanism.
     *
     * Complexity: O(1)
     *
     * @constructor
     * @param dimension The dimension of the vector.
     * @param type The representation type of the vector to be initialized.
     */
    explicit Vector(unsigned int dimension = 0, VECTOR_TYPE type = DEFAULT_VECTOR_TYPE);

    //TODO: Ez mire, mikor kell? Irjuk bele a doxyba, vagy szedjuk ki a fuggvenyt.
    /**
     * This constructor is used to initialize a vector very fast.
     * The vector is not prepared for data after initializing with this constructor.
     * Increases the sm_fullLenghtReferenceCounter for garbage collector mechanism.
     *
     * Complexity: O(1)
     *
     * @constructor
     */
    explicit Vector(void *, void *, void *);

    /**
     * Copy constructor of the Vector class.
     * Copies the given vector and increases sm_fullLenghtReferenceCounter.
     *
     * Complexity: O(n)
     *
     * @constructor
     * @param original The original vector, the function creates a copy of this.
     */
    Vector(const Vector & original);

    /**
     * Creates a scaled copy from a vector.
     * Copies the vector and multiply the elements by lambda., and
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
     * Set every element to zero in the vector.
     */
    void clear();

    /**
     * Fills the vector with a given value.
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
     * @param squareSumPtr Pointer to the variable to store the sum of the squares of
     * the absolute values of the elements.
     * @return The sum of the absolute values of the elements.
     */
    Numerical::Double absMaxSums(Numerical::Double * squareSumPtr) const;

    /**
     * Calculates the sum of the absolute values of the elements.
     * Gives pointers to the squares of the absolute values of the elements, as well as the
     * maximal and minimal absolute values of the elements.
     *
     * @see absMaxSums
     * @param squareSumPtr Pointer to the variable to store the sum of the squares of
     * the absolute values of the elements.
     * @param minPtr Pointer to the variable to store the minimal absolute value of the
     * elements.
     * @param maxPtr Pointer to the variable to store the maximal absolute value of the
     * elements.
     * @return The sum of the absolute values of the elements.
     */
    Numerical::Double absMaxSumsAndMinMax(Numerical::Double * squareSumPtr,
                                          Numerical::Double * minPtr,
                                          Numerical::Double * maxPtr) const;

    /**
     * Scales the vector with the given multipliers.
     * The values are scaled with unique multipliers and a commom multiplier too.
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
     * Set logically the index'th element of the vector.
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
     * m_data = {5.0, 4.0, | X, X, X, X, X}
     * m_index = {2, 4, | X, X, X, X, X}
     *
     * set(2, 0)
     * m_data = {4.0, | X, X, X, X, X, X}
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
     * @param index The function changes the index'th element.
     * @param value The new value of the element.
     */
    void set(unsigned int index, Numerical::Double value);

    /**
     * Similar to the set, but it assumes that the index-th element of vector is not zero.
     * This function is faster in this special case, if the specified element is zero it
     * will lead to undefined behaviour.
     *
     * @see set
     * @param index The function changes the index'th element of vector
     * @param value The new value of the index'th element of vector
     */
    void change(unsigned int index, Numerical::Double value);

    /**
     * Multiplies an element of the vector by a given value.
     *
     * @param index The index of the element to be scaled.
     * @param lambda The multiplier which is applied to the element.
     */
    void scaleElementBy(unsigned int index, Numerical::Double lambda);

    /**
     * Gives a vector as a product of multiplying the vector with a matrix from the right.
     * If the given right matrix's row count (m) is not the same as the length of
     * the vector, then this vector will be interpreted as an m-length row vector,
     * filled up by zeros if needed.
     *
     * @param matrix The matrix used to multiply this vector.
     * @return a row vector containing the result of multiplication.
     */
    Vector operator*(const Matrix& matrix) const;

    /**
     * Constructs new vector which as a multiply of the actual vector.
     * The vector is copied and scaled by the multiplier.
     *
     * @see scaleBy
     * @param lambda The multiplicator.
     * @return A vector containing the result of the multiplication.
     */
    Vector operator*(Numerical::Double lambda) const;

    /**
     * Returns the difference vector yield by subtracting a vector from the actual vector.
     * The vector is copied and the other vector is substracted from the copy.
     *
     * @param vector The vector subtracted from the actual vector.
     * @return A vector containing the result of the subtraction.
     */
    Vector operator-(const Vector& vector) const;

    /**
     * Returns the sum vector yield by adding a vector to the actual vector.
     * The vector is copied and the other vector is added to the copy.
     *
     * @param vector The vector added to this vector
     * @return a vector containing the result of adding
     */
    Vector operator+(const Vector& vector) const;

    /**
     * Gives the value of index'th element.
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
     * @param index The index of the element.
     * @return The index'th element of vector.
     */
    const Numerical::Double &at(unsigned int index) const;

    /**
     * Gives the pointer of the index'th element of a dense vector.
     * Do not use this function on a sparse vector!
     * It will lead to undefined behaviour on sparse vectors.
     *
     * Complexity: O(1)
     *
     * @param index Index of a dense element.
     * @return Pointer to the index'th dense element.
     */
    const Numerical::Double * getDenseElementPointer(unsigned int index) const;

    /**
     * Returns with the logical length (the dimension) of vector.
     *
     * Complexity: O(1)
     *
     * @return Logical lenght of vector, which equals to m_dimension.
     */
    unsigned int length() const;

    /**
     * Returns with the capacity of vector.
     * Capacity means the number of nonzeros that can be stored on the allocated memory.
     *
     * Complexity: O(1)
     *
     * @return Capacity of vector
     */
    unsigned int capacity() const;

    /**
     * Gives the maximal index of the stored nonzeros.
     * Dense vectors give the index of the last nonzero element, while
     * sparse vectors give the maximal index of the stored elements.
     *
     * Complexity: O(n)
     *
     * @return The maximal index of the stored nonzeros.
     */
    unsigned int maxIndex() const;

    /**
     * Muliply the vector by lambda.
     * Every element is multiplied by a given multiplier lambda.
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
     * @param lambda The value of the multiplier.
     * @return Reference of the vector.
     */
    Vector & scaleBy(Numerical::Double lambda);

    /**
     * Gives the result of the dot product of two vectors.
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
     * @param vector The operand vector of the dot product.
     * @param stableAddAbs Use stable addition with absolute tolerance.
     * @param stableAddRel Use stable addition with relative tolerance.
     * @return The value of the dot product.
     */
    Numerical::Double dotProduct(const Vector & vector,
                                 bool stableAddAbs = true,
                                 bool stableAddRel = true) const;

    /**
     * Calculates the Euclidean norm of the vector.
     *
     * @return The Euclidean norm of the vector.
     */
    Numerical::Double euclidNorm() const;

    /**
     * Calculates the square of Euclidean norm of the vector.
     *
     * @return The square of Euclidean norm of the vector.
     */
    Numerical::Double euclidNorm2() const;


    //TODO: Implement l1Norm function
    /**
     * Calculates the one norm of the vector.
     *
     * @return The one norm of the vector.
     */
    Numerical::Double l1Norm() const;


    //TODO: A Numerical::ADD_TYPE-ot egyforman kezelni itt meg a felette levo fuggvenyben.

    /**
     * The function performs the operation: a = a + lambda * b.
     * Where a and b are Vector objects.
     *
     * @see Numerical::stableAdd
     * @param lambda Multiplier of the parameter vector,
     * @param vector The vector to be added to the actual vector.
     * @param addType Specifies the stable addition method.
     * @return The reference of the modified vector.
     */
    Vector & addVector(Numerical::Double lambda,
                       const Vector & vector,
                       Numerical::ADD_TYPE addType = Numerical::ADD_ABS_REL);

    /**
     * Performs an elementary forward transformation on the vector.
     * This operation is the multiplication of the vector with an
     * elementary transformation matrix: v = ETM * v. It differs from the
     * unit matrix in only one column this column can be specified with
     * a vector and its position.
     *
     * @param eta The eta vector of the elementary transformation matrix.
     * @param pivot The index of the eta vector in the elementary transformation matrix.
     * @return Reference of the vector.
     */
    Vector & elementaryFtran(const Vector & eta, unsigned int pivot);

    /**
     * Performs an elementary backward transformation on the vector.
     * This operation is the multiplication of the vector with an
     * elementary transformation matrix: v = v * ETM. It differs from the
     * unit matrix in only one column this column can be specified with
     * a vector and its position.
     *
     * @param eta The eta vector of the elementary transformation matrix.
     * @param pivot The index of the eta vector in the elementary transformation matrix.
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
     * This operation increases the dimension of the vector by the
     * dimension of the operand vector.
     *
     * @param vector The new vector to be appended.
     */
    void appendVector(const Vector & vector);

    /**
     * Reinitializes the vector to the given dimension.
     * This operation also reallocates the data storage of the vector.
     * Data in the vector will be lost after this operation.
     *
     * @param dimension The dimension of the reinitialized vector.
     */
    void reInit(unsigned int dimension);

    /**
     * Changes the sparsity ratio of the vector.
     * It can imply the change of the vector type too.
     *
     * @see SPARSITY_RATIO
     * @param ratio The value of the new sparsity ratio.
     */
    void setSparsityRatio(Numerical::Double ratio);

    /**
     * Gives the actual sparsity ratio of the vector.
     *
     * @see SPARSITY_RATIO
     * @return The value of the sparsity ratio of the vector.
     */
    Numerical::Double getSparsityRatio() const;

    /**
     * Changes a previously zero value of the vector to a new, nonzero value.
     * If this function is applied to a nonzero element it will lead to
     * undefined behaviour.
     *
     * @param index The index of the element to be added.
     * @param value The value of the element to be added.
     */
    void setNewNonzero(unsigned int index, Numerical::Double value);

    /**
     * Assignes a vector to the actual vector.
     * The data in the vector is overwritten, the values of the operand will be
     * copied to the actual vector.
     *
     * @param vector The vector to be copied.
     * @return The reference of the vector.
     */
    Vector & operator=(const Vector & vector);

    /**
     * Gives whether the two vectors are equal.
     *
     * @param vector Reference to the other vector.
     * @return Logical equivalence of the two vectors.
     */
    bool operator==(const Vector & vector) const;

    /**
     * Gives whether the two vectors are not equal.
     *
     * @param vector Reference to the other vector.
     * @return Complement of logical equivalence of the two vectors.
     */
    inline bool operator!=(const Vector & vector) const
    {
        return !(*this == vector);
    }

    /**
     * Gives the current representation type of the vector.
     * It can be sparse or dense vector.
     *
     * @see VECTOR_TYPE
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
     * Prints the properties of the vector to an output stream.
     *
     * @param os Reference of the output stream to be printed on.
     * @param vector The vector to be printed.
     * @return Reference of the output stream with the printed data.
     */
    friend std::ostream & operator<<(std::ostream & os, const Vector & vector);

    /**
     * Gives an Iterator pointing to the first element of the vector.
     * The first element is the element at position 0.
     *
     * @see Iterator
     * @return An Iterator pointing to the first element.
     */
    inline Iterator begin() const
    {
        return Iterator(m_data, m_dataEnd, m_data, m_index);
    }

    /**
     * Gives a NonzeroIterator pointing to the first nonzero element of the vector.
     *
     * @see NonzeroIterator
     * @return A NonzeroIterator pointing to the first nonzero.
     */
    inline NonzeroIterator beginNonzero() const
    {
        if (m_size == 0) {
            return endNonzero();
        }
        return NonzeroIterator(m_data, m_dataEnd, m_data, m_index,
                               m_data < m_dataEnd);
    }

    /**
     * Returns an Iterator pointing to the end of the vector.
     * It points after the last element, whether it is 0 or not.
     *
     * @see Iterator
     * @return An Iterator pointing to the end of the vector.
     */
    inline Iterator end() const
    {
        return Iterator(m_data, m_dataEnd, m_dataEnd,
                        m_index ? m_index + m_size : 0);
    }

    //TODO: Az end es az endNonzero ugyanoda mutatnak?
    /**
     * Returns a NonzeroIterator pointing to the end of the vector.
     * It points after the last element, whether it is 0 or not.
     *
     * @see NonzeroIterator
     * @return A NonzeroIterator pointing to the end of the vector.
     */
    inline NonzeroIterator endNonzero() const
    {
        return NonzeroIterator(m_data, m_dataEnd, m_dataEnd,
                               m_index ? m_index + m_size : 0, false);
    }

    /**
     * Initializes the vector for a given number of nonzeros.
     * The dimension, the number of nonzeros, and the sparsity ratio are customized.
     * This function can speed up the vector initialization if the number of nonzeros
     * of the vector is known in advance. After this initialization the newNonZero()
     * function must be used to set the values of the previously allocated nonzeros.
     * Other usage can lead to undefined behaviour.
     *
     * @see newNonZero
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
        m_sparsityThreshold = (unsigned int)Numerical::round(m_dimension * m_sparsityRatio);
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
                *ptr = 0.0;
                ptr++;
            }
        }
    }

    /**
     * Sets a previously initialized zero element to a given value.
     * This can be used only after the prepareForData() function otherwise
     * it will lead to undefined behaviour.
     *
     * @see prepareForData();
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
     * It chooses among sorting methods intelligently, based on
     * the vector type and the number of nonzeros.
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
     * Every element is scaled individually by the corresponding lambda value.
     * The vector of lambdas must have the same size as the actual vector.
     *
     * @param lambdas The vector of lambdas.
     */
    void scaleByLambdas(const std::vector<Numerical::Double> & lambdas);

    /**
     * Swaps the content of two vectors.
     * The complexity is O(1).
     *
     * @param vector1 The first vector to be swapped.
     * @param vector2 The second vector to be swapped.
     */
    static void swap(Vector * vector1, Vector * vector2);

protected:
    /**
     * Describes the type of a vector.
     * When the value of m_vectorType is DENSE_VECTOR
     * the vector represents a dense vector. In this case m_size = m_dimension,
     * and zeros are stored also. The m_index is NULL pointer, i.e. the
     * indices are omitted. When the value of m_vectorType is SPARSE_VECTOR,
     * the vector represents a sparse vector, and m_size describes the size of
     * the used part of the m_index and m_data arrays.
     */
    VECTOR_TYPE m_vectorType;

    /**
     * Describes the size of used part of m_data and m_index arrays.
     * In dense case it is the same as m_dimension.
     */
    unsigned int m_size;

    /**
     * Describes the dimension of the vector.
     * The function length() returns with the value this data member.
     */
    unsigned int m_dimension;

    /**
     * This variable decsribes the real size of m_data (and m_index in the sparse case) array(s).
     * Capacity is used to manage the resize and reallocation operations if necessary.
     */
    unsigned int m_capacity;

    /**
     * This array stores the elements of the vector.
     */
    Numerical::Double * m_data;

    /**
     * This array stores the indices of the nonzeros in sparse vector.
     */
    unsigned int * m_index;

    /**
     * Points to the end of the used part of m_data array.
     */
    Numerical::Double * m_dataEnd;

    /**
     * Describes the number of nonzero elements in vector.
     * This variable is updated when the elements of the vector are changed.
     */
    unsigned int m_nonZeros;

    /**
     * Describes the maximum number of nonzeros in sparse representation.
     * This variable equals with (unsigned int) round(m_dimension * m_sparsityRatio).
     * This is changed after insert and remove operations.
     *
     * @see m_sparsityRatio
     */
    unsigned int m_sparsityThreshold;

    /**
     * Describes the sparsity ratio, which gives a boundary between sparse and dense representations.
     * When the ratio of nonzeros and dimension of vector grows above this value,
     * the vector representation automatically switches to dense. On the other hand if the
     * ratio of nonzeros decrease below this ratio the representation automatically
     * switches to sparse.
     *
     * @see m_sparsityThreshold
     */
    Numerical::Double m_sparsityRatio;

    /**
     * When m_sorted is true, the sparse elemnets are sorted in ascending order.
     */
    mutable bool m_sorted;

    /**
     * Temporary vector for linear time dot product and add operations.
     */
    static thread_local Numerical::Double * sm_fullLengthVector;

    /**
     * Describes the size of sm_fullLengthVector.
     * The sm_fullLengthVector is not released after dot product or add operations,
     * because memory allocation is time consuming. When an operation needs bigger
     * array, it allocates a new vector.
     *
     * @see sm_fullLengthVector
     */
    static thread_local unsigned int sm_fullLengthVectorLength;

    /**
     * Describes how many Vector type objects exists.
     * It is important for releasing sm_fullLengthVector. When this variable is zero, the program
     * releases sm_fullLengthVector automatically.
     *
     * @see sm_fullLengthVector
     */
    static thread_local unsigned int sm_fullLenghtReferenceCounter;

    /**
     * Temporary vector for the counting sort vector sorting algorithm.
     *
     * @see countingSort
     */
    static thread_local unsigned long * sm_countingSortBitVector;

    /**
     * Describes the size of sm_countingSortBitVector.
     * The sm_countingSortBitVector is not released after dot
     * product or add operations, because memory allocation
     * is time consuming. When an operation needs bigger array, it allocates a new vector.
     *
     * @see sm_countingSortBitVector
     */
    static thread_local unsigned int sm_countingSortBitVectorLength;

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
     * @return Pointer to the allocated data array.
     */
    static Numerical::Double * allocateData(unsigned int capacity);

    /**
     * Allocates the index array of the vector with the given capacity.
     *
     * @param capacity The capacity of the vector.
     * @return  Pointer to the allocated index array.
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
     * @param original The original vector, the function creates a copy from this.
     * @param reallocate Specifies whether the arrays should be reallocated or readily available.
     */
    void copy(const Vector & original, bool reallocate = true);

    /**
     * Helper function for operator+ and addVector, executing the dense vector-dense vector addition.
     *
     * @param lambda Multiplier of the parameter vector.
     * @param vector The function adds this vector to the actual vector.
     * @param addType Specifies the stable addition method.
     */
    void addDenseToDense(Numerical::Double lambda,
                         const Vector & vector,
                         Numerical::ADD_TYPE addType);

    /**
     * Helper function for operator+ and addVector, executing the dense vector-sparse vector addition.
     *
     * @param lambda Multiplier of parameter vector.
     * @param vector The function adds this vector to the actual vector.
     * @param addType Specifies the stable addition method.
     */
    void addDenseToSparse(Numerical::Double lambda,
                          const Vector & vector,
                          Numerical::ADD_TYPE addType);

    /**
     * Helper function for operator+ and addVector, executing the sparse vector-dense vector addition.
     *
     * @param lambda Multiplier of parameter vector.
     * @param vector The function adds this vector to the actual vector.
     * @param addType Specifies the stable addition method.
     */
    void addSparseToDense(Numerical::Double lambda,
                          const Vector & vector,
                          Numerical::ADD_TYPE addType);

    /**
     * Helper function for operator+ and addVector, executing the sparse vector-sparse vector addition.
     *
     * @param lambda Multiplier of parameter vector.
     * @param vector The function adds this vector to the actual vector.
     * @param addType Specifies the stable addition method.
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
     * Helper function returning the index'th element of a sparse vector.
     * This function decides whether to use linear or binary search depending
     * on the actual vector. If it is sorted binary search is used, otherwise
     * linear search.
     *
     * @param index The index of the element to be returned.
     * @return The index'th element of the vector.
     */
    Numerical::Double * getElementSparse(unsigned int index) const;

    /**
     * Helper function adding a new element to a sparse vector.
     *
     * @param index The index of the new element.
     * @param value The value of the new element.
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

    /**
     * Helper function converting a dense vector data array to sparse data and index arrays.
     *
     * @param denseVector Pointer to the dense data array.
     * @param sparseVector Pointer to the sparse data array.
     * @param indexVector Pointer to the index array.
     * @param denseLength Length of the dense data array.
     * @return Pointer to the end of the index array.
     */
    static unsigned int gather(Numerical::Double * denseVector,
                               Numerical::Double * sparseVector,
                               unsigned int * indexVector,
                               unsigned int denseLength);

    /**
     * Helper function creating a dense vector data array from a given sparse vector.
     *
     * @param denseVector Pointer to the dense data array.
     * @param denseLength Length of the dense array.
     * @param sparseVector Reference of the sparse vector to be converted.
     */
    static void scatter(Numerical::Double * & denseVector, unsigned int & denseLength,
                        const Vector& sparseVector);

    //TODO: Egysegesiteni a ket scatter implementaciot (parameterlistat), leheseg szerint a legegyszerubbre
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

    static void _globalInit();
#undef IN_VECTOR
};

///**
// * Multiplies the specified vector by a scalar value.
// *
// * @param m The scalar value.
// * @param v The vector to be multiplied.
// * @return Instance of the result vector.
// */
//Vector operator*(Numerical::Double m, const Vector& v);

#endif // VECTOR_H
