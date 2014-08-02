/**
 * @file vector.cpp
 */

#include <debug.h>
#include <linalg/matrix.h>
#include <linalg/vector.h>
//#include <linalg/arch/archlinalg.h>
#include <utils/numerical.h>
#include <utils/heap.h>
#include <linalg/linalgparameterhandler.h>
//#include <simplex/linalg.h>

#include <climits>
#include <cstring>
#include <cmath>

#include <utils/flags.h>

//const int ELBOWROOM = LinalgParameterHandler::getInstance().getIntegerParameterValue("elbowroom");
//const double SPARSITY_RATIO = LinalgParameterHandler::getInstance().getDoubleParameterValue("sparsity_ratio");
int ELBOWROOM = 0;
double SPARSITY_RATIO = 0.0;

THREAD_STATIC_DEF Numerical::Double * Vector::sm_fullLengthVector = 0;
THREAD_STATIC_DEF unsigned int Vector::sm_fullLengthVectorLenght = 0;
THREAD_STATIC_DEF unsigned int Vector::sm_fullLenghtReferenceCounter = 0;
THREAD_STATIC_DEF unsigned long * Vector::sm_countingSortBitVector = 0;
THREAD_STATIC_DEF unsigned int Vector::sm_countingSortBitVectorLength = 0;

#ifdef ANALYSE_DOT_PRODUCT
    std::vector<int> diffs(3000);
#endif

Vector::Vector(unsigned int dimension, VECTOR_TYPE type)
{
    sm_fullLenghtReferenceCounter++;
    m_vectorType = type;
    init(dimension);
    CHECK;
}

Vector::Vector(void *, void *, void *)
{
    sm_fullLenghtReferenceCounter++;
    m_data = 0;
    m_index = 0;
    m_size = 0;
    m_nonZeros = 0;
    m_capacity = 0;
    m_dimension = 0;
    m_sorted = true;
    m_sparsityRatio = SPARSITY_RATIO;
    m_sparsityThreshold = (unsigned int) Numerical::round((Numerical::Double)m_dimension * m_sparsityRatio);
    //CHECK;
}

void Vector::reInit(unsigned int dimension)
{
    freeData(m_data);
    if (m_vectorType == SPARSE_VECTOR) {
        freeIndex(m_index);
    }
    init(dimension);
    CHECK;
}

Numerical::Double * Vector::allocateData(unsigned int capacity)
{
    if (capacity == 0) {
        return 0;
    }
    return alloc<Numerical::Double, 32>(capacity);
    //return new Numerical::Double[capacity];
}

unsigned int * Vector::allocateIndex(unsigned int capacity)
{
    if (capacity == 0) {
        return 0;
    }
    return alloc<unsigned int, 32>(capacity);
    //return new unsigned int[capacity];
}

void Vector::freeData(Numerical::Double * & data)
{
    release(data);
    //LPINFO("release utan");
    //std::cin.get();
    //delete [] data;
    data = 0;
}

void Vector::freeIndex(unsigned int * & index)
{
    release(index);
    //delete [] index;
    index = 0;
}

void Vector::init(unsigned int dimension)
{
    m_size = 0;
    m_data = 0;
    m_dataEnd = 0;
    m_index = 0;
    m_nonZeros = 0;
    m_capacity = ELBOWROOM; //capacity;
    switch (m_vectorType) {
    case DEFAULT_VECTOR_TYPE:
        m_sparsityRatio = SPARSITY_RATIO;
        break;
    case SPARSE_VECTOR:
        m_sparsityRatio = 2.0;
        break;
    case DENSE_VECTOR:
        m_sparsityRatio = 0.0;
        break;
    }

    m_dimension = dimension;
    m_sorted = true;
    m_sparsityThreshold = (unsigned int) Numerical::round(m_dimension * m_sparsityRatio);
    if (m_sparsityThreshold == 0) {
        m_vectorType = DENSE_VECTOR;
        m_sorted = true;
        if (m_dimension > 0) {
            resizeDense(m_dimension, ELBOWROOM);
        } else {
            if (m_capacity > 0) {
                m_data = allocateData(m_capacity);
                m_dataEnd = m_data;
            }
        }
    } else {
        m_vectorType = SPARSE_VECTOR;
        if (m_capacity > 0) {
            resizeSparse(m_capacity);
        }
    }
}

Vector::~Vector()
{
    freeData(m_data);
    freeIndex(m_index);
    sm_fullLenghtReferenceCounter--;
    if (sm_fullLenghtReferenceCounter == 0) {
        release(sm_fullLengthVector);
        //delete [] sm_fullLengthVector;
        sm_fullLengthVector = 0;
        sm_fullLengthVectorLenght = 0;

        release(sm_countingSortBitVector);
        //delete [] sm_countingSortBitVector;
        sm_countingSortBitVector = 0;
        sm_countingSortBitVectorLength = 0;


#ifdef ANALYSE_DOT_PRODUCT
        std::ofstream ofs("diffs.txt");
        unsigned int index = 0;
        for (index = 0; index < diffs.size(); index++) {
            ofs << index << ";" << diffs[index] << endl;
        }
        ofs.close();
#endif
    }
}

Vector::Vector(const Vector & original)
{
    m_data = 0;
    copy(original);
    sm_fullLenghtReferenceCounter++;
    CHECK;
}

Vector::Vector(const Vector& original, Numerical::Double lambda)
{
    m_vectorType = original.m_vectorType;
    m_size = original.m_size;
    m_dimension = original.m_dimension;
    m_capacity = original.m_capacity;
    m_nonZeros = original.m_nonZeros;
    m_sparsityRatio = original.m_sparsityRatio;
    m_sparsityThreshold = original.m_sparsityThreshold;
    m_sorted = original.m_sorted;
    if (m_capacity == 0) {
        m_data = 0;
        m_index = 0;
    } else {
        if (m_vectorType == DENSE_VECTOR) {
            m_data = allocateData(m_capacity);
            m_index = 0;
        } else {
            m_data = allocateData(m_capacity);
            m_index = allocateIndex(m_capacity);
            panOptMemcpy(m_index, original.m_index, m_size * sizeof (unsigned int));
        }
    }
    m_dataEnd = m_data + m_size;
    Numerical::Double * actualData = m_data;
    Numerical::Double * originalData = original.m_data;
    const Numerical::Double * end = original.m_dataEnd;
    for (; originalData < end; originalData++, actualData++) {
        *actualData = *originalData * lambda;
    }

    sm_fullLenghtReferenceCounter++;
    CHECK;
}

void Vector::show() const
{
    LPINFO("");
    for (unsigned int i = 0; i < length(); i++) {
        LPINFO(i << ".:" << at(i));
    }
}

void Vector::clear()
{
    m_nonZeros = 0;
    if (m_sparsityThreshold == 0) {
        Numerical::Double * ptr = m_data;
        for (; ptr < m_dataEnd; ptr++) {
            *ptr = 0.0;
        }
    } else {
        if (m_vectorType == DENSE_VECTOR) {

            m_index = allocateIndex(m_capacity);
            m_size = 0;
            m_dataEnd = m_data;
            m_vectorType = SPARSE_VECTOR;

        } else {
            m_size = 0;
            m_dataEnd = m_data;
        }
    }
    m_sorted = true;
}

void Vector::fill(Numerical::Double value)
{
    m_sorted = true;
    if (value == 0.0) {
        clear();
        return;
    }

    if (m_sparsityThreshold == 0.0) {
        Numerical::Double * ptr = m_data;
        for (; ptr < m_dataEnd; ptr++) {
            *ptr = value;
        }
        m_nonZeros = m_size;
    } else {

        if (m_vectorType == SPARSE_VECTOR) {
            sparseToDense();
        }
        Numerical::Double * ptr = m_data;
        for (; ptr < m_dataEnd; ptr++) {
            *ptr = value;
        }
        m_nonZeros = m_size;
    }
}

void Vector::copy(const Vector & original, bool reallocate)
{
    m_vectorType = original.m_vectorType;
    m_size = original.m_size;
    m_dimension = original.m_dimension;
    m_capacity = original.m_capacity;
    m_nonZeros = original.m_nonZeros;
    m_sparsityRatio = original.m_sparsityRatio;
    m_sparsityThreshold = original.m_sparsityThreshold;
    m_sorted = original.m_sorted;
    if (m_capacity == 0) {
        m_data = 0;
        m_index = 0;
    } else {
        if (m_vectorType == DENSE_VECTOR) {
            if (reallocate == true) {
                m_data = allocateData(m_capacity);
            }
            COPY_DOUBLES(m_data, original.m_data, m_size);
            m_index = 0;
        } else {
            if (reallocate == true) {
                m_data = allocateData(m_capacity);
                m_index = allocateIndex(m_capacity);
            }
            COPY_DOUBLES(m_data, original.m_data, m_size);
            panOptMemcpy(m_index, original.m_index, m_size * sizeof (unsigned int));
        }
    }
    m_dataEnd = m_data + m_size;
    //    CHECK;
}

