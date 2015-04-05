#include <linalg/sparsevector.h>
#include <linalg/linalgparameterhandler.h>
#include <linalg/densevector.h>
#include <linalg/indexeddensevector.h>
#include <utils/architecture.h>

thread_local Numerical::Double * SparseVector::sm_fullLengthVector;
thread_local unsigned int * SparseVector::sm_indexVector;
thread_local unsigned int ** SparseVector::sm_indexPointerVector;
thread_local unsigned int * SparseVector::sm_removeIndices;
thread_local unsigned int SparseVector::sm_fullLengthVectorLength;
unsigned int SparseVector::sm_elbowRoom;

SparseVector::AddDenseToSparse SparseVector::sm_addDenseToSparse;
SparseVector::AddIndexedDenseToSparse SparseVector::sm_addIndexedDenseToSparse;
SparseVector::AddSparseToSparse SparseVector::sm_addSparseToSparse;
SparseVector::SparseToDenseDotProduct SparseVector::sm_sparseToDenseDotProduct;
SparseVector::SparseToIndexedDenseDotProduct SparseVector::sm_sparseToIndexedDenseDotProduct;
SparseVector::SparseToSparseDotProduct SparseVector::sm_sparseToSparseDotProduct;

//void SparseVector::checkScattered(int nzr, unsigned length) {
//    for(int i = 0; i < nzr; i++) {
//        int index = SparseVector::sm_indexVector[i];
//        if(SparseVector::sm_fullLengthVector[index] ==  0.0 || SparseVector::sm_indexPointerVector[index] == nullptr) {
//            LPWARNING("error during scattered nzr check at idx " << index);
//            return;
//        }
//    }
//    for(unsigned i = 0; i < length; i++)
//    {
//        if(SparseVector::sm_indexPointerVector[i] != nullptr) {
//            if(*(SparseVector::sm_indexPointerVector[i]) != i) {
//                LPERROR(SparseVector::sm_indexPointerVector[i] << " " << SparseVector::sm_indexVector);
//            }
//        }
//    }
//}

//void SparseVector::checkVector(const SparseVector & vector) {
//    for(unsigned i = 0; i < vector.nonZeros(); i++) {
//        if(vector.m_data[i] == 0.0 || vector.m_indices[i] >= vector.m_length)
//        {
//            LPWARNING("error during vector check! nzr index: " << i);
//        }
//    }
//}

template<class ADD>
void addSparseToSparseTemplate(Numerical::Double lambda,
                               SparseVector * vector1,
                               const SparseVector & vector2
                               ) {
    //Make thread_local pointers local to avoid TLS overhead
    auto tls_fullLengthVector = SparseVector::sm_fullLengthVector;
    unsigned int * tls_indexVector = SparseVector::sm_indexVector;
    unsigned int ** tls_indexPointerVector = SparseVector::sm_indexPointerVector;

    vector1->scatter();
    unsigned int nonZeroIndex;
    unsigned int nonZeros = vector1->m_nonZeros;

    for (nonZeroIndex = 0; nonZeroIndex < vector2.m_nonZeros; nonZeroIndex++) {
        const unsigned int index = vector2.m_indices[nonZeroIndex];

        const Numerical::Double sum = ADD::add(tls_fullLengthVector[index],
                                    vector2.m_data[nonZeroIndex] * lambda);

        if (tls_fullLengthVector[index] == 0.0 && sum != 0.0) {
            tls_indexVector[nonZeros] = index;
            tls_indexPointerVector[index] = tls_indexVector + nonZeros;
            nonZeros++;
        } else if (tls_fullLengthVector[index] != 0.0 && sum == 0.0) {
            // TODO: atnezni ezt a reszt itt
            tls_indexPointerVector[tls_indexVector[nonZeros - 1]] = tls_indexPointerVector[index];
            *(tls_indexPointerVector[index]) = tls_indexVector[nonZeros - 1];
            tls_indexPointerVector[index] = nullptr;
            nonZeros--;
        }
        tls_fullLengthVector[index] = sum;
    }

    if (nonZeros > vector1->m_capacity) {
        vector1->resizeCapacity(nonZeros);
    }

    for (nonZeroIndex = 0; nonZeroIndex < nonZeros; nonZeroIndex++) {
        const unsigned int index = tls_indexVector[nonZeroIndex];
        vector1->m_data[nonZeroIndex] = tls_fullLengthVector[index];
        vector1->m_indices[nonZeroIndex] = index;
        tls_fullLengthVector[index] = 0.0;
        tls_indexPointerVector[index] = nullptr;
    }
    vector1->m_nonZeros = nonZeros;

}

