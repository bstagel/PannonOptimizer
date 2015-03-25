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

class DenseVector;
class SparseVector;

/**
 * Describes the IndexedDenseVector class. The class stores a vector in dense form,
 * and stores the indices of the nonnegative values also.
 *
 */
class IndexedDenseVector {
    friend class InitPanOpt;
    friend class DenseVector;
    friend class SparseVector;

    Numerical::Double * m_data; // dense

    unsigned int * m_nonzeroIndices; // sparse

    unsigned int ** m_indexIndices; // dense

    unsigned int m_nonZeros;

    unsigned int m_length;

    template<class ADD>
    friend void addIndexedDenseToSparseTemplate(Numerical::Double lambda,
                                                SparseVector * vector1,
                                                const IndexedDenseVector & vector2);
public:

    /**
     * This iterator iterates on the elements of the vector.
     */
    typedef NormalIterator< std::remove_reference<decltype(*m_data)>::type > Iterator;

    /**
     * This iterator itrates on the nonzeros of the vector.
     */
    typedef IndexedIterator<std::remove_reference<decltype(*m_data)>::type, unsigned int> NonzeroIterator;

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
     * Returns with the value of the specified element. Checks the index,
     * and when it is invalid, the function trowns an InvalidIndexException exception.
     *
     * @param index The index of the element.
     * @return Value of the index^th element.
     */
    Numerical::Double at(unsigned int index) const {
        if (likely(index < m_length)) {
            return m_data[index];
        } else {
            throw InvalidIndexException("Invalid index at DenseVector::at",
                                        0, m_length - 1, index);
        }
    }

    /**
     * Sets the specified element of the vector.
     *
     * @param index The index of the changed element.
     * @param value The new value.
     */
    void set(unsigned int index, Numerical::Double value);