void Vector::resize(unsigned int length)
{
    if (length == m_dimension) {
        return;
    }
    if (m_vectorType == DENSE_VECTOR) {

        if (m_capacity >= length) {

            Numerical::Double * ptr = m_dataEnd;
            m_size = length;

            m_dataEnd = m_data + m_size;
            if (length > m_dimension) {
                while (ptr < m_dataEnd) {
                    *ptr = 0.0;
                    ptr++;
                }
            } else {
                ptr--;
                while (ptr >= m_dataEnd) {
                    if (*ptr != 0.0) {
                        m_nonZeros--;
                    }
                    ptr--;
                }
            }

        } else {

            m_capacity = length + ELBOWROOM;
            Numerical::Double * newData = allocateData(m_capacity);
            m_dataEnd = newData + length;
            Numerical::Double * actual;
            //            if (length > m_size) {
            COPY_DOUBLES(newData, m_data, m_size);
            //memcpy(newData, m_data, sizeof (Numerical::Double) * m_size);
            actual = newData + m_size;
            for (; actual < m_dataEnd; actual++) {
                *actual = 0.0;
            }

            freeData(m_data);
            m_data = newData;
            m_size = length;
        }
    } else {
        if (length < m_dimension && m_nonZeros > 0) {

            Numerical::Double * actualData = m_data;
            unsigned int * actualIndex = m_index;
            Numerical::Double * lastData = m_dataEnd - 1;
            unsigned int * lastIndex = m_index + m_size - 1;
            while (actualData <= lastData) {
                if (*actualIndex >= length) {
                    *actualIndex = *lastIndex;
                    *actualData = *lastData;
                    lastData--;
                    lastIndex--;
                } else {
                    actualData++;
                    actualIndex++;
                }
            }
            m_dataEnd = lastData + 1;
            m_size = m_dataEnd - m_data;
            m_nonZeros = m_size;
        }
    }
    m_dimension = length;
    m_sparsityThreshold = (unsigned int) Numerical::round(m_dimension * m_sparsityRatio);
    if (m_vectorType == DENSE_VECTOR) {
        if (m_nonZeros < m_sparsityThreshold) {
            denseToSparse();
        }
    } else {
        if (m_nonZeros >= m_sparsityThreshold) {
            sparseToDense();
        }
    }

}

void Vector::set(unsigned int index, Numerical::Double value)
{
    if (m_vectorType == DENSE_VECTOR) {
        Numerical::Double & original = m_data[index];
        if (original == 0.0 && value != 0.0) {
            m_nonZeros++;
        } else if (original != 0.0 && value == 0.0) {
            m_nonZeros--;
        }
        m_data[index] = value;
        if (m_nonZeros < m_sparsityThreshold) {
            denseToSparse();
        }
    } else {
        m_sorted = false;
        Numerical::Double * ptr = getElementSparse(index);
        if (value != 0) {
            if (ptr) {
                *ptr = value;
            } else {
                if (m_capacity <= m_size) {
                    resizeSparse(m_size + 1 + ELBOWROOM);
                }
                m_data[m_size] = value;
                m_index[m_size] = index;
                m_size++;
                m_dataEnd++;
                m_nonZeros += value != 0.0;
            }
        } else {
            if (ptr) {
                *ptr = m_data[ m_size - 1 ];
                m_index[ ptr - m_data ] = m_index[ m_size - 1 ];
                m_size--;
                m_dataEnd--;
                m_nonZeros--;
            }
        }
        if (m_nonZeros >= m_sparsityThreshold) {
            sparseToDense();
        }
    }
    CHECK;
}

void Vector::change(unsigned int index, Numerical::Double value)
{
    if (m_vectorType == DENSE_VECTOR) {
        m_data[index] = value;
        if (value == 0.0) {
            m_nonZeros--;
            if (m_nonZeros < m_sparsityThreshold) {
                denseToSparse();
            }
        }
    } else {
        m_sorted = false;
        Numerical::Double * ptr = getElementSparse(index);
        if (value != 0.0) {
            *ptr = value;
        } else {
            *ptr = m_data[ m_size - 1 ];
            m_index[ ptr - m_data ] = m_index[ m_size - 1 ];
            m_size--;
            m_dataEnd--;
            m_nonZeros--;
        }
    }
    CHECK;
}

void Vector::scaleByLambdas(const std::vector<Numerical::Double> & lambdas)
{
    // TODO: tesztelni mindenhol azt az esetet, mikor ures a vektor
    if (m_nonZeros == 0) {
        return;
    }
    if (m_vectorType == DENSE_VECTOR) {
        unsigned int index = 0;
        Numerical::Double * dataPtr = m_data;
        for (; index < m_size; index++, dataPtr++) {
            if (*dataPtr != 0.0) {
                *dataPtr *= lambdas[index];
                if (*dataPtr == 0.0) {
                    m_nonZeros--;
                }
            }
        }
    } else {
        // TODO: lekezelni azt, mikor 0-val szorzunk be
        Numerical::Double * dataPtr = m_data;
        unsigned int * indexPtr = m_index;
        Numerical::Double * lastData = m_dataEnd - 1;
        unsigned int * lastIndex = m_index + m_size - 1;

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
        m_dataEnd = lastData + 1;
        m_size = m_dataEnd - m_data;
        m_nonZeros = m_size;

        m_sorted = false;
    }

    if (m_nonZeros >= m_sparsityThreshold && m_vectorType == SPARSE_VECTOR) {
        sparseToDense();
    } else if (m_nonZeros < m_sparsityThreshold && m_vectorType == DENSE_VECTOR) {
        denseToSparse();
    }
}

void Vector::scaleElementBy(unsigned int index, Numerical::Double lambda)
{
    if (m_vectorType == DENSE_VECTOR) {
        Numerical::Double & original = m_data[index];
        if (original != 0.0 && lambda == 0.0) {
            m_nonZeros--;
        }
        m_data[index] = original * lambda;
        if (m_nonZeros < m_sparsityThreshold) {
            denseToSparse();
        }
    } else {
        m_sorted = false;
        Numerical::Double * ptr = getElementSparse(index);
        if (lambda != 0.0) {
            if (ptr) {
                *ptr *= lambda;
            }
        } else {
            if (ptr) {
                *ptr = m_data[ m_size - 1 ];
                m_index[ ptr - m_data ] = m_index[ m_size - 1 ];
                m_size--;
                m_dataEnd--;
                m_nonZeros--;
            }
        }
        if (m_nonZeros >= m_sparsityThreshold) {
            sparseToDense();
        }
    }
    CHECK;
}

void Vector::setNewNonzero(unsigned int index, Numerical::Double value)
{
    m_nonZeros++;
    if (m_vectorType == DENSE_VECTOR) {
        m_data[index] = value;
        //        if (m_nonZeros < m_sparsityThreshold) {
        //            denseToSparse();
        //        }
    } else {
        //TODO: Is this okay here?
        if(m_nonZeros == 1){
            m_sorted = true;
        } else {
            m_sorted = false;
        }
        if (m_capacity <= m_size) {
            resizeSparse(m_size + 1 + ELBOWROOM);
        }
        m_data[m_size] = value;
        m_index[m_size] = index;
        m_size++;
        m_dataEnd++;
        if (m_nonZeros >= m_sparsityThreshold) {
            sparseToDense();
        }
    }
    CHECK;
}

Vector Vector::operator*(const Matrix& matrix) const
{
    Vector result(matrix.columnCount());
    Vector* justifiedLeftVector = 0;

    if (matrix.rowCount() != this->length()) {
        justifiedLeftVector = new Vector(matrix.rowCount());
        for (unsigned int i = 0; i < matrix.rowCount(); i++) {
            if (i >= this->length()) {
                justifiedLeftVector->set(i, 0.0);
            } else {
                justifiedLeftVector->set(i, this->at(i));
            }
        }
    } else {
        justifiedLeftVector = new Vector(*this);
    }

    for (unsigned int j = 0; j < matrix.columnCount(); j++) {
        Numerical::Double columnResult = 0.0;
        for (unsigned int i = 0; i < matrix.rowCount(); i++) {
            columnResult += matrix.get(i, j) * justifiedLeftVector->at(i);
        }
        result.set(j, columnResult);
    }
    delete justifiedLeftVector;
    justifiedLeftVector = 0;
    return result;
}

