#include <linalg/densevector.h>
#include <linalg/indexeddensevector.h>
#include <linalg/sparsevector.h>
#include <utils/architecture.h>

thread_local DenseVector::AddDenseToDense DenseVector::sm_addDenseToDense;
thread_local DenseVector::AddIndexedDenseToDense DenseVector::sm_addIndexedDenseToDense;
thread_local DenseVector::AddSparseToDense DenseVector::sm_addSparseToDense;
thread_local DenseVector::DenseToDenseDotProduct DenseVector::sm_denseToDenseDotProduct;
thread_local DenseVector::DenseToIndexedDenseDotProduct DenseVector::sm_denseToIndexedDenseDotProduct;
thread_local DenseVector::DenseToSparseDotProduct DenseVector::sm_denseToSparseDotProduct;

DenseVector::DenseVector(unsigned int length)
{
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
    (this->*sm_addIndexedDenseToDense)(lambda, vector);
    return *this;
}

DenseVector &DenseVector::addSparseVector(Numerical::Double lambda,
                                          const SparseVector &vector)
{
    (this->*sm_addSparseToDense)(lambda, vector);
    return *this;
}

Numerical::Double DenseVector::dotProductIndexedDenseVector(const IndexedDenseVector &vector) const
{
    return (this->*sm_denseToIndexedDenseDotProduct)(*this, vector);
}

Numerical::Double DenseVector::dotProductDenseVector(const DenseVector &vector) const
{
    return (this->*sm_denseToDenseDotProduct)(*this, vector);
}

Numerical::Double DenseVector::dotProductSparseVector(const SparseVector &vector) const
{
    return (this->*sm_denseToSparseDotProduct)(*this, vector);
}

DenseVector &DenseVector::addDenseVector(Numerical::Double lambda, const DenseVector &vector)
{
    (this->*sm_addDenseToDense)(lambda, vector);
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
        sm_addDenseToDense = &DenseVector::addDenseToDenseFast;
        sm_addIndexedDenseToDense = &DenseVector::addIndexedDenseToDenseFast;
        sm_addSparseToDense = &DenseVector::addSparseToDenseFast;
        break;
    case Numerical::ADD_ABS:
        sm_addDenseToDense = &DenseVector::addDenseToDenseAbs;
        sm_addIndexedDenseToDense = &DenseVector::addIndexedDenseToDenseAbs;
        sm_addSparseToDense = &DenseVector::addSparseToDenseAbs;
        break;
    case Numerical::ADD_ABS_REL:
        sm_addDenseToDense = &DenseVector::addDenseToDenseAbsRel;
        sm_addIndexedDenseToDense = &DenseVector::addIndexedDenseToDenseAbsRel;
        sm_addSparseToDense = &DenseVector::addSparseToDenseAbsRel;
        break;
    case Numerical::ADD_REL:
        sm_addDenseToDense = &DenseVector::addDenseToDenseRel;
        sm_addIndexedDenseToDense = &DenseVector::addIndexedDenseToDenseRel;
        sm_addSparseToDense = &DenseVector::addSparseToDenseRel;
        break;
    }
}

