#include <linalg/indexeddensevector.h>
#include <linalg/densevector.h>
#include <linalg/sparsevector.h>
#include <utils/architecture.h>

thread_local IndexedDenseVector::AddIndexedDenseToIndexedDense IndexedDenseVector::sm_addIndexedDenseToIndexedDense;
thread_local IndexedDenseVector::AddDenseToIndexedDense IndexedDenseVector::sm_addDenseToIndexedDense;
thread_local IndexedDenseVector::AddSparseToIndexedDense IndexedDenseVector::sm_addSparseToIndexedDense;
thread_local IndexedDenseVector::IndexedDenseToIndexedDenseDotProduct IndexedDenseVector::sm_indexedDenseToIndexedDenseDotProduct;
thread_local IndexedDenseVector::IndexedDenseToDenseDotProduct IndexedDenseVector::sm_indexedDenseToDenseDotProduct;
thread_local IndexedDenseVector::IndexedDenseToSparseDotProduct IndexedDenseVector::sm_indexedDenseToSparseDotProduct;

IndexedDenseVector::IndexedDenseVector(unsigned int length)
{
    m_data = nullptr;
    m_nonzeroIndices = nullptr;
    m_indexIndices = nullptr;
    m_nonZeros = 0;
    m_length = length;

    if (length > 0) {
        m_data = alloc<Numerical::Double, 32>(length);
        m_nonzeroIndices = alloc<unsigned int, 16>(length);
        m_indexIndices = alloc<unsigned int *, 16>(length);
        CLEAR_DOUBLES(m_data, length);
        panOptMemset(m_indexIndices, 0, sizeof( unsigned int * ) * length);
    }
}

IndexedDenseVector::IndexedDenseVector(const IndexedDenseVector &orig)
{
    copy(orig);
}

IndexedDenseVector::IndexedDenseVector(IndexedDenseVector &&orig)
{
    move(orig);
}

IndexedDenseVector &IndexedDenseVector::operator=(const IndexedDenseVector &orig)
{
    if (this == &orig) {
        return *this;
    }
    // TODO: itt majd lehet optimalizalni, ha eleg nagy a mostani tomb
    release();
    copy(orig);
    return *this;
}

IndexedDenseVector &IndexedDenseVector::operator=(IndexedDenseVector &&orig)
{
    if (this == &orig) {
        return *this;
    }
    release();
    move(orig);
    return *this;
}

IndexedDenseVector::~IndexedDenseVector()
{
    release();
}

void IndexedDenseVector::set(unsigned int index, Numerical::Double value)
{
    const Numerical::Double old = m_data[index];
    m_data[index] = value;
    if (old == 0.0 && value != 0.0) {
        m_indexIndices[index] = m_nonzeroIndices + m_nonZeros;
        m_nonzeroIndices[m_nonZeros] = index;
        m_nonZeros++;
    } else if (old != 0.0 && value == 0.0) {
        *(m_indexIndices[index]) = m_nonzeroIndices[m_nonZeros - 1];
        m_indexIndices[index] = nullptr;
        m_nonZeros--;
    }
}

Numerical::Double IndexedDenseVector::euclidNorm() const
{
    Numerical::Double sum = 0.0;
    unsigned int index;
    for (index = 0; index < m_length; index++) {
        sum += m_data[index] * m_data[index];
    }
    return Numerical::sqrt(sum);
}

Numerical::Double IndexedDenseVector::l1Norm() const
{
    Numerical::Double sum = 0.0;
    unsigned int index;
    for (index = 0; index < m_length; index++) {
        sum += Numerical::fabs(m_data[index]);
    }
    return sum;
}

IndexedDenseVector &IndexedDenseVector::addVector(Numerical::Double lambda,
                                                  const IndexedDenseVector &vector)
{
    (this->*sm_addIndexedDenseToIndexedDense)(lambda, vector);
    return *this;
}

IndexedDenseVector &IndexedDenseVector::addVector(Numerical::Double lambda,
                                                  const DenseVector &vector)
{
    (this->*sm_addDenseToIndexedDense)(lambda, vector);
    return *this;
}

IndexedDenseVector &IndexedDenseVector::addVector(Numerical::Double lambda,
                                                  const SparseVector &vector)
{
    (this->*sm_addSparseToIndexedDense)(lambda, vector);
    return *this;
}