Vector Vector::operator*(Numerical::Double m) const
{
    Vector ret(*this);
    ret.scaleBy(m);
    return ret;
}

Vector Vector::operator-(const Vector& vector) const
{
    Vector ret(*this);
    ret.addVector(-1, vector);
    return ret;
}

Vector Vector::operator+(const Vector& vector) const
{
    Vector ret(*this);
    ret.addVector(1, vector);
    return ret;
}

const Numerical::Double& Vector::at(unsigned int index) const
{

    if (m_vectorType == DENSE_VECTOR) {
        CHECK;
        return m_data[index];
    } else {
        Numerical::Double * value = getElementSparse(index);
        if (value) {
            CHECK;
            return *value;
        } else {
            CHECK;
            return ZERO;
        }
    }
    CHECK;
    return ZERO;
}

const Numerical::Double * Vector::getDenseElementPointer(unsigned int index) const {
    return m_data + index;
}

unsigned int Vector::length() const
{
    //    CHECK;
    return m_dimension;
}

unsigned int Vector::capacity() const
{
    CHECK;
    return m_capacity;
}

unsigned int Vector::maxIndex() const
{
    if (m_nonZeros == 0) {
        return 0;
    }
    if (m_vectorType == DENSE_VECTOR) {
        CHECK;
        Numerical::Double * ptr = m_dataEnd - 1;
        unsigned int index = m_dimension - 1;
        while (*ptr == 0.0) {
            ptr--;
            index--;
        }
        return index;
    } else {
        const unsigned int * indexPtr = m_index;
        const unsigned int * const indexPtrEnd = m_index + m_size;
        unsigned int maxIndex = 0;
        while (indexPtr < indexPtrEnd) {
            unsigned int actual = *indexPtr;
            if (actual > maxIndex) {
                maxIndex = actual;
            }
            indexPtr++;
        }
        CHECK;
        return maxIndex;
    }
    CHECK;
    return 0;
}

Vector & Vector::scaleBy(Numerical::Double lambda)
{
    if (lambda == 0.0) {
        m_nonZeros = 0;
        if (m_vectorType == DENSE_VECTOR) {
            if (m_nonZeros < m_sparsityThreshold) {
                m_size = 0;
                m_capacity = ELBOWROOM;
                m_vectorType = SPARSE_VECTOR;
                m_dataEnd = m_data;
                m_sorted = true;
                m_index = allocateIndex(m_capacity);
                if (m_capacity == 0) {
                    freeData(m_data);
                    m_dataEnd = 0;
                }
            } else {
                Numerical::Double * ptr = m_data;
                while (ptr < m_dataEnd) {
                    *ptr = 0;
                    ptr++;
                }
            }
        } else {
            m_sorted = true;
            m_size = 0;
            m_dataEnd = m_data;
        }
    } else {
        Numerical::Double * ptr = m_data;
        while (ptr < m_dataEnd) {
            *ptr *= lambda;
            ptr++;
        }
    }
    CHECK;
    return *this;
}

Numerical::Double Vector::euclidNorm() const
{
    Numerical::Double result = 0.0;
    Numerical::Double * dataPtr = m_data;
    for (; dataPtr < m_dataEnd; dataPtr++) {
        const Numerical::Double value = *dataPtr;
        result += value * value;
    }
    return Numerical::sqrt(result);
}

Numerical::Double Vector::l1Norm() const
{
    Numerical::Double result = 0.0;
    Numerical::Double * dataPtr = m_data;
    for (; dataPtr < m_dataEnd; dataPtr++) {
        result += Numerical::fabs(*dataPtr);
    }
    return result;
}

Numerical::Double Vector::dotProduct(const Vector & vector, bool stableAddAbs, bool stableAddRel) const
{
    if (m_size == 0 || vector.m_size == 0) {
        CHECK;
        return 0.0;
    }

    //LPINFO(*this);
    //LPINFO(vector);
    //std::cin.get();

#ifdef ANALYSE_DOT_PRODUCT
/*    {
        Vector vector1 = *this;
        Vector vector2 = vector;
        vector1.setSparsityRatio(0.0);
        vector2.setSparsityRatio(0.0);
        Vector::Iterator iter1 = vector1.begin();
        Vector::Iterator iter2 = vector2.begin();
        Vector::Iterator endIter1 = vector1.end();
        Vector::Iterator endIter2 = vector2.end();

        Numerical::Double neg = 0, pos = 0;
        unsigned int maxExponent = 0;
        unsigned int minExponent = 0;

        static unsigned int maxExponentGlobal = 0;
        static unsigned int minExponentGlobal = 0;

        Numerical::Double minValue;
        Numerical::Double maxValue;
        Numerical::Double min1, min2, max1, max2;
        for (; iter1 != endIter1 && iter2 != endIter2; iter1++, iter2++) {
            Numerical::Double value = *iter1 * *iter2;
            Numerical::Float64 bits;
            bits.m_value = value;
            if (value != 0.0) {
                if (maxExponent < bits.m_bits.m_exponent) {
                    maxExponent = bits.m_bits.m_exponent;
                    maxValue = value;
                    max1 = *iter1;
                    max2 = *iter2;
                }
                if (minExponent == 0 || minExponent > bits.m_bits.m_exponent) {
                    minExponent = bits.m_bits.m_exponent;
                    minValue = value;
                    min1 = *iter1;
                    min2 = *iter2;
                }
            }
        }
        if (minExponent != 0) {
            static unsigned int counter = 0;
            bool fresh = false;
            if (minExponentGlobal == 0 || minExponentGlobal > minExponent) {
                minExponentGlobal = minExponent;
                fresh = true;
            }
            if (maxExponentGlobal == 0 || maxExponentGlobal < maxExponent) {
                maxExponentGlobal = maxExponent;
                fresh = true;
            }
            if (minExponent <= maxExponent) {
                //diffs[ maxExponent - minExponent ]++;
                counter++;
                if (counter >= 5000) {
                    counter = 0;
                    std::ofstream ofs("diffs.txt");
                    unsigned int index = 0;
                    for (index = 0; index < diffs.size(); index++) {
                        ofs << index << ";" << diffs[index] << endl;
                    }
                    ofs.close();
                }
                //std::cin.get();

            }

        }
    }
*/
#endif

    //    static Numerical::BucketSummarizer summarizer(10); // ez a klasszikus neg-pos-os, minel lejjebb visszuk
    // annal pontosabb, de annal lassabb is
    //Numerical::BucketSummarizer summarizer(1);
    Numerical::Summarizer summarizer;

    if (m_vectorType == SPARSE_VECTOR && vector.m_vectorType == SPARSE_VECTOR &&
            m_sorted && vector.m_sorted) {

        unsigned int * index1 = m_index;
        unsigned int * index2 = vector.m_index;
        const unsigned int * endIndex1 = m_index + m_nonZeros;
        const unsigned int * endIndex2 = vector.m_index + vector.m_nonZeros;

        //LPERROR(*this << vector);

        while (index1 < endIndex1 && index2 < endIndex2) {
            if (*index1 > *index2) {
                //LPERROR(*index1 << " > " << *index2 << " -> index2 ugrik ide: " << (index2[1]));
                index2++;
            } else if (*index2 > *index1) {
                //LPERROR(*index2 << " > " << *index1 << " -> index1 ugrik ide: " << (index1[1]));
                index1++;
            } else {
                Numerical::Double value = m_data[ index1 - m_index ] *
                        vector.m_data[ index2 - vector.m_index ];
                summarizer.add(value);
                index1++;
                index2++;
            }
        }
        Numerical::Double res = summarizer.getResult(stableAddAbs, stableAddRel);
        //LPINFO(res);
        //std::cin.get();
        return res;
    }
    Numerical::Double temp;
    if (m_vectorType == DENSE_VECTOR && vector.m_vectorType == DENSE_VECTOR) {
        const Numerical::Double * ptr1 = m_data;
        const Numerical::Double * ptr2 = vector.m_data;
        const Numerical::Double * end = m_dataEnd;
        while (ptr1 < end) {
            temp = *ptr1 * *ptr2;
            summarizer.add(temp);
            ptr1++;
            ptr2++;
        }
        Numerical::Double res = summarizer.getResult(stableAddAbs, stableAddRel);
        //LPINFO(res);
        //std::cin.get();
        return res;

        //        return summarizer.getResult(stableAddAbs, stableAddRel);
    }

    Numerical::Double * data;
    unsigned int * index;
    unsigned int size;
    bool needScatter;
    Numerical::Double * denseVector = sm_fullLengthVector;
    unsigned int * origIndex = 0;
    unsigned int origSize = 0;
    // when both of them are sparse, it has to be converted to dense
    if (m_vectorType == DENSE_VECTOR || vector.m_vectorType == DENSE_VECTOR) {
        needScatter = false;
        if (m_vectorType == DENSE_VECTOR) {
            denseVector = m_data;
            data = vector.m_data;
            index = vector.m_index;
            size = vector.m_size;
        } else {
            denseVector = vector.m_data;
            data = m_data;
            index = m_index;
            size = m_size;
        }
    } else {
        // both of them are sparse
        needScatter = true;
        if (m_nonZeros >= vector.m_nonZeros) {
            scatter(sm_fullLengthVector, sm_fullLengthVectorLenght, *this);
            denseVector = sm_fullLengthVector;
            data = vector.m_data;
            index = vector.m_index;
            size = vector.m_size;
            origIndex = m_index;
            origSize = m_size;
        } else {
            scatter(sm_fullLengthVector, sm_fullLengthVectorLenght, vector);
            denseVector = sm_fullLengthVector;
            data = m_data;
            index = m_index;
            size = m_size;
            origIndex = vector.m_index;
            origSize = vector.m_size;
        }
    }


    const Numerical::Double * ptrSparse = data;
    const Numerical::Double * const ptrSparseEnd = ptrSparse + size;
    const unsigned int * ptrIndex = index;

    while (ptrSparse < ptrSparseEnd) {

        temp = denseVector[ *ptrIndex ] * *ptrSparse;
        summarizer.add(temp);
        if (needScatter) {
            // denseVector[ *ptrIndex ] = 0.0;
        }
        ptrSparse++;
        ptrIndex++;
    }
    if (needScatter) {
        clearFullLenghtVector(sm_fullLengthVector, origIndex, origSize);
    }
    Numerical::Double res = summarizer.getResult(stableAddAbs, stableAddRel);
    //LPINFO(res);
    //std::cin.get();
    return res;
    //    return summarizer.getResult(stableAddAbs, stableAddRel);
}

