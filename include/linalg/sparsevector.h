/**
 *
 */

#ifndef SPARSEVECTOR_H
#define SPARSEVECTOR_H

#include <globals.h>
#include <utils/numerical.h>
#include <utils/iterator.h>

class DenseVector;
class IndexedDenseVector;

/**
 *
 */
class SparseVector {
    friend class InitPanOpt;
    friend class DenseVector;
    friend class IndexedDenseVector;
    friend class SparseVectorTestSuite;

    /**
     *
     */
    class AddFast {
    public:
        /**
         *
         * @param a
         * @param b
         * @return
         */
        ALWAYS_INLINE static Numerical::Double add(const Numerical::Double & a,
                                                   const Numerical::Double & b) {
            return a + b;
        }
    };

    /**
     *
     */
    class AddAbs {
    public:
        /**
         *
         * @param a
         * @param b
         * @return
         */
        ALWAYS_INLINE static Numerical::Double add(const Numerical::Double & a,
                                                   const Numerical::Double & b) {
            return Numerical::stableAddAbs(a, b);
        }
    };

    /**
     *
     */
    class AddRel {
    public:
        /**
         *
         * @param a
         * @param b
         * @return
         */
        ALWAYS_INLINE static Numerical::Double add(const Numerical::Double & a,
                                                   const Numerical::Double & b) {
            return Numerical::stableAddRel(a, b);
        }
    };

    /**
     *
     */
    class AddAbsRel {
    public:
        /**
         *
         * @param a
         * @param b
         * @return
         */
        ALWAYS_INLINE static Numerical::Double add(const Numerical::Double & a,
                                                   const Numerical::Double & b) {
            return Numerical::stableAdd(a, b);
        }
    };

    template<class ADD>
    friend void addSparseToSparseTemplate(Numerical::Double lambda,
                                          SparseVector * vector1,
                                          const SparseVector & vector2);

    template<class ADD>
    friend void addDenseToSparseTemplate(Numerical::Double lambda,
                                         SparseVector * vector1,
                                         const DenseVector & vector2);

    template<class ADD>
    friend void addIndexedDenseToSparseTemplate(Numerical::Double lambda,
                                                SparseVector * vector1,
                                                const IndexedDenseVector & vector2);
public:

    /**
     *
     */
    typedef ValueIndexPairIterator<Numerical::Double, unsigned int> NonzeroIterator;

    /**
     *
     * @param length
     */
    SparseVector(unsigned int length);

    /**
     *
     * @param orig
     */
    SparseVector(const SparseVector & orig);

    /**
     *
     * @param orig
     */
    SparseVector(SparseVector && orig);

    /**
     *
     * @param orig
     * @return
     */
    SparseVector &operator=(const SparseVector & orig);

    /**
     *
     * @param orig
     * @return
     */
    SparseVector & operator=(SparseVector && orig);

    /**
     *
     */
    ~SparseVector();

    /**
     * Creates a unit vector with the specified length.
     *
     * @param length The length of the new vector.
     * @param index The index of the value 1.
     * @return The result vector.
     */
    static SparseVector createUnitVector(unsigned int length,
                                         unsigned int index);

    /**
     * Creates a vector from the given dense array.
     *
     * @param source The original dense array.
     * @param size Size of the array.
     * @return The result vector.
     */
    static SparseVector createVectorFromDenseArray(const Numerical::Double * source,
                                                   unsigned int length);


    /**
     * Creates a vector from the given sparse array.
     *
     * @param data The array of the nonzero elements.
     * @param indices The array of the nonzero indices.
     * @param count Size of the arrays.
     * @param length Length of the vector.
     * @return The result vector.
     */
    static SparseVector createVectorFromSparseArray(const Numerical::Double * data,
                                                    const unsigned int * indices,
                                                    unsigned int count,                                                         unsigned int length);

    /**
     *
     * @param vector
     * @param pivotValue
     * @param pivotIndex
     * @return
     */
    static SparseVector * createEtaByPivotValue(const SparseVector & vector,
                                                Numerical::Double pivotValue,
                                                unsigned int pivotIndex);

    /**
     *
     * @param vector
     * @param pivotIndex
     * @return
     */
    static SparseVector * createEtaByPivotIndex(const SparseVector & vector,
                                                unsigned int pivotIndex);