Numerical::Double IndexedDenseVector::dotProduct(const IndexedDenseVector &vector) const
{
    return (this->*sm_indexedDenseToIndexedDenseDotProduct)(*this, vector);
}

Numerical::Double IndexedDenseVector::dotProduct(const DenseVector &vector) const
{
    return (this->*sm_indexedDenseToDenseDotProduct)(*this, vector);
}

Numerical::Double IndexedDenseVector::dotProduct(const SparseVector &vector) const
{
    return (this->*sm_indexedDenseToSparseDotProduct)(*this, vector);
}

void IndexedDenseVector::scale(Numerical::Double lambda)
{
    if (lambda == 0.0) {
        clear();
    } else if (lambda != 1.0) {
        // TODO: optimalizalasi lehetoseg, ha eleg ritkas, eleg az
        // indexek szerint menni
        // assembly gyorsitas
        unsigned int index;
        for (index = 0; index < m_length; index++) {
            m_data[index] *= lambda;
        }
    }
}

void IndexedDenseVector::sortIndices()
{
    //TODO: Exploit sorted indices everywhere, where it is possible
    const unsigned int chooseSortSteps = m_nonZeros * m_nonZeros / 2;
    if (chooseSortSteps < m_length) {
        unsigned int index1;
        for (index1 = 0; index1 < m_nonZeros - 1; index1++) {
            unsigned int index2;
            unsigned int minIndex = index1;
            for (index2 = index1 + 1; index2 < m_nonZeros; index2++) {
                if (m_nonzeroIndices[index2] < m_nonzeroIndices[minIndex] ) {
                    minIndex = index2;
                }
            }
            unsigned int temp = m_nonzeroIndices[index1];
            m_nonzeroIndices[index1] = m_nonzeroIndices[minIndex];
            m_nonzeroIndices[minIndex] = temp;
            minIndex = m_nonzeroIndices[index1];
            m_indexIndices[ minIndex ] = m_nonzeroIndices + index1;
        }
    } else {
        unsigned int index;
        unsigned int nonzero = 0;
        for (index = 0; index < m_length; index++) {
            if (m_data[index] != 0.0) {
                m_nonzeroIndices[nonzero] = index;
                m_indexIndices[index] = m_nonzeroIndices + nonzero;
                nonzero++;
            }
        }
    }
}

void IndexedDenseVector::clear()
{
    m_nonZeros = 0;
    CLEAR_DOUBLES(m_data, m_length);
    panOptMemset(m_indexIndices, 0, sizeof( unsigned int * ) * m_length);
}

void IndexedDenseVector::setAddMode(Numerical::ADD_TYPE type)
{
    switch (type) {
    case Numerical::ADD_FAST:
        sm_addIndexedDenseToIndexedDense = &IndexedDenseVector::addIndexedDenseToIndexedDenseFast;
        sm_addDenseToIndexedDense = &IndexedDenseVector::addDenseToIndexedDenseFast;
        sm_addSparseToIndexedDense = &IndexedDenseVector::addSparseToIndexedDenseFast;
        break;
    case Numerical::ADD_ABS:
        sm_addIndexedDenseToIndexedDense = &IndexedDenseVector::addIndexedDenseToIndexedDenseAbs;
        sm_addDenseToIndexedDense = &IndexedDenseVector::addDenseToIndexedDenseAbs;
        sm_addSparseToIndexedDense = &IndexedDenseVector::addSparseToIndexedDenseAbs;
        break;
    case Numerical::ADD_ABS_REL:
        sm_addIndexedDenseToIndexedDense = &IndexedDenseVector::addIndexedDenseToIndexedDenseAbsRel;
        sm_addDenseToIndexedDense = &IndexedDenseVector::addDenseToIndexedDenseAbsRel;
        sm_addSparseToIndexedDense = &IndexedDenseVector::addSparseToIndexedDenseAbsRel;
        break;
    }
}