Vector & Vector::addVector(Numerical::Double lambda,
                           const Vector & vector,
                           Numerical::ADD_TYPE addType)
{
    if (vector.m_size == 0 || lambda == 0.0) {
        CHECK;
        return *this;
    }
    if (m_vectorType == DENSE_VECTOR) {
        if (vector.m_vectorType == DENSE_VECTOR) {
            addDenseToDense(lambda, vector, addType);
        } else {
            addSparseToDense(lambda, vector, addType);
        }
    } else {
        if (vector.m_vectorType == Vector::SPARSE_VECTOR) {
            addSparseToSparse(lambda, vector, addType);
        } else {
            addDenseToSparse(lambda, vector, addType);
        }
    }

    CHECK;
    return *this;
}

void Vector::addDenseToDense(Numerical::Double lambda,
                             const Vector & vector,
                             Numerical::ADD_TYPE addType)
{

    Numerical::Double * ptr1 = m_data;
    const Numerical::Double * ptr2 = vector.m_data;
    const Numerical::Double * end = m_dataEnd;
    //    int nonZeros1 = 0; // DO NOT DELETE YET!!!
    //    int nonZeros2 = 0; // DO NOT DELETE YET!!!
    // TODO: nonzero alapjan valszammal eldonteni, hogy erdemes-e
    // megnezni, hogy 0-t adunk-e hozza a vektor adott elemehez
    // Es a nem nulla adminisztraciojat is lehet okosabban csinalni
    switch (addType) {
    case Numerical::ADD_ABS_REL:
        while (ptr1 < end) {
            if (*ptr2 != 0.0) {

                if (*ptr1 == 0.0) {
                    m_nonZeros++;
                }
                //LPINFO(*ptr1 << " + " << (*ptr2 * lambda));
                *ptr1 = Numerical::stableAdd(*ptr1, *ptr2 * lambda);
                // Numerical::isZero is unnecessary after stableAdd
                if (*ptr1 == 0.0) {
                    *ptr1 = 0.0;
                    m_nonZeros--;
                }
            }
            ptr1++;
            ptr2++;
        }
        //std::cin.get();
        break;
    case Numerical::ADD_ABS:
        while (ptr1 < end) {
            if (*ptr2 != 0.0) {
                if (*ptr1 == 0.0) {
                    m_nonZeros++;
                }
                *ptr1 = Numerical::stableAddAbs(*ptr1, *ptr2 * lambda);
                if (*ptr1 == 0.0) {
                    *ptr1 = 0.0;
                    m_nonZeros--;
                }
            }
            ptr1++;
            ptr2++;
        }
        break;
    case Numerical::ADD_REL:
        while (ptr1 < end) {
            if (*ptr2 != 0.0) {
                if (*ptr1 == 0.0) {
                    m_nonZeros++;
                }
                *ptr1 = Numerical::stableAddRel(*ptr1, *ptr2 * lambda);
                if (*ptr1 == 0.0) {
                    *ptr1 = 0.0;
                    m_nonZeros--;
                }
            }
            ptr1++;
            ptr2++;
        }
        break;
    case Numerical::ADD_FAST:
        while (ptr1 < end) {
            if (*ptr2 != 0.0) {
                if (*ptr1 == 0.0) {
                    m_nonZeros++;
                }
                *ptr1 += *ptr2 * lambda;
                if (*ptr1 == 0.0) {
                    *ptr1 = 0.0;
                    m_nonZeros--;
                }
            }
            ptr1++;
            ptr2++;
        }
        break;
    }

    if (m_nonZeros < m_sparsityThreshold) {
        denseToSparse();
    }

    CHECK;
}

void Vector::addDenseToSparse(Numerical::Double lambda,
                              const Vector & vector,
                              Numerical::ADD_TYPE addType)
{
    m_sorted = false;
    sparseToDense();
    addDenseToDense(lambda, vector, addType);
    CHECK;
}

void Vector::addSparseToDense(Numerical::Double lambda,
                              const Vector & vector,
                              Numerical::ADD_TYPE addType)
{
    const Numerical::Double * ptrData = vector.m_data;
    const Numerical::Double * const ptrDataEnd = vector.m_dataEnd;
    const unsigned int * ptrIndex = vector.m_index;
    switch (addType) {
    case Numerical::ADD_ABS_REL:
        while (ptrData < ptrDataEnd) {
            Numerical::Double & data = m_data[ *ptrIndex ];
            if (data == 0.0) {
                m_nonZeros++;
            }
            data = Numerical::stableAdd(data, lambda * *ptrData);
            if (data == 0.0) {
                data = 0.0;
                m_nonZeros--;
            }
            ptrData++;
            ptrIndex++;
        }
        break;
    case Numerical::ADD_ABS:
        while (ptrData < ptrDataEnd) {
            Numerical::Double & data = m_data[ *ptrIndex ];
            if (data == 0.0) {
                m_nonZeros++;
            }
            data = Numerical::stableAddAbs(data, lambda * *ptrData);
            if (data == 0.0) {
                data = 0.0;
                m_nonZeros--;
            }
            ptrData++;
            ptrIndex++;
        }
        break;
    case Numerical::ADD_REL:
        while (ptrData < ptrDataEnd) {
            Numerical::Double & data = m_data[ *ptrIndex ];
            if (data == 0.0) {
                m_nonZeros++;
            }
            data = Numerical::stableAddRel(data, lambda * *ptrData);
            if (data == 0.0) {
                data = 0.0;
                m_nonZeros--;
            }
            ptrData++;
            ptrIndex++;
        }
        break;
    case Numerical::ADD_FAST:
        while (ptrData < ptrDataEnd) {
            Numerical::Double & data = m_data[ *ptrIndex ];
            if (data == 0.0) {
                m_nonZeros++;
            }
            data += lambda * *ptrData;
            if (data == 0.0) {
                data = 0.0;
                m_nonZeros--;
            }
            ptrData++;
            ptrIndex++;
        }
        break;
    }
    if (m_nonZeros < m_sparsityThreshold) {
        denseToSparse();
    }
    CHECK;
}