    /**
     * Returns with the value of the specified element. Checks the index,
     * and when it is invalid, the function trowns an InvalidIndexException exception.
     *
     * @param index The index of the element.
     * @return Value of the index^th element.
     */
    Numerical::Double at(unsigned int index) const;

    /**
     * Returns with the value of the specified element.
     *
     * @param index The index of the element.
     * @return Value of the index^th element.
     */
    Numerical::Double operator[](unsigned int index) const;

    /**
     * Sets the specified element of the vector.
     *
     * @param index The index of the changed element.
     * @param value The new value.
     */
    void set(unsigned int index, Numerical::Double value);

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
     * Adds the SparseVector type vector spefified by the arguments to
     * the current vector.
     *
     * @param lambda Multiplier of the argument.
     * @param vector The vector to be added to the current vector.
     * @return Reference of the current vector object.
     */
    SparseVector & addSparseVector(Numerical::Double lambda,
                                   const SparseVector & vector);

    /**
     * Adds the DenseVector type vector spefified by the arguments to
     * the current vector.
     *
     * @param lambda Multiplier of the argument.
     * @param vector The vector to be added to the current vector.
     * @return Reference of the current vector object.
     */
    SparseVector & addDenseVector(Numerical::Double lambda,
                                  const DenseVector & vector);