void DenseVector::setDotProductMode(Numerical::DOT_PRODUCT_TYPE type)
{
    switch (type) {
    case Numerical::DOT_UNSTABLE:
        sm_denseToIndexedDenseDotProduct = &DenseVector::dotProductDenseToIndexedDenseUnstable;
        sm_denseToDenseDotProduct = &DenseVector::dotProductDenseToDenseUnstable;
        sm_denseToSparseDotProduct = &DenseVector::dotProductDenseToSparseUnstable;
        break;
    case Numerical::DOT_FAST:
        sm_denseToIndexedDenseDotProduct = &DenseVector::dotProductDenseToIndexedDenseFast;
        sm_denseToDenseDotProduct = &DenseVector::dotProductDenseToDenseFast;
        sm_denseToSparseDotProduct = &DenseVector::dotProductDenseToSparseFast;
        break;
    case Numerical::DOT_ABS:
        sm_denseToIndexedDenseDotProduct = &DenseVector::dotProductDenseToIndexedDenseAbs;
        sm_denseToDenseDotProduct = &DenseVector::dotProductDenseToDenseAbs;
        sm_denseToSparseDotProduct = &DenseVector::dotProductDenseToSparseAbs;
        break;
    case Numerical::DOT_ABS_REL:
        sm_denseToIndexedDenseDotProduct = &DenseVector::dotProductDenseToIndexedDenseAbsRel;
        sm_denseToDenseDotProduct = &DenseVector::dotProductDenseToDenseAbsRel;
        sm_denseToSparseDotProduct = &DenseVector::dotProductDenseToSparseAbsRel;
        break;
    case Numerical::DOT_REL:
        sm_denseToIndexedDenseDotProduct = &DenseVector::dotProductDenseToIndexedDenseRel;
        sm_denseToDenseDotProduct = &DenseVector::dotProductDenseToDenseRel;
        sm_denseToSparseDotProduct = &DenseVector::dotProductDenseToSparseRel;
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
    m_length = orig.m_length;
    m_data = nullptr;
    if (m_length > 0) {
        m_data = alloc<Numerical::Double, 32>(m_length);
        COPY_DOUBLES(m_data, orig.m_data, m_length);
    }
}

void DenseVector::move(DenseVector &orig)
{
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

Numerical::Double DenseVector::dotProductDenseToIndexedDenseUnstable(const DenseVector &vector1,
                                                                     const IndexedDenseVector &vector2) const
{
    return vector2.dotProductIndexedDenseToDenseUnstable(vector2, vector1);
}

Numerical::Double DenseVector::dotProductDenseToIndexedDenseFast(const DenseVector &vector1,
                                                                 const IndexedDenseVector &vector2) const
{
    return vector2.dotProductIndexedDenseToDenseFast(vector2, vector1);
}

Numerical::Double DenseVector::dotProductDenseToIndexedDenseAbs(const DenseVector &vector1,
                                                                const IndexedDenseVector &vector2) const
{
    return vector2.dotProductIndexedDenseToDenseAbs(vector2, vector1);
}

Numerical::Double DenseVector::dotProductDenseToIndexedDenseRel(const DenseVector &vector1,
                                                                const IndexedDenseVector &vector2) const
{
    return vector2.dotProductIndexedDenseToDenseRel(vector2, vector1);
}

Numerical::Double DenseVector::dotProductDenseToIndexedDenseAbsRel(const DenseVector &vector1,
                                                                   const IndexedDenseVector &vector2) const
{
    return vector2.dotProductIndexedDenseToDenseAbsRel(vector2, vector1);
}

Numerical::Double DenseVector::dotProductDenseToDenseUnstable(const DenseVector &vector1,
                                                              const DenseVector &vector2) const
{
    return Architecture::getDenseToDenseDotProductUnstable()(vector1.m_data,
                                                             vector2.m_data,
                                                             vector1.m_length);
}

Numerical::Double DenseVector::dotProductDenseToDenseFast(const DenseVector &vector1,
                                                          const DenseVector &vector2) const
{
    Numerical::Double pos;
    Numerical::Double neg;
    pos = Architecture::getDenseToDenseDotProductStable()(vector1.m_data,
                                                          vector2.m_data,
                                                          vector1.m_length,
                                                          &neg);
    return pos + neg;
}

Numerical::Double DenseVector::dotProductDenseToDenseAbs(const DenseVector &vector1,
                                                         const DenseVector &vector2) const
{
    Numerical::Double pos;
    Numerical::Double neg;
    pos = Architecture::getDenseToDenseDotProductStable()(vector1.m_data,
                                                          vector2.m_data,
                                                          vector1.m_length,
                                                          &neg);
    return Numerical::stableAddAbs(pos, neg);
}

Numerical::Double DenseVector::dotProductDenseToDenseRel(const DenseVector &vector1,
                                                         const DenseVector &vector2) const
{
    Numerical::Double pos;
    Numerical::Double neg;
    pos = Architecture::getDenseToDenseDotProductStable()(vector1.m_data,
                                                          vector2.m_data,
                                                          vector1.m_length,
                                                          &neg);
    return Numerical::stableAddRel(pos, neg);
}

Numerical::Double DenseVector::dotProductDenseToDenseAbsRel(const DenseVector &vector1,
                                                            const DenseVector &vector2) const
{
    Numerical::Double pos;
    Numerical::Double neg;
    pos = Architecture::getDenseToDenseDotProductStable()(vector1.m_data,
                                                          vector2.m_data,
                                                          vector1.m_length,
                                                          &neg);
    return Numerical::stableAdd(pos, neg);
}

Numerical::Double DenseVector::dotProductDenseToSparseUnstable(const DenseVector &vector1,
                                                               const SparseVector &vector2) const
{
    return Architecture::getDenseToSparseDotProductUnstable()(vector1.m_data,
                                                              vector2.m_data,
                                                              vector2.m_indices,
                                                              vector2.m_nonZeros);
}

Numerical::Double DenseVector::dotProductDenseToSparseFast(const DenseVector &vector1,
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

Numerical::Double DenseVector::dotProductDenseToSparseAbs(const DenseVector &vector1,
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

Numerical::Double DenseVector::dotProductDenseToSparseRel(const DenseVector &vector1,
                                                          const SparseVector &vector2) const
{
    Numerical::Double pos;
    Numerical::Double neg;
    pos = Architecture::getDenseToSparseDotProductStable()(vector1.m_data,
                                                           vector2.m_data,
                                                           vector2.m_indices,
                                                           vector2.m_nonZeros,
                                                           &neg);
    return Numerical::stableAddRel(pos, neg);
}

Numerical::Double DenseVector::dotProductDenseToSparseAbsRel(const DenseVector &vector1,
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

void DenseVector::_globalInit()
{
    sm_addDenseToDense = &DenseVector::addDenseToDenseFast;
    sm_addIndexedDenseToDense = &DenseVector::addIndexedDenseToDenseFast;
    sm_addSparseToDense = &DenseVector::addSparseToDenseFast;
    sm_denseToDenseDotProduct = &DenseVector::dotProductDenseToDenseUnstable;
    sm_denseToIndexedDenseDotProduct = &DenseVector::dotProductDenseToIndexedDenseUnstable;
    sm_denseToSparseDotProduct = &DenseVector::dotProductDenseToSparseUnstable;
}

