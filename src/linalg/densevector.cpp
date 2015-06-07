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

#include <linalg/densevector.h>
#include <linalg/indexeddensevector.h>
#include <linalg/sparsevector.h>
#include <utils/architecture.h>
#include <utils/exceptions.h>

DenseVector::AddDenseToDense DenseVector::sm_addDenseToDense;
DenseVector::AddIndexedDenseToDense DenseVector::sm_addIndexedDenseToDense;
DenseVector::AddSparseToDense DenseVector::sm_addSparseToDense;
DenseVector::DenseToDenseDotProduct DenseVector::sm_denseToDenseDotProduct;
DenseVector::DenseToIndexedDenseDotProduct DenseVector::sm_denseToIndexedDenseDotProduct;
DenseVector::DenseToSparseDotProduct DenseVector::sm_denseToSparseDotProduct;

DenseVector::DenseVector(unsigned int length)
{
    m_length = length;
    m_data = nullptr;

    if (m_length > 0) {
        m_data = Numerical::allocDouble(m_length);
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
    if(m_length == orig.length()) {
        COPY_DOUBLES(m_data, orig.m_data, m_length);
    } else {
        release();
        copy(orig);
    }
    return *this;
}

DenseVector &DenseVector::operator=(const SparseVector &orig)
{
    this->reInit(orig.length());
    unsigned int nonZeroIndex;
    for (nonZeroIndex = 0; nonZeroIndex < orig.m_nonZeros; nonZeroIndex++) {
        const unsigned int index = orig.m_indices[nonZeroIndex];
        m_data[index] = orig.m_data[nonZeroIndex];
    }
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

Numerical::Double DenseVector::euclidNorm2() const
{
    unsigned int index;
    Numerical::Double sum = 0.0;
    for (index = 0; index < m_length; index++) {
        sum += m_data[index] * m_data[index];
    }
    return sum;
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

DenseVector &DenseVector::addVector(Numerical::Double lambda,
                                    const SparseVector &vector)
{
    (this->*sm_addSparseToDense)(lambda, vector);
    return *this;
}

DenseVector &DenseVector::addVector(Numerical::Double lambda,
                                    const DenseVector &vector)
{
    (this->*sm_addDenseToDense)(lambda, vector);
    return *this;
}

DenseVector &DenseVector::addVector(Numerical::Double lambda,
                                    const IndexedDenseVector &vector)
{
    (this->*sm_addIndexedDenseToDense)(lambda, vector);
    return *this;
}

Numerical::Double DenseVector::dotProduct(const IndexedDenseVector &vector) const
{
    return (this->*sm_denseToIndexedDenseDotProduct)(*this, vector);
}

Numerical::Double DenseVector::dotProduct(const DenseVector &vector) const
{
    return (this->*sm_denseToDenseDotProduct)(*this, vector);
}

Numerical::Double DenseVector::dotProduct(const SparseVector &vector) const
{
    return (this->*sm_denseToSparseDotProduct)(*this, vector);
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
    }
}

unsigned int DenseVector::nonZeros() const
{
    unsigned int index;
    unsigned int count = 0;
    for (index = 0; index < m_length; index++) {
        if (m_data[index] != 0.0) {
            count++;
        }
    }
    return count;
}

void DenseVector::remove(unsigned int index)
{
#ifndef NDEBUG
    if (index >= m_length) {
        throw InvalidIndexException("Invalid index in DenseVector", 0, m_length, index);
    }
#endif
    for (; index < m_length - 1; index++) {
        m_data[index] = m_data[index + 1];
    }
    m_length--;
}

void DenseVector::insert(unsigned int index, Numerical::Double value)
{
    Numerical::Double * newData = Numerical::allocDouble(m_length + 1);
    COPY_DOUBLES(newData, m_data, index);
    newData[index] = value;
    COPY_DOUBLES(newData + index + 1, m_data + index + 1, m_length - index);
    Numerical::freeDouble(m_data);
    m_data = newData;
    m_length++;
}

void DenseVector::append(Numerical::Double value)
{
    Numerical::Double * newData = Numerical::allocDouble(m_length + 1);
    COPY_DOUBLES(newData, m_data, m_length);
    newData[m_length] = value;
    Numerical::freeDouble(m_data);
    m_data = newData;
    m_length++;
}

void DenseVector::reserve()
{
    Numerical::Double * newData = Numerical::allocDouble(m_length);
    COPY_DOUBLES(newData, m_data, m_length);
    Numerical::freeDouble(m_data);
    m_data = newData;
}

void DenseVector::resize(unsigned int length)
{
    if (length <= m_length) {
        m_length = length;
        return;
    }
    Numerical::Double * newData = Numerical::allocDouble(length);
    COPY_DOUBLES(newData, m_data, m_length);
    CLEAR_DOUBLES(newData + m_length, length - m_length);
    Numerical::freeDouble(m_data);
    m_data = newData;
    m_length = length;
}

void DenseVector::resize(unsigned int length, Numerical::Double value) {
    if (length <= m_length) {
        m_length = length;
        return;
    }
    Numerical::Double * newData = Numerical::allocDouble(length);
    COPY_DOUBLES(newData, m_data, m_length);
    for(unsigned int i = 0; i < m_length; i++) {
        m_data[i] = value;
    }
    Numerical::freeDouble(m_data);
    m_data = newData;
    m_length = length;
}

void DenseVector::reInit(unsigned int length)
{
    if(length <= m_length) {
        CLEAR_DOUBLES(m_data, length)
    } else {
        Numerical::freeDouble(m_data);
        m_data = Numerical::allocDouble(length);
        CLEAR_DOUBLES(m_data, length);
    }
    m_length = length;
    return;
}

std::ostream & operator<<(std::ostream & os, const DenseVector & vector)
{
#ifndef NDEBUG
    unsigned int index;
    os << "data members:" << std::endl;
    os << "\tm_vectorType: " << "DENSE_VECTOR" << std::endl;
    os << "\tm_size: " << vector.m_length << std::endl;
    os << "\tm_data: " << vector.m_data << std::endl;
    os << "m_data: " << std::endl;
    for (index = 0; index < vector.m_length; index++) {
        os << vector.m_data[index] << " ";
    }
    os << std::endl;
#else
    __UNUSED(vector);
#endif
    return os;
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
    result.m_data = Numerical::allocDouble(size);
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
        m_data = Numerical::allocDouble(m_length);
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
    Numerical::freeDouble(m_data);
    m_data = nullptr;
}

void DenseVector::addDenseToDenseFast(Numerical::Double lambda,
                                      const DenseVector &vector)
{
    /*unsigned int index;
    for (index = 0; index < vector.m_length; index++) {
        m_data[index] += vector.m_data[index] * lambda;
    }*/
    if (likely(m_length * sizeof(Numerical::Double) < Architecture::getLargestCacheSize() )) {
        Architecture::getAddDenseToDenseCache()(m_data, vector.m_data, m_data, m_length, lambda);
    } else {
        Architecture::getAddDenseToDenseNoCache()(m_data, vector.m_data, m_data, m_length, lambda);
    }
}

void DenseVector::addDenseToDenseAbs(Numerical::Double lambda,
                                     const DenseVector &vector)
{
/*    unsigned int index;
    for (index = 0; index < vector.m_length; index++) {
        m_data[index] = Numerical::stableAddAbs(m_data[index], vector.m_data[index] * lambda);
    }*/
    if (likely(m_length * sizeof(Numerical::Double) < Architecture::getLargestCacheSize() )) {
        Architecture::getAddDenseToDenseAbsCache()(m_data, vector.m_data, m_data, m_length, lambda,
                                                   Numerical::AbsoluteTolerance);
    } else {
        Architecture::getAddDenseToDenseAbsNoCache()(m_data, vector.m_data, m_data, m_length, lambda,
                                                     Numerical::AbsoluteTolerance);
    }
}

void DenseVector::addDenseToDenseAbsRel(Numerical::Double lambda,
                                        const DenseVector &vector)
{
/*    unsigned int index;
    for (index = 0; index < vector.m_length; index++) {
        m_data[index] = Numerical::stableAdd(m_data[index], vector.m_data[index] * lambda);
    }*/
    if (likely(m_length * sizeof(Numerical::Double) < Architecture::getLargestCacheSize() )) {
        Architecture::getAddDenseToDenseAbsRelCache()(m_data, vector.m_data, m_data, m_length, lambda,
                                                      Numerical::AbsoluteTolerance, Numerical::RelativeTolerance);
    } else {
        Architecture::getAddDenseToDenseAbsRelNoCache()(m_data, vector.m_data, m_data, m_length, lambda,
                                                        Numerical::AbsoluteTolerance, Numerical::RelativeTolerance);
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
    setAddMode(Numerical::ADD_ABS_REL);
    setDotProductMode(Numerical::DOT_ABS_REL);
}