    /**
     * Adds the IndexedDenseVector type vector spefified by the arguments to
     * the current vector.
     *
     * @param lambda Multiplier of the argument.
     * @param vector The vector to be added to the current vector.
     * @return Reference of the current vector object.
     */
    SparseVector & addIndexedDenseVector(Numerical::Double lambda,
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
    static void setAddMode(Numerical::ADD_TYPE type);

    /**
     *
     * @param type
     */
    static void setDotProductMode(Numerical::DOT_PRODUCT_TYPE type);

    /**
     *
     * @return
     */
    ALWAYS_INLINE NonzeroIterator beginNonzero() const {
        return NonzeroIterator(m_data, m_indices);
    }

    /**
     *
     * @return
     */
    ALWAYS_INLINE NonzeroIterator endNonzero() const {
        return NonzeroIterator(m_data + m_nonZeros, m_indices + m_nonZeros);
    }

protected:

    unsigned int m_length;

    unsigned int m_nonZeros;

    Numerical::Double * m_data;

    unsigned int * m_indices;

    unsigned int m_capacity;

    /**
     *
     */
    typedef void (SparseVector::*AddSparseToSparse)(Numerical::Double,
                                                    const SparseVector &);

    /**
     *
     */
    typedef void (SparseVector::*AddDenseToSparse)(Numerical::Double,
                                                   const DenseVector &);

    /**
     *
     */
    typedef void (SparseVector::*AddIndexedDenseToSparse)(Numerical::Double,
                                                          const IndexedDenseVector &);

    typedef Numerical::Double (SparseVector::*SparseToIndexedDenseDotProduct)(
            const SparseVector &,
            const IndexedDenseVector &) const;

    typedef Numerical::Double (SparseVector::*SparseToDenseDotProduct)(
            const SparseVector &,
            const DenseVector &) const;

    typedef Numerical::Double (SparseVector::*SparseToSparseDotProduct)(
            const SparseVector &,
            const SparseVector &) const;

    /**
     */
    static thread_local AddSparseToSparse sm_addSparseToSparse;

    /**
     */
    static thread_local AddDenseToSparse sm_addDenseToSparse;

    /**
     */
    static thread_local AddIndexedDenseToSparse sm_addIndexedDenseToSparse;

    static thread_local SparseToIndexedDenseDotProduct sm_sparseToIndexedDenseDotProduct;

    static thread_local SparseToDenseDotProduct sm_sparseToDenseDotProduct;

    static thread_local SparseToSparseDotProduct sm_sparseToSparseDotProduct;

    /**
     * Temporary vector for linear time dot product and add operations.
     */
    static thread_local Numerical::Double * sm_fullLengthVector;

    /**
     * Temporary vector for collecting nonzero indices.
     */
    static thread_local unsigned int * sm_indexVector;

    /**
     *
     */
    static thread_local unsigned int ** sm_indexPointerVector;

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
     *
     */
    static thread_local unsigned int sm_elbowRoom;

    /**
     *
     * @param length
     */
    static void resizeFullLengthVector(unsigned int length);

    /**
     *
     */
    static void _globalInit();

    static void _globalRelease();

    void copy(const SparseVector & orig);

    void release();

    void move(SparseVector & orig);

    void resize(unsigned int capacity);

    void scatter() const;

    /**
     * @param lambda
     * @param vector
     */
    void addSparseToSparseFast(Numerical::Double lambda,
                               const SparseVector & vector);

    /**
     * @param lambda
     * @param vector
     */
    void addSparseToSparseAbs(Numerical::Double lambda,
                              const SparseVector & vector);

    /**
     * @param lambda
     * @param vector
     */
    void addSparseToSparseRel(Numerical::Double lambda,
                              const SparseVector & vector);

    /**
     * @param lambda
     * @param vector
     */
    void addSparseToSparseAbsRel(Numerical::Double lambda,
                                 const SparseVector & vector);

    /**
     * @param lambda
     * @param vector
     */
    void addDenseToSparseFast(Numerical::Double lambda,
                              const DenseVector & vector);

    /**
     * @param lambda
     * @param vector
     */
    void addDenseToSparseAbs(Numerical::Double lambda,
                             const DenseVector & vector);

    /**
     * @param lambda
     * @param vector
     */
    void addDenseToSparseRel(Numerical::Double lambda,
                             const DenseVector & vector);

    /**
     * @param lambda
     * @param vector
     */
    void addDenseToSparseAbsRel(Numerical::Double lambda,
                                const DenseVector & vector);

    /**
     * @param lambda
     * @param vector
     */
    void addIndexedDenseToSparseFast(Numerical::Double lambda,
                                     const IndexedDenseVector & vector);

    /**
     * @param lambda
     * @param vector
     */
    void addIndexedDenseToSparseAbs(Numerical::Double lambda,
                                    const IndexedDenseVector & vector);

    /**
     * @param lambda
     * @param vector
     */
    void addIndexedDenseToSparseRel(Numerical::Double lambda,
                                    const IndexedDenseVector & vector);

    /**
     * @param lambda
     * @param vector
     */
    void addIndexedDenseToSparseAbsRel(Numerical::Double lambda,
                                       const IndexedDenseVector & vector);

    Numerical::Double dotProductSparseToIndexedDenseUnstable(const SparseVector & vector1,
                                                             const IndexedDenseVector & vector2) const;

    Numerical::Double dotProductSparseToIndexedDenseFast(const SparseVector & vector1,
                                                         const IndexedDenseVector & vector2) const;

    Numerical::Double dotProductSparseToIndexedDenseAbs(const SparseVector & vector1,
                                                        const IndexedDenseVector & vector2) const;

    Numerical::Double dotProductSparseToIndexedDenseRel(const SparseVector & vector1,
                                                        const IndexedDenseVector & vector2) const;

    Numerical::Double dotProductSparseToIndexedDenseAbsRel(const SparseVector & vector1,
                                                           const IndexedDenseVector & vector2) const;

    Numerical::Double dotProductSparseToDenseUnstable(const SparseVector & vector1,
                                                      const DenseVector & vector2) const;

    Numerical::Double dotProductSparseToDenseFast(const SparseVector & vector1,
                                                  const DenseVector & vector2) const;

    Numerical::Double dotProductSparseToDenseAbs(const SparseVector & vector1,
                                                 const DenseVector & vector2) const;

    Numerical::Double dotProductSparseToDenseRel(const SparseVector & vector1,
                                                 const DenseVector & vector2) const;

    Numerical::Double dotProductSparseToDenseAbsRel(const SparseVector & vector1,
                                                    const DenseVector & vector2) const;

    Numerical::Double dotProductSparseToSparseUnstable(const SparseVector & vector1,
                                                       const SparseVector & vector2) const;

    Numerical::Double dotProductSparseToSparseFast(const SparseVector & vector1,
                                                   const SparseVector & vector2) const;

    Numerical::Double dotProductSparseToSparseAbs(const SparseVector & vector1,
                                                  const SparseVector & vector2) const;

    Numerical::Double dotProductSparseToSparseRel(const SparseVector & vector1,
                                                  const SparseVector & vector2) const;

    Numerical::Double dotProductSparseToSparseAbsRel(const SparseVector & vector1,
                                                     const SparseVector & vector2) const;

};

#endif // SPARSEVECTOR_H
