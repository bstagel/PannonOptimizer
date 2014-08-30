#include <linalg/indexeddensevector.h>


IndexedDenseVector::IndexedDenseVector(unsigned int length)
{
    m_data = nullptr;
    m_nonzeroIndices = nullptr;
    m_indexIndices = nullptr;
    m_nonZeros = 0;
    m_length = length;
    m_addIndexedDenseToIndexedDense = &IndexedDenseVector::addIndexedDenseToIndexedDenseFast;

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
    (this->*m_addIndexedDenseToIndexedDense)(lambda, vector);

    return *this;
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
        m_addIndexedDenseToIndexedDense = &IndexedDenseVector::addIndexedDenseToIndexedDenseFast;
        break;
    case Numerical::ADD_ABS:
        m_addIndexedDenseToIndexedDense = &IndexedDenseVector::addIndexedDenseToIndexedDenseAbs;
        break;
    case Numerical::ADD_ABS_REL:
        m_addIndexedDenseToIndexedDense = &IndexedDenseVector::addIndexedDenseToIndexedDenseAbsRel;
        break;
    case Numerical::ADD_REL:
        m_addIndexedDenseToIndexedDense = &IndexedDenseVector::addIndexedDenseToIndexedDenseRel;
        break;
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
    m_addIndexedDenseToIndexedDense = orig.m_addIndexedDenseToIndexedDense;
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
    m_addIndexedDenseToIndexedDense = orig.m_addIndexedDenseToIndexedDense;
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

void IndexedDenseVector::addIndexedDenseToIndexedDenseRel(Numerical::Double lambda,
                                                          const IndexedDenseVector &vector)
{
    unsigned int nonzeroIndex;
    for (nonzeroIndex = 0; nonzeroIndex < vector.m_nonZeros; nonzeroIndex++) {
        const unsigned int index = vector.m_nonzeroIndices[nonzeroIndex];
        set(index, Numerical::stableAddRel(m_data[index], lambda * vector.m_data[index]));
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