void IndexedDenseVector::setDotProductMode(Numerical::DOT_PRODUCT_TYPE type)
{
    switch (type) {
    case Numerical::DOT_UNSTABLE:
        sm_indexedDenseToIndexedDenseDotProduct = &IndexedDenseVector::dotProductIndexedDenseToIndexedDenseUnstable;
        sm_indexedDenseToDenseDotProduct = &IndexedDenseVector::dotProductIndexedDenseToDenseUnstable;
        sm_indexedDenseToSparseDotProduct = &IndexedDenseVector::dotProductIndexedDenseToSparseUnstable;
        break;
    case Numerical::DOT_FAST:
        sm_indexedDenseToIndexedDenseDotProduct = &IndexedDenseVector::dotProductIndexedDenseToIndexedDenseFast;
        sm_indexedDenseToDenseDotProduct = &IndexedDenseVector::dotProductIndexedDenseToDenseFast;
        sm_indexedDenseToSparseDotProduct = &IndexedDenseVector::dotProductIndexedDenseToSparseFast;
        break;
    case Numerical::DOT_ABS:
        sm_indexedDenseToIndexedDenseDotProduct = &IndexedDenseVector::dotProductIndexedDenseToIndexedDenseAbs;
        sm_indexedDenseToDenseDotProduct = &IndexedDenseVector::dotProductIndexedDenseToDenseAbs;
        sm_indexedDenseToSparseDotProduct = &IndexedDenseVector::dotProductIndexedDenseToSparseAbs;
        break;
    case Numerical::DOT_ABS_REL:
        sm_indexedDenseToIndexedDenseDotProduct = &IndexedDenseVector::dotProductIndexedDenseToIndexedDenseAbsRel;
        sm_indexedDenseToDenseDotProduct = &IndexedDenseVector::dotProductIndexedDenseToDenseAbsRel;
        sm_indexedDenseToSparseDotProduct = &IndexedDenseVector::dotProductIndexedDenseToSparseAbsRel;
        break;
    }
}

void IndexedDenseVector::remove(unsigned int index)
{
    if (m_data[index] == 0.0) {
        for (; index < m_length - 1; index++) {
            m_data[index] = m_data[index + 1];
            m_indexIndices[index] = m_indexIndices[index + 1];
        }
        unsigned int index2;
        for (index2 = 0; index2 < m_nonZeros; index2++) {
            if (m_nonzeroIndices[index2] >= index) {
                m_nonzeroIndices[index2]--;
            }
        }
        m_length--;
    } else {
        for (; index < m_length - 1; index++) {
            m_data[index] = m_data[index + 1];
            m_indexIndices[index] = m_indexIndices[index + 1];
        }
        unsigned int index2;
        unsigned int pos = 0;
        for (index2 = 0; index2 < m_nonZeros; index2++) {
            if (m_nonzeroIndices[index2] == index) {
                pos = index2;
            }
            if (m_nonzeroIndices[index2] >= index) {
                m_nonzeroIndices[index2]--;
            }
        }

        if (m_nonZeros >= 1) {
            m_nonzeroIndices[pos] = m_nonzeroIndices[m_nonZeros - 1];
            m_indexIndices[index] = m_nonzeroIndices + pos;
        }
        m_length--;
        m_nonZeros--;
    }
}

void IndexedDenseVector::insert(unsigned int index, Numerical::Double value)
{
    Numerical::Double * newData = alloc<Numerical::Double, 32>(m_length + 1);
    COPY_DOUBLES(newData, m_data, index);
    newData[index] = value;
    COPY_DOUBLES(newData + index + 1, m_data + index + 1, m_length - index);
    ::release(m_data);
    m_data = newData;

    if (value != 0.0) {
        unsigned int * newIndices = alloc<unsigned int, 16>(m_nonZeros + 1);
        unsigned int index2;
        for (index2 = 0; index2 < m_nonZeros; index2++) {
            if (m_nonzeroIndices[index2] < index) {
                newIndices[index2] = m_nonzeroIndices[index2];
            } else {
                newIndices[index2] = m_nonzeroIndices[index2] + 1;
            }
            unsigned int actIndex = newIndices[index2];
            m_indexIndices[actIndex] = newIndices + index2;
        }
        newIndices[m_nonZeros] = index;

        m_nonZeros++;
    }

    m_length++;
}

void IndexedDenseVector::append(Numerical::Double value)
{
    Numerical::Double * newData = alloc<Numerical::Double, 32>(m_length + 1);
    COPY_DOUBLES(newData, m_data, m_length);
    newData[m_length] = value;
    ::release(m_data);
    m_data = newData;

    if (value != 0.0) {
        unsigned int * newIndices = alloc<unsigned int, 16>(m_nonZeros + 1);
        unsigned int index2;
        for (index2 = 0; index2 < m_nonZeros; index2++) {
            newIndices[index2] = m_nonzeroIndices[index2];
            unsigned int actIndex = newIndices[index2];
            m_indexIndices[actIndex] = newIndices + index2;
        }
        newIndices[m_nonZeros] = m_length;

        m_nonZeros++;
    }

    m_length++;

}

