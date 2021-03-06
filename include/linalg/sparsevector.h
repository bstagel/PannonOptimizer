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
 *
 */

#ifndef SPARSEVECTOR_H
#define SPARSEVECTOR_H

#include <globals.h>
#include <utils/numerical.h>
#include <utils/iterator.h>
#include <vector>
#include <type_traits>

class DenseVector;
class IndexedDenseVector;

//extern bool DEBUG_MODE;

/**
 *
 */
class SparseVector {
    friend class InitPanOpt;
    friend class DenseVector;
    friend class IndexedDenseVector;
    friend class SparseVectorTestSuite;
    friend class PfiBasis;
    friend class LuBasis;

    Numerical::Double * m_data;

    unsigned int * m_indices;

    unsigned int m_length;

    unsigned int m_nonZeros;

    unsigned int m_capacity;

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
    typedef ValueIndexPairIterator< std::remove_reference<decltype(*m_data)>::type, unsigned int> NonzeroIterator;

    /**
     *
     * @param length
     */
    SparseVector(unsigned int length=0);

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

    SparseVector &operator=(const DenseVector & orig);

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
    static SparseVector createVectorFromDenseArray(const Numerical::Double *source,
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
                                                    unsigned int count,
                                                    unsigned int length);

    /**
     *
     * @param vector
     * @param pivotValue
     * @param pivotIndex
     * @return
     */
    static SparseVector * createEtaVector(const SparseVector & vector,
                                          Numerical::Double pivotValue,
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

    void setNewNonzero(unsigned int index, Numerical::Double value);

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

    SparseVector &elementaryFtran(const SparseVector & eta, unsigned int pivot);

    /**
     * Adds the SparseVector type vector spefified by the arguments to
     * the current vector.
     *
     * @param lambda Multiplier of the argument.
     * @param vector The vector to be added to the current vector.
     * @return Reference of the current vector object.
     */
    SparseVector & addVector(Numerical::Double lambda,
                             const SparseVector & vector);

    /**
     * Adds the DenseVector type vector spefified by the arguments to
     * the current vector.
     *
     * @param lambda Multiplier of the argument.
     * @param vector The vector to be added to the current vector.
     * @return Reference of the current vector object.
     */
    SparseVector & addVector(Numerical::Double lambda,
                             const DenseVector & vector);

    /**
     * Adds the IndexedDenseVector type vector spefified by the arguments to
     * the current vector.
     *
     * @param lambda Multiplier of the argument.
     * @param vector The vector to be added to the current vector.
     * @return Reference of the current vector object.
     */
    SparseVector & addVector(Numerical::Double lambda,
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

    void scaleByLambdas(const std::vector<Numerical::Double> &lambdas);

    void scaleElementBy(unsigned int index, Numerical::Double lambda);

    bool operator==(const SparseVector& other) const;

//    static void checkScattered(int nzr, unsigned length);

//    static void checkVector(const SparseVector & vector);

    ALWAYS_INLINE bool operator!=(const SparseVector& other) const
    {
        return !(*this == other);
    }

    ALWAYS_INLINE void prepareForData(const unsigned int nonZeros, const unsigned int length)
    {
        ::release(m_data);
        ::release(m_indices);

        m_length = length;
        m_capacity = nonZeros + sm_elbowRoom;
        m_indices = alloc<unsigned int, 16>(nonZeros + sm_elbowRoom);
        m_data = Numerical::allocDouble(nonZeros + sm_elbowRoom);
        m_nonZeros = 0;
    }

    ALWAYS_INLINE void newNonZero(const Numerical::Double value, const unsigned int index)
    {
        m_data[m_nonZeros] = value;
        m_indices[m_nonZeros] = index;
        m_nonZeros++;
    }

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

    Numerical::Double absMaxElement() const;

    Numerical::Double scaleAndGetResults(const std::vector<Numerical::Double> &multipliers,
                                         Numerical::Double lambda,
                                         Numerical::Double * squareSumPtr,
                                         Numerical::Double * minPtr,
                                         Numerical::Double * maxPtr);

    Numerical::Double absMaxSumsAndMinMax(Numerical::Double * squareSumPtr,
                                          Numerical::Double * minPtr,
                                          Numerical::Double * maxPtr) const;

    Numerical::Double absMaxSums(Numerical::Double * squareSumPtr) const;

    void batchRemove(const std::vector<int> & indices);

    void reserve();

    void resize(unsigned int length);

    void reInit(unsigned int length);

protected:

    typedef void (SparseVector::*AddSparseToSparse)(Numerical::Double,
                                                    const SparseVector &);

    typedef void (SparseVector::*AddDenseToSparse)(Numerical::Double,
                                                   const DenseVector &);

    typedef void (SparseVector::*AddIndexedDenseToSparse)(Numerical::Double,
                                                          const IndexedDenseVector &);

    typedef Numerical::Double (SparseVector::*SparseToIndexedDenseDotProduct)(const SparseVector &,
                                                                              const IndexedDenseVector &) const;

    typedef Numerical::Double (SparseVector::*SparseToDenseDotProduct)(const SparseVector &,
                                                                       const DenseVector &) const;

    typedef Numerical::Double (SparseVector::*SparseToSparseDotProduct)(const SparseVector &,
                                                                        const SparseVector &) const;

    static AddSparseToSparse sm_addSparseToSparse;
    static AddDenseToSparse sm_addDenseToSparse;
    static AddIndexedDenseToSparse sm_addIndexedDenseToSparse;

    static SparseToIndexedDenseDotProduct sm_sparseToIndexedDenseDotProduct;
    static SparseToDenseDotProduct sm_sparseToDenseDotProduct;
    static SparseToSparseDotProduct sm_sparseToSparseDotProduct;


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
     *
     */
    static thread_local unsigned int * sm_removeIndices;

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
    static unsigned int sm_elbowRoom;

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

    void move(SparseVector & orig);

    void release();

    void resizeCapacity(unsigned int capacity);

    void scatter() const;

    void scatterData() const;

    void addSparseToSparseFast(Numerical::Double lambda, const SparseVector & vector);
    void addSparseToSparseAbs(Numerical::Double lambda, const SparseVector & vector);
    void addSparseToSparseAbsRel(Numerical::Double lambda, const SparseVector & vector);

    void addDenseToSparseFast(Numerical::Double lambda, const DenseVector & vector);
    void addDenseToSparseAbs(Numerical::Double lambda, const DenseVector & vector);
    void addDenseToSparseAbsRel(Numerical::Double lambda, const DenseVector & vector);

    void addIndexedDenseToSparseFast(Numerical::Double lambda, const IndexedDenseVector & vector);
    void addIndexedDenseToSparseAbs(Numerical::Double lambda, const IndexedDenseVector & vector);
    void addIndexedDenseToSparseAbsRel(Numerical::Double lambda, const IndexedDenseVector & vector);

    Numerical::Double dotProductSparseToIndexedDenseUnstable(const SparseVector & vector1, const IndexedDenseVector & vector2) const;
    Numerical::Double dotProductSparseToIndexedDenseFast(const SparseVector & vector1, const IndexedDenseVector & vector2) const;
    Numerical::Double dotProductSparseToIndexedDenseAbs(const SparseVector & vector1, const IndexedDenseVector & vector2) const;
    Numerical::Double dotProductSparseToIndexedDenseAbsRel(const SparseVector & vector1, const IndexedDenseVector & vector2) const;

    Numerical::Double dotProductSparseToDenseUnstable(const SparseVector & vector1, const DenseVector & vector2) const;
    Numerical::Double dotProductSparseToDenseFast(const SparseVector & vector1,const DenseVector & vector2) const;
    Numerical::Double dotProductSparseToDenseAbs(const SparseVector & vector1,const DenseVector & vector2) const;
    Numerical::Double dotProductSparseToDenseAbsRel(const SparseVector & vector1,const DenseVector & vector2) const;

    Numerical::Double dotProductSparseToSparseUnstable(const SparseVector & vector1,const SparseVector & vector2) const;
    Numerical::Double dotProductSparseToSparseFast(const SparseVector & vector1,const SparseVector & vector2) const;
    Numerical::Double dotProductSparseToSparseAbs(const SparseVector & vector1,const SparseVector & vector2) const;
    Numerical::Double dotProductSparseToSparseAbsRel(const SparseVector & vector1,const SparseVector & vector2) const;

    void convertFromDenseArray(const Numerical::Double *source,
                               unsigned int length);
};

#endif // SPARSEVECTOR_H