void Vector::addSparseToSparse(Numerical::Double lambda,
                               const Vector & vector,
                               Numerical::ADD_TYPE addType)
{
    m_sorted = false;
    scatter(sm_fullLengthVector, sm_fullLengthVectorLenght, vector);

    Numerical::Double * denseVector = sm_fullLengthVector;
    Numerical::Double * ptrActualVector = m_data;
    unsigned int * ptrIndex = m_index;
    switch (addType) {
    case Numerical::ADD_ABS_REL:
        while (ptrActualVector < m_dataEnd) {
            if (denseVector[ *ptrIndex ] != 0.0) {

                Numerical::Double result = Numerical::stableAdd(*ptrActualVector, lambda * denseVector[ *ptrIndex ]);
                denseVector[ *ptrIndex ] = 0.0;
                if (result != 0.0) {
                    *ptrActualVector = result;
                    ptrActualVector++;
                    ptrIndex++;
                } else {
                    m_dataEnd--;
                    m_size--;
                    m_nonZeros--;
                    if (ptrActualVector < m_dataEnd) {
                        *ptrActualVector = *m_dataEnd;
                        *ptrIndex = m_index[ m_size ];
                    }
                }
            } else {
                ptrActualVector++;
                ptrIndex++;
            }
        }
        break;
    case Numerical::ADD_ABS:
        while (ptrActualVector < m_dataEnd) {
            if (denseVector[ *ptrIndex ] != 0.0) {

                Numerical::Double result = Numerical::stableAddAbs(*ptrActualVector, lambda * denseVector[ *ptrIndex ]);
                denseVector[ *ptrIndex ] = 0.0;
                if (result != 0.0) {
                    *ptrActualVector = result;
                    ptrActualVector++;
                    ptrIndex++;
                } else {
                    m_dataEnd--;
                    m_size--;
                    m_nonZeros--;
                    if (ptrActualVector < m_dataEnd) {
                        *ptrActualVector = *m_dataEnd;
                        *ptrIndex = m_index[ m_size ];
                    }
                }
            } else {
                ptrActualVector++;
                ptrIndex++;
            }
        }
        break;
    case Numerical::ADD_REL:
        while (ptrActualVector < m_dataEnd) {
            if (denseVector[ *ptrIndex ] != 0.0) {

                Numerical::Double result = Numerical::stableAddRel(*ptrActualVector, lambda * denseVector[ *ptrIndex ]);
                denseVector[ *ptrIndex ] = 0.0;
                if (result != 0.0) {
                    *ptrActualVector = result;
                    ptrActualVector++;
                    ptrIndex++;
                } else {
                    m_dataEnd--;
                    m_size--;
                    m_nonZeros--;
                    if (ptrActualVector < m_dataEnd) {
                        *ptrActualVector = *m_dataEnd;
                        *ptrIndex = m_index[ m_size ];
                    }
                }
            } else {
                ptrActualVector++;
                ptrIndex++;
            }
        }
        break;
    case Numerical::ADD_FAST:
        while (ptrActualVector < m_dataEnd) {
            if (denseVector[ *ptrIndex ] != 0.0) {
                Numerical::Double result = *ptrActualVector + lambda * denseVector[ *ptrIndex ];
                denseVector[ *ptrIndex ] = 0.0;
                if (result != 0.0) {
                    *ptrActualVector = result;
                    ptrActualVector++;
                    ptrIndex++;
                } else {
                    m_dataEnd--;
                    m_size--;
                    m_nonZeros--;
                    if (ptrActualVector < m_dataEnd) {
                        *ptrActualVector = *m_dataEnd;
                        *ptrIndex = m_index[ m_size ];
                    }
                }
            } else {
                ptrActualVector++;
                ptrIndex++;
            }
        }
        break;
    }

    ptrIndex = vector.m_index;
    ptrActualVector = vector.m_data;
    while (ptrActualVector < vector.m_dataEnd) {
        if (sm_fullLengthVector[ *ptrIndex ] != 0.0) {
            Numerical::Double value = lambda * *ptrActualVector;
            if (value != 0.0) {
                if (m_capacity <= m_size) {
                    resizeSparse(m_size + 1 + ELBOWROOM);
                }
                m_data[m_nonZeros] = value;
                m_index[m_nonZeros] = *ptrIndex;
                m_size++;
                m_dataEnd++;
                m_nonZeros++;
            }
            sm_fullLengthVector[ *ptrIndex ] = 0.0;
        }
        ptrIndex++;
        ptrActualVector++;
    }
    if (m_nonZeros >= m_sparsityThreshold) {
        sparseToDense();
    }
    CHECK;
}

Vector & Vector::elementaryFtran(const Vector & eta, unsigned int pivot)
{
    //LPINFO("this: " << *this);
    //LPINFO("eta: " << eta);
    //LPINFO("pivot: " << at(pivot));
    //std::cin.get();
    Numerical::Double pivotValue = at(pivot);
    m_sorted = m_vectorType == DENSE_VECTOR;
    Numerical::Double atPivot = eta.at(pivot);
    addVector(pivotValue, eta, Numerical::ADD_ABS_REL);
    //    addVector(pivotValue, eta, Numerical::ADD_ABS);
    //addVector(pivotValue, eta, Numerical::ADD_FAST);
    set(pivot, atPivot * pivotValue);
    return *this;
}

Vector & Vector::elementaryBtran(const Vector & eta, unsigned int pivot)
{
    set(pivot, dotProduct(eta));
    return *this;
}

void Vector::removeElement(unsigned int index)
{
    bool minus = false;
    if (m_vectorType == DENSE_VECTOR) {
        minus = m_data[index] != 0.0;
        Numerical::Double * ptrData1 = m_data + index;
        const Numerical::Double * ptrData2 = ptrData1 + 1;
        while (ptrData2 < m_dataEnd) {
            *ptrData1 = *ptrData2;
            ptrData1++;
            ptrData2++;
        }
        m_size--;
        m_dataEnd--;
    } else {
        m_sorted = false;
        unsigned int * indexPtr = m_index;
        const unsigned int * const indexEnd = m_index + m_size;
        int index2 = -1;
        while (indexPtr < indexEnd) {
            if (*indexPtr > index) {
                (*indexPtr)--;
            } else if (*indexPtr == index) {
                index2 = indexPtr - m_index;
                minus = m_data[ index2 ] != 0.0;
            }
            indexPtr++;
        }

        if (index2 >= 0) {
            removeElementSparse((unsigned int) index2);
        }

        // minden ennel nagyobb indexet csokkenteni kell
    }
    m_dimension--;
    m_sparsityThreshold = (unsigned int) Numerical::round(m_dimension * m_sparsityRatio);
    if (minus) {
        m_nonZeros--;
    }
    if (m_vectorType == DENSE_VECTOR) {
        if (m_nonZeros < m_sparsityThreshold) {
            denseToSparse();
        }
    } else if (m_nonZeros >= m_sparsityThreshold) {
        sparseToDense();
    }


    CHECK;
}

void Vector::insertElement(unsigned int index, Numerical::Double value)
{
    if (index == m_dimension) {
        append(value);
        CHECK;
        return;
    }
    if (m_vectorType == DENSE_VECTOR) {
        int sub = 0;
        if (m_capacity <= m_size) {
            resizeDense(m_size + 1, ELBOWROOM);
            sub = 1;
        }

        Numerical::Double * ptrData1 = m_data + m_size - sub;
        const Numerical::Double * ptrData2 = ptrData1 - 1;
        Numerical::Double * const stop = m_data + index;
        while (ptrData2 >= stop) {
            *ptrData1 = *ptrData2;
            ptrData1--;
            ptrData2--;
        }
        m_data[index] = value;
        if (value != 0.0) {
            m_nonZeros++;
        }
        if (sub == 0) {
            m_size++;
            m_dimension++;
            m_dataEnd++;
        }
        m_sorted = true;
    } else {
        m_sorted = false;
        unsigned int * indexPtr = m_index;
        const unsigned int * const indexEnd = m_index + m_size;
        while (indexPtr < indexEnd) {
            if (*indexPtr >= index) {
                (*indexPtr)++;
            }
            indexPtr++;
        }
        if (value != 0.0) {
            m_nonZeros++;
            if (m_capacity <= m_size) {
                resizeSparse(m_size + 1 + ELBOWROOM);
            }
            m_data[m_size] = value;
            m_index[m_size] = index;
            m_size++;
            m_dataEnd++;
        }
        m_dimension++;
    }

    m_sparsityThreshold = (unsigned int) Numerical::round(m_dimension * m_sparsityRatio);
    if (m_nonZeros >= m_sparsityThreshold && m_vectorType == SPARSE_VECTOR) {
        sparseToDense();
    } else if (m_nonZeros < m_sparsityThreshold && m_vectorType == DENSE_VECTOR) {
        denseToSparse();
    }

    CHECK;
}

