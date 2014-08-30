/**
 * @file entry.h This file describes the IndexedDenseVector type.
 * @author Jozsef Smidla
 */

#ifndef INDEXEDDENSEVECTOR_H
#define INDEXEDDENSEVECTOR_H

#include <globals.h>
#include <utils/numerical.h>
#include <utils/iterator.h>
#include <utils/exceptions.h>

/**
 * Describes the IndexedDenseVector class. The class stores a vector in dense form,
 * and stores the indices of the nonnegative values also.
 *
 */
class IndexedDenseVector {
public:

    /**
     * This iterator iterates on the elements of the vector.
     */
    typedef NormalIterator<Numerical::Double> Iterator;

    /**
     * This iterator itrates on the nonzeros of the vector.
     */
    typedef IndexedIterator<Numerical::Double, unsigned int> NonzeroIterator;

    /**
     * Constructor of the class IndexedDenseVector. The dimension of the vector will be
     * equal with the parameter length.
     *
     * @param length Size of the vector.
     */
    IndexedDenseVector(unsigned int length = 0);

    /**
     * Copy constructor.
     *
     * @param orig The original vector object.
     */
    IndexedDenseVector(const IndexedDenseVector & orig);

    /**
     * Move constructor.
     *
     * @param orig The original vector object.
     */
    IndexedDenseVector(IndexedDenseVector && orig);

    /**
     * Assignment operator.
     *
     * @param orig The original vector object.
     * @return Returns with the reference of the object.
     */
    IndexedDenseVector & operator=(const IndexedDenseVector & orig);

    /**
     * Move assignment operator.
     *
     * @param orig The original vector object.
     * @return
     */
    IndexedDenseVector & operator=(IndexedDenseVector && orig);

    /**
     * Destructor of the IndexedDenseVector.
     */
    ~IndexedDenseVector();

    /**
     * Sets the specified element of the vector.
     *
     * @param index The index of the changed element.
     * @param value The new value.
     */
    void set(unsigned int index, Numerical::Double value);

    /**
     * Returns with the value of the specified element. Checks the index,
     * and when it is invalid, the function trowns an InvalidIndexException exception.
     *
     * @param index The index of the element.
     * @return Value of the index^th element.
     */
    ALWAYS_INLINE Numerical::Double at(unsigned int index) const {
        if (likely(index < m_length)) {
            return m_data[index];
        } else {
            throw InvalidIndexException("Invalid index at DenseVector::at",
                                        0, m_length - 1, index);
        }
    }

    /**
     * Returns with the value of the specified element.
     *
     * @param index The index of the element.
     * @return Value of the index^th element.
     */
    Numerical::Double operator[](unsigned int index) const {
        return m_data[index];
    }


    /**
     * Returns with the length of the vector.
     *
     * @return Length of the vector.
     */
    ALWAYS_INLINE unsigned int length() const {
        return m_length;
    }

    /**
     * Returns with the Euclidean norm of the vector.
     *
     * @return The norm of the vector.
     */
    Numerical::Double euclidNorm() const;

    /**
     * Returns with the L1 norm of the vector.
     *
     * @return The norm of the vector.
     */
    Numerical::Double l1Norm() const;

    /**
     * Adds the vector spefified by the arguments to the current vector.
     *
     * @param lambda Multiplier of the argument.
     * @param vector The vector to be added to the current vector.
     * @return Reference of the current vector object.
     */
    IndexedDenseVector & addVector(Numerical::Double lambda,
                                   const IndexedDenseVector & vector);

    /**
     * Returns with the number of nonzeros in the vector.
     *
     * @return Number of nonzeros.
     */
    ALWAYS_INLINE unsigned int nonZeros() const {
        return m_nonZeros;
    }

    /**
     * Scales the elements of the vector by lambda.
     *
     * @param lambda The scale factor.
     */
    void scale(Numerical::Double lambda);

