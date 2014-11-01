#include <linalg/sparsevector.h>
#include <linalg/linalgparameterhandler.h>
#include <linalg/densevector.h>
#include <linalg/indexeddensevector.h>
#include <utils/architecture.h>

thread_local Numerical::Double * SparseVector::sm_fullLengthVector;
thread_local unsigned int * SparseVector::sm_indexVector;
thread_local unsigned int ** SparseVector::sm_indexPointerVector;
thread_local unsigned int SparseVector::sm_fullLengthVectorLength;
thread_local unsigned int SparseVector::sm_elbowRoom;

thread_local SparseVector::AddDenseToSparse SparseVector::sm_addDenseToSparse;
thread_local SparseVector::AddIndexedDenseToSparse SparseVector::sm_addIndexedDenseToSparse;
thread_local SparseVector::AddSparseToSparse SparseVector::sm_addSparseToSparse;
thread_local SparseVector::SparseToDenseDotProduct SparseVector::sm_sparseToDenseDotProduct;
thread_local SparseVector::SparseToIndexedDenseDotProduct SparseVector::sm_sparseToIndexedDenseDotProduct;
thread_local SparseVector::SparseToSparseDotProduct SparseVector::sm_sparseToSparseDotProduct;


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

template<class ADD>
void addDenseToSparseTemplate(Numerical::Double lambda,
                              SparseVector * vector1,
                              const DenseVector & vector2
                              ) {
    vector1->scatter();
    unsigned int index;
    unsigned int nonZeros = vector1->m_nonZeros;
    for (index = 0; index < vector2.m_length; index++) {
        const Numerical::Double sum = ADD::add(SparseVector::sm_fullLengthVector[index],
                                               vector2.m_data[index] * lambda);
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
    unsigned int nonZeroIndex;
    for (nonZeroIndex = 0; nonZeroIndex < nonZeros; nonZeroIndex++) {
        const unsigned int index = SparseVector::sm_indexVector[nonZeroIndex];
        vector1->m_data[nonZeroIndex] = SparseVector::sm_fullLengthVector[index];
        vector1->m_indices[nonZeroIndex] = index;
        SparseVector::sm_fullLengthVector[index] = 0.0;
        SparseVector::sm_indexPointerVector[index] = nullptr;
    }
    vector1->m_nonZeros = nonZeros;

}

template<class ADD>
void addIndexedDenseToSparseTemplate(Numerical::Double lambda,
                                     SparseVector * vector1,
                                     const IndexedDenseVector & vector2
                                     ) {
    vector1->scatter();
    unsigned int nonZeroIndex;
    unsigned int nonZeros = vector1->m_nonZeros;
    for (nonZeroIndex = 0; nonZeroIndex < vector2.m_nonZeros; nonZeroIndex++) {
        const unsigned int index = vector2.m_nonzeroIndices[nonZeroIndex];
        const Numerical::Double sum = ADD::add(SparseVector::sm_fullLengthVector[index],
                                               vector2.m_data[index] * lambda);
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

SparseVector::SparseVector(unsigned int length)
{
    m_length = length;
    m_nonZeros = 0;
    m_data = nullptr;
    m_indices = nullptr;
    m_capacity = 0;

    if (unlikely(sm_fullLengthVectorLength < m_length)) {
        resizeFullLengthVector(m_length);
    }
}

SparseVector::SparseVector(const SparseVector &orig)
{
    copy(orig);
}

SparseVector::SparseVector(SparseVector &&orig)
{
    move(orig);
}

SparseVector &SparseVector::operator=(const SparseVector &orig)
{
    if (this == &orig) {
        return *this;
    }
    release();
    copy(orig);
    return *this;
}

SparseVector &SparseVector::operator=(SparseVector &&orig)
{
    if (this == &orig) {
        return *this;
    }
    release();
    move(orig);
    return *this;
}

SparseVector::~SparseVector()
{
    release();
}

SparseVector SparseVector::createUnitVector(unsigned int length, unsigned int index)
{
    SparseVector result(length);
    result.m_nonZeros = 1;
    result.m_data = alloc<Numerical::Double, 32>(1);
    result.m_data[0] = 1.0;
    result.m_indices = alloc<unsigned int, 16>(1);
    result.m_indices[0] = index;
    result.m_capacity = 1;
    return result;
}

SparseVector SparseVector::createVectorFromDenseArray(const Numerical::Double *source,
                                                      unsigned int length)
{
    SparseVector result(length);
    unsigned int nonZeros = 0;
    unsigned int index;
    for (index = 0; index < length; index++) {
        if (source[index] != 0) {
            sm_fullLengthVector[nonZeros] = source[index];
            sm_indexVector[nonZeros] = index;
            nonZeros++;
        }
    }
    result.m_nonZeros = nonZeros;
    if (result.m_nonZeros == 0) {
        result.m_data = nullptr;
        result.m_indices = nullptr;
        result.m_capacity = 0;
        return result;
    }
    result.m_data = alloc<Numerical::Double, 32>(nonZeros);
    result.m_indices = alloc<unsigned int, 16>(nonZeros);
    for (index = 0; index < nonZeros; index++) {
        result.m_data[index] = sm_fullLengthVector[index];
        sm_fullLengthVector[index] = 0.0;
        result.m_indices[index] = sm_indexVector[index];
        sm_indexVector[index] = 0;
    }
    result.m_capacity = nonZeros;
    return result;
}

SparseVector SparseVector::createVectorFromSparseArray(const Numerical::Double *data,
                                                       const unsigned int *indices,
                                                       unsigned int count,
                                                       unsigned int length)
{
    SparseVector result(length);
    if (count == 0) {
        return result;
    }
    result.m_nonZeros = count;
    result.m_data = alloc<Numerical::Double, 32>(count);
    COPY_DOUBLES(result.m_data, data, count);
    result.m_indices = alloc<unsigned int, 16>(count);
    panOptMemcpy(result.m_indices, indices, count * sizeof(unsigned int));
    result.m_capacity = count;
    return result;
}

SparseVector *SparseVector::createEtaVector(const SparseVector &vector,
                                            Numerical::Double pivotValue,
                                            unsigned int pivotIndex)
{
    //TODO: CreateEtaVector using move operations
    if (unlikely(Numerical::equals(pivotValue, 0.0))) {
        throw NumericalException(std::string("NUMERICAL problem: Pivot element is ") +  std::to_string(pivotValue)
                                 + std::string(" at row ") + std::to_string(pivotIndex) );
    } else {
        SparseVector * result = new SparseVector(vector.length());
        result->m_nonZeros = vector.m_nonZeros;
        result->m_data = alloc<Numerical::Double, 32>(vector.m_nonZeros);
        result->m_indices = alloc<unsigned int, 16>(vector.m_nonZeros);
        panOptMemcpy(result->m_indices, vector.m_indices, vector.m_nonZeros * sizeof(unsigned int));
        result->m_capacity = vector.m_capacity;

        unsigned int index;
        for (index = 0; index < vector.m_nonZeros; index++) {
            const unsigned int nonZeroIndex = vector.m_indices[index];
            if (unlikely(nonZeroIndex == pivotIndex)) {
                result->m_data[index] = 1.0 / pivotValue;
            } else {
                result->m_data[index] = -result->m_data[index] / pivotValue;
            }
        }

        return result;
    }
    return nullptr;
}

Numerical::Double SparseVector::at(unsigned int index) const
{
    if (unlikely(index >= m_length)) {
        throw InvalidIndexException("Invalid index at DenseVector::at",
                                    0, m_length - 1, index);
    } else {
        return operator[](index);
    }
    return 0.0;
}

Numerical::Double SparseVector::operator[](unsigned int index) const
{
    unsigned int nonZeroIndex = 0;
    while (nonZeroIndex < m_nonZeros && m_indices[nonZeroIndex] != index) {
        nonZeroIndex++;
    }
    if (nonZeroIndex < m_nonZeros) {
        return m_data[nonZeroIndex];
    }
    return 0.0;
}

void SparseVector::set(unsigned int index, Numerical::Double value)
{
    unsigned int nonZeroIndex = 0;
    while (nonZeroIndex < m_nonZeros && m_indices[nonZeroIndex] != index) {
        nonZeroIndex++;
    }
    if (nonZeroIndex < m_nonZeros) {
        if (value == 0.0) {
            m_data[nonZeroIndex] = m_data[m_nonZeros];
            m_nonZeros--;
        } else {
            m_data[nonZeroIndex] = value;
        }
    } else if (value != 0.0){
        if (unlikely(m_capacity <= m_nonZeros)) {
            resize(m_capacity + 1 + sm_elbowRoom);
        }
        m_data[m_nonZeros] = value;
        m_indices[m_nonZeros] = index;
        m_nonZeros++;
    }
}

Numerical::Double SparseVector::euclidNorm() const
{
    unsigned int index;
    Numerical::Double sum = 0.0;
    for (index = 0; index < m_nonZeros; index++) {
        sum += m_data[index] * m_data[index];
    }
    return Numerical::sqrt(sum);
}

Numerical::Double SparseVector::l1Norm() const
{
    unsigned int index;
    Numerical::Double sum = 0.0;
    for (index = 0; index < m_nonZeros; index++) {
        sum += Numerical::fabs(m_data[index]);
    }
    return sum;
}

SparseVector & SparseVector::addVector(Numerical::Double lambda,
                                       const SparseVector & vector)
{
    (this->*sm_addSparseToSparse)(lambda, vector);
    return *this;
}

SparseVector & SparseVector::addVector(Numerical::Double lambda,
                                       const DenseVector & vector)
{
    (this->*sm_addDenseToSparse)(lambda, vector);
    return *this;
}

SparseVector & SparseVector::addVector(Numerical::Double lambda,
                                       const IndexedDenseVector & vector)
{
    (this->*sm_addIndexedDenseToSparse)(lambda, vector);
    return *this;
}

Numerical::Double SparseVector::dotProduct(const IndexedDenseVector &vector) const
{
    return (this->*sm_sparseToIndexedDenseDotProduct)(*this, vector);
}

Numerical::Double SparseVector::dotProduct(const DenseVector &vector) const
{
    return (this->*sm_sparseToDenseDotProduct)(*this, vector);
}

Numerical::Double SparseVector::dotProduct(const SparseVector &vector) const
{
    return (this->*sm_sparseToSparseDotProduct)(*this, vector);
}

void SparseVector::scale(Numerical::Double lambda)
{
    if (unlikely(lambda == 0.0)) {
        clear();
    } else {
        unsigned int index;
        for (index = 0; index < m_nonZeros; index++) {
            m_data[index] *= lambda;
        }
    }
}

void SparseVector::sortIndices()
{
    //TODO: Sort the indices!
    //TODO: Exploit sorted indices everywhere, where it is possible
}

void SparseVector::clear()
{
    m_nonZeros = 0;
}

void SparseVector::setAddMode(Numerical::ADD_TYPE type)
{
    switch (type) {
    case Numerical::ADD_FAST:
        sm_addSparseToSparse = &SparseVector::addSparseToSparseFast;
        sm_addDenseToSparse = &SparseVector::addDenseToSparseFast;
        sm_addIndexedDenseToSparse = &SparseVector::addIndexedDenseToSparseFast;
        break;
    case Numerical::ADD_ABS:
        sm_addSparseToSparse = &SparseVector::addSparseToSparseAbs;
        sm_addDenseToSparse = &SparseVector::addDenseToSparseAbs;
        sm_addIndexedDenseToSparse = &SparseVector::addIndexedDenseToSparseAbs;
        break;
    case Numerical::ADD_ABS_REL:
        sm_addSparseToSparse = &SparseVector::addSparseToSparseAbsRel;
        sm_addDenseToSparse = &SparseVector::addDenseToSparseAbsRel;
        sm_addIndexedDenseToSparse = &SparseVector::addIndexedDenseToSparseAbsRel;
        break;
    }
}

void SparseVector::setDotProductMode(Numerical::DOT_PRODUCT_TYPE type)
{
    switch (type) {
    case Numerical::DOT_UNSTABLE:
        sm_sparseToIndexedDenseDotProduct = &SparseVector::dotProductSparseToIndexedDenseUnstable;
        sm_sparseToDenseDotProduct = &SparseVector::dotProductSparseToDenseUnstable;
        sm_sparseToSparseDotProduct = &SparseVector::dotProductSparseToSparseUnstable;
        break;
    case Numerical::DOT_FAST:
        sm_sparseToIndexedDenseDotProduct = &SparseVector::dotProductSparseToIndexedDenseFast;
        sm_sparseToDenseDotProduct = &SparseVector::dotProductSparseToDenseFast;
        sm_sparseToSparseDotProduct = &SparseVector::dotProductSparseToSparseFast;
        break;
    case Numerical::DOT_ABS:
        sm_sparseToIndexedDenseDotProduct = &SparseVector::dotProductSparseToIndexedDenseAbs;
        sm_sparseToDenseDotProduct = &SparseVector::dotProductSparseToDenseAbs;
        sm_sparseToSparseDotProduct = &SparseVector::dotProductSparseToSparseAbs;
        break;
    case Numerical::DOT_ABS_REL:
        sm_sparseToIndexedDenseDotProduct = &SparseVector::dotProductSparseToIndexedDenseAbsRel;
        sm_sparseToDenseDotProduct = &SparseVector::dotProductSparseToDenseAbsRel;
        sm_sparseToSparseDotProduct = &SparseVector::dotProductSparseToSparseAbsRel;
        break;
    }
}

void SparseVector::resizeFullLengthVector(unsigned int length)
{
    ::release(sm_fullLengthVector);
    sm_fullLengthVector = alloc<Numerical::Double, 32>(length);
    CLEAR_DOUBLES(sm_fullLengthVector, length);
    ::release(sm_indexVector);
    sm_indexVector = alloc<unsigned int, 16>(length);
    ::release(sm_indexPointerVector);
    sm_indexPointerVector = alloc<unsigned int*, 16>(length);
    panOptMemset(sm_indexPointerVector, 0, sizeof(unsigned int**) * length);

    sm_fullLengthVectorLength = length;
}

void SparseVector::_globalInit()
{
    sm_fullLengthVector = nullptr;
    sm_indexVector = nullptr;
    sm_indexPointerVector = nullptr;
    sm_fullLengthVectorLength = 0;
    sm_elbowRoom = LinalgParameterHandler::getInstance().getIntegerParameterValue("elbowroom");

    setAddMode(Numerical::ADD_ABS_REL);
    setDotProductMode(Numerical::DOT_ABS_REL);
}

void SparseVector::_globalRelease()
{
    ::release(sm_fullLengthVector);
    sm_fullLengthVector = nullptr;
    ::release(sm_indexVector);
    sm_indexVector = nullptr;
    ::release(sm_indexPointerVector);
    sm_indexPointerVector = nullptr;
    sm_fullLengthVectorLength = 0;
}

void SparseVector::copy(const SparseVector &orig)
{
    m_length = orig.m_length;
    m_nonZeros = orig.m_nonZeros;
    m_capacity = orig.m_capacity;
    if (orig.m_nonZeros != 0) {
        m_data = alloc<Numerical::Double, 32>(m_capacity);
        COPY_DOUBLES(m_data, orig.m_data, m_nonZeros);
        m_indices = alloc<unsigned int, 16>(m_capacity);
        panOptMemcpy(m_indices, orig.m_indices, sizeof(unsigned int) * m_capacity);
    } else {
        m_data = nullptr;
        m_indices = nullptr;
    }
}

void SparseVector::release()
{
    m_length = 0;
    m_nonZeros = 0;
    ::release(m_data);
    m_data = nullptr;
    ::release(m_indices);
    m_indices = nullptr;
    m_capacity = 0;
}

void SparseVector::move(SparseVector &orig)
{
    m_length = orig.m_length;
    orig.m_length = 0;
    m_nonZeros = orig.m_nonZeros;
    orig.m_nonZeros = 0;
    m_data = orig.m_data;
    orig.m_data = nullptr;
    m_indices = orig.m_indices;
    orig.m_indices = nullptr;
    m_capacity = orig.m_capacity;
    orig.m_capacity = 0;
}

void SparseVector::resize(unsigned int capacity)
{
    Numerical::Double * data = alloc<Numerical::Double, 32>(capacity);
    COPY_DOUBLES(data, m_data, m_nonZeros);
    ::release(m_data);
    m_data = data;

    unsigned int * indices = alloc<unsigned int, 16>(capacity);
    panOptMemcpy(indices, m_indices, m_nonZeros * sizeof(unsigned int));
    ::release(m_indices);
    m_indices = indices;
    m_capacity = capacity;
}

void SparseVector::scatter() const
{
    unsigned int nonZeroIndex;
    for (nonZeroIndex = 0; nonZeroIndex < m_nonZeros; nonZeroIndex++) {
        const unsigned int index = m_indices[nonZeroIndex];
        sm_indexVector[m_nonZeros] = index;
        sm_fullLengthVector[index] = m_data[index];
        sm_indexPointerVector[index] = m_indices + m_nonZeros;
    }
}

void SparseVector::addSparseToSparseFast(Numerical::Double lambda,
                                         const SparseVector &vector)
{
    addSparseToSparseTemplate<AddFast>(lambda, this, vector);
}

void SparseVector::addSparseToSparseAbs(Numerical::Double lambda,
                                        const SparseVector &vector)
{
    addSparseToSparseTemplate<AddAbs>(lambda, this, vector);
}

void SparseVector::addSparseToSparseAbsRel(Numerical::Double lambda,
                                           const SparseVector &vector)
{
    addSparseToSparseTemplate<AddAbsRel>(lambda, this, vector);
}

void SparseVector::addDenseToSparseFast(Numerical::Double lambda,
                                        const DenseVector &vector)
{
    addDenseToSparseTemplate<AddFast>(lambda, this, vector);
}

void SparseVector::addDenseToSparseAbs(Numerical::Double lambda,
                                       const DenseVector &vector)
{
    addDenseToSparseTemplate<AddAbs>(lambda, this, vector);
}

void SparseVector::addDenseToSparseAbsRel(Numerical::Double lambda,
                                          const DenseVector &vector)
{
    addDenseToSparseTemplate<AddAbsRel>(lambda, this, vector);
}

void SparseVector::addIndexedDenseToSparseFast(Numerical::Double lambda,
                                               const IndexedDenseVector &vector)
{
    addIndexedDenseToSparseTemplate<AddFast>(lambda, this, vector);
}

void SparseVector::addIndexedDenseToSparseAbs(Numerical::Double lambda,
                                              const IndexedDenseVector &vector)
{
    addIndexedDenseToSparseTemplate<AddAbs>(lambda, this, vector);
}

void SparseVector::addIndexedDenseToSparseAbsRel(Numerical::Double lambda,
                                                 const IndexedDenseVector &vector)
{
    addIndexedDenseToSparseTemplate<AddAbsRel>(lambda, this, vector);
}

Numerical::Double SparseVector::dotProductSparseToIndexedDenseUnstable(const SparseVector &vector1,
                                                                       const IndexedDenseVector &vector2) const
{
    return vector2.dotProductIndexedDenseToSparseUnstable(vector2, vector1);
}

Numerical::Double SparseVector::dotProductSparseToIndexedDenseFast(const SparseVector &vector1,
                                                                   const IndexedDenseVector &vector2) const
{
    return vector2.dotProductIndexedDenseToSparseFast(vector2, vector1);
}

Numerical::Double SparseVector::dotProductSparseToIndexedDenseAbs(const SparseVector &vector1,
                                                                  const IndexedDenseVector &vector2) const
{
    return vector2.dotProductIndexedDenseToSparseAbs(vector2, vector1);
}

Numerical::Double SparseVector::dotProductSparseToIndexedDenseAbsRel(const SparseVector &vector1,
                                                                     const IndexedDenseVector &vector2) const
{
    return vector2.dotProductIndexedDenseToSparseAbsRel(vector2, vector1);
}

Numerical::Double SparseVector::dotProductSparseToDenseUnstable(const SparseVector &vector1,
                                                                const DenseVector &vector2) const
{
    return vector2.dotProductDenseToSparseUnstable(vector2, vector1);
}

Numerical::Double SparseVector::dotProductSparseToDenseFast(const SparseVector &vector1,
                                                            const DenseVector &vector2) const
{
    return vector2.dotProductDenseToSparseFast(vector2, vector1);
}

Numerical::Double SparseVector::dotProductSparseToDenseAbs(const SparseVector &vector1,
                                                           const DenseVector &vector2) const
{
    return vector2.dotProductDenseToSparseAbs(vector2, vector1);
}

Numerical::Double SparseVector::dotProductSparseToDenseAbsRel(const SparseVector &vector1,
                                                              const DenseVector &vector2) const
{
    return vector2.dotProductDenseToSparseAbsRel(vector2, vector1);
}

Numerical::Double SparseVector::dotProductSparseToSparseUnstable(const SparseVector &vector1,
                                                                 const SparseVector &vector2) const
{
    const SparseVector * smallVector;
    const SparseVector * largeVector;
    if (vector1.m_nonZeros < vector2.m_nonZeros) {
        smallVector = &vector1;
        largeVector = &vector2;
    } else {
        smallVector = &vector2;
        largeVector = &vector1;
    }
    smallVector->scatter();
    Numerical::Double result = Architecture::getDenseToSparseDotProductUnstable()(
                sm_fullLengthVector,
                largeVector->m_data,
                largeVector->m_indices,
                largeVector->m_nonZeros);
    unsigned int nonZeroIndex;
    for (nonZeroIndex = 0; nonZeroIndex < smallVector->m_nonZeros; nonZeroIndex++) {
        sm_fullLengthVector[ smallVector->m_indices[nonZeroIndex] ] = 0.0;
    }
    return result;
}

Numerical::Double SparseVector::dotProductSparseToSparseFast(const SparseVector &vector1,
                                                             const SparseVector &vector2) const
{
    const SparseVector * smallVector;
    const SparseVector * largeVector;
    if (vector1.m_nonZeros < vector2.m_nonZeros) {
        smallVector = &vector1;
        largeVector = &vector2;
    } else {
        smallVector = &vector2;
        largeVector = &vector1;
    }
    smallVector->scatter();
    Numerical::Double neg;
    Numerical::Double pos = Architecture::getDenseToSparseDotProductStable()(
                sm_fullLengthVector,
                largeVector->m_data,
                largeVector->m_indices,
                largeVector->m_nonZeros,
                &neg);
    unsigned int nonZeroIndex;
    for (nonZeroIndex = 0; nonZeroIndex < smallVector->m_nonZeros; nonZeroIndex++) {
        sm_fullLengthVector[ smallVector->m_indices[nonZeroIndex] ] = 0.0;
    }
    return pos + neg;
}

Numerical::Double SparseVector::dotProductSparseToSparseAbs(const SparseVector &vector1,
                                                            const SparseVector &vector2) const
{
    const SparseVector * smallVector;
    const SparseVector * largeVector;
    if (vector1.m_nonZeros < vector2.m_nonZeros) {
        smallVector = &vector1;
        largeVector = &vector2;
    } else {
        smallVector = &vector2;
        largeVector = &vector1;
    }
    smallVector->scatter();
    Numerical::Double neg;
    Numerical::Double pos = Architecture::getDenseToSparseDotProductStable()(
                sm_fullLengthVector,
                largeVector->m_data,
                largeVector->m_indices,
                largeVector->m_nonZeros,
                &neg);
    unsigned int nonZeroIndex;
    for (nonZeroIndex = 0; nonZeroIndex < smallVector->m_nonZeros; nonZeroIndex++) {
        sm_fullLengthVector[ smallVector->m_indices[nonZeroIndex] ] = 0.0;
    }
    return Numerical::stableAddAbs(pos, neg);
}

Numerical::Double SparseVector::dotProductSparseToSparseAbsRel(const SparseVector &vector1,
                                                               const SparseVector &vector2) const
{
    const SparseVector * smallVector;
    const SparseVector * largeVector;
    if (vector1.m_nonZeros < vector2.m_nonZeros) {
        smallVector = &vector1;
        largeVector = &vector2;
    } else {
        smallVector = &vector2;
        largeVector = &vector1;
    }
    smallVector->scatter();
    Numerical::Double neg;
    Numerical::Double pos = Architecture::getDenseToSparseDotProductStable()(
                sm_fullLengthVector,
                largeVector->m_data,
                largeVector->m_indices,
                largeVector->m_nonZeros,
                &neg);
    unsigned int nonZeroIndex;
    for (nonZeroIndex = 0; nonZeroIndex < smallVector->m_nonZeros; nonZeroIndex++) {
        sm_fullLengthVector[ smallVector->m_indices[nonZeroIndex] ] = 0.0;
    }
    return Numerical::stableAdd(pos, neg);
}