    /**
     * Returns with the value of the specified element.
     *
     * @param index The index of the element.
     * @return Value of the index^th element.
     */
    ALWAYS_INLINE Numerical::Double operator[](unsigned int index) const {
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
     * Returns with the number of nonzeros in the vector.
     *
     * @return Number of nonzeros.
     */
    ALWAYS_INLINE unsigned int nonZeros() const {
        return m_nonZeros;
    }

    //TODO: L^Inf Norm
    //TODO: L^2^2 Norm
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
     * Adds the IndexedDenseVector type vector spefified by the arguments to
     * the current vector.
     *
     * @param lambda Multiplier of the argument.
     * @param vector The vector to be added to the current vector.
     * @return Reference of the current vector object.
     */
    IndexedDenseVector & addVector(Numerical::Double lambda,
                                   const IndexedDenseVector & vector);

    /**
     * Adds the DenseVector type vector spefified by the arguments to the
     * current vector.
     *
     * @param lambda Multiplier of the argument.
     * @param vector The vector to be added to the current vector.
     * @return Reference of the current vector object.
     */
    IndexedDenseVector & addVector(Numerical::Double lambda,
                                   const DenseVector & vector);

    /**
     * Adds the SparseVector type vector spefified by the arguments to the
     * current vector.
     *
     * @param lambda Multiplier of the argument.
     * @param vector The vector to be added to the current vector.
     * @return Reference of the current vector object.
     */
    IndexedDenseVector & addVector(Numerical::Double lambda,
                                   const SparseVector & vector);

    /**
     *
     * @param vector
     * @return
     */
    Numerical::Double dotProduct(const IndexedDenseVector & vector) const;

    /**
     *
     * @param vector
     * @return
     */
    Numerical::Double dotProduct(const DenseVector & vector) const;

    /**
     *
     * @param vector
     * @return
     */
    Numerical::Double dotProduct(const SparseVector & vector) const;

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
     * Sets the elements of the vector to zero.
     */
    void clearDense();

    /**
     * Sets the add mode for the vector-vector addition operations.
     * The default value is the Numerical::ADD_FAST.
     *
     * @param type The add mode.
     */
    static void setAddMode(Numerical::ADD_TYPE type);

    /**
     *
     * @param type The dot product mode.
     */
    static void setDotProductMode(Numerical::DOT_PRODUCT_TYPE type);

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

    /**
     * Removes an element from the vector.
     * This operation decreases the dimension of the vector by 1.
     *
     * @param index The index of the element to be removed.
     */
    void remove(unsigned int index);

    /**
     * Inserts a new element to the vector.
     * This operation increases the dimension of the vector by 1.
     *
     * @param index The index of the element to be added.
     * @param value The value of the element to be added.
     */
    void insert(unsigned int index, Numerical::Double value);

    /**
     * Appends a new element to the end of the vector.
     * This operation increases the dimension of the vector by 1.
     *
     * @see insertElement()
     * @param value The value of the element to be appended.
     */
    void append(Numerical::Double value);

    void reserve();

    void resize(unsigned int length);

protected:

    typedef void (IndexedDenseVector::*AddIndexedDenseToIndexedDense)(Numerical::Double, const IndexedDenseVector &);

    typedef void (IndexedDenseVector::*AddDenseToIndexedDense)(Numerical::Double, const DenseVector &);

    typedef void (IndexedDenseVector::*AddSparseToIndexedDense)(Numerical::Double, const SparseVector &);

    typedef Numerical::Double (IndexedDenseVector::*IndexedDenseToIndexedDenseDotProduct)(const IndexedDenseVector &,
                                                                                          const IndexedDenseVector &) const;

    typedef Numerical::Double (IndexedDenseVector::*IndexedDenseToDenseDotProduct)(const IndexedDenseVector &,
                                                                                   const DenseVector &) const;

    typedef Numerical::Double (IndexedDenseVector::*IndexedDenseToSparseDotProduct)(const IndexedDenseVector &,
                                                                                    const SparseVector &) const;

    static AddIndexedDenseToIndexedDense sm_addIndexedDenseToIndexedDense;
    static AddDenseToIndexedDense sm_addDenseToIndexedDense;
    static AddSparseToIndexedDense sm_addSparseToIndexedDense;

    static IndexedDenseToIndexedDenseDotProduct sm_indexedDenseToIndexedDenseDotProduct;
    static IndexedDenseToDenseDotProduct sm_indexedDenseToDenseDotProduct;
    static IndexedDenseToSparseDotProduct sm_indexedDenseToSparseDotProduct;

    static void _globalInit();

    void copy(const IndexedDenseVector & orig);

    void move(IndexedDenseVector & orig);

    void release();

    void addIndexedDenseToIndexedDenseFast(Numerical::Double lambda, const IndexedDenseVector & vector);
    void addIndexedDenseToIndexedDenseAbs(Numerical::Double lambda, const IndexedDenseVector & vector);
    void addIndexedDenseToIndexedDenseAbsRel(Numerical::Double lambda, const IndexedDenseVector & vector);

    void addDenseToIndexedDenseFast(Numerical::Double lambda, const DenseVector & vector);
    void addDenseToIndexedDenseAbs(Numerical::Double lambda, const DenseVector & vector);
    void addDenseToIndexedDenseAbsRel(Numerical::Double lambda, const DenseVector & vector);

    void addSparseToIndexedDenseFast(Numerical::Double lambda, const SparseVector & vector);
    void addSparseToIndexedDenseAbs(Numerical::Double lambda, const SparseVector & vector);
    void addSparseToIndexedDenseAbsRel(Numerical::Double lambda, const SparseVector & vector);

    Numerical::Double dotProductIndexedDenseToIndexedDenseUnstable(const IndexedDenseVector & vector1, const IndexedDenseVector & vector2) const;
    Numerical::Double dotProductIndexedDenseToIndexedDenseFast(const IndexedDenseVector & vector1, const IndexedDenseVector & vector2) const;
    Numerical::Double dotProductIndexedDenseToIndexedDenseAbs(const IndexedDenseVector & vector1, const IndexedDenseVector & vector2) const;
    Numerical::Double dotProductIndexedDenseToIndexedDenseAbsRel(const IndexedDenseVector & vector1, const IndexedDenseVector & vector2) const;


    Numerical::Double dotProductIndexedDenseToDenseUnstable(const IndexedDenseVector & vector1, const DenseVector & vector2) const;
    Numerical::Double dotProductIndexedDenseToDenseFast(const IndexedDenseVector & vector1, const DenseVector & vector2) const;
    Numerical::Double dotProductIndexedDenseToDenseAbs(const IndexedDenseVector & vector1, const DenseVector & vector2) const;
    Numerical::Double dotProductIndexedDenseToDenseAbsRel(const IndexedDenseVector & vector1, const DenseVector & vector2) const;

    Numerical::Double dotProductIndexedDenseToSparseUnstable(const IndexedDenseVector & vector1, const SparseVector & vector2) const;
    Numerical::Double dotProductIndexedDenseToSparseFast(const IndexedDenseVector & vector1, const SparseVector & vector2) const;
    Numerical::Double dotProductIndexedDenseToSparseAbs(const IndexedDenseVector & vector1, const SparseVector & vector2) const;
    Numerical::Double dotProductIndexedDenseToSparseAbsRel(const IndexedDenseVector & vector1, const SparseVector & vector2) const;
};

#endif // DENSEVECTOR_H