void IndexedDenseVector::reserve()
{
    Numerical::Double * newData = alloc<Numerical::Double, 32>(m_length);
    COPY_DOUBLES(newData, m_data, m_length);
    ::release(m_data);
    m_data = newData;

    unsigned int * newNonzeroIndices = alloc<unsigned int, 16>(m_length);
    panOptMemcpy(newNonzeroIndices, m_nonzeroIndices, m_nonZeros);
    ::release(m_nonzeroIndices);
    m_nonzeroIndices = newNonzeroIndices;

    unsigned int ** newIndexIndices = alloc<unsigned int *, 16>(m_length);
    panOptMemcpy(newIndexIndices, m_indexIndices, sizeof(unsigned int *) * m_length);
    ::release(m_indexIndices);
    m_indexIndices = newIndexIndices;
}

void IndexedDenseVector::resize(unsigned int length)
{
    if (length <= m_length) {
        unsigned int pos = 0;
        while (pos < m_nonZeros) {
            if (m_nonzeroIndices[pos] >= length) {
                m_nonzeroIndices[pos] = m_nonzeroIndices[m_nonZeros - 1];
                unsigned int index = m_nonzeroIndices[pos];
                m_indexIndices[index] = m_nonzeroIndices + pos;
                m_nonZeros--;
            } else {
                pos++;
            }
        }
        m_length = length;
    } else {
        Numerical::Double * newData = alloc<Numerical::Double, 32>(length);
        COPY_DOUBLES(newData, m_data, m_length);
        CLEAR_DOUBLES(newData + m_length, length - m_length);
        ::release(m_data);
        m_data = newData;

        unsigned int * newNonzeroIndices = alloc<unsigned int, 16>(length);
        panOptMemcpy(newNonzeroIndices, m_nonzeroIndices, m_nonZeros * sizeof(unsigned int));
        ::release(m_nonzeroIndices);
        m_nonzeroIndices = newNonzeroIndices;

        unsigned ** newIndexIndices = alloc<unsigned int *, 16>(length);
        panOptMemcpy(newIndexIndices, m_indexIndices, m_length * sizeof(unsigned int *));
        panOptMemset(newIndexIndices + m_length, 0, (length - m_length) * sizeof(unsigned int *));
        ::release(m_indexIndices);
        m_indexIndices = newIndexIndices;
        m_length = length;
    }
}

IndexedDenseVector IndexedDenseVector::createUnitVector(unsigned int length, unsigned int index)
{
    IndexedDenseVector result(length);
    result.set(index, 1.0);
    return result;
}

IndexedDenseVector IndexedDenseVector::createVectorFromDenseArray(const Numerical::Double *source,
                                                                  unsigned int size)
{
    IndexedDenseVector result;
    result.m_length = size;
    result.m_data = alloc<Numerical::Double, 32>(size);
    result.m_nonzeroIndices = alloc<unsigned int, 16>(size);
    result.m_indexIndices = alloc<unsigned int *, 16>(size);
    panOptMemset(result.m_indexIndices, 0, sizeof( unsigned int * ) * size);

    unsigned int index = 0;
    for (index = 0; index < size; index++) {
        result.set(index, source[index]);
    }
    return result;
}

IndexedDenseVector IndexedDenseVector::createVectorFromSparseArray(const Numerical::Double *data,
                                                                   const unsigned int *indices,
                                                                   unsigned int count,
                                                                   unsigned int length)
{
    IndexedDenseVector result;
    result.m_length = length;
    result.m_nonZeros = count;
    result.m_data = alloc<Numerical::Double, 32>(count);
    result.m_nonzeroIndices = alloc<unsigned int, 16>(count);
    result.m_indexIndices = alloc<unsigned int *, 16>(count);
    CLEAR_DOUBLES(result.m_data, length);
    panOptMemset(result.m_indexIndices, 0, sizeof( unsigned int * ) * count);

    unsigned int index;
    for (index = 0; index < count; index++) {
        result.set( indices[index], data[index] );
    }

    return result;
}