void Vector::append(Numerical::Double value)
{
    if (value != 0.0) {
        m_nonZeros++;
    }
    if (m_vectorType == DENSE_VECTOR) {
        if (m_capacity <= m_size) {
            resizeDense(m_size + 1, ELBOWROOM);
            m_data[m_size - 1] = value;
        } else {
            m_data[m_size] = value;
            m_size++;
            m_dataEnd++;
            m_dimension++;
        }

    } else {
        m_sorted = false;
        if (value != 0.0) {
            if (m_capacity <= m_size) {
                resizeSparse(m_size + 1 + ELBOWROOM);
            }
            m_data[m_size] = value;
            m_index[m_size] = m_dimension;
            m_size++;
            m_dataEnd++;
        }
        m_dimension++;
    }

    m_sparsityThreshold = (unsigned int) Numerical::round(m_dimension * m_sparsityRatio);
    if (m_nonZeros >= m_sparsityThreshold && m_vectorType == SPARSE_VECTOR) {
        sparseToDense();
    } else if (m_nonZeros < m_sparsityThreshold && m_vectorType == DENSE_VECTOR) {
        denseToSparse();
    }
    CHECK;
}

void Vector::appendVector(const Vector & vector) {
    int index = m_dimension;
    resize( m_dimension + vector.m_dimension );
    Vector::NonzeroIterator iter = vector.beginNonzero();
    Vector::NonzeroIterator iterEnd = vector.endNonzero();
    for (; iter != iterEnd; iter++) {
        setNewNonzero( index + iter.getIndex(), *iter );
    }
}

Vector & Vector::operator=(const Vector & vector)
{
    if (this == &vector) {
        return *this;
    }
    bool reallocate = false;
    if(m_size!=vector.m_size ){
        freeData(m_data);
        freeIndex(m_index);
        reallocate = true;
    }
    copy(vector, reallocate);
    CHECK;
    return *this;
}

void Vector::sortElements() const
{
    if (m_vectorType == DENSE_VECTOR || m_nonZeros < 2) {
        return;
    }
    m_sorted = true;
    if (m_nonZeros < 10) {
        selectionSort();
        return;
    }
    // calculating number of steps in different sorting algoirthms

    //bool sorted = true;
    unsigned int *minPtr = m_index;
    unsigned int *maxPtr = m_index;
    unsigned int * actual = m_index;
    const unsigned int * end = m_index + m_nonZeros;
    for (; actual < end; actual++) {
        if (*actual < *minPtr) {
            minPtr = actual;
        } else if (*actual > *maxPtr) {
            maxPtr = actual;
        }
    }

    unsigned int selectionCount = m_nonZeros * m_nonZeros / 2;
    unsigned int countingCount = m_nonZeros * 2 + (*maxPtr - *minPtr);
    unsigned int heapsortCount = 1;
    while ((1U << heapsortCount) < m_nonZeros) {
        heapsortCount++;
    }
    heapsortCount *= m_nonZeros;
    heapsortCount <<= 1;

    unsigned int sortingAlgorithm = 0;
    unsigned int min = selectionCount;
    if (countingCount < min) {
        min = countingCount;
        sortingAlgorithm = 1;
    }
    if (heapsortCount < min) {
        min = heapsortCount;
        sortingAlgorithm = 2;
    }

    switch (sortingAlgorithm) {
    case 0:
        insertionSort();
        //selectionSort();
        break;
    case 1:
        countingSort();
        break;
    case 2:
        heapSort();
        break;
    }
}

void Vector::countingSort() const
{
    const unsigned int bitCount = sizeof (unsigned long) * CHAR_BIT;
    const unsigned int shift = bitCount == 16 ? 4 : (bitCount == 32 ? 5 : 6);
    const unsigned int arraySize = m_dimension / bitCount + 1;
    unsigned long mask = bitCount == 16 ? 0xf : (bitCount == 32 ? 0x1f : 0x3f);
    unsigned long * actualBits = sm_countingSortBitVector;
    const unsigned long * endBits = sm_countingSortBitVector + arraySize;

    if (sm_countingSortBitVectorLength < arraySize) {
        release(sm_countingSortBitVector);
        // delete [] sm_countingSortBitVector;
        sm_countingSortBitVectorLength = arraySize;
        sm_countingSortBitVector = alloc<unsigned long, 32>(arraySize); //new unsigned long[ arraySize ];
        actualBits = sm_countingSortBitVector;
        endBits = sm_countingSortBitVector + arraySize;
        for (; actualBits < endBits; actualBits++) {
            *actualBits = 0UL;
        }

    }


    scatter(sm_fullLengthVector, sm_fullLengthVectorLenght, *this);
    static int count = 0;
    count++;

    //std::cin.get();


    unsigned int max = 0;
    unsigned int min = 0;
    unsigned int * actual = m_index;
    const unsigned int * end = m_index + m_nonZeros;

    for (; actual < end; actual++) {
        //originalIndices[*actual] = actual - m_index;
        const unsigned int index = *actual >> shift;
        const unsigned int bitIndex = *actual & mask;
        sm_countingSortBitVector[index] |= 1UL << bitIndex;
        if (max < index) {
            max = index;
        } else if (min > index) {
            min = index;
        }
    }

    Numerical::Double * actualData = m_data;
    actual = m_index;
    actualBits = sm_countingSortBitVector + min;
    unsigned int position = 0;
    const unsigned long * endBits2 = sm_countingSortBitVector + max + 1;
    for (; actualBits < endBits2; actualBits++, position += bitCount) {
        unsigned int index = 0;
        mask = 0x1;
        const unsigned long actualElement = *actualBits;
        do {

            if (actualElement & mask) {
                *actual = position + index;
                actual++;
                *actualData = sm_fullLengthVector[ position + index ];
                actualData++;
                sm_fullLengthVector[ position + index ] = 0.0;
            }

            mask <<= 1;
            index++;
        } while (index < bitCount);
        *actualBits = 0;
    }

}

void Vector::heapSort() const
{
    Heap<unsigned int, Numerical::Double> heap(m_nonZeros);
    unsigned int * actualIndex = m_index;
    Numerical::Double * actualData = m_data;
    heap.startBuild();
    for (; actualData < m_dataEnd; actualData++, actualIndex++) {
        heap.addForBuild(*actualIndex, *actualData);
    }
    heap.build();
    //actualIndex = m_index + m_nonZeros - 1;
    //actualData = m_data + m_nonZeros - 1;
    actualIndex--;
    actualData--;
    for (; actualData >= m_data; actualData--, actualIndex--) {
        *actualData = heap.getData(0);
        *actualIndex = heap.getMax();
        heap.remove(0);
    }
}

void Vector::selectionSort() const
{
    unsigned int * actual = m_index;
    const unsigned int * end1 = m_index + m_nonZeros - 1;
    const unsigned int * end2 = end1 + 1;
    for (; actual < end1; actual++) {
        unsigned int * minPtr = actual;
        unsigned int * actual2 = actual + 1;
        for (; actual2 < end2; actual2++) {
            if (*actual2 < *minPtr) {
                minPtr = actual2;
            }
        }

        // swap of indices
        unsigned int tempIndex = *minPtr;
        *minPtr = *actual;
        *actual = tempIndex;

        // swap of values
        Numerical::Double tempValue = m_data[ minPtr - m_index ];
        m_data[ minPtr - m_index ] = m_data[ actual - m_index ];
        m_data[ actual - m_index ] = tempValue;
    }
}

void Vector::insertionSort() const
{
    if (m_nonZeros < 2) {
        return;
    }
    unsigned int * actualIndex;
    unsigned int * previousIndex;
    Numerical::Double * actualData;
    Numerical::Double * previousData;
    unsigned int index = 1;
    for (; index < m_nonZeros; index++) {
        actualIndex = m_index + index;
        previousIndex = actualIndex - 1;
        actualData = m_data + index;
        previousData = actualData - 1;
        const Numerical::Double insertData = *actualData;
        const unsigned int insertIndex = *actualIndex;
        //        LPWARNING("index: " << index);
        //        LPWARNING("element: " << insertIndex << ", " << insertData);
        //        LPWARNING("vector: " << *this);
        while (previousIndex >= m_index && *previousIndex > insertIndex) {
            *actualIndex = *previousIndex;
            *actualData = *previousData;

            actualData--;
            actualIndex--;
            previousIndex--;
            previousData--;
        }
        *actualData = insertData;
        *actualIndex = insertIndex;

        //        LPWARNING("insert position: " << (previousIndex - m_index));
        //        LPWARNING("after insert: " << *this);
        //        std::cin.get();
    }
}