bool DEBUG_MODE = false;

template<class ADD>
void addDenseToSparseTemplate(Numerical::Double lambda,
                              SparseVector * vector1,
                              const DenseVector & vector2
                              ) {
    //Make thread_local pointers local to avoid TLS overhead
    auto tls_fullLengthVector = SparseVector::sm_fullLengthVector;

    vector1->scatterData();
    unsigned int index;
    unsigned int nonZeros = vector1->m_nonZeros;
    for (index = 0; index < vector2.m_length; index++) {
        const Numerical::Double sum = ADD::add(tls_fullLengthVector[index],
                                    vector2.m_data[index] * lambda);

        if (tls_fullLengthVector[index] == 0.0 && sum != 0.0) {
            nonZeros++;
        } else if (tls_fullLengthVector[index] != 0.0 && sum == 0.0) {
            nonZeros--;
        }
        tls_fullLengthVector[index] = sum;
    }
    if (nonZeros > vector1->m_capacity) {
        vector1->resizeCapacity(nonZeros);
    }
    unsigned int nonZeroIndex = 0;
    for (index = 0; index < vector1->m_length; index++) {
        if (tls_fullLengthVector[index] == 0.0) {
            continue;
        }
        vector1->m_data[nonZeroIndex] = tls_fullLengthVector[index];
        vector1->m_indices[nonZeroIndex] = index;
        tls_fullLengthVector[index] = 0.0;
        nonZeroIndex++;
    }
    vector1->m_nonZeros = nonZeros;
}

