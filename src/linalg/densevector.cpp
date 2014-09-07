#include <linalg/densevector.h>
#include <linalg/indexeddensevector.h>
#include <linalg/sparsevector.h>

DenseVector::DenseVector(unsigned int length)
{
    m_addDenseToDense = &DenseVector::addDenseToDenseFast;
    m_addIndexedDenseToDense = &DenseVector::addIndexedDenseToDenseFast;
    m_addSparseToDense = &DenseVector::addSparseToDenseFast;
    m_length = length;
    m_data = nullptr;

    if (m_length > 0) {
        m_data = alloc<Numerical::Double, 32>(m_length);
        CLEAR_DOUBLES(m_data, m_length);
    }
}

DenseVector::DenseVector(const DenseVector &orig)
{
    copy(orig);
}

DenseVector::DenseVector(DenseVector &&orig)
{
    move(orig);
}

DenseVector &DenseVector::operator=(const DenseVector &orig)
{
    if (this == &orig) {
        return *this;
    }
    release();
    copy(orig);
    return *this;
}

DenseVector &DenseVector::operator=(DenseVector &&orig)
{
    if (this == &orig) {
        return *this;
    }
    release();
    move(orig);
    return *this;
}

DenseVector::~DenseVector()
{
    release();
}

Numerical::Double DenseVector::euclidNorm() const
{
    unsigned int index;
    Numerical::Double sum = 0.0;
    for (index = 0; index < m_length; index++) {
        sum += m_data[index] * m_data[index];
    }
    return Numerical::sqrt(sum);
}

Numerical::Double DenseVector::l1Norm() const
{
    unsigned int index;
    Numerical::Double sum = 0.0;
    for (index = 0; index < m_length; index++) {
        sum += Numerical::fabs(m_data[index]);
    }
    return sum;
}

DenseVector &DenseVector::addIndexedDenseVector(Numerical::Double lambda,
                                                const IndexedDenseVector &vector)
{
    (this->*m_addIndexedDenseToDense)(lambda, vector);
    return *this;
}

DenseVector &DenseVector::addSparseVector(Numerical::Double lambda,
                                           const SparseVector &vector)
{
    (this->*m_addSparseToDense)(lambda, vector);
    return *this;
}

DenseVector &DenseVector::addDenseVector(Numerical::Double lambda, const DenseVector &vector)
{
    (this->*m_addDenseToDense)(lambda, vector);
    return *this;
}

void DenseVector::scale(Numerical::Double lambda)
{
    unsigned int index;
    for (index = 0; index < m_length; index++) {
        m_data[index] *= lambda;
    }
}

void DenseVector::clear()
{
    CLEAR_DOUBLES(m_data, m_length);
}

void DenseVector::setAddMode(Numerical::ADD_TYPE type)
{
    switch (type) {
    case Numerical::ADD_FAST:
        m_addDenseToDense = &DenseVector::addDenseToDenseFast;
        m_addIndexedDenseToDense = &DenseVector::addIndexedDenseToDenseFast;
        m_addSparseToDense = &DenseVector::addSparseToDenseFast;
        break;
    case Numerical::ADD_ABS:
        m_addDenseToDense = &DenseVector::addDenseToDenseAbs;
        m_addIndexedDenseToDense = &DenseVector::addIndexedDenseToDenseAbs;
        m_addSparseToDense = &DenseVector::addSparseToDenseAbs;
        break;
    case Numerical::ADD_ABS_REL:
        m_addDenseToDense = &DenseVector::addDenseToDenseAbsRel;
        m_addIndexedDenseToDense = &DenseVector::addIndexedDenseToDenseAbsRel;
        m_addSparseToDense = &DenseVector::addSparseToDenseAbsRel;
        break;
    case Numerical::ADD_REL:
        m_addDenseToDense = &DenseVector::addDenseToDenseRel;
        m_addIndexedDenseToDense = &DenseVector::addIndexedDenseToDenseRel;
        m_addSparseToDense = &DenseVector::addSparseToDenseRel;
        break;
    }
}

DenseVector DenseVector::createUnitVector(unsigned int length,
                                          unsigned int index)
{
    DenseVector result(length);
    result.set(index, 1.0);
    return result;
}

DenseVector DenseVector::createVectorFromDenseArray(const Numerical::Double *source,
                                                    unsigned int size)
{
    DenseVector result;
    result.m_length = size;
    result.m_data = alloc<Numerical::Double, 32>(size);
    COPY_DOUBLES(result.m_data, source, size);
    return result;
}

DenseVector DenseVector::createVectorFromSparseArray(const Numerical::Double *data,
                                                     const unsigned int *indices,
                                                     unsigned int count,
                                                     unsigned int length)
{
    DenseVector result(length);
    unsigned int index;
    for (index = 0; index < count; index++) {
        result.m_data[ indices[index] ] = data[ indices[index] ];
    }
    return result;
}

void DenseVector::copy(const DenseVector &orig)
{
    m_addDenseToDense = orig.m_addDenseToDense;
    m_addIndexedDenseToDense = orig.m_addIndexedDenseToDense;
    m_addSparseToDense = orig.m_addSparseToDense;
    m_length = orig.m_length;
    m_data = nullptr;
    if (m_length > 0) {
        m_data = alloc<Numerical::Double, 32>(m_length);
        COPY_DOUBLES(m_data, orig.m_data, m_length);
    }
}