void Vector::resizeDense(unsigned int size, unsigned int elbowroom)
{
    m_capacity = size + elbowroom;
    Numerical::Double * temp = allocateData(m_capacity);
    COPY_DOUBLES(temp, m_data, (size > m_size ? m_size : size));
    freeData(m_data);
    m_data = temp;
    m_dataEnd = m_data + size;
    // a maradekot le kell nullazni
    Numerical::Double * ptr = m_data + m_size;
    while (ptr < m_dataEnd) {
        *ptr = 0.0;
        ptr++;
    }
    m_size = size;
    m_dimension = size;
    m_sparsityThreshold = (unsigned int) Numerical::round(m_dimension * m_sparsityRatio);
    CHECK;
}

void Vector::resizeSparse(unsigned int capacity)
{
    if (capacity == 0) {
        freeData(m_data);
        freeIndex(m_index);
        m_dataEnd = 0;
        m_nonZeros = 0;
        m_size = 0;
        m_sparsityThreshold = 0;
        m_capacity = 0;
        return;
    }
    if (capacity < m_size) {
        capacity = m_size;
    }

    Numerical::Double * tempData = allocateData(capacity);
    unsigned int * tempIndex = allocateIndex(capacity);
    COPY_DOUBLES(tempData, m_data, m_size);
    panOptMemcpy(tempIndex, m_index, m_size * sizeof (unsigned int));
    freeData(m_data);
    m_data = tempData;
    freeIndex(m_index);
    m_index = tempIndex;
    m_dataEnd = m_data + m_size;
    m_capacity = capacity;

    m_sparsityThreshold = (unsigned int) Numerical::round(m_dimension * m_sparsityRatio);
    CHECK;
}

Numerical::Double * Vector::getElementSparseLinear(unsigned int index) const
{
    unsigned int * indexPtr = m_index;
    const unsigned int * const indexPtrEnd = m_index + m_size;
    while (indexPtr < indexPtrEnd && *indexPtr != index) {
        indexPtr++;
    }
    if (indexPtr < indexPtrEnd) {
        CHECK;
        return m_data + (indexPtr - m_index);
    }
    CHECK;
    return 0;
}

Numerical::Double * Vector::getElementSparseBinary(unsigned int index) const
{
    if (m_nonZeros == 0) {
        return 0;
    }
    unsigned int * middle;
    int min = 0;
    int max = m_nonZeros - 1;
    do {
        middle = m_index + ((min + max) >> 1);
        if (index > *middle) {
            min = (middle - m_index) + 1;
        } else {
            max = (middle - m_index) - 1;
        }
    } while (*middle != index && min <= max);
    if (*middle == index) {
        CHECK;
        return m_data + (middle - m_index);
    }
    CHECK;
    return 0;
}

Numerical::Double * Vector::getElementSparse(unsigned int index) const
{
    if (m_sorted == false) {
        return getElementSparseLinear(index);
    }
    return getElementSparseBinary(index);
}

void Vector::addNewElementSparse(unsigned int index, Numerical::Double value)
{
    if (value == 0.0) {
        return;
    }
    if (m_size >= m_capacity) {
        if (ELBOWROOM == 0) {
            m_capacity++;
        } else {
            m_capacity += ELBOWROOM;
        }
        resizeSparse(m_capacity);
    }
    m_data[m_size] = value;
    m_index[m_size] = index;
    m_size++;
    m_dataEnd++;
    //CHECK;
}

void Vector::removeElementSparse(unsigned int index)
{
    m_data[index] = m_data[m_size - 1];
    m_index[index] = m_index[m_size - 1];
    if (m_index[index] >= m_dimension) {
        LPERROR("Invalid index: " << index << ", but greater index = " << m_dimension - 1);
    }
    m_dataEnd--;
    m_size--;
    CHECK;
}

unsigned int Vector::gather(Numerical::Double * denseVector, Numerical::Double * sparseVector,
                            unsigned int * indexVector, unsigned int denseLength)
{
    // a denseVector-bol kigyujti a nem nulla elemeket a sparseVector-ba
    // es az indexeket az indexVector-ba
    unsigned int index = 0;
    Numerical::Double * ptrDense = denseVector;
    Numerical::Double * ptrSparse = sparseVector;
    unsigned int * ptrIndex = indexVector;
    while (index < denseLength) {
        if (*ptrDense != 0.0) {
            *ptrIndex = index;
            *ptrSparse = *ptrDense;
            ptrIndex++;
            ptrSparse++;
        }
        ptrDense++;
        index++;
    }
    return ptrIndex - indexVector;
}

//TODO: Parameterlista egyszerusitese a sima scatter alapjan, itt meg a gathernel
Numerical::Double * Vector::scatterWithPivot(Numerical::Double * & denseVector, unsigned int & denseLength,
                                             Numerical::Double * sparseVector, unsigned int * index,
                                             unsigned int sparseLength, unsigned int sparseMaxIndex, unsigned int pivot)
{
    // sparse -> dense
    Numerical::Double * res = 0;
    if (denseLength < sparseMaxIndex) {
        release(denseVector);
        //delete [] denseVector;
        denseVector = allocateData(sparseMaxIndex + 1);
        Numerical::Double * ptrDense = denseVector;
        const Numerical::Double * const ptrDenseEnd = denseVector + sparseMaxIndex + 1;
        while (ptrDense < ptrDenseEnd) {
            *ptrDense = 0.0;
            ptrDense++;
        }
        denseLength = sparseMaxIndex;
    }
    if (sparseVector == 0) {
        return 0;
    }
    Numerical::Double * ptrSparse = sparseVector;
    const unsigned int * ptrIndex = index;
    const Numerical::Double * const ptrSparseEnd = sparseVector + sparseLength;
    Numerical::Double * ptrDense = denseVector;

    while (ptrSparse < ptrSparseEnd) {
        if (*ptrIndex == pivot) {
            res = ptrSparse;
        }
        ptrDense[ *ptrIndex ] = *ptrSparse;
        ptrIndex++;
        ptrSparse++;
    }
    return res;
}

void Vector::scatter(Numerical::Double * & denseVector, unsigned int & denseLength,
                     const Vector& sparseVector)
{
    // sparse -> dense
    if (denseLength < sparseVector.m_dimension) {
        release(denseVector);
        //delete [] denseVector;
        denseVector = allocateData(sparseVector.m_dimension + 1);
        Numerical::Double * ptrDense = denseVector;
        const Numerical::Double * const ptrDenseEnd = denseVector + sparseVector.m_dimension + 1;
        while (ptrDense < ptrDenseEnd) {
            *ptrDense = 0.0;
            ptrDense++;
        }
        denseLength = sparseVector.m_dimension;
    }
    if (sparseVector.m_data == 0) {
        return;
    }
    const Numerical::Double * ptrSparse = sparseVector.m_data;
    const unsigned int * ptrIndex = sparseVector.m_index;
    const Numerical::Double * const ptrSparseEnd = sparseVector.m_data + sparseVector.m_size;
    Numerical::Double * ptrDense = denseVector;
    while (ptrSparse < ptrSparseEnd) {
        //LPINFO("*ptrIndex: " << *ptrIndex);
        ptrDense[ *ptrIndex ] = *ptrSparse;
        ptrIndex++;
        ptrSparse++;
    }

}

//void Vector::scatter(Numerical::Double * & denseVector, unsigned int & denseLength,
//    Numerical::Double * sparseVector, unsigned int * index,
//    unsigned int sparseLength, unsigned int sparseMaxIndex)
//{
//    // sparse -> dense
//    if (denseLength < sparseMaxIndex) {
//        delete [] denseVector;
//        denseVector = allocateData(sparseMaxIndex + 1);
//        Numerical::Double * ptrDense = denseVector;
//        const Numerical::Double * const ptrDenseEnd = denseVector + sparseMaxIndex + 1;
//        while (ptrDense < ptrDenseEnd) {
//            *ptrDense = 0.0;
//            ptrDense++;
//        }
//        denseLength = sparseMaxIndex;
//    }
//    if (sparseVector == 0) {
//        return;
//    }
//    const Numerical::Double * ptrSparse = sparseVector;
//    const unsigned int * ptrIndex = index;
//    const Numerical::Double * const ptrSparseEnd = sparseVector + sparseLength;
//    Numerical::Double * ptrDense = denseVector;
//    while (ptrSparse < ptrSparseEnd) {
//        //LPINFO("*ptrIndex: " << *ptrIndex);
//        ptrDense[ *ptrIndex ] = *ptrSparse;
//        ptrIndex++;
//        ptrSparse++;
//    }

