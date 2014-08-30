#include <linalg/densevector.h>

DenseVector::DenseVector(unsigned int length)
{
    m_addDenseToDense = &DenseVector::addDenseToDenseFast;
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

DenseVector &DenseVector::addVector(Numerical::Double lambda, const DenseVector &vector)
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
        break;
    case Numerical::ADD_ABS:
        m_addDenseToDense = &DenseVector::addDenseToDenseAbs;
        break;
    case Numerical::ADD_ABS_REL:
        m_addDenseToDense = &DenseVector::addDenseToDenseAbsRel;
        break;
    case Numerical::ADD_REL:
        m_addDenseToDense = &DenseVector::addDenseToDenseRel;
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