    /**
     * Sorts the stored nonzero indices.
     */
    void sortIndices();

    /**
     * Sets the elements of the vector to zero.
     */
    void clear();

    /**
     * Sets the add mode for the vector-vector addition operations.
     * The default value is the Numerical::ADD_FAST.
     *
     * @param type The add mode.
     */
    void setAddMode(Numerical::ADD_TYPE type);

    /**
     * Creates a unit vector with the specified length.
     *
     * @param length The length of the new vector.
     * @param index The index of the value 1.
     * @return The result vector.
     */
    static IndexedDenseVector createUnitVector(unsigned int length, unsigned int index);

    /**
     * Creates a vector from the given dense array.
     *
     * @param source The original dense array.
     * @param size Size of the array.
     * @return The result vector.
     */
    static IndexedDenseVector createVectorFromDenseArray(const Numerical::Double * source,
                                                         unsigned int size);

    /**
     * Creates a vector from the given sparse array.
     *
     * @param data The array of the nonzero elements.
     * @param indices The array of the nonzero indices.
     * @param count Size of the arrays.
     * @param length Length of the vector.
     * @return The result vector.
     */
    static IndexedDenseVector createVectorFromSparseArray(const Numerical::Double * data,
                                                          const unsigned int * indices,
                                                          unsigned int count,                                                         unsigned int length);

    /**
     * Returns an iterator which points to the
     * first element.
     *
     * @return Iterator for the first element.
     */
    ALWAYS_INLINE Iterator begin() const {
        return Iterator(m_data);
    }

    /**
     * Returns an iterator which points to the end of the vector.
     *
     * @return Iterator to the end of the vector.
     */
    ALWAYS_INLINE Iterator end() const {
        return Iterator(m_data + m_length);
    }

    /**
     * Returns an iterator to the first nonzero element of the vector.
     *
     * @return Iterator to the first nonzero of the vector.
     */
    ALWAYS_INLINE NonzeroIterator beginNonzero() const {
        return NonzeroIterator(m_nonzeroIndices, m_data);
    }

    /**
     * Returns an iterator which points to the end of the vector.
     *
     * @return Iterator to the end of the vector.
     */
    ALWAYS_INLINE NonzeroIterator endNonzero() const {
        return NonzeroIterator(m_nonzeroIndices + m_nonZeros, m_data);
    }

protected:

    /**
     *
     */
    typedef void (IndexedDenseVector::*AddIndexedDenseToIndexedDense)(Numerical::Double,
                                                                      const IndexedDenseVector &);

    /**
     */
    AddIndexedDenseToIndexedDense m_addIndexedDenseToIndexedDense;

    /**
     */
    Numerical::Double * m_data;

    /**
     */
    unsigned int * m_nonzeroIndices;

    /**
     */
    unsigned int ** m_indexIndices;

    /**
     */
    unsigned int m_nonZeros;

    /**
     */
    unsigned int m_length;

    /**
     * @param orig
     */
    void copy(const IndexedDenseVector & orig);

    /**
     */
    void release();

    /**
     * @param orig
     */
    void move(IndexedDenseVector & orig);

    /**
     * @param lambda
     * @param vector
     */
    void addIndexedDenseToIndexedDenseFast(Numerical::Double lambda,
                                           const IndexedDenseVector & vector);

    /**
     * @param lambda
     * @param vector
     */
    void addIndexedDenseToIndexedDenseAbs(Numerical::Double lambda,
                                          const IndexedDenseVector & vector);

    /**
     * @param lambda
     * @param vector
     */
    void addIndexedDenseToIndexedDenseRel(Numerical::Double lambda,
                                          const IndexedDenseVector & vector);

    /**
     * @param lambda
     * @param vector
     */
    void addIndexedDenseToIndexedDenseAbsRel(Numerical::Double lambda,
                                             const IndexedDenseVector & vector);

};

#endif // DENSEVECTOR_H