void IndexedDenseVector::copy(const IndexedDenseVector &orig)
{
    m_length = orig.m_length;
    m_nonZeros = orig.m_nonZeros;
    m_data = alloc<Numerical::Double, 32>(m_length);
    m_nonzeroIndices = alloc<unsigned int, 16>(m_length);
    m_indexIndices = alloc<unsigned int *, 16>(m_length);
    COPY_DOUBLES(m_data, orig.m_data, m_length);
    panOptMemcpy(m_nonzeroIndices, orig.m_nonzeroIndices, sizeof(unsigned int) * m_nonZeros);
    panOptMemcpy(m_indexIndices, orig.m_indexIndices, sizeof(unsigned int*) * m_length);
}

void IndexedDenseVector::release()
{
    ::release(m_data);
    m_data = 0;
    ::release(m_nonzeroIndices);
    m_nonzeroIndices = 0;
    ::release(m_indexIndices);
    m_indexIndices = 0;
    m_nonZeros = 0;
    m_length = 0;
}

void IndexedDenseVector::move(IndexedDenseVector &orig)
{
    m_data = orig.m_data;
    orig.m_data = nullptr;
    m_nonzeroIndices = orig.m_nonzeroIndices;
    orig.m_nonzeroIndices = nullptr;
    m_indexIndices = orig.m_indexIndices;
    orig.m_indexIndices = nullptr;
    m_nonZeros = orig.m_nonZeros;
    orig.m_nonZeros = 0;
    m_length = orig.m_length;
    orig.m_length = 0;
}

void IndexedDenseVector::addIndexedDenseToIndexedDenseFast(Numerical::Double lambda,
                                                           const IndexedDenseVector &vector)
{
    unsigned int nonzeroIndex;
    for (nonzeroIndex = 0; nonzeroIndex < vector.m_nonZeros; nonzeroIndex++) {
        const unsigned int index = vector.m_nonzeroIndices[nonzeroIndex];
        set(index, m_data[index] + lambda * vector.m_data[index]);
    }
}

void IndexedDenseVector::addIndexedDenseToIndexedDenseAbs(Numerical::Double lambda,
                                                          const IndexedDenseVector &vector)
{
    unsigned int nonzeroIndex;
    for (nonzeroIndex = 0; nonzeroIndex < vector.m_nonZeros; nonzeroIndex++) {
        const unsigned int index = vector.m_nonzeroIndices[nonzeroIndex];
        set(index, Numerical::stableAddAbs(m_data[index], lambda * vector.m_data[index]));
    }
}

void IndexedDenseVector::addIndexedDenseToIndexedDenseAbsRel(Numerical::Double lambda,
                                                             const IndexedDenseVector &vector)
{
    unsigned int nonzeroIndex;
    for (nonzeroIndex = 0; nonzeroIndex < vector.m_nonZeros; nonzeroIndex++) {
        const unsigned int index = vector.m_nonzeroIndices[nonzeroIndex];
        set(index, Numerical::stableAdd(m_data[index], lambda * vector.m_data[index]));
    }

}

void IndexedDenseVector::addDenseToIndexedDenseFast(Numerical::Double lambda,
                                                    const DenseVector &vector)
{
    unsigned int index;
    for (index = 0; index < vector.m_length; index++) {
        set(index, m_data[index] + lambda * vector.m_data[index]);
    }
}

void IndexedDenseVector::addDenseToIndexedDenseAbs(Numerical::Double lambda,
                                                   const DenseVector &vector)
{
    unsigned int index;
    for (index = 0; index < vector.m_length; index++) {
        set(index, Numerical::stableAddAbs(m_data[index], lambda * vector.m_data[index]));
    }
}

void IndexedDenseVector::addDenseToIndexedDenseAbsRel(Numerical::Double lambda,
                                                      const DenseVector &vector)
{
    unsigned int index;
    for (index = 0; index < vector.m_length; index++) {
        set(index, Numerical::stableAdd(m_data[index], lambda * vector.m_data[index]));
    }
}

void IndexedDenseVector::addSparseToIndexedDenseFast(Numerical::Double lambda,
                                                     const SparseVector &vector)
{
    unsigned int nonzeroIndex;
    for (nonzeroIndex = 0; nonzeroIndex < vector.m_nonZeros; nonzeroIndex++) {
        const unsigned int index = vector.m_indices[nonzeroIndex];
        set(index, m_data[index] + lambda * vector.m_data[nonzeroIndex]);
    }
}

