#ifndef DENSEVECTOR_H
#define DENSEVECTOR_H

#include <globals.h>
#include <utils/numerical.h>
#include <utils/iterator.h>
#include <utils/exceptions.h>

class IndexedDenseVector;
class SparseVector;

/**
 *
 */
class DenseVector {
    friend class InitPanOpt;
    friend class IndexedDenseVector;
    friend class SparseVector;
private:

    template<class ADD>
    friend void addDenseToSparseTemplate(Numerical::Double lambda,
                                         SparseVector * vector1,
                                         const DenseVector & vector2);

    /**
     *
     */
    class SkipZeros {
    public:
        /**
         *
         * @param ptr
         * @param end
         */
        static void init(const Numerical::Double * & ptr,
                         const Numerical::Double * end) {
            while (ptr < end && *ptr == 0.0) {
                ptr++;
            }
        }

        /**
         *
         * @param ptr
         * @param end
         */
        static void next(const Numerical::Double * & ptr,
                         const Numerical::Double * end) {
            do {
                ptr++;
            } while (ptr < end && *ptr == 0.0);

        }
    };

public:

    /**
     * This iterator iterates on the elements of the vector.
     */
    typedef NormalIterator<Numerical::Double> Iterator;

    /**
     * This iterator iterates only on the nonzero elements of the vector.
     */
    typedef SkipIterator<Numerical::Double, SkipZeros> NonzeroIterator;

    /**
     * Constructor of the class DenseVector. The dimension of the vector will be
     * equal with the parameter length.
     *
     * @param length Size of the vector.
     */
    DenseVector(unsigned int length=0);

    /**
     * Copy constructor.
     *
     * @param orig The original vector object.
     */
    DenseVector(const DenseVector & orig);

    /**
     * Move constructor.
     *
     * @param orig The original vector object.
     */
    DenseVector(DenseVector && orig);

    /**
     * Assignment operator.
     *
     * @param orig The original vector object.
     * @return Returns with the reference of the object.
     */
    DenseVector & operator=(const DenseVector & orig);

    /**
     * Move assignment operator.
     *
     * @param orig The original vector object.
     * @return
     */
    DenseVector & operator=(DenseVector && orig);

    /**
     * Destructor of the DenseVector.
     */
    ~DenseVector();

    /**
     * Creates a unit vector with the specified length.
     *
     * @param length The length of the new vector.
     * @param index The index of the value 1.
     * @return The result vector.
     */
    static DenseVector createUnitVector(unsigned int length, unsigned int index);