void DenseVector::move(DenseVector &orig)
{
    m_addDenseToDense = orig.m_addDenseToDense;
    m_addIndexedDenseToDense = orig.m_addIndexedDenseToDense;
    m_addSparseToDense = orig.m_addSparseToDense;
    m_length = orig.m_length;
    orig.m_length = 0;
    m_data = orig.m_data;
    orig.m_data = 0;
}

void DenseVector::release()
{
    ::release(m_data);
    m_data = nullptr;
}

void DenseVector::addDenseToDenseFast(Numerical::Double lambda,
                                      const DenseVector &vector)
{
    unsigned int index;
    for (index = 0; index < vector.m_length; index++) {
        m_data[index] += vector.m_data[index] * lambda;
    }
}

void DenseVector::addDenseToDenseAbs(Numerical::Double lambda,
                                     const DenseVector &vector)
{
    unsigned int index;
    for (index = 0; index < vector.m_length; index++) {
        m_data[index] = Numerical::stableAddAbs(m_data[index], vector.m_data[index] * lambda);
    }
}

void DenseVector::addDenseToDenseRel(Numerical::Double lambda,
                                     const DenseVector &vector)
{
    unsigned int index;
    for (index = 0; index < vector.m_length; index++) {
        m_data[index] = Numerical::stableAddRel(m_data[index], vector.m_data[index] * lambda);
    }
}

void DenseVector::addDenseToDenseAbsRel(Numerical::Double lambda,
                                        const DenseVector &vector)
{
    unsigned int index;
    for (index = 0; index < vector.m_length; index++) {
        m_data[index] = Numerical::stableAdd(m_data[index], vector.m_data[index] * lambda);
    }

}

void DenseVector::addIndexedDenseToDenseFast(Numerical::Double lambda,
                                             const IndexedDenseVector &vector)
{
    unsigned int nonZeroIndex;
    for (nonZeroIndex = 0; nonZeroIndex < vector.m_nonZeros; nonZeroIndex++) {
        const unsigned int index = vector.m_nonzeroIndices[nonZeroIndex];
        m_data[index] += lambda * vector.m_data[index];
    }
}

void DenseVector::addIndexedDenseToDenseAbs(Numerical::Double lambda,
                                            const IndexedDenseVector &vector)
{
    unsigned int nonZeroIndex;
    for (nonZeroIndex = 0; nonZeroIndex < vector.m_nonZeros; nonZeroIndex++) {
        const unsigned int index = vector.m_nonzeroIndices[nonZeroIndex];
        m_data[index] = Numerical::stableAddAbs(m_data[index], lambda * vector.m_data[index]);
    }
}

void DenseVector::addIndexedDenseToDenseRel(Numerical::Double lambda,
                                            const IndexedDenseVector &vector)
{
    unsigned int nonZeroIndex;
    for (nonZeroIndex = 0; nonZeroIndex < vector.m_nonZeros; nonZeroIndex++) {
        const unsigned int index = vector.m_nonzeroIndices[nonZeroIndex];
        m_data[index] = Numerical::stableAddRel(m_data[index], lambda * vector.m_data[index]);
    }
}

void DenseVector::addIndexedDenseToDenseAbsRel(Numerical::Double lambda,
                                               const IndexedDenseVector &vector)
{
    unsigned int nonZeroIndex;
    for (nonZeroIndex = 0; nonZeroIndex < vector.m_nonZeros; nonZeroIndex++) {
        const unsigned int index = vector.m_nonzeroIndices[nonZeroIndex];
        m_data[index] = Numerical::stableAdd(m_data[index], lambda * vector.m_data[index]);
    }
}

void DenseVector::addSparseToDenseFast(Numerical::Double lambda,
                                       const SparseVector &vector)
{
    unsigned int nonZeroIndex;
    for (nonZeroIndex = 0; nonZeroIndex < vector.m_nonZeros; nonZeroIndex++) {
        const unsigned int index = vector.m_indices[nonZeroIndex];
        m_data[index] += lambda * vector.m_data[nonZeroIndex];
    }
}

void DenseVector::addSparseToDenseAbs(Numerical::Double lambda,
                                      const SparseVector &vector)
{
    unsigned int nonZeroIndex;
    for (nonZeroIndex = 0; nonZeroIndex < vector.m_nonZeros; nonZeroIndex++) {
        const unsigned int index = vector.m_indices[nonZeroIndex];
        m_data[index] = Numerical::stableAddAbs(m_data[index], lambda * vector.m_data[nonZeroIndex]);
    }
}

void DenseVector::addSparseToDenseRel(Numerical::Double lambda,
                                      const SparseVector &vector)
{
    unsigned int nonZeroIndex;
    for (nonZeroIndex = 0; nonZeroIndex < vector.m_nonZeros; nonZeroIndex++) {
        const unsigned int index = vector.m_indices[nonZeroIndex];
        m_data[index] = Numerical::stableAddRel(m_data[index], lambda * vector.m_data[nonZeroIndex]);
    }
}

void DenseVector::addSparseToDenseAbsRel(Numerical::Double lambda,
                                         const SparseVector &vector)
{
    unsigned int nonZeroIndex;
    for (nonZeroIndex = 0; nonZeroIndex < vector.m_nonZeros; nonZeroIndex++) {
        const unsigned int index = vector.m_indices[nonZeroIndex];
        m_data[index] = Numerical::stableAdd(m_data[index], lambda * vector.m_data[nonZeroIndex]);
    }
}

void DenseVector::_globalInit()
{

}

