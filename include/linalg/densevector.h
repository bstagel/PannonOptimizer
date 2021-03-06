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

#ifndef DENSEVECTOR_H
#define DENSEVECTOR_H

#include <globals.h>
#include <utils/numerical.h>
#include <utils/iterator.h>
#include <utils/exceptions.h>
#include <utils/stacktrace.h>

class IndexedDenseVector;
class SparseVector;

/**
 *
 */
class DenseVector {
    friend class InitPanOpt;
    friend class IndexedDenseVector;
    friend class SparseVector;
    friend class PfiBasis;
    friend class LuBasis;

    //TODO remove, debug only
    friend class DualSimplex;
private:

    Numerical::Double * m_data;

    unsigned int m_length;

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
    typedef NormalIterator< std::remove_reference<decltype(*m_data)>::type > Iterator;

    /**
     * This iterator iterates only on the nonzero elements of the vector.
     */
    typedef SkipIterator<std::remove_reference<decltype(*m_data)>::type, SkipZeros> NonzeroIterator;

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

    DenseVector & operator=(const SparseVector & orig);

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
    decltype(*m_data) at(unsigned int index) const {
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

    Numerical::Double euclidNorm2() const;

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
        return NonzeroIterator(m_data, m_data, m_data + m_length);
    }

    /**
     * Returns an iterator which points to the end of the vector.
     *
     * @return Iterator to the end of the vector.
     */
    ALWAYS_INLINE NonzeroIterator endNonzero() const {
        return NonzeroIterator(m_data, m_data + m_length, m_data + m_length);
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

    void resize(unsigned int length, Numerical::Double value);

    void reInit(unsigned int length);

    friend std::ostream & operator<<(std::ostream & os, const DenseVector & vector);

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

    static AddDenseToDense sm_addDenseToDense;
    static AddIndexedDenseToDense sm_addIndexedDenseToDense;
    static AddSparseToDense sm_addSparseToDense;

    static DenseToIndexedDenseDotProduct sm_denseToIndexedDenseDotProduct;
    static DenseToDenseDotProduct sm_denseToDenseDotProduct;
    static DenseToSparseDotProduct sm_denseToSparseDotProduct;

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
