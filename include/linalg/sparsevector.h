/**
 *
 */

#ifndef SPARSEVECTOR_H
#define SPARSEVECTOR_H

#include <globals.h>
#include <utils/numerical.h>
#include <utils/iterator.h>

/**
 *
 */
class SparseVector {
    friend class InitPanOpt;

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

public:

    SparseVector(unsigned int length);

    SparseVector(const SparseVector & orig);

    SparseVector(SparseVector && orig);

    SparseVector &operator=(const SparseVector & orig);

    SparseVector & operator=(SparseVector && orig);

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
     * Adds the vector spefified by the arguments to the current vector.
     *
     * @param lambda Multiplier of the argument.
     * @param vector The vector to be added to the current vector.
     * @return Reference of the current vector object.
     */
    SparseVector & addVector(Numerical::Double lambda,
                             const SparseVector & vector);

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

protected:

    /**
     *
     */
    typedef void (SparseVector::*AddSparseToSparse)(Numerical::Double,
                                                    const SparseVector &);

    /**
     */
    AddSparseToSparse m_addSparseToSparse;

    unsigned int m_length;

    unsigned int m_nonZeros;

    Numerical::Double * m_data;

    unsigned int * m_indices;

    unsigned int m_capacity;

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
     * Describes how many Vector type objects exists.
     * It is important for releasing sm_fullLengthVector. When this variable is zero, the program
     * releases sm_fullLengthVector automatically.
     *
     * @see sm_fullLengthVector
     */
    static thread_local unsigned int sm_fullLenghtReferenceCounter;

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

    void copy(const SparseVector & orig);

    void release();

    void move(SparseVector & orig);

    void resize(unsigned int capacity);

    void scatter();

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


};

template<class ADD>
void addSparseToSparseTemplate(Numerical::Double lambda,
                               SparseVector * vector1,
                               const SparseVector & vector2
                               ) {
    vector1->scatter();
    unsigned int nonZeroIndex;
    unsigned int nonZeros = vector1->m_nonZeros;
    for (nonZeroIndex = 0; nonZeroIndex < vector2.m_nonZeros; nonZeroIndex++) {
        const unsigned int index = vector2.m_indices[nonZeroIndex];
        const Numerical::Double sum = ADD::add(SparseVector::sm_fullLengthVector[index],
                                               vector2.m_data[nonZeroIndex] * lambda);
        if (SparseVector::sm_fullLengthVector[index] == 0.0 && sum != 0.0) {
            SparseVector::sm_indexVector[nonZeros] = index;
            nonZeros++;
        } else if (SparseVector::sm_fullLengthVector[index] != 0.0 && sum == 0.0) {
            *(SparseVector::sm_indexPointerVector[index]) = SparseVector::sm_indexVector[nonZeros - 1];
            *(SparseVector::sm_indexPointerVector[SparseVector::sm_indexVector[nonZeros - 1]]) = index;
            SparseVector::sm_indexPointerVector[index] = nullptr;
            nonZeros--;
        }
        SparseVector::sm_fullLengthVector[index] = sum;
    }
    if (nonZeros > vector1->m_capacity) {
        vector1->resize(nonZeros);
    }
    for (nonZeroIndex = 0; nonZeroIndex < nonZeros; nonZeroIndex++) {
        const unsigned int index = SparseVector::sm_indexVector[nonZeroIndex];
        vector1->m_data[nonZeroIndex] = SparseVector::sm_fullLengthVector[index];
        vector1->m_indices[nonZeroIndex] = index;
        SparseVector::sm_fullLengthVector[index] = 0.0;
        SparseVector::sm_indexPointerVector[index] = nullptr;
    }
    vector1->m_nonZeros = nonZeros;

}


#endif // SPARSEVECTOR_H