template<class ADD>
void addIndexedDenseToSparseTemplate(Numerical::Double lambda,
                                     SparseVector * vector1,
                                     const IndexedDenseVector & vector2
                                     ) {
    //Make thread_local pointers local to avoid TLS overhead
    auto tls_fullLengthVector = SparseVector::sm_fullLengthVector;
    unsigned int * tls_indexVector = SparseVector::sm_indexVector;
    unsigned int ** tls_indexPointerVector = SparseVector::sm_indexPointerVector;

    vector1->scatter();
    unsigned int nonZeroIndex;
    unsigned int nonZeros = vector1->m_nonZeros;

    if (DEBUG_MODE) {
        std::cout << "nonzeros: " << vector2.m_nonZeros << std::endl;
        std::cout << "nonzero index: " << vector2.m_nonzeroIndices[0] << std::endl;
        for (int i = 0; i < vector2.m_length; i++) {
            std::cout << vector2.m_data[i] << " ";
        }
        std::cout << std::endl;
    }
    for (nonZeroIndex = 0; nonZeroIndex < vector2.m_nonZeros; nonZeroIndex++) {
        const unsigned int index = vector2.m_nonzeroIndices[nonZeroIndex];
        const Numerical::Double sum = ADD::add(tls_fullLengthVector[index],
                                    vector2.m_data[index] * lambda);

        if (DEBUG_MODE) {
            std::cout << "sum = " << sum << std::endl;
        }

        if (tls_fullLengthVector[index] == 0.0 && sum != 0.0) {
            tls_indexVector[nonZeros] = index;
            tls_indexPointerVector[index] = tls_indexVector + nonZeros;
            nonZeros++;
        } else if (tls_fullLengthVector[index] != 0.0 && sum == 0.0) {
            *(tls_indexPointerVector[index]) = tls_indexVector[nonZeros - 1];
            *(tls_indexPointerVector[tls_indexVector[nonZeros - 1]]) = index;
            tls_indexPointerVector[index] = nullptr;
            nonZeros--;
        }
        tls_fullLengthVector[index] = sum;
    }
    if (nonZeros > vector1->m_capacity) {
        vector1->resizeCapacity(nonZeros);
    }
    for (nonZeroIndex = 0; nonZeroIndex < nonZeros; nonZeroIndex++) {
        const unsigned int index = tls_indexVector[nonZeroIndex];
        vector1->m_data[nonZeroIndex] = tls_fullLengthVector[index];
        vector1->m_indices[nonZeroIndex] = index;
        tls_fullLengthVector[index] = 0.0;
        tls_indexPointerVector[index] = nullptr;
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

SparseVector &SparseVector::operator =(const DenseVector &orig)
{
    release();
    convertFromDenseArray(orig.m_data, orig.m_length);
    /*this->prepareForData(orig.nonZeros(), orig.m_length);
    for(unsigned i = 0; i < m_length; i++) {
        if(orig[i] != 0.0) this->newNonZero(orig[i], i);
    }*/
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
    result.m_data = Numerical::allocDouble(1);
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
    result.convertFromDenseArray(source, length);
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
    result.m_data = Numerical::allocDouble(count);
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
        result->m_data = Numerical::allocDouble(vector.m_nonZeros);
        result->m_indices = alloc<unsigned int, 16>(vector.m_nonZeros);
        panOptMemcpy(result->m_indices, vector.m_indices, vector.m_nonZeros * sizeof(unsigned int));
        result->m_capacity = vector.m_capacity;

        unsigned int index;
        for (index = 0; index < vector.m_nonZeros; index++) {
            const unsigned int nonZeroIndex = vector.m_indices[index];
            if (unlikely(nonZeroIndex == pivotIndex)) {
                result->m_data[index] = 1.0 / pivotValue;
            } else {
                result->m_data[index] = -vector.m_data[index] / pivotValue;
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
            m_data[nonZeroIndex] = m_data[m_nonZeros-1];
            m_indices[nonZeroIndex] = m_indices[m_nonZeros-1];
            m_nonZeros--;
        } else {
            m_data[nonZeroIndex] = value;
        }
    } else if (value != 0.0){
        if (unlikely(m_capacity <= m_nonZeros)) {
            resizeCapacity(m_capacity + 1 + sm_elbowRoom);
        }
        m_data[m_nonZeros] = value;
        m_indices[m_nonZeros] = index;
        m_nonZeros++;
    }
}

void SparseVector::setNewNonzero(unsigned int index, Numerical::Double value)
{
    if (unlikely(m_capacity <= m_nonZeros)) {
        resizeCapacity(m_capacity + 1 + sm_elbowRoom);
    }
    m_data[m_nonZeros] = value;
    m_indices[m_nonZeros] = index;
    m_nonZeros++;
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

SparseVector & SparseVector::elementaryFtran(const SparseVector & eta, unsigned int pivot)
{
    Numerical::Double pivotValue = this->operator [](pivot);
    Numerical::Double atPivot = eta[pivot];
    if (pivotValue == 0.0) {
        return *this;
    }
    addVector(pivotValue, eta);
    set(pivot, atPivot * pivotValue);
    return *this;
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

void SparseVector::scaleByLambdas(const std::vector<Numerical::Double> & lambdas)
{
    // TODO: lekezelni azt, mikor 0-val szorzunk be
    decltype(m_data) dataPtr = m_data;
    unsigned int * indexPtr = m_indices;
    decltype(m_data) lastData = m_data + m_nonZeros - 1;
    unsigned int * lastIndex = m_indices + m_nonZeros - 1;

    while (dataPtr <= lastData) {
        *dataPtr *= lambdas[*indexPtr];
        if (*dataPtr == 0.0) {
            m_nonZeros--;
            *dataPtr = *lastData;
            *indexPtr = *lastIndex;
            lastData--;
            lastIndex--;
        } else {
            dataPtr++;
            indexPtr++;
        }
    }
}

void SparseVector::scaleElementBy(unsigned int index, Numerical::Double lambda)
{
    decltype(m_data) ptr = nullptr;
    unsigned int nonZeroIndex = 0;
    while (nonZeroIndex < m_nonZeros && m_indices[nonZeroIndex] != index) {
        nonZeroIndex++;
    }
    if (nonZeroIndex < m_nonZeros) {
        ptr = m_data + nonZeroIndex;
    }
    if (lambda != 0.0) {
        if (ptr) {
            *ptr *= lambda;
        }
    } else {
        if (ptr) {
            *ptr = m_data[ m_nonZeros - 1 ];
            m_indices[ ptr - m_data ] = m_indices[ m_nonZeros - 1 ];
            m_nonZeros--;
        }
    }
}

bool SparseVector::operator ==(const SparseVector& other) const
{
    if (m_nonZeros != other.m_nonZeros) {
        return false;
    }
    SparseVector::NonzeroIterator iterator1 = other.beginNonzero();
    SparseVector::NonzeroIterator iterator2 = beginNonzero();
    SparseVector::NonzeroIterator iteratorEnd1 = other.endNonzero();
    for (; iterator1 < iteratorEnd1; ++iterator1, ++iterator2) {
        if (iterator1.getIndex() != iterator2.getIndex()) {
            return false;
        }
        if (*iterator1 != *iterator2) {
            return false;
        }
    }
    return true;
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

void SparseVector::remove(unsigned int index)
{
    unsigned int pos = 0;
    while (pos < m_nonZeros && m_indices[pos] != index) {
        if(m_indices[pos]>index) m_indices[pos]--;
        pos++;
    }
    if (pos < m_nonZeros) {
        m_indices[pos] = m_indices[m_nonZeros - 1];
        m_data[pos] = m_data[m_nonZeros - 1];
        m_nonZeros--;
    }
    while(pos < m_nonZeros) {
        if(m_indices[pos]>index) m_indices[pos]--;
        pos++;
    }

    m_length--;
}

void SparseVector::insert(unsigned int index, Numerical::Double value)
{
    unsigned int pos;
    for (pos = 0; pos < m_nonZeros; pos++) {
        if (m_indices[pos] >= index) {
            m_indices[pos]++;
        }
    }
    if (value != 0.0) {
        if (m_nonZeros >= m_capacity) {
            resizeCapacity(m_capacity + 1 + sm_elbowRoom);
        }
        m_indices[m_nonZeros] = index;
        m_data[m_nonZeros] = value;
        m_nonZeros++;
    }
    m_length++;
}

void SparseVector::append(Numerical::Double value)
{
    if (value != 0.0) {
        if (m_nonZeros >= m_capacity) {
            resizeCapacity(m_capacity + 1 + sm_elbowRoom);
        }
        m_indices[m_nonZeros] = m_length;
        m_data[m_nonZeros] = value;
        m_nonZeros++;
    }
    m_length++;
}

Numerical::Double SparseVector::absMaxElement() const
{
    Numerical::Double result = 0;
    if (m_nonZeros > 0) {
        decltype(m_data) dataPtr = m_data;
        decltype(m_data) dataEnd = m_data + m_nonZeros;
        for (; dataPtr < dataEnd; dataPtr++) {
            const Numerical::Double absValue = Numerical::fabs(*dataPtr);
            if (absValue > result) {
                result = absValue;
            }
        }
    }
    return result;
}

Numerical::Double SparseVector::scaleAndGetResults(const std::vector<Numerical::Double> & multipliers,
                                        Numerical::Double lambda,
                                        Numerical::Double * squareSumPtr,
                                        Numerical::Double * minPtr,
                                        Numerical::Double * maxPtr) {
    Numerical::Double absSum = 0;
    Numerical::Double squareSum = 0;
    Numerical::Double min = Numerical::Infinity;
    Numerical::Double max = 0;
    if (m_nonZeros > 0) {
        auto dataPtr = m_data;
        const auto dataEnd = m_data + m_nonZeros;
        const unsigned int * indexPtr = m_indices;
        for (; dataPtr < dataEnd; dataPtr++, indexPtr++) {
            *dataPtr *= lambda * multipliers[*indexPtr];
            const Numerical::Double absValue = Numerical::fabs(*dataPtr);
            absSum += absValue;
            squareSum += absValue * absValue;
            if (min > absValue) {
                min = absValue;
            }
            if (max < absValue) {
                max = absValue;
            }
        }
    }
    *squareSumPtr = squareSum;
    *minPtr = min;
    *maxPtr = max;
    return absSum;

}

Numerical::Double SparseVector::absMaxSumsAndMinMax(Numerical::Double * squareSumPtr,
                                         Numerical::Double * minPtr,
                                         Numerical::Double * maxPtr) const {
    Numerical::Double absSum = 0;
    Numerical::Double squareSum = 0;
    Numerical::Double min = Numerical::Infinity;
    Numerical::Double max = 0;
    if (m_nonZeros > 0) {
        auto dataPtr = m_data;
        const auto dataEnd = m_data + m_nonZeros;
        for (; dataPtr < dataEnd; dataPtr++) {
            const Numerical::Double absValue = Numerical::fabs(*dataPtr);
            absSum += absValue;
            squareSum += absValue * absValue;
            if (min > absValue) {
                min = absValue;
            }
            if (max < absValue) {
                max = absValue;
            }
        }
    }
    *squareSumPtr = squareSum;
    *minPtr = min;
    *maxPtr = max;
    return absSum;
}

Numerical::Double SparseVector::absMaxSums(Numerical::Double * squareSumPtr) const {
    Numerical::Double absSum = 0;
    Numerical::Double squareSum = 0;
    if (m_nonZeros > 0) {
        auto dataPtr = m_data;
        auto dataEnd = m_data + m_nonZeros;
        for (; dataPtr < dataEnd; dataPtr++) {
            const Numerical::Double absValue = Numerical::fabs(*dataPtr);
            absSum += absValue;
            squareSum += absValue * absValue;
        }
    }
    *squareSumPtr = squareSum;
    return absSum;
}

void SparseVector::batchRemove(const std::vector<int> &indices)
{
    //Make thread_local pointers local to avoid TLS overhead
    unsigned int * tls_removeIndices = SparseVector::sm_removeIndices;

    if (unlikely(sm_fullLengthVectorLength < m_length)) {
        resizeFullLengthVector(m_length);
        tls_removeIndices = SparseVector::sm_removeIndices;
    }

    unsigned int pos = 1;
    for (auto i: indices) {
        tls_removeIndices[i] = pos;
        pos++;
    }

    pos = 0;
    while (pos < m_nonZeros) {
        unsigned int index = m_indices[pos];
        if (tls_removeIndices[index] != 0) {
            m_data[pos] = m_data[m_nonZeros - 1];
            m_indices[pos] = m_indices[m_nonZeros - 1];
            m_nonZeros--;
        } else {
            m_indices[pos] -= std::lower_bound(
                        indices.begin(),
                        indices.end(),
                        index) - indices.begin();

            pos++;
        }
    }

    for (auto i: indices) {
        tls_removeIndices[i] = 0;
    }
    m_length -= indices.size();
}

void SparseVector::reserve()
{
    m_capacity = m_nonZeros;
    auto newData = Numerical::allocDouble(m_capacity);
    COPY_DOUBLES(newData, m_data, m_nonZeros);
    Numerical::freeDouble(m_data);
    m_data = newData;

    unsigned int * newIndices = alloc<unsigned int, 16>(m_capacity);
    panOptMemcpy(newIndices, m_indices, m_nonZeros * sizeof(unsigned int));
    ::release(m_indices);
    m_indices = newIndices;
}

void SparseVector::resize(unsigned int length)
{
    if (length >= m_length) {
        m_length = length;
        return;
    }
    unsigned int index = 0;
    while (index < m_nonZeros) {
        if (m_indices[index] >= length) {
            m_indices[index] = m_indices[m_nonZeros - 1];
            m_data[index] = m_data[m_nonZeros - 1];
            m_nonZeros--;
        } else {
            index++;
        }
    }
}

void SparseVector::reInit(unsigned int length)
{
    CLEAR_DOUBLES(m_data, m_nonZeros);
    panOptMemset(m_indices, 0, sizeof(unsigned int) * m_nonZeros);
    m_length = length;
    return;
}

void SparseVector::resizeFullLengthVector(unsigned int length)
{
    Numerical::freeDouble(sm_fullLengthVector);
    sm_fullLengthVector = Numerical::allocDouble(length);
    CLEAR_DOUBLES(sm_fullLengthVector, length);
    ::release(sm_indexVector);
    sm_indexVector = alloc<unsigned int, 16>(length);
    ::release(sm_indexPointerVector);
    sm_indexPointerVector = alloc<unsigned int*, 16>(length);
    panOptMemset(sm_indexPointerVector, 0, sizeof(unsigned int*) * length);

    delete [] sm_removeIndices;
    sm_removeIndices = new unsigned int[length];
    panOptMemset(sm_removeIndices, 0, sizeof(unsigned int) * length);

    sm_fullLengthVectorLength = length;
}

void SparseVector::_globalInit()
{
    sm_fullLengthVector = nullptr;
    sm_indexVector = nullptr;
    sm_indexPointerVector = nullptr;
    sm_removeIndices = nullptr;
    sm_fullLengthVectorLength = 0;
    sm_elbowRoom = LinalgParameterHandler::getInstance().getIntegerParameterValue("elbowroom");

    setAddMode(Numerical::ADD_ABS_REL);
    setDotProductMode(Numerical::DOT_ABS_REL);
}

void SparseVector::_globalRelease()
{
    Numerical::freeDouble(sm_fullLengthVector);
    sm_fullLengthVector = nullptr;
    ::release(sm_indexVector);
    sm_indexVector = nullptr;
    ::release(sm_indexPointerVector);
    sm_indexPointerVector = nullptr;
    delete [] sm_removeIndices;
    sm_removeIndices = nullptr;
    sm_fullLengthVectorLength = 0;
}

void SparseVector::copy(const SparseVector &orig)
{
    m_length = orig.m_length;
    m_nonZeros = orig.m_nonZeros;
    m_capacity = orig.m_capacity;
    if (orig.m_nonZeros != 0) {
        m_data = Numerical::allocDouble(m_capacity);
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
    Numerical::freeDouble(m_data);
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

void SparseVector::resizeCapacity(unsigned int capacity)
{
    auto data = Numerical::allocDouble(capacity);
    COPY_DOUBLES(data, m_data, m_nonZeros);
    Numerical::freeDouble(m_data);
    m_data = data;

    unsigned int * indices = alloc<unsigned int, 16>(capacity);
    panOptMemcpy(indices, m_indices, m_nonZeros * sizeof(unsigned int));
    ::release(m_indices);
    m_indices = indices;
    m_capacity = capacity;
}

void SparseVector::scatter() const
{
    //Make thread_local pointers local to avoid TLS overhead
    auto tls_fullLengthVector = SparseVector::sm_fullLengthVector;
    unsigned int * tls_indexVector = SparseVector::sm_indexVector;
    unsigned int ** tls_indexPointerVector = SparseVector::sm_indexPointerVector;

    unsigned int nonZeroIndex;
    for (nonZeroIndex = 0; nonZeroIndex < m_nonZeros; nonZeroIndex++) {
        const unsigned int index = m_indices[nonZeroIndex];
        tls_indexVector[nonZeroIndex] = index;
        tls_fullLengthVector[index] = m_data[nonZeroIndex];
        tls_indexPointerVector[index] = tls_indexVector + nonZeroIndex;
    }
}

void SparseVector::scatterData() const
{
    //Make thread_local pointers local to avoid TLS overhead
    auto tls_fullLengthVector = SparseVector::sm_fullLengthVector;

    unsigned int nonZeroIndex;
    for (nonZeroIndex = 0; nonZeroIndex < m_nonZeros; nonZeroIndex++) {
        const unsigned int index = m_indices[nonZeroIndex];
        tls_fullLengthVector[index] = m_data[nonZeroIndex];
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
    //Make thread_local pointers local to avoid TLS overhead
    auto tls_fullLengthVector = SparseVector::sm_fullLengthVector;

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
                tls_fullLengthVector,
                largeVector->m_data,
                largeVector->m_indices,
                largeVector->m_nonZeros);
    unsigned int nonZeroIndex;
    for (nonZeroIndex = 0; nonZeroIndex < smallVector->m_nonZeros; nonZeroIndex++) {
        tls_fullLengthVector[ smallVector->m_indices[nonZeroIndex] ] = 0.0;
    }
    return result;
}

Numerical::Double SparseVector::dotProductSparseToSparseFast(const SparseVector &vector1,
                                                  const SparseVector &vector2) const
{
    //Make thread_local pointers local to avoid TLS overhead
    auto tls_fullLengthVector = SparseVector::sm_fullLengthVector;

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
                tls_fullLengthVector,
                largeVector->m_data,
                largeVector->m_indices,
                largeVector->m_nonZeros,
                &neg);
    unsigned int nonZeroIndex;
    for (nonZeroIndex = 0; nonZeroIndex < smallVector->m_nonZeros; nonZeroIndex++) {
        tls_fullLengthVector[ smallVector->m_indices[nonZeroIndex] ] = 0.0;
    }
    return pos + neg;
}

Numerical::Double SparseVector::dotProductSparseToSparseAbs(const SparseVector &vector1,
                                                 const SparseVector &vector2) const
{
    //Make thread_local pointers local to avoid TLS overhead
    auto tls_fullLengthVector = SparseVector::sm_fullLengthVector;

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
                tls_fullLengthVector,
                largeVector->m_data,
                largeVector->m_indices,
                largeVector->m_nonZeros,
                &neg);
    unsigned int nonZeroIndex;
    for (nonZeroIndex = 0; nonZeroIndex < smallVector->m_nonZeros; nonZeroIndex++) {
        tls_fullLengthVector[ smallVector->m_indices[nonZeroIndex] ] = 0.0;
    }
    return Numerical::stableAddAbs(pos, neg);
}

Numerical::Double SparseVector::dotProductSparseToSparseAbsRel(const SparseVector &vector1,
                                                    const SparseVector &vector2) const
{
    //Make thread_local pointers local to avoid TLS overhead
    auto tls_fullLengthVector = SparseVector::sm_fullLengthVector;
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
                tls_fullLengthVector,
                largeVector->m_data,
                largeVector->m_indices,
                largeVector->m_nonZeros,
                &neg);
    unsigned int nonZeroIndex;
    for (nonZeroIndex = 0; nonZeroIndex < smallVector->m_nonZeros; nonZeroIndex++) {
        tls_fullLengthVector[ smallVector->m_indices[nonZeroIndex] ] = 0.0;
    }
    return Numerical::stableAdd(pos, neg);
}

void SparseVector::convertFromDenseArray(const Numerical::Double *source, unsigned int length)
{
    if (unlikely(sm_fullLengthVectorLength < length)) {
        resizeFullLengthVector(length);
    }

    //Make thread_local pointers local to avoid TLS overhead
    auto tls_fullLengthVector = SparseVector::sm_fullLengthVector;
    unsigned int * tls_indexVector = SparseVector::sm_indexVector;

    m_nonZeros = 0;
    unsigned int index;
    m_length = length;
    for (index = 0; index < length; index++) {
        if (source[index] != 0) {
            tls_fullLengthVector[m_nonZeros] = source[index];
            tls_indexVector[m_nonZeros] = index;
            m_nonZeros++;
        }
    }
    if (m_nonZeros == 0) {
        m_data = nullptr;
        m_indices = nullptr;
        m_capacity = 0;
        return;
    }
    m_data = Numerical::allocDouble(m_nonZeros);
    m_indices = alloc<unsigned int, 16>(m_nonZeros);
    for (index = 0; index < m_nonZeros; index++) {
        m_data[index] = tls_fullLengthVector[index];
        tls_fullLengthVector[index] = 0.0;
        m_indices[index] = tls_indexVector[index];
        tls_indexVector[index] = 0;
    }
    m_capacity = m_nonZeros;
}