void IndexedDenseVector::addSparseToIndexedDenseAbs(Numerical::Double lambda,
                                                    const SparseVector &vector)
{
    unsigned int nonzeroIndex;
    for (nonzeroIndex = 0; nonzeroIndex < vector.m_nonZeros; nonzeroIndex++) {
        const unsigned int index = vector.m_indices[nonzeroIndex];
        set(index, Numerical::stableAddAbs(m_data[index], lambda * vector.m_data[nonzeroIndex]));
    }
}

void IndexedDenseVector::addSparseToIndexedDenseAbsRel(Numerical::Double lambda,
                                                       const SparseVector &vector)
{
    unsigned int nonzeroIndex;
    for (nonzeroIndex = 0; nonzeroIndex < vector.m_nonZeros; nonzeroIndex++) {
        const unsigned int index = vector.m_indices[nonzeroIndex];
        set(index, Numerical::stableAdd(m_data[index], lambda * vector.m_data[nonzeroIndex]));
    }
}

Numerical::Double IndexedDenseVector::dotProductIndexedDenseToIndexedDenseUnstable(
        const IndexedDenseVector &vector1,
        const IndexedDenseVector &vector2) const
{
    unsigned int nonZeros = vector1.m_nonZeros;
    const unsigned int * indices = vector1.m_nonzeroIndices;
    if (vector1.m_nonZeros > vector2.m_nonZeros) {
        nonZeros = vector2.m_nonZeros;
        indices = vector2.m_nonzeroIndices;
    }

    return Architecture::getIndexedDenseToIndexedDenseDotProductUnstable()(vector1.m_data,
                                                                           vector2.m_data,
                                                                           indices,
                                                                           nonZeros);
}

Numerical::Double IndexedDenseVector::dotProductIndexedDenseToIndexedDenseFast(
        const IndexedDenseVector &vector1,
        const IndexedDenseVector &vector2) const
{
    Numerical::Double pos;
    Numerical::Double neg;
    unsigned int nonZeros = vector1.m_nonZeros;
    const unsigned int * indices = vector1.m_nonzeroIndices;
    if (vector1.m_nonZeros > vector2.m_nonZeros) {
        nonZeros = vector2.m_nonZeros;
        indices = vector2.m_nonzeroIndices;
    }
    // indexeddensetodense
    pos = Architecture::getIndexedDenseToIndexedDenseDotProductStable()(vector1.m_data,
                                                                        vector2.m_data,
                                                                        indices,
                                                                        nonZeros,
                                                                        &neg);
    return pos + neg;
}

Numerical::Double IndexedDenseVector::dotProductIndexedDenseToIndexedDenseAbs(
        const IndexedDenseVector &vector1,
        const IndexedDenseVector &vector2) const
{
    Numerical::Double pos;
    Numerical::Double neg;
    unsigned int nonZeros = vector1.m_nonZeros;
    const unsigned int * indices = vector1.m_nonzeroIndices;
    if (vector1.m_nonZeros > vector2.m_nonZeros) {
        nonZeros = vector2.m_nonZeros;
        indices = vector2.m_nonzeroIndices;
    }
    pos = Architecture::getIndexedDenseToIndexedDenseDotProductStable()(vector1.m_data,
                                                                        vector2.m_data,
                                                                        indices,
                                                                        nonZeros,
                                                                        &neg);
    return Numerical::stableAddAbs(pos, neg);
}

Numerical::Double IndexedDenseVector::dotProductIndexedDenseToIndexedDenseAbsRel(
        const IndexedDenseVector &vector1,
        const IndexedDenseVector &vector2) const
{
    Numerical::Double pos;
    Numerical::Double neg;
    unsigned int nonZeros = vector1.m_nonZeros;
    const unsigned int * indices = vector1.m_nonzeroIndices;
    if (vector1.m_nonZeros > vector2.m_nonZeros) {
        nonZeros = vector2.m_nonZeros;
        indices = vector2.m_nonzeroIndices;
    }
    pos = Architecture::getIndexedDenseToIndexedDenseDotProductStable()(vector1.m_data,
                                                                        vector2.m_data,
                                                                        indices,
                                                                        nonZeros,
                                                                        &neg);
    return Numerical::stableAdd(pos, neg);
}