//}

void Vector::clearFullLenghtVector(Numerical::Double * denseVector,
                                   unsigned int * sparseIndex, unsigned int sparseLength)
{
    const unsigned int * ptrIndex = sparseIndex;
    const unsigned int * const ptrIndexEnd = sparseIndex + sparseLength;
    while (ptrIndex < ptrIndexEnd) {
        denseVector[ *ptrIndex ] = 0.0;
        ptrIndex++;
    }
}

void Vector::denseToSparse()
{
    m_capacity = m_nonZeros + ELBOWROOM;
    m_index = allocateIndex(m_capacity);
    Numerical::Double * temp = allocateData(m_capacity);
    gather(m_data, temp, m_index, m_size);
    m_sorted = true;
    m_size = m_nonZeros;
    freeData(m_data);
    m_data = temp;
    m_dataEnd = m_data + m_size;
    m_vectorType = SPARSE_VECTOR;
    CHECK;
}

void Vector::sparseToDense()
{
    m_sorted = true;
    m_capacity = m_dimension + ELBOWROOM;
    Numerical::Double * temp = allocateData(m_capacity);
    Numerical::Double * ptr = temp;
    const Numerical::Double * const end = temp + m_dimension;
    while (ptr < end) {
        *ptr = 0.0;
        ptr++;
    }
    scatter(temp, m_dimension, *this);

    freeData(m_data);
    m_data = temp;
    freeIndex(m_index);
    m_size = m_dimension;
    m_dataEnd = m_data + m_size;
    m_vectorType = DENSE_VECTOR;
    CHECK;
}

void Vector::setSparsityRatio(Numerical::Double ratio)
{
    m_sparsityRatio = ratio;
    m_sparsityThreshold = (unsigned int) Numerical::round(m_dimension * m_sparsityRatio);
    if (m_vectorType == DENSE_VECTOR && m_nonZeros < m_sparsityThreshold) {
        denseToSparse();
    } else if (m_vectorType == SPARSE_VECTOR && m_nonZeros >= m_sparsityThreshold) {
        sparseToDense();
    }
    CHECK;
}

Numerical::Double Vector::getSparsityRatio() const
{
    CHECK;
    return m_sparsityRatio;
}

bool Vector::operator==(const Vector & vector) const
{
    if (m_nonZeros != vector.m_nonZeros) {
        return false;
    }
    Vector::NonzeroIterator iterator1 = vector.beginNonzero();
    Vector::NonzeroIterator iterator2 = beginNonzero();
    Vector::NonzeroIterator iteratorEnd1 = vector.endNonzero();
    for (; iterator1 < iteratorEnd1; iterator1++, iterator2++) {
        if (iterator1.getIndex() != iterator2.getIndex()) {
            return false;
        }
        if (*iterator1 != *iterator2) {
            return false;
        }
    }
    return true;
}

std::ostream & operator<<(std::ostream & os, const Vector & vector)
{
    unsigned int index;
#ifndef NODEBUG
    os << "data members:" << std::endl;
    os << "\tm_vectorType: " << (vector.m_vectorType == Vector::DENSE_VECTOR ?
                                     "DENSE_VECTOR" : "SPARSE_VECTOR") << std::endl;
    os << "\tm_size: " << vector.m_size << std::endl;
    os << "\tm_dimension: " << vector.m_dimension << std::endl;
    os << "\tm_capacity: " << vector.m_capacity << std::endl;
    os << "\tm_data: " << vector.m_data << std::endl;
    os << "\tm_dataEnd: " << vector.m_dataEnd << std::endl;
    os << "\tm_index: " << vector.m_index << std::endl;
    os << "\tm_nonZeros: " << vector.m_nonZeros << std::endl;
    os << "\tm_sparsityRatio: " << vector.m_sparsityRatio << std::endl;
    os << "\tm_sparsityThreshold: " << vector.m_sparsityThreshold << std::endl;
    os << "\tm_sorted: " << vector.m_sorted << std::endl;
    os << "m_data: " << std::endl;
    for (index = 0; index < vector.m_size; index++) {
        os << vector.m_data[index] << " ";
    }
    os << std::endl;
    if (vector.m_vectorType == Vector::SPARSE_VECTOR) {
        for (index = 0; vector.m_index && index < vector.m_size; index++) {
            os << "[ " << vector.m_index[index] << "; " << vector.m_data[index] << "] ";
        }
    }
    os << std::endl;

    //    if (vector.m_vectorType == Vector::DENSE_VECTOR) {
    //        for (index = 0; index < vector.m_size; index++) {
    //            os << vector.m_data[index] << " ";
    //        }
    //    } else {
    //    for (unsigned int index = 0; index < vector.m_dimension; index++) {
    //        os << "[ " << index << "; " <<  vector.at(index) << "] ";
    //    }
    os << std::endl;
    //    }

#endif
    return os;
}

Numerical::Double Vector::absMaxElement() const
{
    Numerical::Double result = 0;
    if (m_nonZeros > 0) {
        const Numerical::Double * dataPtr = m_data;
        for (; dataPtr < m_dataEnd; dataPtr++) {
            const Numerical::Double absValue = Numerical::fabs(*dataPtr);
            if (absValue > result) {
                result = absValue;
            }
        }
    }
    return result;
}

Numerical::Double Vector::scaleAndGetResults(const std::vector<Numerical::Double> & multipliers,
                                             Numerical::Double lambda,
                                             Numerical::Double * squareSumPtr,
                                             Numerical::Double * minPtr,
                                             Numerical::Double * maxPtr) {
    Numerical::Double absSum = 0;
    Numerical::Double squareSum = 0;
    Numerical::Double min = Numerical::Infinity;
    Numerical::Double max = 0;
    if (m_nonZeros > 0) {
        Numerical::Double * dataPtr = m_data;
        if ( m_index != 0 ) {
            const unsigned int * indexPtr = m_index;
            for (; dataPtr < m_dataEnd; dataPtr++, indexPtr++) {
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
        } else {
            unsigned int index = 0;

            for (; dataPtr < m_dataEnd; dataPtr++, index++) {
                if (*dataPtr == 0.0) {
                    continue;
                }
                *dataPtr *= lambda * multipliers[index];

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
    }
    *squareSumPtr = squareSum;
    *minPtr = min;
    *maxPtr = max;
    return absSum;

}

Numerical::Double Vector::absMaxSumsAndMinMax(Numerical::Double * squareSumPtr,
                                              Numerical::Double * minPtr,
                                              Numerical::Double * maxPtr) const {
    Numerical::Double absSum = 0;
    Numerical::Double squareSum = 0;
    Numerical::Double min = Numerical::Infinity;
    Numerical::Double max = 0;
    if (m_nonZeros > 0) {
        const Numerical::Double * dataPtr = m_data;
        if (m_index != 0) {
            for (; dataPtr < m_dataEnd; dataPtr++) {
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
        } else {
            for (; dataPtr < m_dataEnd; dataPtr++) {
                if (*dataPtr == 0.0) {
                    continue;
                }
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
    }
    *squareSumPtr = squareSum;
    *minPtr = min;
    *maxPtr = max;
    return absSum;
}

Numerical::Double Vector::absMaxSums(Numerical::Double * squareSumPtr) const {
    Numerical::Double absSum = 0;
    Numerical::Double squareSum = 0;
    if (m_nonZeros > 0) {
        const Numerical::Double * dataPtr = m_data;
        for (; dataPtr < m_dataEnd; dataPtr++) {
            const Numerical::Double absValue = Numerical::fabs(*dataPtr);
            absSum += absValue;
            squareSum += absValue * absValue;
        }
    }
    *squareSumPtr = squareSum;
    return absSum;
}

Vector operator*(Numerical::Double m, const Vector& v)
{
    Vector ret(v);
    ret.scaleBy(m);
    return ret;
}

void Vector::swap(Vector * vector1, Vector * vector2) {
    // new and delete are too slow
    THREAD_STATIC_DECL char buffer[sizeof(Vector)];
    panOptMemcpy(buffer, vector1, sizeof(Vector));
    panOptMemcpy(vector1, vector2, sizeof(Vector));
    panOptMemcpy(vector2, buffer, sizeof(Vector));
}