    /**
     * Creates a vector from the given dense array.
     *
     * @param source The original dense array.
     * @param size Size of the array.
     * @return The result vector.
     */
    static DenseVector createVectorFromDenseArray(const Numerical::Double * source,
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
    static DenseVector createVectorFromSparseArray(const Numerical::Double * data,
                                                   const unsigned int * indices,
                                                   unsigned int count,
                                                   unsigned int length);

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
     * Returns with the value of the specified element.
     *
     * @param index The index of the element.
     * @return Value of the index^th element.
     */
    ALWAYS_INLINE Numerical::Double operator[](unsigned int index) const {
        return m_data[index];
    }

    /**
     * Sets the specified element of the vector.
     *
     * @param index The index of the changed element.
     * @param value The new value.
     */
    ALWAYS_INLINE void set(unsigned int index, Numerical::Double value) {
        m_data[index] = value;
    }

    /**
     * Returns with the length of the vector.
     *
     * @return Length of the vector.
     */
    ALWAYS_INLINE unsigned int length() const {
        return m_length;
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
     * Adds the SparseVector type vector spefified by the arguments
     * to the current vector.
     *
     * @param lambda Multiplier of the argument.
     * @param vector The vector to be added to the current vector.
     * @return Reference of the current vector object.
     */
    DenseVector & addVector(Numerical::Double lambda,
                            const SparseVector & vector);

    /**
     * Adds the DenseVector type vector spefified by the arguments
     * to the current vector.
     *
     * @param lambda Multiplier of the argument.
     * @param vector The vector to be added to the current vector.
     * @return Reference of the current vector object.
     */
    DenseVector & addVector(Numerical::Double lambda,
                            const DenseVector & vector);

    /**
     * Adds the IndexedDenseVector type vector spefified by the arguments
     * to the current vector.
     *
     * @param lambda Multiplier of the argument.
     * @param vector The vector to be added to the current vector.
     * @return Reference of the current vector object.
     */
    DenseVector & addVector(Numerical::Double lambda,
                            const IndexedDenseVector & vector);

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
     * Sets the elements of the vector to zero.
     */
    void clear();

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
        return NonzeroIterator(m_data, m_data + m_length);
    }

    /**
     * Returns an iterator which points to the end of the vector.
     *
     * @return Iterator to the end of the vector.
     */
    ALWAYS_INLINE NonzeroIterator endNonzero() const {
        return NonzeroIterator(m_data + m_length, m_data + m_length);
    }

    /**
     * Returns with the number of nonzeros in the vector.
     * Complexity: O(n)
     *
     * @return Number of nonzeros.
     */
    unsigned int nonZeros() const;


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

protected:

    typedef void (DenseVector::*AddDenseToDense)(Numerical::Double,
                                                 const DenseVector &);

    typedef void (DenseVector::*AddIndexedDenseToDense)(Numerical::Double,
                                                        const IndexedDenseVector &);

    typedef void (DenseVector::*AddSparseToDense)(Numerical::Double,
                                                  const SparseVector &);

    typedef Numerical::Double (DenseVector::*DenseToIndexedDenseDotProduct)(const DenseVector &,
                                                                            const IndexedDenseVector &) const;

    typedef Numerical::Double (DenseVector::*DenseToDenseDotProduct)(const DenseVector &,
                                                                     const DenseVector &) const;

    typedef Numerical::Double (DenseVector::*DenseToSparseDotProduct)(const DenseVector &,
                                                                      const SparseVector &) const;

    static thread_local AddDenseToDense sm_addDenseToDense;
    static thread_local AddIndexedDenseToDense sm_addIndexedDenseToDense;
    static thread_local AddSparseToDense sm_addSparseToDense;

    static thread_local DenseToIndexedDenseDotProduct sm_denseToIndexedDenseDotProduct;
    static thread_local DenseToDenseDotProduct sm_denseToDenseDotProduct;
    static thread_local DenseToSparseDotProduct sm_denseToSparseDotProduct;

    Numerical::Double * m_data;

    unsigned int m_length;

    static void _globalInit();

    /**
     *
     * @param orig
     */
    void copy(const DenseVector & orig);

    /**
     *
     * @param orig
     */
    void move(DenseVector & orig);

    /**
     *
     */
    void release();

    void addDenseToDenseFast(Numerical::Double lambda, const DenseVector & vector);
    void addDenseToDenseAbs(Numerical::Double lambda, const DenseVector & vector);
    void addDenseToDenseAbsRel(Numerical::Double lambda,  const DenseVector & vector);

    void addIndexedDenseToDenseFast(Numerical::Double lambda, const IndexedDenseVector & vector);
    void addIndexedDenseToDenseAbs(Numerical::Double lambda, const IndexedDenseVector & vector);
    void addIndexedDenseToDenseAbsRel(Numerical::Double lambda, const IndexedDenseVector & vector);

    void addSparseToDenseFast(Numerical::Double lambda, const SparseVector & vector);
    void addSparseToDenseAbs(Numerical::Double lambda, const SparseVector & vector);
    void addSparseToDenseAbsRel(Numerical::Double lambda, const SparseVector & vector);

    Numerical::Double dotProductDenseToIndexedDenseUnstable(const DenseVector & vector1, const IndexedDenseVector & vector2) const;
    Numerical::Double dotProductDenseToIndexedDenseFast(const DenseVector & vector1, const IndexedDenseVector & vector2) const;
    Numerical::Double dotProductDenseToIndexedDenseAbs(const DenseVector & vector1, const IndexedDenseVector & vector2) const;
    Numerical::Double dotProductDenseToIndexedDenseAbsRel(const DenseVector & vector1, const IndexedDenseVector & vector2) const;

    Numerical::Double dotProductDenseToDenseUnstable(const DenseVector & vector1, const DenseVector & vector2) const;
    Numerical::Double dotProductDenseToDenseFast(const DenseVector & vector1, const DenseVector & vector2) const;
    Numerical::Double dotProductDenseToDenseAbs(const DenseVector & vector1, const DenseVector & vector2) const;
    Numerical::Double dotProductDenseToDenseAbsRel(const DenseVector & vector1, const DenseVector & vector2) const;

    Numerical::Double dotProductDenseToSparseUnstable(const DenseVector & vector1, const SparseVector & vector2) const;
    Numerical::Double dotProductDenseToSparseFast(const DenseVector & vector1, const SparseVector & vector2) const;
    Numerical::Double dotProductDenseToSparseAbs(const DenseVector & vector1, const SparseVector & vector2) const;
    Numerical::Double dotProductDenseToSparseAbsRel(const DenseVector & vector1, const SparseVector & vector2) const;
};

#endif // DENSEVECTOR_H