Numerical::Double IndexedDenseVector::dotProductIndexedDenseToDenseUnstable(const IndexedDenseVector &vector1,
                                                                            const DenseVector &vector2) const
{
    //TODO: What is this actually? This function is not suitable
    return Architecture::getIndexedDenseToIndexedDenseDotProductUnstable()(vector1.m_data,
                                                                           vector2.m_data,
                                                                           vector1.m_nonzeroIndices,
                                                                           vector1.m_nonZeros);
}

Numerical::Double IndexedDenseVector::dotProductIndexedDenseToDenseFast(const IndexedDenseVector &vector1,
                                                                        const DenseVector &vector2) const
{
    Numerical::Double pos;
    Numerical::Double neg;
    pos = Architecture::getIndexedDenseToIndexedDenseDotProductStable()(vector1.m_data,
                                                                        vector2.m_data,
                                                                        vector1.m_nonzeroIndices,
                                                                        vector1.m_nonZeros,
                                                                        &neg);
    return pos + neg;
}

Numerical::Double IndexedDenseVector::dotProductIndexedDenseToDenseAbs(const IndexedDenseVector &vector1,
                                                                       const DenseVector &vector2) const
{
    Numerical::Double pos;
    Numerical::Double neg;
    pos = Architecture::getIndexedDenseToIndexedDenseDotProductStable()(vector1.m_data,
                                                                        vector2.m_data,
                                                                        vector1.m_nonzeroIndices,
                                                                        vector1.m_nonZeros,
                                                                        &neg);
    return Numerical::stableAddAbs(pos, neg);
}


Numerical::Double IndexedDenseVector::dotProductIndexedDenseToDenseAbsRel(const IndexedDenseVector &vector1,
                                                                          const DenseVector &vector2) const
{
    Numerical::Double pos;
    Numerical::Double neg;
    pos = Architecture::getIndexedDenseToIndexedDenseDotProductStable()(vector1.m_data,
                                                                        vector2.m_data,
                                                                        vector1.m_nonzeroIndices,
                                                                        vector1.m_nonZeros,
                                                                        &neg);
    return Numerical::stableAdd(pos, neg);
}

Numerical::Double IndexedDenseVector::dotProductIndexedDenseToSparseUnstable(const IndexedDenseVector &vector1,
                                                                             const SparseVector &vector2) const
{
    return Architecture::getDenseToSparseDotProductUnstable()(vector1.m_data,
                                                              vector2.m_data,
                                                              vector2.m_indices,
                                                              vector2.m_nonZeros);
}

Numerical::Double IndexedDenseVector::dotProductIndexedDenseToSparseFast(const IndexedDenseVector &vector1,
                                                                         const SparseVector &vector2) const
{
    Numerical::Double pos;
    Numerical::Double neg;
    pos = Architecture::getDenseToSparseDotProductStable()(vector1.m_data,
                                                     vector2.m_data,
                                                     vector2.m_indices,
                                                     vector2.m_nonZeros,
                                                     &neg);
    return pos + neg;
}

Numerical::Double IndexedDenseVector::dotProductIndexedDenseToSparseAbs(const IndexedDenseVector &vector1,
                                                                        const SparseVector &vector2) const
{
    Numerical::Double pos;
    Numerical::Double neg;
    pos = Architecture::getDenseToSparseDotProductStable()(vector1.m_data,
                                                     vector2.m_data,
                                                     vector2.m_indices,
                                                     vector2.m_nonZeros,
                                                     &neg);
    return Numerical::stableAddAbs(pos, neg);
}

Numerical::Double IndexedDenseVector::dotProductIndexedDenseToSparseAbsRel(const IndexedDenseVector &vector1,
                                                                           const SparseVector &vector2) const
{
    Numerical::Double pos;
    Numerical::Double neg;
    pos = Architecture::getDenseToSparseDotProductStable()(vector1.m_data,
                                                     vector2.m_data,
                                                     vector2.m_indices,
                                                     vector2.m_nonZeros,
                                                     &neg);
    return Numerical::stableAdd(pos, neg);
}

void IndexedDenseVector::_globalInit()
{
    setAddMode(Numerical::ADD_ABS_REL);
    setDotProductMode(Numerical::DOT_ABS_REL);
}
