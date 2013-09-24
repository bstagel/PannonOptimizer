#include <vectortest.h>
#include <testmacros.h>

VectorTestSuite::VectorTestSuite(const char * name) : UnitTest(name)
{
    //      ADD_TEST_EXTRA(VectorTestSuite::demo1, VectorTester);
    //      ADD_TEST_EXTRA(VectorTestSuite::demo2, VectorTester);
    ADD_TEST_EXTRA(VectorTestSuite::init, VectorTester);
    ADD_TEST_EXTRA(VectorTestSuite::fillAndClear, VectorTester);
    ADD_TEST_EXTRA(VectorTestSuite::set, VectorTester);
    ADD_TEST_EXTRA(VectorTestSuite::change, VectorTester);
    ADD_TEST_EXTRA(VectorTestSuite::setNewNonzero, VectorTester);
    ADD_TEST_EXTRA(VectorTestSuite::insertElement, VectorTester);
    ADD_TEST_EXTRA(VectorTestSuite::append, VectorTester);
    ADD_TEST_EXTRA(VectorTestSuite::removeElement, VectorTester);
    ADD_TEST_EXTRA(VectorTestSuite::resize, VectorTester);
    ADD_TEST_EXTRA(VectorTestSuite::setSparsityRatio, VectorTester);
    ADD_TEST_EXTRA(VectorTestSuite::maxIndex, VectorTester);
    ADD_TEST_EXTRA(VectorTestSuite::absMaxElement, VectorTester);
    ADD_TEST_EXTRA(VectorTestSuite::scaleBy, VectorTester);
    ADD_TEST_EXTRA(VectorTestSuite::scaleElementBy, VectorTester);
    ADD_TEST_EXTRA(VectorTestSuite::scaleByLambdas, VectorTester);
    ADD_TEST_EXTRA(VectorTestSuite::addVector, VectorTester);
    ADD_TEST_EXTRA(VectorTestSuite::dotProduct, VectorTester);
    ADD_TEST_EXTRA(VectorTestSuite::euclidNorm, VectorTester);
    ADD_TEST_EXTRA(VectorTestSuite::elementaryFtran, VectorTester);
    ADD_TEST_EXTRA(VectorTestSuite::elementaryBtran, VectorTester);
    ADD_TEST_EXTRA(VectorTestSuite::plus, VectorTester);
    ADD_TEST_EXTRA(VectorTestSuite::minus, VectorTester);
    ADD_TEST_EXTRA(VectorTestSuite::asterisk, VectorTester);
    ADD_TEST_EXTRA(VectorTestSuite::asterisk2, VectorTester);
    ADD_TEST_EXTRA(VectorTestSuite::equal, VectorTester);
    ADD_TEST_EXTRA(VectorTestSuite::sortElements, VectorTester);
    ADD_TEST_EXTRA(VectorTestSuite::reInit, VectorTester);
    ADD_TEST_EXTRA(VectorTestSuite::heap, VectorTester);
    ADD_TEST_EXTRA(VectorTestSuite::prepare, VectorTester);
    ADD_TEST_EXTRA(VectorTestSuite::iterators, VectorTester);
}

bool VectorTestSuite::integrityCheck(const Vector & vector)
{
    if (Vector::sm_fullLenghtReferenceCounter == 0 && Vector::sm_fullLengthVectorLenght != 0) {
        return false;
    }
    if (Vector::sm_fullLenghtReferenceCounter == 0 && Vector::sm_fullLengthVector != 0) {
        return false;
    }
    if (Vector::sm_countingSortBitVectorLength == 0 && Vector::sm_countingSortBitVector != 0) {
        return false;
    }

    Numerical::Double * dataPtr = Vector::sm_fullLengthVector;
    Numerical::Double * dataPtrEnd = dataPtr + Vector::sm_fullLengthVectorLenght;
    for (; dataPtr < dataPtrEnd; dataPtr++) {
        if (*dataPtr != 0.0) {
            return false;
        }
    }

    unsigned long * countingPtr = Vector::sm_countingSortBitVector;
    unsigned long * countingPtrEnd = countingPtr + Vector::sm_countingSortBitVectorLength;
    for (; countingPtr < countingPtrEnd; countingPtr++) {
        if (*countingPtr != 0) {
            return false;
        }
    }

    if (vector.m_capacity == 0 && (vector.m_data != 0 || vector.m_index != 0)) {
        return false;
    }

    if (vector.m_capacity < vector.m_size) {
        return false;
    }

    if (vector.m_data != vector.m_dataEnd - vector.m_size) {
        return false;
    }
    return true;
}

bool VectorTestSuite::sortedCheck(const Vector & vector, bool testedValue,
    bool whatever)
{
    if (vector.m_index == 0 && vector.m_vectorType == Vector::SPARSE_VECTOR && vector.m_size > 0) {
        return false;
    }
    if (vector.m_index != 0 && vector.m_vectorType == Vector::DENSE_VECTOR) {
        return false;
    }
    if (vector.m_sorted != testedValue && whatever == false) {
        return false;
    }

    if (testedValue == true && vector.m_sorted == true && vector.m_vectorType == Vector::SPARSE_VECTOR) {
        const unsigned int * indexPtr = vector.m_index;
        const unsigned int * nextPtr = vector.m_index + 1;
        if (indexPtr == 0 && vector.m_size > 0) {
            return false;
        }
        const unsigned int * endPtr = vector.m_index + vector.m_size;
        for (; nextPtr < endPtr; indexPtr++, nextPtr++) {
            if (*nextPtr <= *indexPtr) {
                return false;
            }
        }
    }

    return true;
}

void VectorTestSuite::demo1()
{
    if (SPARSITY_RATIO == 0.7) {
        TEST_ASSERT(1 == 2);
    }
    TEST_ASSERT(1 == 1);
}

void VectorTestSuite::demo2()
{
    TEST_ASSERT(1 == 1);
}

void VectorTestSuite::init()
{
    unsigned int testDimensions[] = {0, 1, 2, 3, 4, 5, 10, 20, 50, 100};
    const unsigned int testDimensionCount = sizeof (testDimensions) / sizeof (unsigned int);
    unsigned int dimensionIndex;

    for (dimensionIndex = 0; dimensionIndex < testDimensionCount; dimensionIndex++) {
        const unsigned int actualDimension = testDimensions[dimensionIndex];

        Vector myVect(actualDimension);
        TEST_ASSERT(integrityCheck(myVect));
        TEST_ASSERT(myVect.m_dimension == actualDimension);

        unsigned int testSparsityThreshold1 = (unsigned int) Numerical::round(
            myVect.m_dimension * SPARSITY_RATIO);
        TEST_ASSERT(myVect.m_sparsityThreshold == testSparsityThreshold1);
        TEST_ASSERT(myVect.m_nonZeros == 0);
        TEST_ASSERT(myVect.m_sparsityRatio == SPARSITY_RATIO);
        TEST_ASSERT(myVect.sm_fullLenghtReferenceCounter == 1);

        if (myVect.m_nonZeros < myVect.m_sparsityThreshold) {
            TEST_ASSERT(myVect.m_capacity == ELBOWROOM);
            TEST_ASSERT(myVect.m_data == myVect.m_dataEnd);
            TEST_ASSERT(sortedCheck(myVect, true));
            TEST_ASSERT(myVect.m_vectorType == Vector::SPARSE_VECTOR);
            TEST_ASSERT(myVect.m_size == 0);
        } else {
            TEST_ASSERT(myVect.m_capacity == myVect.m_dimension + ELBOWROOM);
            TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - myVect.m_dimension);
            TEST_ASSERT(sortedCheck(myVect, true));
            TEST_ASSERT(myVect.m_vectorType == Vector::DENSE_VECTOR);
            TEST_ASSERT(myVect.m_size == myVect.m_dimension);
        }

        unsigned int index;
        for (index = 0; index < actualDimension; index++) {
            TEST_ASSERT(myVect.at(index) == 0);
        }

        unsigned int myCounter = myVect.sm_fullLenghtReferenceCounter;
        myCounter++;
        Vector myVect2(myVect);
        TEST_ASSERT(integrityCheck(myVect2));
        TEST_ASSERT(myVect2.sm_fullLenghtReferenceCounter == myCounter);

        TEST_ASSERT(myVect2.m_dimension == actualDimension);

        unsigned int testSparsityThreshold2 = (unsigned int) Numerical::round(
            myVect2.m_dimension * SPARSITY_RATIO);
        TEST_ASSERT(myVect2.m_sparsityThreshold == testSparsityThreshold2);
        TEST_ASSERT(myVect2.m_nonZeros == 0);
        TEST_ASSERT(myVect2.m_sparsityRatio == SPARSITY_RATIO);

        if (myVect2.m_nonZeros < myVect2.m_sparsityThreshold) {
            TEST_ASSERT(myVect2.m_capacity == ELBOWROOM);
            TEST_ASSERT(myVect2.m_data == myVect2.m_dataEnd);
            TEST_ASSERT(sortedCheck(myVect2, true));
            TEST_ASSERT(myVect2.m_vectorType == Vector::SPARSE_VECTOR);
            TEST_ASSERT(myVect2.m_size == 0);
        } else {
            TEST_ASSERT(myVect2.m_capacity == myVect2.m_dimension + ELBOWROOM);
            TEST_ASSERT(myVect2.m_data == myVect2.m_dataEnd - myVect2.m_dimension);
            TEST_ASSERT(sortedCheck(myVect2, true));
            TEST_ASSERT(myVect2.m_vectorType == Vector::DENSE_VECTOR);
            TEST_ASSERT(myVect2.m_size == myVect2.m_dimension);
        }

        for (index = 0; index < actualDimension; index++) {
            TEST_ASSERT(myVect2.at(index) == 0);
        }
    }
}

void VectorTestSuite::fillAndClear()
{
    Vector myVect(5);

    myVect.fill(7);
    TEST_ASSERT(integrityCheck(myVect));
    unsigned int testSparsityThreshold1 = (unsigned int) Numerical::round(
        5 * SPARSITY_RATIO);
    TEST_ASSERT(myVect.m_dimension == 5);
    TEST_ASSERT(myVect.m_capacity == ELBOWROOM + 5);
    TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - 5);
    TEST_ASSERT(sortedCheck(myVect, true));
    TEST_ASSERT(myVect.m_vectorType == Vector::DENSE_VECTOR);
    TEST_ASSERT(myVect.m_size == 5);
    TEST_ASSERT(myVect.m_sparsityThreshold == testSparsityThreshold1);
    TEST_ASSERT(myVect.m_nonZeros == 5);
    TEST_ASSERT(myVect.m_sparsityRatio == SPARSITY_RATIO);

    TEST_ASSERT(myVect.at(0) == 7);
    TEST_ASSERT(myVect.at(1) == 7);
    TEST_ASSERT(myVect.at(2) == 7);
    TEST_ASSERT(myVect.at(3) == 7);
    TEST_ASSERT(myVect.at(4) == 7);

    unsigned int originalDenseCapacity = myVect.m_capacity;
    myVect.fill(0);
    TEST_ASSERT(integrityCheck(myVect));

    TEST_ASSERT(myVect.m_dimension == 5);
    TEST_ASSERT(myVect.m_sparsityThreshold == testSparsityThreshold1);
    TEST_ASSERT(myVect.m_nonZeros == 0);
    TEST_ASSERT(myVect.m_sparsityRatio == SPARSITY_RATIO);
    TEST_ASSERT(sortedCheck(myVect, true));
    if (myVect.m_nonZeros < myVect.m_sparsityThreshold) {
        TEST_ASSERT(myVect.m_capacity == ELBOWROOM + 5);
        TEST_ASSERT(myVect.m_data == myVect.m_dataEnd);
        TEST_ASSERT(myVect.m_vectorType == Vector::SPARSE_VECTOR);
        TEST_ASSERT(myVect.m_size == 0);
    } else {
        TEST_ASSERT(myVect.m_vectorType == Vector::DENSE_VECTOR);
        TEST_ASSERT(myVect.m_size == myVect.m_dimension);
        TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - myVect.m_size);
        TEST_ASSERT(myVect.m_capacity == originalDenseCapacity);
    }

    TEST_ASSERT(myVect.at(0) == 0);
    TEST_ASSERT(myVect.at(1) == 0);
    TEST_ASSERT(myVect.at(2) == 0);
    TEST_ASSERT(myVect.at(3) == 0);
    TEST_ASSERT(myVect.at(4) == 0);
}

void VectorTestSuite::set()
{
    Vector myVect(5);
    unsigned int testSparsityThreshold1 = (unsigned int) Numerical::round(
        5 * SPARSITY_RATIO);

    unsigned int testIndices[] = {2, 4, 0, 4, 2, 1, 3, 4,
        1, 1, 4, 0, 2, 1, 3, 2};
    Numerical::Double testValues[] = {1, 2, 3, 4, 5, 6, 7, 8,
        0, 0, 0, 0, 0, 0, 0, 0};
    unsigned int testNonzeros[] = {1, 2, 3, 3, 3, 4, 5, 5,
        4, 4, 3, 2, 1, 1, 0, 0};
    Numerical::Double testVectors[][5] = {
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 2},
        {3, 0, 1, 0, 2},
        {3, 0, 1, 0, 4},
        {3, 0, 5, 0, 4},
        {3, 6, 5, 0, 4},
        {3, 6, 5, 7, 4},
        {3, 6, 5, 7, 8},
        {3, 0, 5, 7, 8},
        {3, 0, 5, 7, 8},
        {3, 0, 5, 7, 0},
        {0, 0, 5, 7, 0},
        {0, 0, 0, 7, 0},
        {0, 0, 0, 7, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
    };

    const unsigned testCount = sizeof (testIndices) / sizeof (unsigned int);

    unsigned int index;
    unsigned int capacity = ELBOWROOM;
    Vector::VECTOR_TYPE previousType = myVect.m_vectorType;
    for (index = 0; index < testCount; index++) {
        myVect.set(testIndices[index], testValues[index]);
        TEST_ASSERT(integrityCheck(myVect));

        TEST_ASSERT(myVect.m_dimension == 5);
        TEST_ASSERT(myVect.m_sparsityThreshold == testSparsityThreshold1);
        TEST_ASSERT(myVect.m_nonZeros == testNonzeros[index]);
        TEST_ASSERT(myVect.m_sparsityRatio == SPARSITY_RATIO);
        TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - myVect.m_size);
        if (myVect.m_nonZeros < myVect.m_sparsityThreshold) {
            if (myVect.m_size > capacity) {
                capacity = myVect.m_size + ELBOWROOM;
            }
            TEST_ASSERT(myVect.m_vectorType == Vector::SPARSE_VECTOR);
            if (previousType == Vector::DENSE_VECTOR) {
                capacity = myVect.m_size + ELBOWROOM;
                TEST_ASSERT(sortedCheck(myVect, true));
            } else {
                TEST_ASSERT(sortedCheck(myVect, false));
            }
            TEST_ASSERT(myVect.m_size == testNonzeros[index]);
            TEST_ASSERT(myVect.m_capacity == capacity);
        } else {
            TEST_ASSERT(myVect.m_vectorType == Vector::DENSE_VECTOR);
            TEST_ASSERT(sortedCheck(myVect, true));
            TEST_ASSERT(myVect.m_capacity == myVect.m_size + ELBOWROOM);
            TEST_ASSERT(myVect.m_size == myVect.m_dimension);
            capacity = myVect.m_capacity;
        }
        previousType = myVect.m_vectorType;

        TEST_ASSERT(myVect.at(0) == testVectors[index][0]);
        TEST_ASSERT(myVect.at(1) == testVectors[index][1]);
        TEST_ASSERT(myVect.at(2) == testVectors[index][2]);
        TEST_ASSERT(myVect.at(3) == testVectors[index][3]);
        TEST_ASSERT(myVect.at(4) == testVectors[index][4]);
    }

}

void VectorTestSuite::change()
{
    Vector myVect(5);

    myVect.set(0, 3);
    myVect.set(1, 6);
    myVect.set(2, 5);
    myVect.set(3, 7);
    myVect.set(4, 8);

    unsigned int testSparsityThreshold1 = (unsigned int) Numerical::round(
        5 * SPARSITY_RATIO);

    unsigned int testIndices[] = {1, 4, 0,
        1, 4, 0, 2, 3};
    Numerical::Double testValues[] = {1, 2, 4,
        0, 0, 0, 0, 0};
    unsigned int testNonzeros[] = {5, 5, 5,
        4, 3, 2, 1, 0};
    Numerical::Double testVectors[][5] = {
        {3, 1, 5, 7, 8},
        {3, 1, 5, 7, 2},
        {4, 1, 5, 7, 2},
        {4, 0, 5, 7, 2},
        {4, 0, 5, 7, 0},
        {0, 0, 5, 7, 0},
        {0, 0, 0, 7, 0},
        {0, 0, 0, 0, 0},
    };

    const unsigned testCount = sizeof (testIndices) / sizeof (unsigned int);

    unsigned int index;
    unsigned int capacity = ELBOWROOM;
    Vector::VECTOR_TYPE previousType = myVect.m_vectorType;
    for (index = 0; index < testCount; index++) {
        myVect.change(testIndices[index], testValues[index]);
        TEST_ASSERT(integrityCheck(myVect));

        TEST_ASSERT(myVect.m_dimension == 5);
        TEST_ASSERT(myVect.m_sparsityThreshold == testSparsityThreshold1);
        TEST_ASSERT(myVect.m_nonZeros == testNonzeros[index]);
        TEST_ASSERT(myVect.m_sparsityRatio == SPARSITY_RATIO);
        TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - myVect.m_size);
        if (myVect.m_nonZeros < myVect.m_sparsityThreshold) {
            if (myVect.m_size > capacity) {
                capacity = myVect.m_size + ELBOWROOM;
            }
            TEST_ASSERT(myVect.m_vectorType == Vector::SPARSE_VECTOR);
            if (previousType == Vector::DENSE_VECTOR) {
                capacity = myVect.m_size + ELBOWROOM;
                TEST_ASSERT(sortedCheck(myVect, true));
            } else {
                TEST_ASSERT(sortedCheck(myVect, false));
            }
            TEST_ASSERT(myVect.m_size == testNonzeros[index]);
            TEST_ASSERT(myVect.m_capacity == capacity);
        } else {
            TEST_ASSERT(myVect.m_vectorType == Vector::DENSE_VECTOR);
            TEST_ASSERT(sortedCheck(myVect, true));
            TEST_ASSERT(myVect.m_capacity == myVect.m_size + ELBOWROOM);
            TEST_ASSERT(myVect.m_size == myVect.m_dimension);
            capacity = myVect.m_capacity;
        }
        previousType = myVect.m_vectorType;
        TEST_ASSERT(myVect.at(0) == testVectors[index][0]);
        TEST_ASSERT(myVect.at(1) == testVectors[index][1]);
        TEST_ASSERT(myVect.at(2) == testVectors[index][2]);
        TEST_ASSERT(myVect.at(3) == testVectors[index][3]);
        TEST_ASSERT(myVect.at(4) == testVectors[index][4]);
    }
}

void VectorTestSuite::setNewNonzero()
{
    Vector myVect(5);

    unsigned int testSparsityThreshold1 = (unsigned int) Numerical::round(
        5 * SPARSITY_RATIO);

    unsigned int testIndices[] = {2, 4, 0, 1, 3};
    Numerical::Double testValues[] = {1, 2, 3, 6, 7};
    unsigned int testNonzeros[] = {1, 2, 3, 4, 5};

    Numerical::Double testVectors[][5] = {
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 2},
        {3, 0, 1, 0, 2},
        {3, 6, 1, 0, 2},
        {3, 6, 1, 7, 2},
    };

    const unsigned testCount = sizeof (testIndices) / sizeof (unsigned int);

    unsigned int index;
    unsigned int capacity = ELBOWROOM;
    Vector::VECTOR_TYPE previousType = myVect.m_vectorType;
    for (index = 0; index < testCount; index++) {
        myVect.setNewNonzero(testIndices[index], testValues[index]);
        TEST_ASSERT(integrityCheck(myVect));

        TEST_ASSERT(myVect.m_dimension == 5);
        TEST_ASSERT(myVect.m_sparsityThreshold == testSparsityThreshold1);
        TEST_ASSERT(myVect.m_nonZeros == testNonzeros[index]);
        TEST_ASSERT(myVect.m_sparsityRatio == SPARSITY_RATIO);
        TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - myVect.m_size);
        if (myVect.m_nonZeros < myVect.m_sparsityThreshold) {
            if (myVect.m_size > capacity) {
                capacity = myVect.m_size + ELBOWROOM;
            }
            TEST_ASSERT(myVect.m_vectorType == Vector::SPARSE_VECTOR);
            if (previousType == Vector::DENSE_VECTOR) {
                capacity = myVect.m_size + ELBOWROOM;
                TEST_ASSERT(sortedCheck(myVect, true));
            } else {
                TEST_ASSERT(sortedCheck(myVect, false));
            }
            TEST_ASSERT(myVect.m_size == testNonzeros[index]);
            TEST_ASSERT(myVect.m_capacity == capacity);
        } else {
            TEST_ASSERT(myVect.m_vectorType == Vector::DENSE_VECTOR);
            TEST_ASSERT(sortedCheck(myVect, true));
            TEST_ASSERT(myVect.m_capacity == myVect.m_size + ELBOWROOM);
            TEST_ASSERT(myVect.m_size == myVect.m_dimension);
            capacity = myVect.m_capacity;
        }
        previousType = myVect.m_vectorType;

        TEST_ASSERT(myVect.at(0) == testVectors[index][0]);
        TEST_ASSERT(myVect.at(1) == testVectors[index][1]);
        TEST_ASSERT(myVect.at(2) == testVectors[index][2]);
        TEST_ASSERT(myVect.at(3) == testVectors[index][3]);
        TEST_ASSERT(myVect.at(4) == testVectors[index][4]);
    }
}

void VectorTestSuite::insertElement()
{
    Vector myVect1(5);

    myVect1.set(1, 4);

    const unsigned int insertIndices1[] = {1, 1, 4, 7, 3, 0, 3, 2};
    const Numerical::Double insertValues1[] = {4, 9, 5, 1, 8, 10, 11, 12};
    const unsigned int insertCount1 = sizeof (insertIndices1) / sizeof (unsigned int);

    unsigned int index;
    unsigned int capacity = ELBOWROOM;
    if (myVect1.m_vectorType == Vector::DENSE_VECTOR) {
        capacity = myVect1.m_size + ELBOWROOM;
    }
    Vector::VECTOR_TYPE type = myVect1.m_vectorType;
    for (index = 0; index < insertCount1; index++) {

        if (myVect1.m_capacity <= myVect1.m_size) {
            capacity = myVect1.m_size + 1 + ELBOWROOM;
        }

        myVect1.insertElement(insertIndices1[index], insertValues1[index]);
        TEST_ASSERT(integrityCheck(myVect1));

        const unsigned int testSparsityThreshold = (unsigned int) Numerical::round(
            (5 + index + 1) * SPARSITY_RATIO);
        TEST_ASSERT(myVect1.m_nonZeros == index + 2);
        TEST_ASSERT(myVect1.m_sparsityThreshold == testSparsityThreshold);
        TEST_ASSERT(myVect1.m_sparsityRatio == SPARSITY_RATIO);
        TEST_ASSERT(myVect1.m_dimension == 5 + index + 1);
        if (myVect1.m_nonZeros < myVect1.m_sparsityThreshold) {
            // SPARSE
            TEST_ASSERT(myVect1.m_vectorType == Vector::SPARSE_VECTOR);
            TEST_ASSERT(myVect1.m_size == myVect1.m_nonZeros);
            TEST_ASSERT(myVect1.m_data == myVect1.m_dataEnd - myVect1.m_nonZeros);
            TEST_ASSERT(sortedCheck(myVect1, false));
        } else {
            // DENSE
            if (type == Vector::SPARSE_VECTOR) {
                // type had changed
                capacity = myVect1.m_size + ELBOWROOM;
            }
            TEST_ASSERT(myVect1.m_vectorType == Vector::DENSE_VECTOR);
            TEST_ASSERT(myVect1.m_size == 5 + index + 1);
            TEST_ASSERT(myVect1.m_data == myVect1.m_dataEnd - 5 - index - 1);
            TEST_ASSERT(sortedCheck(myVect1, true));
        }
        TEST_ASSERT(myVect1.m_capacity == capacity);
        type = myVect1.m_vectorType;
        switch (index) {
            case 0:
                TEST_ASSERT(myVect1.at(0) == 0);
                TEST_ASSERT(myVect1.at(1) == 4);
                TEST_ASSERT(myVect1.at(2) == 4);
                TEST_ASSERT(myVect1.at(3) == 0);
                TEST_ASSERT(myVect1.at(4) == 0);
                TEST_ASSERT(myVect1.at(5) == 0);
                break;
            case 1:
                TEST_ASSERT(myVect1.at(0) == 0);
                TEST_ASSERT(myVect1.at(1) == 9);
                TEST_ASSERT(myVect1.at(2) == 4);
                TEST_ASSERT(myVect1.at(3) == 4);
                TEST_ASSERT(myVect1.at(4) == 0);
                TEST_ASSERT(myVect1.at(5) == 0);
                TEST_ASSERT(myVect1.at(6) == 0);
                break;
            case 2:
                TEST_ASSERT(myVect1.at(0) == 0);
                TEST_ASSERT(myVect1.at(1) == 9);
                TEST_ASSERT(myVect1.at(2) == 4);
                TEST_ASSERT(myVect1.at(3) == 4);
                TEST_ASSERT(myVect1.at(4) == 5);
                TEST_ASSERT(myVect1.at(5) == 0);
                TEST_ASSERT(myVect1.at(6) == 0);
                TEST_ASSERT(myVect1.at(7) == 0);
                break;
            case 3:
                TEST_ASSERT(myVect1.at(0) == 0);
                TEST_ASSERT(myVect1.at(1) == 9);
                TEST_ASSERT(myVect1.at(2) == 4);
                TEST_ASSERT(myVect1.at(3) == 4);
                TEST_ASSERT(myVect1.at(4) == 5);
                TEST_ASSERT(myVect1.at(5) == 0);
                TEST_ASSERT(myVect1.at(6) == 0);
                TEST_ASSERT(myVect1.at(7) == 1);
                TEST_ASSERT(myVect1.at(8) == 0);
                break;
            case 4:
                TEST_ASSERT(myVect1.at(0) == 0);
                TEST_ASSERT(myVect1.at(1) == 9);
                TEST_ASSERT(myVect1.at(2) == 4);
                TEST_ASSERT(myVect1.at(3) == 8);
                TEST_ASSERT(myVect1.at(4) == 4);
                TEST_ASSERT(myVect1.at(5) == 5);
                TEST_ASSERT(myVect1.at(6) == 0);
                TEST_ASSERT(myVect1.at(7) == 0);
                TEST_ASSERT(myVect1.at(8) == 1);
                TEST_ASSERT(myVect1.at(9) == 0);
                break;
            case 5:
                TEST_ASSERT(myVect1.at(0) == 10);
                TEST_ASSERT(myVect1.at(1) == 0);
                TEST_ASSERT(myVect1.at(2) == 9);
                TEST_ASSERT(myVect1.at(3) == 4);
                TEST_ASSERT(myVect1.at(4) == 8);
                TEST_ASSERT(myVect1.at(5) == 4);
                TEST_ASSERT(myVect1.at(6) == 5);
                TEST_ASSERT(myVect1.at(7) == 0);
                TEST_ASSERT(myVect1.at(8) == 0);
                TEST_ASSERT(myVect1.at(9) == 1);
                TEST_ASSERT(myVect1.at(10) == 0);
                break;
            case 6:
                TEST_ASSERT(myVect1.at(0) == 10);
                TEST_ASSERT(myVect1.at(1) == 0);
                TEST_ASSERT(myVect1.at(2) == 9);
                TEST_ASSERT(myVect1.at(3) == 11);
                TEST_ASSERT(myVect1.at(4) == 4);
                TEST_ASSERT(myVect1.at(5) == 8);
                TEST_ASSERT(myVect1.at(6) == 4);
                TEST_ASSERT(myVect1.at(7) == 5);
                TEST_ASSERT(myVect1.at(8) == 0);
                TEST_ASSERT(myVect1.at(9) == 0);
                TEST_ASSERT(myVect1.at(10) == 1);
                TEST_ASSERT(myVect1.at(11) == 0);
                break;
            case 7:
                TEST_ASSERT(myVect1.at(0) == 10);
                TEST_ASSERT(myVect1.at(1) == 0);
                TEST_ASSERT(myVect1.at(2) == 12);
                TEST_ASSERT(myVect1.at(3) == 9);
                TEST_ASSERT(myVect1.at(4) == 11);
                TEST_ASSERT(myVect1.at(5) == 4);
                TEST_ASSERT(myVect1.at(6) == 8);
                TEST_ASSERT(myVect1.at(7) == 4);
                TEST_ASSERT(myVect1.at(8) == 5);
                TEST_ASSERT(myVect1.at(9) == 0);
                TEST_ASSERT(myVect1.at(10) == 0);
                TEST_ASSERT(myVect1.at(11) == 1);
                TEST_ASSERT(myVect1.at(12) == 0);
                break;
        }

    }

    Vector myVect2(2);

    myVect2.set(0, 1);
    myVect2.set(1, 2);

    const unsigned int insertIndices2[] = {0, 1, 2, 2, 4, 3, 0};
    const unsigned int insertCount2 = sizeof (insertIndices2) / sizeof (unsigned int);
    type = myVect2.m_vectorType;
    capacity = 2 + ELBOWROOM;
    for (index = 0; index < insertCount2; index++) {
        myVect2.insertElement(insertIndices2[index], 0);
        TEST_ASSERT(integrityCheck(myVect2));

        const unsigned int testSparsityThreshold = (unsigned int) Numerical::round(
            (2 + index + 1) * SPARSITY_RATIO);
        TEST_ASSERT(myVect2.m_nonZeros == 2);
        TEST_ASSERT(myVect2.m_sparsityThreshold == testSparsityThreshold);
        TEST_ASSERT(myVect2.m_sparsityRatio == SPARSITY_RATIO);
        TEST_ASSERT(myVect2.m_dimension == 2 + index + 1);
        if (myVect2.m_nonZeros < myVect2.m_sparsityThreshold) {
            TEST_ASSERT(myVect2.m_vectorType == Vector::SPARSE_VECTOR);
            TEST_ASSERT(myVect2.m_size == 2);
            TEST_ASSERT(myVect2.m_data == myVect2.m_dataEnd - 2);
            TEST_ASSERT(myVect2.m_capacity == 2 + ELBOWROOM);
            if (type == Vector::DENSE_VECTOR) {
                TEST_ASSERT(sortedCheck(myVect2, true));
            } else {
                TEST_ASSERT(sortedCheck(myVect2, false));
            }
        } else {
            if (capacity < myVect2.m_size) {
                capacity = myVect2.m_size + ELBOWROOM;
            }
            TEST_ASSERT(myVect2.m_vectorType == Vector::DENSE_VECTOR);
            TEST_ASSERT(myVect2.m_size == 2 + index + 1);
            TEST_ASSERT(myVect2.m_data == myVect2.m_dataEnd - 2 - index - 1);
            TEST_ASSERT(sortedCheck(myVect2, true));
            TEST_ASSERT(myVect2.m_capacity == capacity);
        }
        type = myVect2.m_vectorType;

        switch (index) {
            case 0:
                TEST_ASSERT(myVect2.at(0) == 0);
                TEST_ASSERT(myVect2.at(1) == 1);
                TEST_ASSERT(myVect2.at(2) == 2);
                break;
            case 1:
                TEST_ASSERT(myVect2.at(0) == 0);
                TEST_ASSERT(myVect2.at(1) == 0);
                TEST_ASSERT(myVect2.at(2) == 1);
                TEST_ASSERT(myVect2.at(3) == 2);
                break;
            case 2:
                TEST_ASSERT(myVect2.at(0) == 0);
                TEST_ASSERT(myVect2.at(1) == 0);
                TEST_ASSERT(myVect2.at(2) == 0);
                TEST_ASSERT(myVect2.at(3) == 1);
                TEST_ASSERT(myVect2.at(4) == 2);
                break;
            case 3:
                TEST_ASSERT(myVect2.at(0) == 0);
                TEST_ASSERT(myVect2.at(1) == 0);
                TEST_ASSERT(myVect2.at(2) == 0);
                TEST_ASSERT(myVect2.at(3) == 0);
                TEST_ASSERT(myVect2.at(4) == 1);
                TEST_ASSERT(myVect2.at(5) == 2);
                break;
            case 4:
                TEST_ASSERT(myVect2.at(0) == 0);
                TEST_ASSERT(myVect2.at(1) == 0);
                TEST_ASSERT(myVect2.at(2) == 0);
                TEST_ASSERT(myVect2.at(3) == 0);
                TEST_ASSERT(myVect2.at(4) == 0);
                TEST_ASSERT(myVect2.at(5) == 1);
                TEST_ASSERT(myVect2.at(6) == 2);
                break;
            case 5:
                TEST_ASSERT(myVect2.at(0) == 0);
                TEST_ASSERT(myVect2.at(1) == 0);
                TEST_ASSERT(myVect2.at(2) == 0);
                TEST_ASSERT(myVect2.at(3) == 0);
                TEST_ASSERT(myVect2.at(4) == 0);
                TEST_ASSERT(myVect2.at(5) == 0);
                TEST_ASSERT(myVect2.at(6) == 1);
                TEST_ASSERT(myVect2.at(7) == 2);
                break;
            case 6:
                TEST_ASSERT(myVect2.at(0) == 0);
                TEST_ASSERT(myVect2.at(1) == 0);
                TEST_ASSERT(myVect2.at(2) == 0);
                TEST_ASSERT(myVect2.at(3) == 0);
                TEST_ASSERT(myVect2.at(4) == 0);
                TEST_ASSERT(myVect2.at(5) == 0);
                TEST_ASSERT(myVect2.at(6) == 0);
                TEST_ASSERT(myVect2.at(7) == 1);
                TEST_ASSERT(myVect2.at(8) == 2);
                break;
        }
    }
}

void VectorTestSuite::append()
{
    Vector myVect1(5);

    myVect1.set(1, 4);

    const Numerical::Double insertValues1[] = {4, 9, 5, 1, 8, 10, 11, 12};
    const unsigned int insertCount1 = sizeof (insertValues1) / sizeof (Numerical::Double);

    unsigned int index;
    unsigned int capacity = ELBOWROOM;
    if (myVect1.m_vectorType == Vector::DENSE_VECTOR) {
        capacity = myVect1.m_size + ELBOWROOM;
    }
    Vector::VECTOR_TYPE type = myVect1.m_vectorType;
    for (index = 0; index < insertCount1; index++) {
        if (myVect1.m_capacity <= myVect1.m_size) {
            capacity = myVect1.m_size + 1 + ELBOWROOM;
        }

        myVect1.append(insertValues1[index]);
        TEST_ASSERT(integrityCheck(myVect1));

        const unsigned int testSparsityThreshold = (unsigned int) Numerical::round(
            (5 + index + 1) * SPARSITY_RATIO);
        TEST_ASSERT(myVect1.m_nonZeros == index + 2);
        TEST_ASSERT(myVect1.m_sparsityThreshold == testSparsityThreshold);
        TEST_ASSERT(myVect1.m_sparsityRatio == SPARSITY_RATIO);
        TEST_ASSERT(myVect1.m_dimension == 5 + index + 1);
        if (myVect1.m_nonZeros < myVect1.m_sparsityThreshold) {
            // SPARSE
            TEST_ASSERT(myVect1.m_vectorType == Vector::SPARSE_VECTOR);
            TEST_ASSERT(myVect1.m_size == myVect1.m_nonZeros);
            TEST_ASSERT(myVect1.m_data == myVect1.m_dataEnd - myVect1.m_nonZeros);
            TEST_ASSERT(sortedCheck(myVect1, false));
        } else {
            // DENSE
            if (type == Vector::SPARSE_VECTOR) {
                // type had changed
                capacity = myVect1.m_size + ELBOWROOM;
            }
            TEST_ASSERT(myVect1.m_vectorType == Vector::DENSE_VECTOR);
            TEST_ASSERT(myVect1.m_size == 5 + index + 1);
            TEST_ASSERT(myVect1.m_data == myVect1.m_dataEnd - 5 - index - 1);
            TEST_ASSERT(sortedCheck(myVect1, true));
        }
        TEST_ASSERT(myVect1.m_capacity == capacity);
        type = myVect1.m_vectorType;
        switch (index) {
            case 0:
                TEST_ASSERT(myVect1.at(0) == 0);
                TEST_ASSERT(myVect1.at(1) == 4);
                TEST_ASSERT(myVect1.at(2) == 0);
                TEST_ASSERT(myVect1.at(3) == 0);
                TEST_ASSERT(myVect1.at(4) == 0);
                TEST_ASSERT(myVect1.at(5) == 4);
                break;
            case 1:
                TEST_ASSERT(myVect1.at(0) == 0);
                TEST_ASSERT(myVect1.at(1) == 4);
                TEST_ASSERT(myVect1.at(2) == 0);
                TEST_ASSERT(myVect1.at(3) == 0);
                TEST_ASSERT(myVect1.at(4) == 0);
                TEST_ASSERT(myVect1.at(5) == 4);
                TEST_ASSERT(myVect1.at(6) == 9);
                break;
            case 2:
                TEST_ASSERT(myVect1.at(0) == 0);
                TEST_ASSERT(myVect1.at(1) == 4);
                TEST_ASSERT(myVect1.at(2) == 0);
                TEST_ASSERT(myVect1.at(3) == 0);
                TEST_ASSERT(myVect1.at(4) == 0);
                TEST_ASSERT(myVect1.at(5) == 4);
                TEST_ASSERT(myVect1.at(6) == 9);
                TEST_ASSERT(myVect1.at(7) == 5);
                break;
            case 3:
                TEST_ASSERT(myVect1.at(0) == 0);
                TEST_ASSERT(myVect1.at(1) == 4);
                TEST_ASSERT(myVect1.at(2) == 0);
                TEST_ASSERT(myVect1.at(3) == 0);
                TEST_ASSERT(myVect1.at(4) == 0);
                TEST_ASSERT(myVect1.at(5) == 4);
                TEST_ASSERT(myVect1.at(6) == 9);
                TEST_ASSERT(myVect1.at(7) == 5);
                TEST_ASSERT(myVect1.at(8) == 1);
                break;
            case 4:
                TEST_ASSERT(myVect1.at(0) == 0);
                TEST_ASSERT(myVect1.at(1) == 4);
                TEST_ASSERT(myVect1.at(2) == 0);
                TEST_ASSERT(myVect1.at(3) == 0);
                TEST_ASSERT(myVect1.at(4) == 0);
                TEST_ASSERT(myVect1.at(5) == 4);
                TEST_ASSERT(myVect1.at(6) == 9);
                TEST_ASSERT(myVect1.at(7) == 5);
                TEST_ASSERT(myVect1.at(8) == 1);
                TEST_ASSERT(myVect1.at(9) == 8);
                break;
            case 5:
                TEST_ASSERT(myVect1.at(0) == 0);
                TEST_ASSERT(myVect1.at(1) == 4);
                TEST_ASSERT(myVect1.at(2) == 0);
                TEST_ASSERT(myVect1.at(3) == 0);
                TEST_ASSERT(myVect1.at(4) == 0);
                TEST_ASSERT(myVect1.at(5) == 4);
                TEST_ASSERT(myVect1.at(6) == 9);
                TEST_ASSERT(myVect1.at(7) == 5);
                TEST_ASSERT(myVect1.at(8) == 1);
                TEST_ASSERT(myVect1.at(9) == 8);
                TEST_ASSERT(myVect1.at(10) == 10);
                break;
            case 6:
                TEST_ASSERT(myVect1.at(0) == 0);
                TEST_ASSERT(myVect1.at(1) == 4);
                TEST_ASSERT(myVect1.at(2) == 0);
                TEST_ASSERT(myVect1.at(3) == 0);
                TEST_ASSERT(myVect1.at(4) == 0);
                TEST_ASSERT(myVect1.at(5) == 4);
                TEST_ASSERT(myVect1.at(6) == 9);
                TEST_ASSERT(myVect1.at(7) == 5);
                TEST_ASSERT(myVect1.at(8) == 1);
                TEST_ASSERT(myVect1.at(9) == 8);
                TEST_ASSERT(myVect1.at(10) == 10);
                TEST_ASSERT(myVect1.at(11) == 11);
                break;
            case 7:
                TEST_ASSERT(myVect1.at(0) == 0);
                TEST_ASSERT(myVect1.at(1) == 4);
                TEST_ASSERT(myVect1.at(2) == 0);
                TEST_ASSERT(myVect1.at(3) == 0);
                TEST_ASSERT(myVect1.at(4) == 0);
                TEST_ASSERT(myVect1.at(5) == 4);
                TEST_ASSERT(myVect1.at(6) == 9);
                TEST_ASSERT(myVect1.at(7) == 5);
                TEST_ASSERT(myVect1.at(8) == 1);
                TEST_ASSERT(myVect1.at(9) == 8);
                TEST_ASSERT(myVect1.at(10) == 10);
                TEST_ASSERT(myVect1.at(11) == 11);
                TEST_ASSERT(myVect1.at(12) == 12);
                break;
        }

    }

    Vector myVect2(2);

    myVect2.set(0, 1);
    myVect2.set(1, 2);

    const unsigned int insertCount2 = 7;
    type = myVect2.m_vectorType;
    capacity = 2 + ELBOWROOM;
    for (index = 0; index < insertCount2; index++) {
        myVect2.append(0);
        TEST_ASSERT(integrityCheck(myVect2));

        const unsigned int testSparsityThreshold = (unsigned int) Numerical::round(
            (2 + index + 1) * SPARSITY_RATIO);
        TEST_ASSERT(myVect2.m_nonZeros == 2);
        TEST_ASSERT(myVect2.m_sparsityThreshold == testSparsityThreshold);
        TEST_ASSERT(myVect2.m_sparsityRatio == SPARSITY_RATIO);
        TEST_ASSERT(myVect2.m_dimension == 2 + index + 1);
        if (myVect2.m_nonZeros < myVect2.m_sparsityThreshold) {
            TEST_ASSERT(myVect2.m_vectorType == Vector::SPARSE_VECTOR);
            TEST_ASSERT(myVect2.m_size == 2);
            TEST_ASSERT(myVect2.m_data == myVect2.m_dataEnd - 2);
            TEST_ASSERT(myVect2.m_capacity == 2 + ELBOWROOM);
            if (type == Vector::DENSE_VECTOR) {
                TEST_ASSERT(sortedCheck(myVect2, true));
            } else {
                TEST_ASSERT(sortedCheck(myVect2, false));
            }
        } else {
            if (capacity < myVect2.m_size) {
                capacity = myVect2.m_size + ELBOWROOM;
            }
            TEST_ASSERT(myVect2.m_vectorType == Vector::DENSE_VECTOR);
            TEST_ASSERT(myVect2.m_size == 2 + index + 1);
            TEST_ASSERT(myVect2.m_data == myVect2.m_dataEnd - 2 - index - 1);
            TEST_ASSERT(sortedCheck(myVect2, true));
            TEST_ASSERT(myVect2.m_capacity == capacity);
        }
        type = myVect2.m_vectorType;

        switch (index) {
            case 0:
                TEST_ASSERT(myVect2.at(0) == 1);
                TEST_ASSERT(myVect2.at(1) == 2);
                TEST_ASSERT(myVect2.at(2) == 0);
                break;
            case 1:
                TEST_ASSERT(myVect2.at(0) == 1);
                TEST_ASSERT(myVect2.at(1) == 2);
                TEST_ASSERT(myVect2.at(2) == 0);
                TEST_ASSERT(myVect2.at(3) == 0);
                break;
            case 2:
                TEST_ASSERT(myVect2.at(0) == 1);
                TEST_ASSERT(myVect2.at(1) == 2);
                TEST_ASSERT(myVect2.at(2) == 0);
                TEST_ASSERT(myVect2.at(3) == 0);
                TEST_ASSERT(myVect2.at(4) == 0);
                break;
            case 3:
                TEST_ASSERT(myVect2.at(0) == 1);
                TEST_ASSERT(myVect2.at(1) == 2);
                TEST_ASSERT(myVect2.at(2) == 0);
                TEST_ASSERT(myVect2.at(3) == 0);
                TEST_ASSERT(myVect2.at(4) == 0);
                TEST_ASSERT(myVect2.at(5) == 0);
                break;
            case 4:
                TEST_ASSERT(myVect2.at(0) == 1);
                TEST_ASSERT(myVect2.at(1) == 2);
                TEST_ASSERT(myVect2.at(2) == 0);
                TEST_ASSERT(myVect2.at(3) == 0);
                TEST_ASSERT(myVect2.at(4) == 0);
                TEST_ASSERT(myVect2.at(5) == 0);
                TEST_ASSERT(myVect2.at(6) == 0);
                break;
            case 5:
                TEST_ASSERT(myVect2.at(0) == 1);
                TEST_ASSERT(myVect2.at(1) == 2);
                TEST_ASSERT(myVect2.at(2) == 0);
                TEST_ASSERT(myVect2.at(3) == 0);
                TEST_ASSERT(myVect2.at(4) == 0);
                TEST_ASSERT(myVect2.at(5) == 0);
                TEST_ASSERT(myVect2.at(6) == 0);
                TEST_ASSERT(myVect2.at(7) == 0);
                break;
            case 6:
                TEST_ASSERT(myVect2.at(0) == 1);
                TEST_ASSERT(myVect2.at(1) == 2);
                TEST_ASSERT(myVect2.at(2) == 0);
                TEST_ASSERT(myVect2.at(3) == 0);
                TEST_ASSERT(myVect2.at(4) == 0);
                TEST_ASSERT(myVect2.at(5) == 0);
                TEST_ASSERT(myVect2.at(6) == 0);
                TEST_ASSERT(myVect2.at(7) == 0);
                TEST_ASSERT(myVect2.at(8) == 0);
                break;
        }
    }
}

void VectorTestSuite::removeElement()
{
    Vector myVect1(14);

    myVect1.set(2, 9);
    myVect1.set(5, 4);
    myVect1.set(6, 5);
    myVect1.set(11, 1);

    //Vector: [0, 0, 9, 0, 0, 4, 5, 0, 0, 0, 0, 1, 0, 0]
    const Numerical::Double testValues1[14][13] = {
        {0, 0, 9, 0, 0, 4, 5, 0, 0, 0, 0, 1, 0},
        {0, 9, 0, 0, 4, 5, 0, 0, 0, 0, 1, 0},
        {0, 9, 0, 4, 5, 0, 0, 0, 0, 1, 0},
        {0, 9, 0, 4, 5, 0, 0, 0, 0, 1},
        {9, 0, 4, 5, 0, 0, 0, 0, 1},
        {9, 0, 4, 5, 0, 0, 0, 1},
        {9, 0, 4, 5, 0, 0, 1},
        {9, 0, 4, 5, 0, 1},
        {9, 4, 5, 0, 1},
        {9, 4, 5, 1},
        {4, 5, 1},
        {4, 5},
        {4},
        {}
    };
    const unsigned int testIndices1[] = {13,
        //Vector: [0, 0, 9, 0, 0, 4, 5, 0, 0, 0, 0, 1, 0]
        0,
        //Vector: [0, 9, 0, 0, 4, 5, 0, 0, 0, 0, 1, 0]
        2,
        //Vector: [0, 9, 0, 4, 5, 0, 0, 0, 0, 1, 0]
        10,
        //Vector: [0, 9, 0, 4, 5, 0, 0, 0, 0, 1]
        0,
        //Vector: [9, 0, 4, 5, 0, 0, 0, 0, 1]
        6,
        //Vector: [9, 0, 4, 5, 0, 0, 0, 1]
        4,
        //Vector: [9, 0, 4, 5, 0, 0, 1]
        4,
        //Vector: [9, 0, 4, 5, 0, 1]
        1,
        //Vector: [9, 4, 5, 0, 1]
        3,
        //Vector: [9, 4, 5, 1]
        0,
        //Vector: [4, 5, 1]
        2,
        //Vector: [4, 5]
        1,
        //Vector: [4]
        0
        //Vector: []
    };

    const unsigned int testCount1 = STATIC_ARRAY_SIZE(testIndices1);
    unsigned int index;
    Vector::VECTOR_TYPE previousType = myVect1.m_vectorType;
    unsigned int capacity = myVect1.m_capacity; // set function has been tested before

    for (index = 0; index < testCount1; index++) {
        myVect1.removeElement(testIndices1[index]);
        TEST_ASSERT(integrityCheck(myVect1));

        const unsigned int testSparsityThreshold = (unsigned int) Numerical::round(
            (myVect1.m_dimension) * SPARSITY_RATIO);
        TEST_ASSERT(myVect1.m_dimension == 14 - index - 1);
        TEST_ASSERT(myVect1.m_sparsityRatio == SPARSITY_RATIO);
        TEST_ASSERT(myVect1.m_sparsityThreshold == testSparsityThreshold);
        TEST_ASSERT(myVect1.m_data == myVect1.m_dataEnd - myVect1.m_size);
        if (index < 9) {
            TEST_ASSERT(myVect1.m_nonZeros == 4);
        } else {
            TEST_ASSERT(myVect1.m_nonZeros == 4 - (index - 9));
        }

        if (myVect1.m_nonZeros < myVect1.m_sparsityThreshold) {
            if (previousType == Vector::DENSE_VECTOR) {
                capacity = myVect1.m_nonZeros + ELBOWROOM;
                TEST_ASSERT(sortedCheck(myVect1, true));
            } else {
                TEST_ASSERT(sortedCheck(myVect1, false));
            }
            TEST_ASSERT(sortedCheck(myVect1, false));
            TEST_ASSERT(myVect1.m_vectorType == Vector::SPARSE_VECTOR);
            TEST_ASSERT(myVect1.m_size == myVect1.m_nonZeros);
            TEST_ASSERT(myVect1.m_capacity == capacity);
        } else {
            if (previousType == Vector::SPARSE_VECTOR) {
                capacity = myVect1.m_dimension + ELBOWROOM;
            }
            TEST_ASSERT(sortedCheck(myVect1, true));
            TEST_ASSERT(myVect1.m_vectorType == Vector::DENSE_VECTOR);
            TEST_ASSERT(myVect1.m_size == myVect1.m_dimension);
            TEST_ASSERT(myVect1.m_capacity == capacity);
        }
        previousType = myVect1.m_vectorType;
        unsigned int valueIndex;
        for (valueIndex = 0; valueIndex < 13 - index; valueIndex++) {
            TEST_ASSERT(myVect1.at(valueIndex) == testValues1[index][valueIndex]);
        }
    }
    //exit(0);
    Vector myVect2(14);

    myVect2.set(2, 9);
    myVect2.set(5, 4);
    myVect2.set(6, 5);
    myVect2.set(11, 1);

    //Vector: [0, 0, 9, 0, 0, 4, 5, 0, 0, 0, 0, 1, 0, 0]

    //Vector: [0, 0, 9, 0, 0, 4, 5, 0, 0, 0, 0, 1, 0, 0]
    const Numerical::Double testValues2[4][13] = {
        {0, 0, 0, 0, 4, 5, 0, 0, 0, 0, 1, 0, 0},
        {0, 0, 0, 0, 4, 0, 0, 0, 0, 1, 0, 0},
        {0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
    };

    const unsigned int testIndices2[] = {
        2,
        //Vector: [0, 0, 0, 0, 4, 5, 0, 0, 0, 0, 1, 0, 0]
        5,
        //Vector: [0, 0, 0, 0, 4, 0, 0, 0, 0, 1, 0, 0]
        9,
        //Vector: [0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0]
        4,
        //Vector: [0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
        5, 0, 7, 1, 2, 3, 3, 0, 1, 0
    };

    const unsigned int testCount2 = STATIC_ARRAY_SIZE(testIndices2);
    previousType = myVect2.m_vectorType;
    capacity = myVect2.m_capacity; // set function has been tested before
    for (index = 0; index < testCount2; index++) {
        myVect2.removeElement(testIndices2[index]);
        TEST_ASSERT(integrityCheck(myVect2));

        const unsigned int testSparsityThreshold = (unsigned int) Numerical::round(
            (myVect2.m_dimension) * SPARSITY_RATIO);
        TEST_ASSERT(myVect2.m_dimension == 14 - index - 1);
        TEST_ASSERT(myVect2.m_sparsityRatio == SPARSITY_RATIO);
        TEST_ASSERT(myVect2.m_sparsityThreshold == testSparsityThreshold);
        TEST_ASSERT(myVect2.m_data == myVect2.m_dataEnd - myVect2.m_size);
        if (index < 4) {
            TEST_ASSERT(myVect2.m_nonZeros == 4 - index - 1);
        } else {
            TEST_ASSERT(myVect2.m_nonZeros == 0);
        }

        if (myVect2.m_nonZeros < myVect2.m_sparsityThreshold) {
            if (previousType == Vector::DENSE_VECTOR) {
                capacity = myVect2.m_nonZeros + ELBOWROOM;
                TEST_ASSERT(sortedCheck(myVect2, true));
            } else {
                TEST_ASSERT(sortedCheck(myVect2, false));
            }
            TEST_ASSERT(myVect2.m_vectorType == Vector::SPARSE_VECTOR);
            TEST_ASSERT(myVect2.m_size == myVect2.m_nonZeros);
            TEST_ASSERT(myVect2.m_capacity == capacity);
        } else {
            if (previousType == Vector::SPARSE_VECTOR) {
                capacity = myVect2.m_dimension + ELBOWROOM;
            }
            TEST_ASSERT(sortedCheck(myVect2, true));
            TEST_ASSERT(myVect2.m_vectorType == Vector::DENSE_VECTOR);
            TEST_ASSERT(myVect2.m_size == myVect2.m_dimension);
            TEST_ASSERT(myVect2.m_capacity == capacity);
        }
        previousType = myVect2.m_vectorType;
        unsigned int valueIndex;
        for (valueIndex = 0; valueIndex < 13 - index; valueIndex++) {
            if (index < 4) {
                TEST_ASSERT(myVect2.at(valueIndex) == testValues2[index][valueIndex]);
            } else {
                TEST_ASSERT(myVect2.at(valueIndex) == 0);
            }
        }
    }
}

void VectorTestSuite::resize()
{
    Vector myVect1(8);
    myVect1.set(1, 3);
    myVect1.set(0, 7);

    Vector::VECTOR_TYPE originalType = myVect1.m_vectorType;
    unsigned int originalCapacity = myVect1.m_capacity;
    myVect1.resize(9);
    TEST_ASSERT(integrityCheck(myVect1));

    const unsigned int testSparsityThreshold1 = (unsigned int) Numerical::round(
        (myVect1.m_dimension) * SPARSITY_RATIO);

    TEST_ASSERT(myVect1.m_dimension == 9);
    TEST_ASSERT(myVect1.m_nonZeros == 2);
    TEST_ASSERT(myVect1.m_sparsityThreshold == testSparsityThreshold1);
    TEST_ASSERT(myVect1.m_sparsityRatio == SPARSITY_RATIO);
    TEST_ASSERT(myVect1.m_data == myVect1.m_dataEnd - myVect1.m_size);
    if (myVect1.m_nonZeros < myVect1.m_sparsityThreshold) {
        TEST_ASSERT(myVect1.m_vectorType == Vector::SPARSE_VECTOR);
        TEST_ASSERT(myVect1.m_size == myVect1.m_nonZeros);
        if (originalType == Vector::DENSE_VECTOR) {
            TEST_ASSERT(sortedCheck(myVect1, true));
            TEST_ASSERT(myVect1.m_capacity == myVect1.m_size + ELBOWROOM);
        } else {
            TEST_ASSERT(myVect1.m_capacity == originalCapacity);
            TEST_ASSERT(sortedCheck(myVect1, false));
        }

    } else {
        TEST_ASSERT(myVect1.m_vectorType == Vector::DENSE_VECTOR);
        TEST_ASSERT(sortedCheck(myVect1, true));
        TEST_ASSERT(myVect1.m_size == myVect1.m_dimension);
        if (originalType == Vector::DENSE_VECTOR) {
            if (originalCapacity >= myVect1.m_dimension) {
                TEST_ASSERT(myVect1.m_capacity == originalCapacity);
            } else {
                TEST_ASSERT(myVect1.m_capacity == myVect1.m_size + ELBOWROOM);
            }
        } else {
            TEST_ASSERT(myVect1.m_capacity == myVect1.m_size + ELBOWROOM);
        }
    }

    TEST_ASSERT(myVect1.at(0) == 7);
    TEST_ASSERT(myVect1.at(1) == 3);
    TEST_ASSERT(myVect1.at(2) == 0);
    TEST_ASSERT(myVect1.at(3) == 0);
    TEST_ASSERT(myVect1.at(4) == 0);
    TEST_ASSERT(myVect1.at(5) == 0);
    TEST_ASSERT(myVect1.at(6) == 0);
    TEST_ASSERT(myVect1.at(7) == 0);
    TEST_ASSERT(myVect1.at(8) == 0);

    Vector myVect2(8);
    myVect2.set(1, 3);
    myVect2.set(3, 1);
    myVect2.set(6, 7);

    originalType = myVect2.m_vectorType;
    originalCapacity = myVect2.m_capacity;
    myVect2.resize(3);
    TEST_ASSERT(integrityCheck(myVect2));

    const unsigned int testSparsityThreshold2 = (unsigned int) Numerical::round(
        (myVect2.m_dimension) * SPARSITY_RATIO);

    TEST_ASSERT(myVect2.m_dimension == 3);
    TEST_ASSERT(myVect2.m_nonZeros == 1);

    TEST_ASSERT(myVect2.m_sparsityThreshold == testSparsityThreshold2);
    TEST_ASSERT(myVect2.m_sparsityRatio == SPARSITY_RATIO);
    TEST_ASSERT(myVect2.m_data == myVect2.m_dataEnd - myVect2.m_size);
    if (myVect2.m_nonZeros < myVect2.m_sparsityThreshold) {
        TEST_ASSERT(myVect2.m_vectorType == Vector::SPARSE_VECTOR);
        TEST_ASSERT(myVect2.m_size == myVect2.m_nonZeros);
        if (originalType == Vector::DENSE_VECTOR) {
            TEST_ASSERT(sortedCheck(myVect2, true));
            TEST_ASSERT(myVect2.m_capacity == myVect2.m_size + ELBOWROOM);
        } else {
            TEST_ASSERT(myVect2.m_capacity == originalCapacity);
            TEST_ASSERT(sortedCheck(myVect2, false));
        }

    } else {

        TEST_ASSERT(myVect2.m_vectorType == Vector::DENSE_VECTOR);
        TEST_ASSERT(sortedCheck(myVect2, true));
        TEST_ASSERT(myVect2.m_size == myVect2.m_dimension);
        if (originalType == Vector::DENSE_VECTOR) {
            if (originalCapacity >= myVect2.m_dimension) {
                TEST_ASSERT(myVect2.m_capacity == originalCapacity);
            } else {
                TEST_ASSERT(myVect2.m_capacity == myVect2.m_size + ELBOWROOM);
            }
        } else {
            TEST_ASSERT(myVect2.m_capacity == myVect2.m_size + ELBOWROOM);
        }
    }
    TEST_ASSERT(myVect2.at(0) == 0);
    TEST_ASSERT(myVect2.at(1) == 3);
    //exit(0);

    Vector myVect3(10);
    myVect3.set(4, 2);
    myVect3.set(7, 1);

    originalType = myVect3.m_vectorType;
    originalCapacity = myVect3.m_capacity;

    myVect3.resize(2);
    TEST_ASSERT(integrityCheck(myVect3));

    const unsigned int testSparsityThreshold3 = (unsigned int) Numerical::round(
        (myVect3.m_dimension) * SPARSITY_RATIO);

    TEST_ASSERT(myVect3.m_dimension == 2);
    TEST_ASSERT(myVect3.m_nonZeros == 0);

    TEST_ASSERT(myVect3.m_sparsityThreshold == testSparsityThreshold3);
    TEST_ASSERT(myVect3.m_sparsityRatio == SPARSITY_RATIO);
    TEST_ASSERT(myVect3.m_data == myVect3.m_dataEnd - myVect3.m_size);
    if (myVect3.m_nonZeros < myVect3.m_sparsityThreshold) {
        TEST_ASSERT(myVect3.m_vectorType == Vector::SPARSE_VECTOR);
        TEST_ASSERT(myVect3.m_size == myVect3.m_nonZeros);
        if (originalType == Vector::DENSE_VECTOR) {
            TEST_ASSERT(sortedCheck(myVect3, true));
            TEST_ASSERT(myVect3.m_capacity == myVect3.m_size + ELBOWROOM);
        } else {
            TEST_ASSERT(myVect3.m_capacity == originalCapacity);
            TEST_ASSERT(sortedCheck(myVect3, false));
        }

    } else {

        TEST_ASSERT(myVect3.m_vectorType == Vector::DENSE_VECTOR);
        TEST_ASSERT(sortedCheck(myVect3, true));
        TEST_ASSERT(myVect3.m_size == myVect3.m_dimension);
        if (originalType == Vector::DENSE_VECTOR) {
            if (originalCapacity >= myVect3.m_dimension) {
                TEST_ASSERT(myVect3.m_capacity == originalCapacity);
            } else {
                TEST_ASSERT(myVect3.m_capacity == myVect3.m_size + ELBOWROOM);
            }
        } else {
            TEST_ASSERT(myVect3.m_capacity == myVect3.m_size + ELBOWROOM);
        }
    }
    TEST_ASSERT(myVect3.at(0) == 0);
    TEST_ASSERT(myVect3.at(1) == 0);

    Vector myVect4(10);

    originalType = myVect4.m_vectorType;
    originalCapacity = myVect4.m_capacity;

    myVect4.resize(2);
    TEST_ASSERT(integrityCheck(myVect4));

    const unsigned int testSparsityThreshold4 = (unsigned int) Numerical::round(
        (myVect4.m_dimension) * SPARSITY_RATIO);

    TEST_ASSERT(myVect4.m_dimension == 2);
    TEST_ASSERT(myVect4.m_nonZeros == 0);

    TEST_ASSERT(myVect4.m_sparsityThreshold == testSparsityThreshold4);
    TEST_ASSERT(myVect4.m_sparsityRatio == SPARSITY_RATIO);
    TEST_ASSERT(myVect4.m_data == myVect4.m_dataEnd - myVect4.m_size);
    if (myVect4.m_nonZeros < myVect4.m_sparsityThreshold) {
        TEST_ASSERT(myVect4.m_vectorType == Vector::SPARSE_VECTOR);
        TEST_ASSERT(myVect4.m_size == myVect4.m_nonZeros);
        if (originalType == Vector::DENSE_VECTOR) {
            TEST_ASSERT(sortedCheck(myVect4, true));
            TEST_ASSERT(myVect4.m_capacity == myVect4.m_size + ELBOWROOM);
        } else {
            TEST_ASSERT(myVect4.m_capacity == originalCapacity);
            TEST_ASSERT(sortedCheck(myVect4, true));
        }

    } else {

        TEST_ASSERT(myVect4.m_vectorType == Vector::DENSE_VECTOR);
        TEST_ASSERT(sortedCheck(myVect4, true));
        TEST_ASSERT(myVect4.m_size == myVect4.m_dimension);
        if (originalType == Vector::DENSE_VECTOR) {
            if (originalCapacity >= myVect4.m_dimension) {
                TEST_ASSERT(myVect4.m_capacity == originalCapacity);
            } else {
                TEST_ASSERT(myVect4.m_capacity == myVect4.m_size + ELBOWROOM);
            }
        } else {
            TEST_ASSERT(myVect4.m_capacity == myVect4.m_size + ELBOWROOM);
        }
    }
    TEST_ASSERT(myVect4.at(0) == 0);
    TEST_ASSERT(myVect4.at(1) == 0);
}

void VectorTestSuite::setSparsityRatio()
{
    Vector myVect(10);
    myVect.set(3, 8);
    myVect.set(7, -2);
    myVect.set(5, 6.76);
    Vector::VECTOR_TYPE previousType = myVect.m_vectorType;

    const unsigned int N = 100;
    unsigned int index;
    bool testSorted = false;
    unsigned int capacity = myVect.m_capacity;
    for (index = 0; index <= 2 * N; index++) {

        double sparsityRatio;
        if (index <= N) {
            sparsityRatio = (1.0 / N) * index;
        } else {
            sparsityRatio = (1.0 / N) * (N - (index - N));
        }
        myVect.setSparsityRatio(sparsityRatio);
        TEST_ASSERT(integrityCheck(myVect));

        const unsigned int testSparsityThreshold = (unsigned int) Numerical::round(
            (myVect.m_dimension) * sparsityRatio);

        TEST_ASSERT(myVect.m_dimension == 10);
        TEST_ASSERT(myVect.m_sparsityThreshold == testSparsityThreshold);
        TEST_ASSERT(myVect.m_sparsityRatio == sparsityRatio);
        TEST_ASSERT(myVect.m_nonZeros == 3);
        TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - myVect.m_size);

        if (myVect.m_nonZeros < myVect.m_sparsityThreshold) {
            TEST_ASSERT(myVect.m_vectorType == Vector::SPARSE_VECTOR);
            TEST_ASSERT(myVect.m_size == myVect.m_nonZeros);

            if (previousType == Vector::DENSE_VECTOR) {
                TEST_ASSERT(sortedCheck(myVect, true));
                testSorted = true;
                capacity = myVect.m_size + ELBOWROOM;
            } else {
                TEST_ASSERT(sortedCheck(myVect, testSorted));
            }
        } else {
            if (previousType == Vector::SPARSE_VECTOR) {
                capacity = myVect.m_size + ELBOWROOM;
            }
            TEST_ASSERT(myVect.m_vectorType == Vector::DENSE_VECTOR);
            TEST_ASSERT(sortedCheck(myVect, true));
            TEST_ASSERT(myVect.m_size == myVect.m_dimension);
        }
        TEST_ASSERT(myVect.m_capacity == capacity);

        TEST_ASSERT(myVect.at(0) == 0);
        TEST_ASSERT(myVect.at(1) == 0);
        TEST_ASSERT(myVect.at(2) == 0);
        TEST_ASSERT(myVect.at(3) == 8);
        TEST_ASSERT(myVect.at(4) == 0);
        TEST_ASSERT(myVect.at(5) == 6.76);
        TEST_ASSERT(myVect.at(6) == 0);
        TEST_ASSERT(myVect.at(7) == -2);
        TEST_ASSERT(myVect.at(8) == 0);
        TEST_ASSERT(myVect.at(9) == 0);
        previousType = myVect.m_vectorType;

    }
}

void VectorTestSuite::maxIndex()
{
    Vector myVect(5);

    TEST_ASSERT(myVect.maxIndex() == 0);
    TEST_ASSERT(integrityCheck(myVect));

    myVect.set(3, 99);

    //In case of sparse vector

    TEST_ASSERT(myVect.maxIndex() == 3);
    TEST_ASSERT(integrityCheck(myVect));

    myVect.set(2, 5);
    myVect.set(0, -7);

    //In case of dense vector

    TEST_ASSERT(myVect.maxIndex() == 3);
    TEST_ASSERT(integrityCheck(myVect));
}

void VectorTestSuite::absMaxElement()
{
    Vector myVect(8);

    TEST_ASSERT(myVect.absMaxElement() == 0);
    TEST_ASSERT(integrityCheck(myVect));
    myVect.set(2, 6);

    TEST_ASSERT(myVect.absMaxElement() == 6);
    TEST_ASSERT(integrityCheck(myVect));
    myVect.set(0, -11.7);

    TEST_ASSERT(myVect.absMaxElement() == 11.7);
    TEST_ASSERT(integrityCheck(myVect));
    //In case of dense vector

    myVect.set(6, 5);
    myVect.set(5, -3);

    TEST_ASSERT(myVect.absMaxElement() == 11.7);
    TEST_ASSERT(integrityCheck(myVect));
}

void VectorTestSuite::scaleBy()
{
    Vector myVect(5);
    myVect.set(1, 7);
    myVect.set(3, 5);
    myVect.set(4, 2);

    const unsigned int testSparsityThreshold = (unsigned int) Numerical::round(
        (myVect.m_dimension) * SPARSITY_RATIO);
    unsigned int capacity = myVect.m_capacity;
    bool sorted = myVect.m_sorted;

    const Numerical::Double multipliers[] = {-1, 1, 2, 3, 100, 0.25, -0.125, 0};
    const unsigned int count = STATIC_ARRAY_SIZE(multipliers);
    unsigned int index;
    for (index = 0; index < count; index++) {
        myVect.set(1, 7);
        myVect.set(3, 5);
        myVect.set(4, 2);

        Vector::VECTOR_TYPE originalType = myVect.m_vectorType;
        myVect.scaleBy(multipliers[index]);
        TEST_ASSERT(integrityCheck(myVect));

        if (multipliers[index] == 0.0) {
            sorted = true;
        }

        TEST_ASSERT(myVect.m_sparsityThreshold == testSparsityThreshold);
        TEST_ASSERT(myVect.m_dimension == 5);
        TEST_ASSERT(myVect.m_nonZeros == (multipliers[index] == 0.0 ? 0 : 3));
        TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - myVect.m_size);
        if (myVect.m_nonZeros < myVect.m_sparsityThreshold) {
            if (originalType == Vector::DENSE_VECTOR) {
                capacity = ELBOWROOM;
                sorted = true;
            }
            TEST_ASSERT(myVect.m_vectorType == Vector::SPARSE_VECTOR);
            TEST_ASSERT(myVect.m_size == myVect.m_nonZeros);

        } else {
            TEST_ASSERT(myVect.m_vectorType == Vector::DENSE_VECTOR);
            TEST_ASSERT(myVect.m_size == myVect.m_dimension);

        }
        TEST_ASSERT(myVect.m_capacity == capacity);
        TEST_ASSERT(sortedCheck(myVect, sorted));

        TEST_ASSERT(myVect.at(0) == 0);
        TEST_ASSERT(myVect.at(1) == 7 * multipliers[index]);
        TEST_ASSERT(myVect.at(2) == 0);
        TEST_ASSERT(myVect.at(3) == 5 * multipliers[index]);
        TEST_ASSERT(myVect.at(4) == 2 * multipliers[index]);
    }
}

void VectorTestSuite::scaleElementBy()
{
    Vector myVect(7);
    myVect.set(2, 4);
    myVect.set(3, -3);
    myVect.set(5, -6);
    myVect.set(6, 6);

    unsigned int capacity = myVect.m_capacity;
    Numerical::Double * data = myVect.m_data;
    Numerical::Double * dataEnd = myVect.m_dataEnd;
    unsigned int dimension = myVect.m_dimension;
    unsigned int * indexPtr = myVect.m_index;
    unsigned int nonZeros = myVect.m_nonZeros;
    unsigned int m_size = myVect.m_size;
    bool sorted = myVect.m_sorted;
    Numerical::Double sparsityRatio = myVect.m_sparsityRatio;
    unsigned int threshold = myVect.m_sparsityThreshold;
    Vector::VECTOR_TYPE type = myVect.m_vectorType;

    myVect.scaleElementBy(3, -2);
    TEST_ASSERT(integrityCheck(myVect));

    TEST_ASSERT(capacity == myVect.m_capacity);
    TEST_ASSERT(data == myVect.m_data);
    TEST_ASSERT(dataEnd == myVect.m_dataEnd);
    TEST_ASSERT(dimension == myVect.m_dimension);
    TEST_ASSERT(indexPtr == myVect.m_index);
    TEST_ASSERT(nonZeros == myVect.m_nonZeros);
    TEST_ASSERT(m_size == myVect.m_size);
    if (myVect.m_vectorType == Vector::SPARSE_VECTOR) {
        TEST_ASSERT(sortedCheck(myVect, false));
    } else {
        TEST_ASSERT(sortedCheck(myVect, sorted));
    }
    TEST_ASSERT(sparsityRatio == myVect.m_sparsityRatio);
    TEST_ASSERT(threshold == myVect.m_sparsityThreshold);
    TEST_ASSERT(type == myVect.m_vectorType);

    TEST_ASSERT(myVect.at(0) == 0);
    TEST_ASSERT(myVect.at(1) == 0);
    TEST_ASSERT(myVect.at(2) == 4);
    TEST_ASSERT(myVect.at(3) == 6);
    TEST_ASSERT(myVect.at(4) == 0);
    TEST_ASSERT(myVect.at(5) == -6);
    TEST_ASSERT(myVect.at(6) == 6);

    myVect.scaleElementBy(4, 2);
    TEST_ASSERT(integrityCheck(myVect));

    TEST_ASSERT(capacity == myVect.m_capacity);
    TEST_ASSERT(data == myVect.m_data);
    TEST_ASSERT(dataEnd == myVect.m_dataEnd);
    TEST_ASSERT(dimension == myVect.m_dimension);
    TEST_ASSERT(indexPtr == myVect.m_index);
    TEST_ASSERT(nonZeros == myVect.m_nonZeros);
    TEST_ASSERT(m_size == myVect.m_size);
    if (myVect.m_vectorType == Vector::SPARSE_VECTOR) {
        TEST_ASSERT(sortedCheck(myVect, false));
    } else {
        TEST_ASSERT(sortedCheck(myVect, sorted));
    }
    TEST_ASSERT(sparsityRatio == myVect.m_sparsityRatio);
    TEST_ASSERT(threshold == myVect.m_sparsityThreshold);
    TEST_ASSERT(type == myVect.m_vectorType);

    TEST_ASSERT(myVect.at(0) == 0);
    TEST_ASSERT(myVect.at(1) == 0);
    TEST_ASSERT(myVect.at(2) == 4);
    TEST_ASSERT(myVect.at(3) == 6);
    TEST_ASSERT(myVect.at(4) == 0);
    TEST_ASSERT(myVect.at(5) == -6);
    TEST_ASSERT(myVect.at(6) == 6);

    myVect.scaleElementBy(2, 0);
    TEST_ASSERT(integrityCheck(myVect));

    TEST_ASSERT(dimension == myVect.m_dimension);
    TEST_ASSERT(nonZeros - 1 == myVect.m_nonZeros);
    TEST_ASSERT(sparsityRatio == myVect.m_sparsityRatio);
    TEST_ASSERT(threshold == myVect.m_sparsityThreshold);
    TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - myVect.m_size)

    if (myVect.m_nonZeros < myVect.m_sparsityThreshold) {
        TEST_ASSERT(myVect.m_vectorType == Vector::SPARSE_VECTOR);
        TEST_ASSERT(myVect.m_size == myVect.m_nonZeros);
        if (type == Vector::DENSE_VECTOR) {
            TEST_ASSERT(sortedCheck(myVect, true));
            TEST_ASSERT(myVect.m_capacity == myVect.m_size + ELBOWROOM);
        } else {
            TEST_ASSERT(sortedCheck(myVect, false));
            TEST_ASSERT(capacity == myVect.m_capacity);
        }
    } else {
        TEST_ASSERT(myVect.m_vectorType == Vector::DENSE_VECTOR);
        TEST_ASSERT(sortedCheck(myVect, true));
        TEST_ASSERT(myVect.m_size == myVect.m_dimension);
        TEST_ASSERT(myVect.m_capacity == myVect.m_size + ELBOWROOM);
    }

    TEST_ASSERT(myVect.at(0) == 0);
    TEST_ASSERT(myVect.at(1) == 0);
    TEST_ASSERT(myVect.at(2) == 0);
    TEST_ASSERT(myVect.at(3) == 6);
    TEST_ASSERT(myVect.at(4) == 0);
    TEST_ASSERT(myVect.at(5) == -6);
    TEST_ASSERT(myVect.at(6) == 6);

    const unsigned int testIndices[] = {3, 5, 6};
    const unsigned int testCount = STATIC_ARRAY_SIZE(testIndices);

    unsigned int index;
    for (index = 0; index < testCount; index++) {
        sorted = myVect.m_sorted;
        type = myVect.m_vectorType;
        capacity = myVect.m_capacity;

        myVect.scaleElementBy(testIndices[index], 0);
        TEST_ASSERT(integrityCheck(myVect));

        TEST_ASSERT(dimension == myVect.m_dimension);
        TEST_ASSERT(myVect.m_nonZeros == 3 - index - 1);
        TEST_ASSERT(sparsityRatio == myVect.m_sparsityRatio);
        TEST_ASSERT(threshold == myVect.m_sparsityThreshold);
        TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - myVect.m_size)

        if (myVect.m_nonZeros < myVect.m_sparsityThreshold) {
            TEST_ASSERT(myVect.m_vectorType == Vector::SPARSE_VECTOR);
            TEST_ASSERT(myVect.m_size == myVect.m_nonZeros);
            if (type == Vector::DENSE_VECTOR) {
                TEST_ASSERT(sortedCheck(myVect, true));
                TEST_ASSERT(myVect.m_capacity == myVect.m_size + ELBOWROOM);
            } else {
                TEST_ASSERT(sortedCheck(myVect, false));
                TEST_ASSERT(capacity == myVect.m_capacity);
            }
        } else {
            TEST_ASSERT(myVect.m_vectorType == Vector::DENSE_VECTOR);
            TEST_ASSERT(sortedCheck(myVect, true));
            TEST_ASSERT(myVect.m_size == myVect.m_dimension);
            TEST_ASSERT(myVect.m_capacity == myVect.m_size + ELBOWROOM);
        }

        TEST_ASSERT(myVect.at(0) == 0);
        TEST_ASSERT(myVect.at(1) == 0);
        TEST_ASSERT(myVect.at(2) == 0);
        TEST_ASSERT(myVect.at(3) == 0);
        TEST_ASSERT(myVect.at(4) == 0);
        if (index >= 1) {
            TEST_ASSERT(myVect.at(5) == 0);
            if (index >= 2) {
                TEST_ASSERT(myVect.at(6) == 0);
            } else {
                TEST_ASSERT(myVect.at(6) == 6);
            }
        } else {
            TEST_ASSERT(myVect.at(5) == -6);
            TEST_ASSERT(myVect.at(6) == 6);
        }
    }
}

void VectorTestSuite::scaleByLambdas()
{
    Vector myVect(6);
    myVect.set(0, -9);
    myVect.set(1, 4);
    myVect.set(2, 55);
    myVect.set(3, -3);
    myVect.set(4, -1);
    myVect.set(5, 2);

    std::vector<Numerical::Double> myLambdas(6);
    myLambdas[0] = 1;
    myLambdas[1] = -2;
    myLambdas[2] = 0;
    myLambdas[3] = 0;
    myLambdas[4] = -4;
    myLambdas[5] = 10;

    const unsigned int testSparsityThreshold = (unsigned int) Numerical::round(
        (myVect.m_dimension) * SPARSITY_RATIO);

    Vector::VECTOR_TYPE originalType = myVect.m_vectorType;
    unsigned int capacity = myVect.m_capacity;
    myVect.scaleByLambdas(myLambdas);
    TEST_ASSERT(integrityCheck(myVect));

    TEST_ASSERT(myVect.m_sparsityThreshold == testSparsityThreshold);
    TEST_ASSERT(myVect.m_sparsityRatio == SPARSITY_RATIO);
    TEST_ASSERT(myVect.m_nonZeros == 4);
    TEST_ASSERT(myVect.m_dimension == 6);
    TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - myVect.m_size);

    if (myVect.m_nonZeros < myVect.m_sparsityThreshold) {
        TEST_ASSERT(myVect.m_vectorType == Vector::SPARSE_VECTOR);
        TEST_ASSERT(myVect.m_size == myVect.m_nonZeros);
        if (originalType == Vector::DENSE_VECTOR) {
            TEST_ASSERT(sortedCheck(myVect, true));
            capacity = myVect.m_size + ELBOWROOM;
        } else {
            TEST_ASSERT(sortedCheck(myVect, false));
        }
    } else {
        TEST_ASSERT(myVect.m_vectorType == Vector::DENSE_VECTOR);
        TEST_ASSERT(myVect.m_size == myVect.m_dimension);
        TEST_ASSERT(sortedCheck(myVect, true));
    }
    TEST_ASSERT(myVect.m_capacity == capacity);
    TEST_ASSERT(myVect.at(0) == -9);
    TEST_ASSERT(myVect.at(1) == -8);
    TEST_ASSERT(myVect.at(2) == 0);
    TEST_ASSERT(myVect.at(3) == 0);
    TEST_ASSERT(myVect.at(4) == 4);
    TEST_ASSERT(myVect.at(5) == 20);

    myLambdas[0] = 1;
    myLambdas[1] = 2;
    myLambdas[2] = 3;
    myLambdas[3] = -11;
    myLambdas[4] = 0;
    myLambdas[5] = 0;

    originalType = myVect.m_vectorType;
    capacity = myVect.m_capacity;
    myVect.scaleByLambdas(myLambdas);
    TEST_ASSERT(integrityCheck(myVect));

    TEST_ASSERT(myVect.m_sparsityThreshold == testSparsityThreshold);
    TEST_ASSERT(myVect.m_sparsityRatio == SPARSITY_RATIO);
    TEST_ASSERT(myVect.m_nonZeros == 2);
    TEST_ASSERT(myVect.m_dimension == 6);
    TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - myVect.m_size);

    if (myVect.m_nonZeros < myVect.m_sparsityThreshold) {
        TEST_ASSERT(myVect.m_vectorType == Vector::SPARSE_VECTOR);
        TEST_ASSERT(myVect.m_size == myVect.m_nonZeros);
        if (originalType == Vector::DENSE_VECTOR) {
            TEST_ASSERT(sortedCheck(myVect, true));
            capacity = myVect.m_size + ELBOWROOM;
        } else {
            TEST_ASSERT(sortedCheck(myVect, false));
        }
    } else {
        TEST_ASSERT(myVect.m_vectorType == Vector::DENSE_VECTOR);
        TEST_ASSERT(myVect.m_size == myVect.m_dimension);
        TEST_ASSERT(sortedCheck(myVect, true));
    }

    TEST_ASSERT(myVect.at(0) == -9);
    TEST_ASSERT(myVect.at(1) == -16);
    TEST_ASSERT(myVect.at(2) == 0);
    TEST_ASSERT(myVect.at(3) == 0);
    TEST_ASSERT(myVect.at(4) == 0);
    TEST_ASSERT(myVect.at(5) == 0);

    myLambdas[0] = 0;
    myLambdas[1] = 0;
    myLambdas[2] = 3;
    myLambdas[3] = -11;
    myLambdas[4] = 0;
    myLambdas[5] = 0;

    originalType = myVect.m_vectorType;
    capacity = myVect.m_capacity;
    myVect.scaleByLambdas(myLambdas);
    TEST_ASSERT(integrityCheck(myVect));

    TEST_ASSERT(myVect.m_sparsityThreshold == testSparsityThreshold);
    TEST_ASSERT(myVect.m_sparsityRatio == SPARSITY_RATIO);
    TEST_ASSERT(myVect.m_nonZeros == 0);
    TEST_ASSERT(myVect.m_dimension == 6);
    TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - myVect.m_size);

    if (myVect.m_nonZeros < myVect.m_sparsityThreshold) {
        TEST_ASSERT(myVect.m_vectorType == Vector::SPARSE_VECTOR);
        TEST_ASSERT(myVect.m_size == myVect.m_nonZeros);
        if (originalType == Vector::DENSE_VECTOR) {
            TEST_ASSERT(sortedCheck(myVect, true));
            capacity = myVect.m_size + ELBOWROOM;
        } else {
            TEST_ASSERT(sortedCheck(myVect, false));
        }
    } else {
        TEST_ASSERT(myVect.m_vectorType == Vector::DENSE_VECTOR);
        TEST_ASSERT(myVect.m_size == myVect.m_dimension);
        TEST_ASSERT(sortedCheck(myVect, true));
    }

    TEST_ASSERT(myVect.at(0) == 0);
    TEST_ASSERT(myVect.at(1) == 0);
    TEST_ASSERT(myVect.at(2) == 0);
    TEST_ASSERT(myVect.at(3) == 0);
    TEST_ASSERT(myVect.at(4) == 0);
    TEST_ASSERT(myVect.at(5) == 0);
}

void VectorTestSuite::addVector()
{
    //Add dense to dense

    Numerical::Double myLambda = 2;

    Vector myVect(5);
    myVect.set(0, 4);
    myVect.set(2, 2);
    myVect.set(3, 5);

    const unsigned int testSparsityThreshold = (unsigned int) Numerical::round(
        (myVect.m_dimension) * SPARSITY_RATIO);

    Vector myVectAdd(5);
    myVectAdd.set(1, 1);
    myVectAdd.set(2, -2);
    myVectAdd.set(3, 2);

    //myVect: [4, 0, 2, 5, 0]
    //myVectAdd: [0, 1, -2, 2, 0]
    //myLambda * myVectAdd: [0, 2, -4, 4, 0]
    //Sum: [4, 2, -2, 9, 0]

    Vector::VECTOR_TYPE originalType = myVect.m_vectorType;

    myVect.addVector(myLambda, myVectAdd);
    TEST_ASSERT(integrityCheck(myVect));

    TEST_ASSERT(myVect.m_sparsityThreshold == testSparsityThreshold);
    TEST_ASSERT(myVect.m_sparsityRatio == SPARSITY_RATIO);
    TEST_ASSERT(myVect.m_nonZeros == 4);
    TEST_ASSERT(myVect.m_dimension == 5);
    TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - myVect.m_size);
    TEST_ASSERT(myVect.m_capacity >= myVect.m_size);
    if (myVect.m_nonZeros < myVect.m_sparsityThreshold) {
        TEST_ASSERT(myVect.m_vectorType == Vector::SPARSE_VECTOR);
        TEST_ASSERT(myVect.m_size == myVect.m_nonZeros);
        if (originalType == Vector::DENSE_VECTOR) {
            TEST_ASSERT(sortedCheck(myVect, true));
        } else {
            TEST_ASSERT(sortedCheck(myVect, true, true));
        }
    } else {
        TEST_ASSERT(myVect.m_vectorType == Vector::DENSE_VECTOR);
        TEST_ASSERT(myVect.m_size == myVect.m_dimension);
        TEST_ASSERT(sortedCheck(myVect, true));
    }

    TEST_ASSERT(myVect.at(0) == 4);
    TEST_ASSERT(myVect.at(1) == 2);
    TEST_ASSERT(myVect.at(2) == -2);
    TEST_ASSERT(myVect.at(3) == 9);
    TEST_ASSERT(myVect.at(4) == 0);

    //Should switch to sparse

    myVectAdd.set(1, -2);
    myVectAdd.set(2, 2);
    myVectAdd.set(3, -9);
    myLambda = 1;

    //myVect: [4, 2, -2, 9, 0]
    //myVectAdd: [0, -2, 2, -9, 0]
    //myLambda * myVectAdd: [0, -2, 2, -9, 0]
    //Sum: [4, 0, 0, 0, 0]

    originalType = myVect.m_vectorType;

    myVect.addVector(myLambda, myVectAdd);
    TEST_ASSERT(integrityCheck(myVect));

    TEST_ASSERT(myVect.m_sparsityThreshold == testSparsityThreshold);
    TEST_ASSERT(myVect.m_sparsityRatio == SPARSITY_RATIO);
    TEST_ASSERT(myVect.m_nonZeros == 1);
    TEST_ASSERT(myVect.m_dimension == 5);
    TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - myVect.m_size);
    TEST_ASSERT(myVect.m_capacity >= myVect.m_size);
    if (myVect.m_nonZeros < myVect.m_sparsityThreshold) {
        TEST_ASSERT(myVect.m_vectorType == Vector::SPARSE_VECTOR);
        TEST_ASSERT(myVect.m_size == myVect.m_nonZeros);
        if (originalType == Vector::DENSE_VECTOR) {
            TEST_ASSERT(sortedCheck(myVect, true));
        } else {
            TEST_ASSERT(sortedCheck(myVect, true, true));
        }
    } else {
        TEST_ASSERT(myVect.m_vectorType == Vector::DENSE_VECTOR);
        TEST_ASSERT(myVect.m_size == myVect.m_dimension);
        TEST_ASSERT(sortedCheck(myVect, true));
    }

    TEST_ASSERT(myVect.at(0) == 4);
    TEST_ASSERT(myVect.at(1) == 0);
    TEST_ASSERT(myVect.at(2) == 0);
    TEST_ASSERT(myVect.at(3) == 0);
    TEST_ASSERT(myVect.at(4) == 0);

    //Add dense to sparse

    myVectAdd.set(0, 4);
    myVectAdd.set(1, 0);
    myVectAdd.set(2, 0);
    myVectAdd.set(3, 9);
    myVectAdd.set(4, 0);
    myLambda = -1;

    //myVect: [4, 0, 0, 0, 0]
    //myVectAdd: [4, 0, 0, 9, 0]
    //myLambda * myVectAdd: [-4, 0, 0, -9, 0]
    //Sum: [0, 0, 0, -9, 0]

    originalType = myVect.m_vectorType;

    myVect.addVector(myLambda, myVectAdd);
    TEST_ASSERT(integrityCheck(myVect));

    TEST_ASSERT(myVect.m_sparsityThreshold == testSparsityThreshold);
    TEST_ASSERT(myVect.m_sparsityRatio == SPARSITY_RATIO);
    TEST_ASSERT(myVect.m_nonZeros == 1);
    TEST_ASSERT(myVect.m_dimension == 5);
    TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - myVect.m_size);
    TEST_ASSERT(myVect.m_capacity >= myVect.m_size);
    if (myVect.m_nonZeros < myVect.m_sparsityThreshold) {
        TEST_ASSERT(myVect.m_vectorType == Vector::SPARSE_VECTOR);
        TEST_ASSERT(myVect.m_size == myVect.m_nonZeros);
        if (originalType == Vector::DENSE_VECTOR) {
            TEST_ASSERT(sortedCheck(myVect, true));
        } else {
            TEST_ASSERT(sortedCheck(myVect, true, true));
        }
    } else {
        TEST_ASSERT(myVect.m_vectorType == Vector::DENSE_VECTOR);
        TEST_ASSERT(myVect.m_size == myVect.m_dimension);
        TEST_ASSERT(sortedCheck(myVect, true));
    }

    TEST_ASSERT(myVect.at(0) == 0);
    TEST_ASSERT(myVect.at(1) == 0);
    TEST_ASSERT(myVect.at(2) == 0);
    TEST_ASSERT(myVect.at(3) == -9);
    TEST_ASSERT(myVect.at(4) == 0);

    //Should switch to dense

    myVectAdd.set(0, 3);
    myVectAdd.set(1, 0);
    myVectAdd.set(2, 0);
    myVectAdd.set(3, 3);
    myVectAdd.set(4, 3);
    myLambda = 1;

    //myVect:    [0, 0, 0, -9, 0]
    //myVectAdd: [3, 0, 0, 3, 3]
    //myLambda * myVectAdd: [3, 0, 0, 3, 3]
    //Sum: [3, 0, 0, -6, 3]

    originalType = myVect.m_vectorType;

    myVect.addVector(myLambda, myVectAdd);
    TEST_ASSERT(integrityCheck(myVect));

    TEST_ASSERT(myVect.m_sparsityThreshold == testSparsityThreshold);
    TEST_ASSERT(myVect.m_sparsityRatio == SPARSITY_RATIO);
    TEST_ASSERT(myVect.m_nonZeros == 3);
    TEST_ASSERT(myVect.m_dimension == 5);
    TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - myVect.m_size);
    TEST_ASSERT(myVect.m_capacity >= myVect.m_size);
    if (myVect.m_nonZeros < myVect.m_sparsityThreshold) {
        TEST_ASSERT(myVect.m_vectorType == Vector::SPARSE_VECTOR);
        TEST_ASSERT(myVect.m_size == myVect.m_nonZeros);
        if (originalType == Vector::DENSE_VECTOR) {
            TEST_ASSERT(sortedCheck(myVect, true));
        } else {
            TEST_ASSERT(sortedCheck(myVect, true, true));
        }
    } else {
        TEST_ASSERT(myVect.m_vectorType == Vector::DENSE_VECTOR);
        TEST_ASSERT(myVect.m_size == myVect.m_dimension);
        TEST_ASSERT(sortedCheck(myVect, true));
    }

    TEST_ASSERT(myVect.at(0) == 3);
    TEST_ASSERT(myVect.at(1) == 0);
    TEST_ASSERT(myVect.at(2) == 0);
    TEST_ASSERT(myVect.at(3) == -6);
    TEST_ASSERT(myVect.at(4) == 3);

    //Add sparse to dense

    myVectAdd.set(0, 0);
    myVectAdd.set(1, 0);
    myVectAdd.set(2, 0);
    myVectAdd.set(3, -3);
    myVectAdd.set(4, 0);
    myLambda = -2;

    //myVect: [3, 0, 0, -6, 3]
    //myVectAdd: [0, 0, 0, -3, 0]
    //myLambda * myVectAdd: [0, 0, 0, 6, 0]
    //Sum: [3, 0, 0, 0, 3]

    originalType = myVect.m_vectorType;

    myVect.addVector(myLambda, myVectAdd);
    TEST_ASSERT(integrityCheck(myVect));

    TEST_ASSERT(myVect.m_sparsityThreshold == testSparsityThreshold);
    TEST_ASSERT(myVect.m_sparsityRatio == SPARSITY_RATIO);
    TEST_ASSERT(myVect.m_nonZeros == 2);
    TEST_ASSERT(myVect.m_dimension == 5);
    TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - myVect.m_size);
    TEST_ASSERT(myVect.m_capacity >= myVect.m_size);
    if (myVect.m_nonZeros < myVect.m_sparsityThreshold) {
        TEST_ASSERT(myVect.m_vectorType == Vector::SPARSE_VECTOR);
        TEST_ASSERT(myVect.m_size == myVect.m_nonZeros);
        if (originalType == Vector::DENSE_VECTOR) {
            TEST_ASSERT(sortedCheck(myVect, true));
        } else {
            TEST_ASSERT(sortedCheck(myVect, true, true));
        }
    } else {
        TEST_ASSERT(myVect.m_vectorType == Vector::DENSE_VECTOR);
        TEST_ASSERT(myVect.m_size == myVect.m_dimension);
        TEST_ASSERT(sortedCheck(myVect, true));
    }

    TEST_ASSERT(myVect.at(0) == 3);
    TEST_ASSERT(myVect.at(1) == 0);
    TEST_ASSERT(myVect.at(2) == 0);
    TEST_ASSERT(myVect.at(3) == 0);
    TEST_ASSERT(myVect.at(4) == 3);

    //Should switch to sparse

    myVectAdd.set(0, 0);
    myVectAdd.set(1, 0);
    myVectAdd.set(2, 0);
    myVectAdd.set(3, 0);
    myVectAdd.set(4, -3);
    myLambda = 1;

    //myVect: [3, 0, 0, 0, 3]
    //myVectAdd: [0, 0, 0, 0, -3]
    //myLambda * myVectAdd: [0, 0, 0, 0, -3]
    //Sum: [3, 0, 0, 0, 0]

    originalType = myVect.m_vectorType;

    myVect.addVector(myLambda, myVectAdd);
    TEST_ASSERT(integrityCheck(myVect));

    TEST_ASSERT(myVect.m_sparsityThreshold == testSparsityThreshold);
    TEST_ASSERT(myVect.m_sparsityRatio == SPARSITY_RATIO);
    TEST_ASSERT(myVect.m_nonZeros == 1);
    TEST_ASSERT(myVect.m_dimension == 5);
    TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - myVect.m_size);
    TEST_ASSERT(myVect.m_capacity >= myVect.m_size);
    if (myVect.m_nonZeros < myVect.m_sparsityThreshold) {
        TEST_ASSERT(myVect.m_vectorType == Vector::SPARSE_VECTOR);
        TEST_ASSERT(myVect.m_size == myVect.m_nonZeros);
        if (originalType == Vector::DENSE_VECTOR) {
            TEST_ASSERT(sortedCheck(myVect, true));
        } else {
            TEST_ASSERT(sortedCheck(myVect, true, true));
        }
    } else {
        TEST_ASSERT(myVect.m_vectorType == Vector::DENSE_VECTOR);
        TEST_ASSERT(myVect.m_size == myVect.m_dimension);
        TEST_ASSERT(sortedCheck(myVect, true));
    }

    TEST_ASSERT(myVect.at(0) == 3);
    TEST_ASSERT(myVect.at(1) == 0);
    TEST_ASSERT(myVect.at(2) == 0);
    TEST_ASSERT(myVect.at(3) == 0);
    TEST_ASSERT(myVect.at(4) == 0);

    //Add sparse to sparse

    myVectAdd.set(0, 0);
    myVectAdd.set(1, 0);
    myVectAdd.set(2, 0);
    myVectAdd.set(3, 0);
    myVectAdd.set(4, -3);
    myLambda = 0;

    //myVect: [3, 0, 0, 0, 0]
    //myVectAdd: [0, 0, 0, 0, -3]
    //myLambda * myVectAdd: [0, 0, 0, 0, 0]
    //Sum: [3, 0, 0, 0, 0]

    originalType = myVect.m_vectorType;

    myVect.addVector(myLambda, myVectAdd);
    TEST_ASSERT(integrityCheck(myVect));

    TEST_ASSERT(myVect.m_sparsityThreshold == testSparsityThreshold);
    TEST_ASSERT(myVect.m_sparsityRatio == SPARSITY_RATIO);
    TEST_ASSERT(myVect.m_nonZeros == 1);
    TEST_ASSERT(myVect.m_dimension == 5);
    TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - myVect.m_size);
    TEST_ASSERT(myVect.m_capacity >= myVect.m_size);
    if (myVect.m_nonZeros < myVect.m_sparsityThreshold) {
        TEST_ASSERT(myVect.m_vectorType == Vector::SPARSE_VECTOR);
        TEST_ASSERT(myVect.m_size == myVect.m_nonZeros);
        if (originalType == Vector::DENSE_VECTOR) {
            TEST_ASSERT(sortedCheck(myVect, true));
        } else {
            TEST_ASSERT(sortedCheck(myVect, true, true));
        }
    } else {
        TEST_ASSERT(myVect.m_vectorType == Vector::DENSE_VECTOR);
        TEST_ASSERT(myVect.m_size == myVect.m_dimension);
        TEST_ASSERT(sortedCheck(myVect, true));
    }

    TEST_ASSERT(myVect.at(0) == 3);
    TEST_ASSERT(myVect.at(1) == 0);
    TEST_ASSERT(myVect.at(2) == 0);
    TEST_ASSERT(myVect.at(3) == 0);
    TEST_ASSERT(myVect.at(4) == 0);

    //Should switch to dense

    myVectAdd.set(0, 0);
    myVectAdd.set(1, 2);
    myVectAdd.set(2, 0);
    myVectAdd.set(3, 0);
    myVectAdd.set(4, 0);
    myLambda = 2;

    //myVect: [3, 0, 0, 0, 0]
    //myVectAdd: [0, 2, 0, 0, 0]
    //myLambda * myVectAdd: [0, 4, 0, 0, 0]
    //Sum: [3, 4, 0, 0, 0]

    originalType = myVect.m_vectorType;

    myVect.addVector(myLambda, myVectAdd);
    TEST_ASSERT(integrityCheck(myVect));

    TEST_ASSERT(myVect.m_sparsityThreshold == testSparsityThreshold);
    TEST_ASSERT(myVect.m_sparsityRatio == SPARSITY_RATIO);
    TEST_ASSERT(myVect.m_nonZeros == 2);
    TEST_ASSERT(myVect.m_dimension == 5);
    TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - myVect.m_size);
    TEST_ASSERT(myVect.m_capacity >= myVect.m_size);
    if (myVect.m_nonZeros < myVect.m_sparsityThreshold) {
        TEST_ASSERT(myVect.m_vectorType == Vector::SPARSE_VECTOR);
        TEST_ASSERT(myVect.m_size == myVect.m_nonZeros);
        if (originalType == Vector::DENSE_VECTOR) {
            TEST_ASSERT(sortedCheck(myVect, true));
        } else {
            TEST_ASSERT(sortedCheck(myVect, true, true));
        }
    } else {
        TEST_ASSERT(myVect.m_vectorType == Vector::DENSE_VECTOR);
        TEST_ASSERT(myVect.m_size == myVect.m_dimension);
        TEST_ASSERT(sortedCheck(myVect, true));
    }

    TEST_ASSERT(myVect.at(0) == 3);
    TEST_ASSERT(myVect.at(1) == 4);
    TEST_ASSERT(myVect.at(2) == 0);
    TEST_ASSERT(myVect.at(3) == 0);
    TEST_ASSERT(myVect.at(4) == 0);

    myVectAdd.set(0, 1.5);
    myVectAdd.set(1, 2);
    myVectAdd.set(2, 0);
    myVectAdd.set(3, 0);
    myVectAdd.set(4, 0);
    myLambda = -2;

    originalType = myVect.m_vectorType;

    myVect.addVector(myLambda, myVectAdd);
    TEST_ASSERT(integrityCheck(myVect));

    TEST_ASSERT(myVect.m_sparsityThreshold == testSparsityThreshold);
    TEST_ASSERT(myVect.m_sparsityRatio == SPARSITY_RATIO);
    TEST_ASSERT(myVect.m_nonZeros == 0);
    TEST_ASSERT(myVect.m_dimension == 5);
    TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - myVect.m_size);
    TEST_ASSERT(myVect.m_capacity >= myVect.m_size);
    if (myVect.m_nonZeros < myVect.m_sparsityThreshold) {
        TEST_ASSERT(myVect.m_vectorType == Vector::SPARSE_VECTOR);
        TEST_ASSERT(myVect.m_size == myVect.m_nonZeros);
        if (originalType == Vector::DENSE_VECTOR) {
            TEST_ASSERT(sortedCheck(myVect, true));
        } else {
            TEST_ASSERT(sortedCheck(myVect, true, true));
        }
    } else {
        TEST_ASSERT(myVect.m_vectorType == Vector::DENSE_VECTOR);
        TEST_ASSERT(myVect.m_size == myVect.m_dimension);
        TEST_ASSERT(sortedCheck(myVect, true));
    }

    TEST_ASSERT(myVect.at(0) == 0);
    TEST_ASSERT(myVect.at(1) == 0);
    TEST_ASSERT(myVect.at(2) == 0);
    TEST_ASSERT(myVect.at(3) == 0);
    TEST_ASSERT(myVect.at(4) == 0);

    originalType = myVect.m_vectorType;

    myVect.addVector(myLambda, myVectAdd);
    TEST_ASSERT(integrityCheck(myVect));

    TEST_ASSERT(myVect.m_sparsityThreshold == testSparsityThreshold);
    TEST_ASSERT(myVect.m_sparsityRatio == SPARSITY_RATIO);
    TEST_ASSERT(myVect.m_nonZeros == 2);
    TEST_ASSERT(myVect.m_dimension == 5);
    TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - myVect.m_size);
    TEST_ASSERT(myVect.m_capacity >= myVect.m_size);
    if (myVect.m_nonZeros < myVect.m_sparsityThreshold) {
        TEST_ASSERT(myVect.m_vectorType == Vector::SPARSE_VECTOR);
        TEST_ASSERT(myVect.m_size == myVect.m_nonZeros);
        if (originalType == Vector::DENSE_VECTOR) {
            TEST_ASSERT(sortedCheck(myVect, true));
        } else {
            TEST_ASSERT(sortedCheck(myVect, true, true));
        }
    } else {
        TEST_ASSERT(myVect.m_vectorType == Vector::DENSE_VECTOR);
        TEST_ASSERT(myVect.m_size == myVect.m_dimension);
        TEST_ASSERT(sortedCheck(myVect, true));
    }

    TEST_ASSERT(myVect.at(0) == -3);
    TEST_ASSERT(myVect.at(1) == -4);
    TEST_ASSERT(myVect.at(2) == 0);
    TEST_ASSERT(myVect.at(3) == 0);
    TEST_ASSERT(myVect.at(4) == 0);
}

void VectorTestSuite::dotProduct()
{

    //Two sparse vectors

    Vector myVect(5);
    myVect.set(3, 6);
    Vector mySecondVect(myVect);

    //myVect: [0, 0, 0, 6, 0]
    //mySecondVect: [0, 0, 0, 6, 0]
    //Dot product: 6 * 6 = 36

    TEST_ASSERT(myVect.dotProduct(mySecondVect) == 36);
    TEST_ASSERT(integrityCheck(myVect));

    //Two dense vectors

    myVect.set(1, 2);
    myVect.set(2, 5);
    myVect.set(4, 7);
    mySecondVect.set(0, 3);
    mySecondVect.set(1, 5);
    mySecondVect.set(4, 1);

    //myVect: [0, 2, 5, 6, 7]
    //mySecondVect: [3, 5, 0, 6, 1]
    //Dot product: 2 * 5 + 6 * 6 + 7 * 1 = 53

    TEST_ASSERT(myVect.dotProduct(mySecondVect) == 53);
    TEST_ASSERT(integrityCheck(myVect));

    //Different vector types

    mySecondVect.set(0, 0);
    mySecondVect.set(3, 0);
    mySecondVect.set(4, 0);

    //myVect: [0, 2, 5, 6, 7]
    //mySecondVect: [0, 5, 0, 0, 0]
    //Dot product: 2 * 5 = 10

    TEST_ASSERT(myVect.dotProduct(mySecondVect) == 10);
    TEST_ASSERT(integrityCheck(myVect));
}

void VectorTestSuite::euclidNorm()
{

    //Case of sparse vector

    Vector myVect(5);
    myVect.set(1, 4);

    TEST_ASSERT(myVect.euclidNorm() == 4);
    TEST_ASSERT(integrityCheck(myVect));
    //Case of dense vector

    myVect.set(3, 3);
    myVect.set(4, 12);

    //myVect: [0, 4, 0, 3, 12]
    //16 (4^2) + 9 (3^2) + 144 (12^2) = 169 (13^2)

    TEST_ASSERT(myVect.euclidNorm() == 13);
    TEST_ASSERT(integrityCheck(myVect));
}

void VectorTestSuite::elementaryFtran()
{

    Vector myVect(5);
    myVect.set(0, 4);
    myVect.set(1, 2);
    myVect.set(2, 8);
    myVect.set(3, -2);
    myVect.set(4, 3);
    Vector myEta(5);
    myEta.set(1, 6);
    myEta.set(2, 2);
    myEta.set(3, 9);
    myEta.set(4, -2);
    unsigned int myPivot;
    myPivot = 1;

    //myVect: [4, 2, 8, -2, 3]
    //myEta: [0, 6, 2, 9, -2]
    //FTRAN:
    //                   [  4
    //                      2
    //                      8
    //                     -2
    //                      3  ]
    // +--+--+--+--+--+
    // | 1| 0| 0| 0| 0|  [  4
    // +--+--+--+--+--+
    // | 0| 6| 0| 0| 0|    12
    // +--+--+--+--+--+
    // | 0| 2| 1| 0| 0|    12
    // +--+--+--+--+--+
    // | 0| 9| 0| 1| 0|    16
    // +--+--+--+--+--+
    // | 0|-2| 0| 0| 1|    -1  ]
    // +--+--+--+--+--+

    const unsigned int testSparsityThreshold = (unsigned int) Numerical::round(
        (myVect.m_dimension) * SPARSITY_RATIO);

    myVect.elementaryFtran(myEta, myPivot);
    TEST_ASSERT(integrityCheck(myVect));
    TEST_ASSERT(myVect.m_dimension == 5);
    TEST_ASSERT(myVect.m_sparsityThreshold == testSparsityThreshold);
    TEST_ASSERT(myVect.m_nonZeros == 5);
    TEST_ASSERT(myVect.m_sparsityRatio == SPARSITY_RATIO);
    TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - myVect.m_size);
    TEST_ASSERT(myVect.m_capacity >= myVect.m_size);

    if (myVect.m_nonZeros < myVect.m_sparsityThreshold) {
        TEST_ASSERT(myVect.m_vectorType == Vector::SPARSE_VECTOR);
        TEST_ASSERT(sortedCheck(myVect, true, true));
        TEST_ASSERT(myVect.m_size == myVect.m_nonZeros);
    } else {
        TEST_ASSERT(myVect.m_vectorType == Vector::DENSE_VECTOR);
        TEST_ASSERT(sortedCheck(myVect, true));
        TEST_ASSERT(myVect.m_size == myVect.m_dimension);
    }

    TEST_ASSERT(myVect.at(0) == 4);
    TEST_ASSERT(myVect.at(1) == 12);
    TEST_ASSERT(myVect.at(2) == 12);
    TEST_ASSERT(myVect.at(3) == 16);
    TEST_ASSERT(myVect.at(4) == -1);

    //Should switch to sparse

    myEta.set(1, -3);
    myEta.set(2, -3);
    myEta.set(3, -4);
    myEta.set(4, 1);
    myPivot = 0;


    //myVect: [4, 12, 12, 16, -1]
    //myEta: [0, -3, -3, -4, 1]
    //FTRAN:
    //                   [  4
    //                     12
    //                     12
    //                     16
    //                     -1  ]
    // +--+--+--+--+--+
    // | 0| 0| 0| 0| 0|  [  0
    // +--+--+--+--+--+
    // |-3| 1| 0| 0| 0|     0
    // +--+--+--+--+--+
    // |-3| 0| 1| 0| 0|     0
    // +--+--+--+--+--+
    // |-4| 0| 0| 1| 0|     0
    // +--+--+--+--+--+
    // | 1| 0| 0| 0| 1|     3  ]
    // +--+--+--+--+--+

    myVect.elementaryFtran(myEta, myPivot);
    TEST_ASSERT(integrityCheck(myVect));

    TEST_ASSERT(myVect.m_dimension == 5);
    TEST_ASSERT(myVect.m_sparsityThreshold == testSparsityThreshold);
    TEST_ASSERT(myVect.m_nonZeros == 1);
    TEST_ASSERT(myVect.m_sparsityRatio == SPARSITY_RATIO);
    TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - myVect.m_size);
    TEST_ASSERT(myVect.m_capacity >= myVect.m_size);

    if (myVect.m_nonZeros < myVect.m_sparsityThreshold) {
        TEST_ASSERT(myVect.m_vectorType == Vector::SPARSE_VECTOR);
        TEST_ASSERT(sortedCheck(myVect, true, true));
        TEST_ASSERT(myVect.m_size == myVect.m_nonZeros);
    } else {
        TEST_ASSERT(myVect.m_vectorType == Vector::DENSE_VECTOR);
        TEST_ASSERT(sortedCheck(myVect, true));
        TEST_ASSERT(myVect.m_size == myVect.m_dimension);
    }

    TEST_ASSERT(myVect.at(0) == 0);
    TEST_ASSERT(myVect.at(1) == 0);
    TEST_ASSERT(myVect.at(2) == 0);
    TEST_ASSERT(myVect.at(3) == 0);
    TEST_ASSERT(myVect.at(4) == 3);

    //Should switch to dense

    myEta.set(0, 2);
    myEta.set(1, -5);
    myEta.set(2, 1);
    myEta.set(3, 0);
    myEta.set(4, 1);
    myPivot = 4;

    //myVect: [0, 0, 0, 0, 3]
    //myEta: [2, -5, 1, 0, 1]
    //FTRAN:
    //                   [ 0
    //                     0
    //                     0
    //                     0
    //                     3 ]
    // +--+--+--+--+--+
    // | 1| 0| 0| 0| 2|  [  6
    // +--+--+--+--+--+
    // | 0| 1| 0| 0|-5|   -15
    // +--+--+--+--+--+
    // | 0| 0| 1| 0| 1|     3
    // +--+--+--+--+--+
    // | 0| 0| 0| 1| 0|     0
    // +--+--+--+--+--+
    // | 0| 0| 0| 0| 1|     3  ]
    // +--+--+--+--+--+

    myVect.elementaryFtran(myEta, myPivot);
    TEST_ASSERT(integrityCheck(myVect));

    TEST_ASSERT(myVect.m_dimension == 5);
    TEST_ASSERT(myVect.m_sparsityThreshold == testSparsityThreshold);
    TEST_ASSERT(myVect.m_nonZeros == 4);
    TEST_ASSERT(myVect.m_sparsityRatio == SPARSITY_RATIO);
    TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - myVect.m_size);
    TEST_ASSERT(myVect.m_capacity >= myVect.m_size);

    if (myVect.m_nonZeros < myVect.m_sparsityThreshold) {
        TEST_ASSERT(myVect.m_vectorType == Vector::SPARSE_VECTOR);
        TEST_ASSERT(sortedCheck(myVect, true, true));
        TEST_ASSERT(myVect.m_size == myVect.m_nonZeros);
    } else {
        TEST_ASSERT(myVect.m_vectorType == Vector::DENSE_VECTOR);
        TEST_ASSERT(sortedCheck(myVect, true));
        TEST_ASSERT(myVect.m_size == myVect.m_dimension);
    }

    TEST_ASSERT(myVect.at(0) == 6);
    TEST_ASSERT(myVect.at(1) == -15);
    TEST_ASSERT(myVect.at(2) == 3);
    TEST_ASSERT(myVect.at(3) == 0);
    TEST_ASSERT(myVect.at(4) == 3);

    myEta.set(0, -2);
    myEta.set(1, 5);
    myEta.set(2, 1);
    myEta.set(3, 0);
    myEta.set(4, -1);
    myPivot = 2;

    //myVect: [6, -15, 3, 0, 3]
    //myEta:  [-2, 5, 1, 0, -1]
    //FTRAN:
    //                   [ 6
    //                     -15
    //                     3
    //                     0
    //                     3 ]
    // +--+--+--+--+--+
    // | 1| 0|-2| 0| 0|  [  0
    // +--+--+--+--+--+
    // | 0| 1| 5| 0| 0|     0
    // +--+--+--+--+--+
    // | 0| 0| 1| 0| 0|     3
    // +--+--+--+--+--+
    // | 0| 0| 0| 1| 0|     0
    // +--+--+--+--+--+
    // | 0| 0|-1| 0| 1|     0  ]
    // +--+--+--+--+--+

    myVect.elementaryFtran(myEta, myPivot);
    TEST_ASSERT(integrityCheck(myVect));

    TEST_ASSERT(myVect.m_dimension == 5);
    TEST_ASSERT(myVect.m_sparsityThreshold == testSparsityThreshold);
    TEST_ASSERT(myVect.m_nonZeros == 1);
    TEST_ASSERT(myVect.m_sparsityRatio == SPARSITY_RATIO);
    TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - myVect.m_size);
    TEST_ASSERT(myVect.m_capacity >= myVect.m_size);

    if (myVect.m_nonZeros < myVect.m_sparsityThreshold) {
        TEST_ASSERT(myVect.m_vectorType == Vector::SPARSE_VECTOR);
        TEST_ASSERT(sortedCheck(myVect, true, true));
        TEST_ASSERT(myVect.m_size == myVect.m_nonZeros);
    } else {
        TEST_ASSERT(myVect.m_vectorType == Vector::DENSE_VECTOR);
        TEST_ASSERT(sortedCheck(myVect, true));
        TEST_ASSERT(myVect.m_size == myVect.m_dimension);
    }

    TEST_ASSERT(myVect.at(0) == 0);
    TEST_ASSERT(myVect.at(1) == 0);
    TEST_ASSERT(myVect.at(2) == 3);
    TEST_ASSERT(myVect.at(3) == 0);
    TEST_ASSERT(myVect.at(4) == 0);
}

void VectorTestSuite::elementaryBtran()
{

    Vector myVect(5);
    myVect.set(0, 1);
    myVect.set(2, 4);
    Vector myEta(5);
    myEta.set(0, 3);
    myEta.set(1, -3);
    myEta.set(2, 6);
    myEta.set(3, 6);
    myEta.set(4, 6);
    unsigned int myPivot;
    myPivot = 2;

    const unsigned int testSparsityThreshold = (unsigned int) Numerical::round(
        (myVect.m_dimension) * SPARSITY_RATIO);

    //myVect: [1, 0, 4, 0, 0]
    //myEta: [3, -3, 6, 6, 6]
    //BTRAN:
    //                    +--+--+--+--+--+
    //                    | 1| 0| 3| 0| 0|
    //                    +--+--+--+--+--+
    //                    | 0| 1|-3| 0| 0|
    //                    +--+--+--+--+--+
    //                    | 0| 0| 6| 0| 0|
    //                    +--+--+--+--+--+
    //                    | 0| 0| 6| 1| 0|
    //                    +--+--+--+--+--+
    //                    | 0| 0| 6| 0| 1|
    //                    +--+--+--+--+--+
    //
    // [ 1  0  4  0  0]   [ 1  0 27  0  0]

    myVect.elementaryBtran(myEta, myPivot);
    TEST_ASSERT(integrityCheck(myVect));

    TEST_ASSERT(myVect.m_dimension == 5);
    TEST_ASSERT(myVect.m_sparsityThreshold == testSparsityThreshold);
    TEST_ASSERT(myVect.m_nonZeros == 2);
    TEST_ASSERT(myVect.m_sparsityRatio == SPARSITY_RATIO);
    TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - myVect.m_size);
    TEST_ASSERT(myVect.m_capacity >= myVect.m_size);

    if (myVect.m_nonZeros < myVect.m_sparsityThreshold) {
        TEST_ASSERT(myVect.m_vectorType == Vector::SPARSE_VECTOR);
        TEST_ASSERT(sortedCheck(myVect, true, true));
        TEST_ASSERT(myVect.m_size == myVect.m_nonZeros);
    } else {
        TEST_ASSERT(myVect.m_vectorType == Vector::DENSE_VECTOR);
        TEST_ASSERT(sortedCheck(myVect, true));
        TEST_ASSERT(myVect.m_size == myVect.m_dimension);
    }

    TEST_ASSERT(myVect.at(0) == 1);
    TEST_ASSERT(myVect.at(1) == 0);
    TEST_ASSERT(myVect.at(2) == 27);
    TEST_ASSERT(myVect.at(3) == 0);
    TEST_ASSERT(myVect.at(4) == 0);

    //Should switch to sparse

    myEta.set(0, 0);
    myEta.set(1, 8);
    myEta.set(2, 0);
    myEta.set(3, -7);
    myEta.set(4, 0);
    myPivot = 0;

    //myVect: [1, 0, 27, 0, 0]
    //myEta: [0, 8, 0, -7, 0]
    //BTRAN:
    //                    +--+--+--+--+--+
    //                    | 0| 0| 0| 0| 0|
    //                    +--+--+--+--+--+
    //                    | 8| 1| 0| 0| 0|
    //                    +--+--+--+--+--+
    //                    | 0| 0| 1| 0| 0|
    //                    +--+--+--+--+--+
    //                    |-7| 0| 0| 1| 0|
    //                    +--+--+--+--+--+
    //                    | 0| 0| 0| 0| 1|
    //                    +--+--+--+--+--+
    //
    // [ 1  0 27  0  0]   [ 0  0 27  0  0]

    myVect.elementaryBtran(myEta, myPivot);
    TEST_ASSERT(integrityCheck(myVect));

    TEST_ASSERT(myVect.m_dimension == 5);
    TEST_ASSERT(myVect.m_sparsityThreshold == testSparsityThreshold);
    TEST_ASSERT(myVect.m_nonZeros == 1);
    TEST_ASSERT(myVect.m_sparsityRatio == SPARSITY_RATIO);
    TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - myVect.m_size);
    TEST_ASSERT(myVect.m_capacity >= myVect.m_size);

    if (myVect.m_nonZeros < myVect.m_sparsityThreshold) {
        TEST_ASSERT(myVect.m_vectorType == Vector::SPARSE_VECTOR);
        TEST_ASSERT(sortedCheck(myVect, true, true));
        TEST_ASSERT(myVect.m_size == myVect.m_nonZeros);
    } else {
        TEST_ASSERT(myVect.m_vectorType == Vector::DENSE_VECTOR);
        TEST_ASSERT(sortedCheck(myVect, true));
        TEST_ASSERT(myVect.m_size == myVect.m_dimension);
    }

    TEST_ASSERT(myVect.at(0) == 0);
    TEST_ASSERT(myVect.at(1) == 0);
    TEST_ASSERT(myVect.at(2) == 27);
    TEST_ASSERT(myVect.at(3) == 0);
    TEST_ASSERT(myVect.at(4) == 0);
}

void VectorTestSuite::plus()
{
    //Add dense to dense

    Vector myVect(5);
    myVect.set(0, 4);
    myVect.set(2, 2);
    myVect.set(3, 5);

    Vector mySecondVect(5);
    mySecondVect.set(1, 2);
    mySecondVect.set(2, -4);
    mySecondVect.set(3, 4);

    const unsigned int testSparsityThreshold = (unsigned int) Numerical::round(
        (myVect.m_dimension) * SPARSITY_RATIO);

    //myVect:       [4, 0, 2, 5, 0]
    //mySecondVect: [0, 2, -4, 4, 0]
    //Sum: [4, 2, -2, 9, 0]

    Vector::VECTOR_TYPE originalType = myVect.m_vectorType;

    myVect = myVect + mySecondVect;
    TEST_ASSERT(integrityCheck(myVect));

    //myVect:    [4, 0, 2, 5, 0]
    //myVectAdd: [0, 1, -2, 2, 0]
    //Sum: [4, 1, 0, 7, 0]

    TEST_ASSERT(myVect.m_sparsityThreshold == testSparsityThreshold);
    TEST_ASSERT(myVect.m_sparsityRatio == SPARSITY_RATIO);
    TEST_ASSERT(myVect.m_nonZeros == 4);
    TEST_ASSERT(myVect.m_dimension == 5);
    TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - myVect.m_size);
    TEST_ASSERT(myVect.m_capacity >= myVect.m_size);
    if (myVect.m_nonZeros < myVect.m_sparsityThreshold) {
        TEST_ASSERT(myVect.m_vectorType == Vector::SPARSE_VECTOR);
        TEST_ASSERT(myVect.m_size == myVect.m_nonZeros);
        if (originalType == Vector::DENSE_VECTOR) {
            TEST_ASSERT(sortedCheck(myVect, true));
        } else {
            TEST_ASSERT(sortedCheck(myVect, true, true));
        }
    } else {
        TEST_ASSERT(myVect.m_vectorType == Vector::DENSE_VECTOR);
        TEST_ASSERT(myVect.m_size == myVect.m_dimension);
        TEST_ASSERT(sortedCheck(myVect, true));
    }

    TEST_ASSERT(myVect.at(0) == 4);
    TEST_ASSERT(myVect.at(1) == 2);
    TEST_ASSERT(myVect.at(2) == -2);
    TEST_ASSERT(myVect.at(3) == 9);
    TEST_ASSERT(myVect.at(4) == 0);

    //Should switch to sparse

    mySecondVect.set(1, -2);
    mySecondVect.set(2, 2);
    mySecondVect.set(3, -9);

    //myVect: [4, 2, -2, 9, 0]
    //mySecondVect: [0, -2, 2, -9, 0]
    //Sum: [4, 0, 0, 0, 0]

    originalType = myVect.m_vectorType;

    myVect = myVect + mySecondVect;
    TEST_ASSERT(integrityCheck(myVect));

    TEST_ASSERT(myVect.m_sparsityThreshold == testSparsityThreshold);
    TEST_ASSERT(myVect.m_sparsityRatio == SPARSITY_RATIO);
    TEST_ASSERT(myVect.m_nonZeros == 1);
    TEST_ASSERT(myVect.m_dimension == 5);
    TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - myVect.m_size);
    TEST_ASSERT(myVect.m_capacity >= myVect.m_size);
    if (myVect.m_nonZeros < myVect.m_sparsityThreshold) {
        TEST_ASSERT(myVect.m_vectorType == Vector::SPARSE_VECTOR);
        TEST_ASSERT(myVect.m_size == myVect.m_nonZeros);
        if (originalType == Vector::DENSE_VECTOR) {
            TEST_ASSERT(sortedCheck(myVect, true));
        } else {
            TEST_ASSERT(sortedCheck(myVect, true, true));
        }
    } else {
        TEST_ASSERT(myVect.m_vectorType == Vector::DENSE_VECTOR);
        TEST_ASSERT(myVect.m_size == myVect.m_dimension);
        TEST_ASSERT(sortedCheck(myVect, true));
    }

    TEST_ASSERT(myVect.at(0) == 4);
    TEST_ASSERT(myVect.at(1) == 0);
    TEST_ASSERT(myVect.at(2) == 0);
    TEST_ASSERT(myVect.at(3) == 0);
    TEST_ASSERT(myVect.at(4) == 0);

    //Add dense to sparse

    mySecondVect.set(0, -4);
    mySecondVect.set(1, 0);
    mySecondVect.set(2, 0);
    mySecondVect.set(3, -9);
    mySecondVect.set(4, 0);

    //myVect: [4, 0, 0, 0, 0]
    //mySecondVect: [-4, 0, 0, -9, 0]
    //Sum: [0, 0, 0, -9, 0]

    originalType = myVect.m_vectorType;

    myVect = myVect + mySecondVect;
    TEST_ASSERT(integrityCheck(myVect));

    TEST_ASSERT(myVect.m_sparsityThreshold == testSparsityThreshold);
    TEST_ASSERT(myVect.m_sparsityRatio == SPARSITY_RATIO);
    TEST_ASSERT(myVect.m_nonZeros == 1);
    TEST_ASSERT(myVect.m_dimension == 5);
    TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - myVect.m_size);
    TEST_ASSERT(myVect.m_capacity >= myVect.m_size);
    if (myVect.m_nonZeros < myVect.m_sparsityThreshold) {
        TEST_ASSERT(myVect.m_vectorType == Vector::SPARSE_VECTOR);
        TEST_ASSERT(myVect.m_size == myVect.m_nonZeros);
        if (originalType == Vector::DENSE_VECTOR) {
            TEST_ASSERT(sortedCheck(myVect, true));
        } else {
            TEST_ASSERT(sortedCheck(myVect, true, true));
        }
    } else {
        TEST_ASSERT(myVect.m_vectorType == Vector::DENSE_VECTOR);
        TEST_ASSERT(myVect.m_size == myVect.m_dimension);
        TEST_ASSERT(sortedCheck(myVect, true));
    }

    TEST_ASSERT(myVect.at(0) == 0);
    TEST_ASSERT(myVect.at(1) == 0);
    TEST_ASSERT(myVect.at(2) == 0);
    TEST_ASSERT(myVect.at(3) == -9);
    TEST_ASSERT(myVect.at(4) == 0);

    //Should switch to dense

    mySecondVect.set(0, 3);
    mySecondVect.set(1, 0);
    mySecondVect.set(2, 0);
    mySecondVect.set(3, 3);
    mySecondVect.set(4, 3);

    //myVect: [0, 0, 0, -9, 0]
    //mySecondVect: [3, 0, 0, 3, 3]
    //Sum: [3, 0, 0, -6, 3]

    originalType = myVect.m_vectorType;

    myVect = myVect + mySecondVect;
    TEST_ASSERT(integrityCheck(myVect));

    TEST_ASSERT(myVect.m_sparsityThreshold == testSparsityThreshold);
    TEST_ASSERT(myVect.m_sparsityRatio == SPARSITY_RATIO);
    TEST_ASSERT(myVect.m_nonZeros == 3);
    TEST_ASSERT(myVect.m_dimension == 5);
    TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - myVect.m_size);
    TEST_ASSERT(myVect.m_capacity >= myVect.m_size);
    if (myVect.m_nonZeros < myVect.m_sparsityThreshold) {
        TEST_ASSERT(myVect.m_vectorType == Vector::SPARSE_VECTOR);
        TEST_ASSERT(myVect.m_size == myVect.m_nonZeros);
        if (originalType == Vector::DENSE_VECTOR) {
            TEST_ASSERT(sortedCheck(myVect, true));
        } else {
            TEST_ASSERT(sortedCheck(myVect, true, true));
        }
    } else {
        TEST_ASSERT(myVect.m_vectorType == Vector::DENSE_VECTOR);
        TEST_ASSERT(myVect.m_size == myVect.m_dimension);
        TEST_ASSERT(sortedCheck(myVect, true));
    }

    TEST_ASSERT(myVect.at(0) == 3);
    TEST_ASSERT(myVect.at(1) == 0);
    TEST_ASSERT(myVect.at(2) == 0);
    TEST_ASSERT(myVect.at(3) == -6);
    TEST_ASSERT(myVect.at(4) == 3);

    //Add sparse to dense

    mySecondVect.set(0, 0);
    mySecondVect.set(1, 0);
    mySecondVect.set(2, 0);
    mySecondVect.set(3, 6);
    mySecondVect.set(4, 0);

    //myVect: [3, 0, 0, -6, 3]
    //mySecondVect: [0, 0, 0, 6, 0]
    //Sum: [3, 0, 0, 0, 3]

    originalType = myVect.m_vectorType;

    myVect = myVect + mySecondVect;
    TEST_ASSERT(integrityCheck(myVect));

    TEST_ASSERT(myVect.m_sparsityThreshold == testSparsityThreshold);
    TEST_ASSERT(myVect.m_sparsityRatio == SPARSITY_RATIO);
    TEST_ASSERT(myVect.m_nonZeros == 2);
    TEST_ASSERT(myVect.m_dimension == 5);
    TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - myVect.m_size);
    TEST_ASSERT(myVect.m_capacity >= myVect.m_size);
    if (myVect.m_nonZeros < myVect.m_sparsityThreshold) {
        TEST_ASSERT(myVect.m_vectorType == Vector::SPARSE_VECTOR);
        TEST_ASSERT(myVect.m_size == myVect.m_nonZeros);
        if (originalType == Vector::DENSE_VECTOR) {
            TEST_ASSERT(sortedCheck(myVect, true));
        } else {
            TEST_ASSERT(sortedCheck(myVect, true, true));
        }
    } else {
        TEST_ASSERT(myVect.m_vectorType == Vector::DENSE_VECTOR);
        TEST_ASSERT(myVect.m_size == myVect.m_dimension);
        TEST_ASSERT(sortedCheck(myVect, true));
    }

    TEST_ASSERT(myVect.at(0) == 3);
    TEST_ASSERT(myVect.at(1) == 0);
    TEST_ASSERT(myVect.at(2) == 0);
    TEST_ASSERT(myVect.at(3) == 0);
    TEST_ASSERT(myVect.at(4) == 3);

    //Should switch to sparse

    mySecondVect.set(0, 0);
    mySecondVect.set(1, 0);
    mySecondVect.set(2, 0);
    mySecondVect.set(3, 0);
    mySecondVect.set(4, -3);

    //myVect: [3, 0, 0, 0, 3]
    //mySecondVect: [0, 0, 0, 0, -3]
    //Sum: [3, 0, 0, 0, 0]

    originalType = myVect.m_vectorType;

    myVect = myVect + mySecondVect;
    TEST_ASSERT(integrityCheck(myVect));

    TEST_ASSERT(myVect.m_sparsityThreshold == testSparsityThreshold);
    TEST_ASSERT(myVect.m_sparsityRatio == SPARSITY_RATIO);
    TEST_ASSERT(myVect.m_nonZeros == 1);
    TEST_ASSERT(myVect.m_dimension == 5);
    TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - myVect.m_size);
    TEST_ASSERT(myVect.m_capacity >= myVect.m_size);
    if (myVect.m_nonZeros < myVect.m_sparsityThreshold) {
        TEST_ASSERT(myVect.m_vectorType == Vector::SPARSE_VECTOR);
        TEST_ASSERT(myVect.m_size == myVect.m_nonZeros);
        if (originalType == Vector::DENSE_VECTOR) {
            TEST_ASSERT(sortedCheck(myVect, true));
        } else {
            TEST_ASSERT(sortedCheck(myVect, true, true));
        }
    } else {
        TEST_ASSERT(myVect.m_vectorType == Vector::DENSE_VECTOR);
        TEST_ASSERT(myVect.m_size == myVect.m_dimension);
        TEST_ASSERT(sortedCheck(myVect, true));
    }

    TEST_ASSERT(myVect.at(0) == 3);
    TEST_ASSERT(myVect.at(1) == 0);
    TEST_ASSERT(myVect.at(2) == 0);
    TEST_ASSERT(myVect.at(3) == 0);
    TEST_ASSERT(myVect.at(4) == 0);

    //Add sparse to sparse

    mySecondVect.set(0, 0);
    mySecondVect.set(1, 0);
    mySecondVect.set(2, 0);
    mySecondVect.set(3, 0);
    mySecondVect.set(4, -3);

    //myVect:    [3, 0, 0, 0, 0]
    //myVectAdd: [0, 0, 0, 0, -3]
    //myLambda * myVectAdd: [0, 0, 0, 0, 0]
    //Sum: [3, 0, 0, 0, -3]

    originalType = myVect.m_vectorType;

    myVect = myVect + mySecondVect;
    TEST_ASSERT(integrityCheck(myVect));

    TEST_ASSERT(myVect.m_sparsityThreshold == testSparsityThreshold);
    TEST_ASSERT(myVect.m_sparsityRatio == SPARSITY_RATIO);
    TEST_ASSERT(myVect.m_nonZeros == 2);
    TEST_ASSERT(myVect.m_dimension == 5);
    TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - myVect.m_size);
    TEST_ASSERT(myVect.m_capacity >= myVect.m_size);
    if (myVect.m_nonZeros < myVect.m_sparsityThreshold) {
        TEST_ASSERT(myVect.m_vectorType == Vector::SPARSE_VECTOR);
        TEST_ASSERT(myVect.m_size == myVect.m_nonZeros);
        if (originalType == Vector::DENSE_VECTOR) {
            TEST_ASSERT(sortedCheck(myVect, true));
        } else {
            TEST_ASSERT(sortedCheck(myVect, true, true));
        }
    } else {
        TEST_ASSERT(myVect.m_vectorType == Vector::DENSE_VECTOR);
        TEST_ASSERT(myVect.m_size == myVect.m_dimension);
        TEST_ASSERT(sortedCheck(myVect, true));
    }

    TEST_ASSERT(myVect.at(0) == 3);
    TEST_ASSERT(myVect.at(1) == 0);
    TEST_ASSERT(myVect.at(2) == 0);
    TEST_ASSERT(myVect.at(3) == 0);
    TEST_ASSERT(myVect.at(4) == -3);

    //Should switch to dense

    mySecondVect.set(0, 0);
    mySecondVect.set(1, 4);
    mySecondVect.set(2, 0);
    mySecondVect.set(3, 0);
    mySecondVect.set(4, 3);

    //myVect: [3, 0, 0, 0, -3]
    //myVectAdd: [0, 4, 0, 0, 3]
    //myLambda * myVectAdd: [0, 4, 0, 0, 0]
    //Sum: [3, 4, 0, 0, 0]

    originalType = myVect.m_vectorType;

    myVect = myVect + mySecondVect;
    TEST_ASSERT(integrityCheck(myVect));

    TEST_ASSERT(myVect.m_sparsityThreshold == testSparsityThreshold);
    TEST_ASSERT(myVect.m_sparsityRatio == SPARSITY_RATIO);
    TEST_ASSERT(myVect.m_nonZeros == 2);
    TEST_ASSERT(myVect.m_dimension == 5);
    TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - myVect.m_size);
    TEST_ASSERT(myVect.m_capacity >= myVect.m_size);
    if (myVect.m_nonZeros < myVect.m_sparsityThreshold) {
        TEST_ASSERT(myVect.m_vectorType == Vector::SPARSE_VECTOR);
        TEST_ASSERT(myVect.m_size == myVect.m_nonZeros);
        if (originalType == Vector::DENSE_VECTOR) {
            TEST_ASSERT(sortedCheck(myVect, true));
        } else {
            TEST_ASSERT(sortedCheck(myVect, true, true));
        }
    } else {
        TEST_ASSERT(myVect.m_vectorType == Vector::DENSE_VECTOR);
        TEST_ASSERT(myVect.m_size == myVect.m_dimension);
        TEST_ASSERT(sortedCheck(myVect, true));
    }

    TEST_ASSERT(myVect.at(0) == 3);
    TEST_ASSERT(myVect.at(1) == 4);
    TEST_ASSERT(myVect.at(2) == 0);
    TEST_ASSERT(myVect.at(3) == 0);
    TEST_ASSERT(myVect.at(4) == 0);

    mySecondVect.set(0, -3);
    mySecondVect.set(1, -4);
    mySecondVect.set(2, 0);
    mySecondVect.set(3, 0);
    mySecondVect.set(4, 0);

    originalType = myVect.m_vectorType;

    myVect = myVect + mySecondVect;
    TEST_ASSERT(integrityCheck(myVect));

    TEST_ASSERT(myVect.m_sparsityThreshold == testSparsityThreshold);
    TEST_ASSERT(myVect.m_sparsityRatio == SPARSITY_RATIO);
    TEST_ASSERT(myVect.m_nonZeros == 0);
    TEST_ASSERT(myVect.m_dimension == 5);
    TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - myVect.m_size);
    TEST_ASSERT(myVect.m_capacity >= myVect.m_size);
    if (myVect.m_nonZeros < myVect.m_sparsityThreshold) {
        TEST_ASSERT(myVect.m_vectorType == Vector::SPARSE_VECTOR);
        TEST_ASSERT(myVect.m_size == myVect.m_nonZeros);
        if (originalType == Vector::DENSE_VECTOR) {
            TEST_ASSERT(sortedCheck(myVect, true));
        } else {
            TEST_ASSERT(sortedCheck(myVect, true, true));
        }
    } else {
        TEST_ASSERT(myVect.m_vectorType == Vector::DENSE_VECTOR);
        TEST_ASSERT(myVect.m_size == myVect.m_dimension);
        TEST_ASSERT(sortedCheck(myVect, true));
    }

    TEST_ASSERT(myVect.at(0) == 0);
    TEST_ASSERT(myVect.at(1) == 0);
    TEST_ASSERT(myVect.at(2) == 0);
    TEST_ASSERT(myVect.at(3) == 0);
    TEST_ASSERT(myVect.at(4) == 0);

    originalType = myVect.m_vectorType;

    myVect = myVect + mySecondVect;
    TEST_ASSERT(integrityCheck(myVect));

    TEST_ASSERT(myVect.m_sparsityThreshold == testSparsityThreshold);
    TEST_ASSERT(myVect.m_sparsityRatio == SPARSITY_RATIO);
    TEST_ASSERT(myVect.m_nonZeros == 2);
    TEST_ASSERT(myVect.m_dimension == 5);
    TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - myVect.m_size);
    TEST_ASSERT(myVect.m_capacity >= myVect.m_size);
    if (myVect.m_nonZeros < myVect.m_sparsityThreshold) {
        TEST_ASSERT(myVect.m_vectorType == Vector::SPARSE_VECTOR);
        TEST_ASSERT(myVect.m_size == myVect.m_nonZeros);
        if (originalType == Vector::DENSE_VECTOR) {
            TEST_ASSERT(sortedCheck(myVect, true));
        } else {
            TEST_ASSERT(sortedCheck(myVect, true, true));
        }
    } else {
        TEST_ASSERT(myVect.m_vectorType == Vector::DENSE_VECTOR);
        TEST_ASSERT(myVect.m_size == myVect.m_dimension);
        TEST_ASSERT(sortedCheck(myVect, true));
    }

    TEST_ASSERT(myVect.at(0) == -3);
    TEST_ASSERT(myVect.at(1) == -4);
    TEST_ASSERT(myVect.at(2) == 0);
    TEST_ASSERT(myVect.at(3) == 0);
    TEST_ASSERT(myVect.at(4) == 0);
}

void VectorTestSuite::minus()
{

    Vector myVect(5);
    myVect.set(0, 4);
    myVect.set(2, 2);
    myVect.set(3, 5);

    Vector mySecondVect(5);
    mySecondVect.set(1, 2);
    mySecondVect.set(2, -4);
    mySecondVect.set(3, 4);

    const unsigned int testSparsityThreshold = (unsigned int) Numerical::round(
        (myVect.m_dimension) * SPARSITY_RATIO);

    //myVect:       [4, 0, 2, 5, 0]
    //mySecondVect: [0, 2, -4, 4, 0]
    //Sum:          [4, -2, 6, 1, 0]

    Vector::VECTOR_TYPE originalType = myVect.m_vectorType;

    myVect = myVect - mySecondVect;
    TEST_ASSERT(integrityCheck(myVect));

    TEST_ASSERT(myVect.m_sparsityThreshold == testSparsityThreshold);
    TEST_ASSERT(myVect.m_sparsityRatio == SPARSITY_RATIO);
    TEST_ASSERT(myVect.m_nonZeros == 4);
    TEST_ASSERT(myVect.m_dimension == 5);
    TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - myVect.m_size);
    TEST_ASSERT(myVect.m_capacity >= myVect.m_size);
    if (myVect.m_nonZeros < myVect.m_sparsityThreshold) {
        TEST_ASSERT(myVect.m_vectorType == Vector::SPARSE_VECTOR);
        TEST_ASSERT(myVect.m_size == myVect.m_nonZeros);
        if (originalType == Vector::DENSE_VECTOR) {
            TEST_ASSERT(sortedCheck(myVect, true));
        } else {
            TEST_ASSERT(sortedCheck(myVect, true, true));
        }
    } else {
        TEST_ASSERT(myVect.m_vectorType == Vector::DENSE_VECTOR);
        TEST_ASSERT(myVect.m_size == myVect.m_dimension);
        TEST_ASSERT(sortedCheck(myVect, true));
    }

    TEST_ASSERT(myVect.at(0) == 4);
    TEST_ASSERT(myVect.at(1) == -2);
    TEST_ASSERT(myVect.at(2) == 6);
    TEST_ASSERT(myVect.at(3) == 1);
    TEST_ASSERT(myVect.at(4) == 0);

    //Should switch to sparse

    mySecondVect.set(1, -2);
    mySecondVect.set(2, 2);
    mySecondVect.set(3, -9);

    //myVect:       [4, -2, 6, 1, 0]
    //mySecondVect: [0, -2, 2, -9, 0]
    //Sum:          [4, 0, 4, 10, 0]

    originalType = myVect.m_vectorType;

    myVect = myVect - mySecondVect;
    TEST_ASSERT(integrityCheck(myVect));

    TEST_ASSERT(myVect.m_sparsityThreshold == testSparsityThreshold);
    TEST_ASSERT(myVect.m_sparsityRatio == SPARSITY_RATIO);
    TEST_ASSERT(myVect.m_nonZeros == 3);
    TEST_ASSERT(myVect.m_dimension == 5);
    TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - myVect.m_size);
    TEST_ASSERT(myVect.m_capacity >= myVect.m_size);
    if (myVect.m_nonZeros < myVect.m_sparsityThreshold) {
        TEST_ASSERT(myVect.m_vectorType == Vector::SPARSE_VECTOR);
        TEST_ASSERT(myVect.m_size == myVect.m_nonZeros);
        if (originalType == Vector::DENSE_VECTOR) {
            TEST_ASSERT(sortedCheck(myVect, true));
        } else {
            TEST_ASSERT(sortedCheck(myVect, true, true));
        }
    } else {
        TEST_ASSERT(myVect.m_vectorType == Vector::DENSE_VECTOR);
        TEST_ASSERT(myVect.m_size == myVect.m_dimension);
        TEST_ASSERT(sortedCheck(myVect, true));
    }

    TEST_ASSERT(myVect.at(0) == 4);
    TEST_ASSERT(myVect.at(1) == 0);
    TEST_ASSERT(myVect.at(2) == 4);
    TEST_ASSERT(myVect.at(3) == 10);
    TEST_ASSERT(myVect.at(4) == 0);

    //Add dense to sparse

    mySecondVect.set(0, -4);
    mySecondVect.set(1, 0);
    mySecondVect.set(2, 0);
    mySecondVect.set(3, -9);
    mySecondVect.set(4, 0);

    //myVect:       [4,  0, 4, 10, 0]
    //mySecondVect: [-4, 0, 0, -9, 0]
    //Sum:          [8,  0, 4, 19, 0]

    originalType = myVect.m_vectorType;

    myVect = myVect - mySecondVect;
    TEST_ASSERT(integrityCheck(myVect));

    TEST_ASSERT(myVect.m_sparsityThreshold == testSparsityThreshold);
    TEST_ASSERT(myVect.m_sparsityRatio == SPARSITY_RATIO);
    TEST_ASSERT(myVect.m_nonZeros == 3);
    TEST_ASSERT(myVect.m_dimension == 5);
    TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - myVect.m_size);
    TEST_ASSERT(myVect.m_capacity >= myVect.m_size);
    if (myVect.m_nonZeros < myVect.m_sparsityThreshold) {
        TEST_ASSERT(myVect.m_vectorType == Vector::SPARSE_VECTOR);
        TEST_ASSERT(myVect.m_size == myVect.m_nonZeros);
        if (originalType == Vector::DENSE_VECTOR) {
            TEST_ASSERT(sortedCheck(myVect, true));
        } else {
            TEST_ASSERT(sortedCheck(myVect, true, true));
        }
    } else {
        TEST_ASSERT(myVect.m_vectorType == Vector::DENSE_VECTOR);
        TEST_ASSERT(myVect.m_size == myVect.m_dimension);
        TEST_ASSERT(sortedCheck(myVect, true));
    }

    TEST_ASSERT(myVect.at(0) == 8);
    TEST_ASSERT(myVect.at(1) == 0);
    TEST_ASSERT(myVect.at(2) == 4);
    TEST_ASSERT(myVect.at(3) == 19);
    TEST_ASSERT(myVect.at(4) == 0);

    //Should switch to dense

    mySecondVect.set(0, 3);
    mySecondVect.set(1, 0);
    mySecondVect.set(2, 0);
    mySecondVect.set(3, 3);
    mySecondVect.set(4, 3);

    //myVect:       [8, 0, 4, 19, 0]
    //mySecondVect: [3, 0, 0, 3, 3]
    //Sum:          [5, 0, 4, 16, -3]

    originalType = myVect.m_vectorType;

    myVect = myVect - mySecondVect;
    TEST_ASSERT(integrityCheck(myVect));

    TEST_ASSERT(myVect.m_sparsityThreshold == testSparsityThreshold);
    TEST_ASSERT(myVect.m_sparsityRatio == SPARSITY_RATIO);
    TEST_ASSERT(myVect.m_nonZeros == 4);
    TEST_ASSERT(myVect.m_dimension == 5);
    TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - myVect.m_size);
    TEST_ASSERT(myVect.m_capacity >= myVect.m_size);
    if (myVect.m_nonZeros < myVect.m_sparsityThreshold) {
        TEST_ASSERT(myVect.m_vectorType == Vector::SPARSE_VECTOR);
        TEST_ASSERT(myVect.m_size == myVect.m_nonZeros);
        if (originalType == Vector::DENSE_VECTOR) {
            TEST_ASSERT(sortedCheck(myVect, true));
        } else {
            TEST_ASSERT(sortedCheck(myVect, true, true));
        }
    } else {
        TEST_ASSERT(myVect.m_vectorType == Vector::DENSE_VECTOR);
        TEST_ASSERT(myVect.m_size == myVect.m_dimension);
        TEST_ASSERT(sortedCheck(myVect, true));
    }

    TEST_ASSERT(myVect.at(0) == 5);
    TEST_ASSERT(myVect.at(1) == 0);
    TEST_ASSERT(myVect.at(2) == 4);
    TEST_ASSERT(myVect.at(3) == 16);
    TEST_ASSERT(myVect.at(4) == -3);

    //Add sparse to dense

    mySecondVect.set(0, 0);
    mySecondVect.set(1, 0);
    mySecondVect.set(2, 0);
    mySecondVect.set(3, 6);
    mySecondVect.set(4, 0);

    //myVect:       [5, 0, 4, 16, -3]
    //mySecondVect: [0, 0, 0, 6, 0]
    //Sum:          [5, 0, 4, 10, -3]

    originalType = myVect.m_vectorType;

    myVect = myVect - mySecondVect;
    TEST_ASSERT(integrityCheck(myVect));

    TEST_ASSERT(myVect.m_sparsityThreshold == testSparsityThreshold);
    TEST_ASSERT(myVect.m_sparsityRatio == SPARSITY_RATIO);
    TEST_ASSERT(myVect.m_nonZeros == 4);
    TEST_ASSERT(myVect.m_dimension == 5);
    TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - myVect.m_size);
    TEST_ASSERT(myVect.m_capacity >= myVect.m_size);
    if (myVect.m_nonZeros < myVect.m_sparsityThreshold) {
        TEST_ASSERT(myVect.m_vectorType == Vector::SPARSE_VECTOR);
        TEST_ASSERT(myVect.m_size == myVect.m_nonZeros);
        if (originalType == Vector::DENSE_VECTOR) {
            TEST_ASSERT(sortedCheck(myVect, true));
        } else {
            TEST_ASSERT(sortedCheck(myVect, true, true));
        }
    } else {
        TEST_ASSERT(myVect.m_vectorType == Vector::DENSE_VECTOR);
        TEST_ASSERT(myVect.m_size == myVect.m_dimension);
        TEST_ASSERT(sortedCheck(myVect, true));
    }

    TEST_ASSERT(myVect.at(0) == 5);
    TEST_ASSERT(myVect.at(1) == 0);
    TEST_ASSERT(myVect.at(2) == 4);
    TEST_ASSERT(myVect.at(3) == 10);
    TEST_ASSERT(myVect.at(4) == -3);

    //Should switch to sparse

    mySecondVect.set(0, 0);
    mySecondVect.set(1, 0);
    mySecondVect.set(2, 0);
    mySecondVect.set(3, 0);
    mySecondVect.set(4, -3);

    //myVect:       [5, 0, 4, 10, -3]
    //mySecondVect: [0, 0, 0, 0, -3]
    //Sum:          [5, 0, 4, 10, 0]

    originalType = myVect.m_vectorType;

    myVect = myVect - mySecondVect;
    TEST_ASSERT(integrityCheck(myVect));

    TEST_ASSERT(myVect.m_sparsityThreshold == testSparsityThreshold);
    TEST_ASSERT(myVect.m_sparsityRatio == SPARSITY_RATIO);
    TEST_ASSERT(myVect.m_nonZeros == 3);
    TEST_ASSERT(myVect.m_dimension == 5);
    TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - myVect.m_size);
    TEST_ASSERT(myVect.m_capacity >= myVect.m_size);
    if (myVect.m_nonZeros < myVect.m_sparsityThreshold) {
        TEST_ASSERT(myVect.m_vectorType == Vector::SPARSE_VECTOR);
        TEST_ASSERT(myVect.m_size == myVect.m_nonZeros);
        if (originalType == Vector::DENSE_VECTOR) {
            TEST_ASSERT(sortedCheck(myVect, true));
        } else {
            TEST_ASSERT(sortedCheck(myVect, true, true));
        }
    } else {
        TEST_ASSERT(myVect.m_vectorType == Vector::DENSE_VECTOR);
        TEST_ASSERT(myVect.m_size == myVect.m_dimension);
        TEST_ASSERT(sortedCheck(myVect, true));
    }

    TEST_ASSERT(myVect.at(0) == 5);
    TEST_ASSERT(myVect.at(1) == 0);
    TEST_ASSERT(myVect.at(2) == 4);
    TEST_ASSERT(myVect.at(3) == 10);
    TEST_ASSERT(myVect.at(4) == 0);

    //Add sparse to sparse

    mySecondVect.set(0, 0);
    mySecondVect.set(1, 0);
    mySecondVect.set(2, 0);
    mySecondVect.set(3, 0);
    mySecondVect.set(4, -3);

    //myVect:    [5, 0, 4, 10, 0]
    //myVectAdd: [0, 0, 0, 0, -3]
    //Sum:       [5, 0, 4, 10, 3]

    originalType = myVect.m_vectorType;

    myVect = myVect - mySecondVect;
    TEST_ASSERT(integrityCheck(myVect));

    TEST_ASSERT(myVect.m_sparsityThreshold == testSparsityThreshold);
    TEST_ASSERT(myVect.m_sparsityRatio == SPARSITY_RATIO);
    TEST_ASSERT(myVect.m_nonZeros == 4);
    TEST_ASSERT(myVect.m_dimension == 5);
    TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - myVect.m_size);
    TEST_ASSERT(myVect.m_capacity >= myVect.m_size);
    if (myVect.m_nonZeros < myVect.m_sparsityThreshold) {
        TEST_ASSERT(myVect.m_vectorType == Vector::SPARSE_VECTOR);
        TEST_ASSERT(myVect.m_size == myVect.m_nonZeros);
        if (originalType == Vector::DENSE_VECTOR) {
            TEST_ASSERT(sortedCheck(myVect, true));
        } else {
            TEST_ASSERT(sortedCheck(myVect, true, true));
        }
    } else {
        TEST_ASSERT(myVect.m_vectorType == Vector::DENSE_VECTOR);
        TEST_ASSERT(myVect.m_size == myVect.m_dimension);
        TEST_ASSERT(sortedCheck(myVect, true));
    }

    TEST_ASSERT(myVect.at(0) == 5);
    TEST_ASSERT(myVect.at(1) == 0);
    TEST_ASSERT(myVect.at(2) == 4);
    TEST_ASSERT(myVect.at(3) == 10);
    TEST_ASSERT(myVect.at(4) == 3);

    //Should switch to dense

    mySecondVect.set(0, 0);
    mySecondVect.set(1, 4);
    mySecondVect.set(2, 0);
    mySecondVect.set(3, 0);
    mySecondVect.set(4, 3);

    //myVect:    [5, 0, 4, 10, 3]
    //myVectAdd: [0, 4, 0, 0, 3]
    //Sum:       [5, -4, 4, 10, 0]

    originalType = myVect.m_vectorType;

    myVect = myVect - mySecondVect;
    TEST_ASSERT(integrityCheck(myVect));

    TEST_ASSERT(myVect.m_sparsityThreshold == testSparsityThreshold);
    TEST_ASSERT(myVect.m_sparsityRatio == SPARSITY_RATIO);
    TEST_ASSERT(myVect.m_nonZeros == 4);
    TEST_ASSERT(myVect.m_dimension == 5);
    TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - myVect.m_size);
    TEST_ASSERT(myVect.m_capacity >= myVect.m_size);
    if (myVect.m_nonZeros < myVect.m_sparsityThreshold) {
        TEST_ASSERT(myVect.m_vectorType == Vector::SPARSE_VECTOR);
        TEST_ASSERT(myVect.m_size == myVect.m_nonZeros);
        if (originalType == Vector::DENSE_VECTOR) {
            TEST_ASSERT(sortedCheck(myVect, true));
        } else {
            TEST_ASSERT(sortedCheck(myVect, true, true));
        }
    } else {
        TEST_ASSERT(myVect.m_vectorType == Vector::DENSE_VECTOR);
        TEST_ASSERT(myVect.m_size == myVect.m_dimension);
        TEST_ASSERT(sortedCheck(myVect, true));
    }

    TEST_ASSERT(myVect.at(0) == 5);
    TEST_ASSERT(myVect.at(1) == -4);
    TEST_ASSERT(myVect.at(2) == 4);
    TEST_ASSERT(myVect.at(3) == 10);
    TEST_ASSERT(myVect.at(4) == 0);

    mySecondVect.set(0, 5);
    mySecondVect.set(1, -4);
    mySecondVect.set(2, 4);
    mySecondVect.set(3, 10);
    mySecondVect.set(4, 0);

    originalType = myVect.m_vectorType;

    myVect = myVect - mySecondVect;
    TEST_ASSERT(integrityCheck(myVect));

    TEST_ASSERT(myVect.m_sparsityThreshold == testSparsityThreshold);
    TEST_ASSERT(myVect.m_sparsityRatio == SPARSITY_RATIO);
    TEST_ASSERT(myVect.m_nonZeros == 0);
    TEST_ASSERT(myVect.m_dimension == 5);
    TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - myVect.m_size);
    TEST_ASSERT(myVect.m_capacity >= myVect.m_size);
    if (myVect.m_nonZeros < myVect.m_sparsityThreshold) {
        TEST_ASSERT(myVect.m_vectorType == Vector::SPARSE_VECTOR);
        TEST_ASSERT(myVect.m_size == myVect.m_nonZeros);
        if (originalType == Vector::DENSE_VECTOR) {
            TEST_ASSERT(sortedCheck(myVect, true));
        } else {
            TEST_ASSERT(sortedCheck(myVect, true, true));
        }
    } else {
        TEST_ASSERT(myVect.m_vectorType == Vector::DENSE_VECTOR);
        TEST_ASSERT(myVect.m_size == myVect.m_dimension);
        TEST_ASSERT(sortedCheck(myVect, true));
    }

    TEST_ASSERT(myVect.at(0) == 0);
    TEST_ASSERT(myVect.at(1) == 0);
    TEST_ASSERT(myVect.at(2) == 0);
    TEST_ASSERT(myVect.at(3) == 0);
    TEST_ASSERT(myVect.at(4) == 0);

    originalType = myVect.m_vectorType;

    myVect = myVect - mySecondVect;
    TEST_ASSERT(integrityCheck(myVect));

    TEST_ASSERT(myVect.m_sparsityThreshold == testSparsityThreshold);
    TEST_ASSERT(myVect.m_sparsityRatio == SPARSITY_RATIO);
    TEST_ASSERT(myVect.m_nonZeros == 4);
    TEST_ASSERT(myVect.m_dimension == 5);
    TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - myVect.m_size);
    TEST_ASSERT(myVect.m_capacity >= myVect.m_size);
    if (myVect.m_nonZeros < myVect.m_sparsityThreshold) {
        TEST_ASSERT(myVect.m_vectorType == Vector::SPARSE_VECTOR);
        TEST_ASSERT(myVect.m_size == myVect.m_nonZeros);
        if (originalType == Vector::DENSE_VECTOR) {
            TEST_ASSERT(sortedCheck(myVect, true));
        } else {
            TEST_ASSERT(sortedCheck(myVect, true, true));
        }
    } else {
        TEST_ASSERT(myVect.m_vectorType == Vector::DENSE_VECTOR);
        TEST_ASSERT(myVect.m_size == myVect.m_dimension);
        TEST_ASSERT(sortedCheck(myVect, true));
    }

    TEST_ASSERT(myVect.at(0) == -5);
    TEST_ASSERT(myVect.at(1) == 4);
    TEST_ASSERT(myVect.at(2) == -4);
    TEST_ASSERT(myVect.at(3) == -10);
    TEST_ASSERT(myVect.at(4) == 0);
}

void VectorTestSuite::asterisk()
{
    Vector myVect(5);
    myVect.set(1, 7);
    myVect.set(3, 5);
    myVect.set(4, 2);

    const unsigned int testSparsityThreshold = (unsigned int) Numerical::round(
        (myVect.m_dimension) * SPARSITY_RATIO);
    unsigned int capacity = myVect.m_capacity;
    bool sorted = myVect.m_sorted;

    const Numerical::Double multipliers[] = {-1, 1, 2, 3, 100, 0.25, -0.125, 0};
    const unsigned int count = STATIC_ARRAY_SIZE(multipliers);
    unsigned int index;
    for (index = 0; index < count; index++) {
        myVect.set(1, 7);
        myVect.set(3, 5);
        myVect.set(4, 2);

        Vector::VECTOR_TYPE originalType = myVect.m_vectorType;

        myVect = myVect * multipliers[index];
        TEST_ASSERT(integrityCheck(myVect));

        if (multipliers[index] == 0.0) {
            sorted = true;
        }

        TEST_ASSERT(myVect.m_sparsityThreshold == testSparsityThreshold);
        TEST_ASSERT(myVect.m_dimension == 5);
        TEST_ASSERT(myVect.m_nonZeros == (multipliers[index] == 0.0 ? 0 : 3));
        TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - myVect.m_size);
        if (myVect.m_nonZeros < myVect.m_sparsityThreshold) {
            if (originalType == Vector::DENSE_VECTOR) {
                capacity = ELBOWROOM;
                sorted = true;
            }
            TEST_ASSERT(myVect.m_vectorType == Vector::SPARSE_VECTOR);
            TEST_ASSERT(myVect.m_size == myVect.m_nonZeros);

        } else {
            TEST_ASSERT(myVect.m_vectorType == Vector::DENSE_VECTOR);
            TEST_ASSERT(myVect.m_size == myVect.m_dimension);

        }
        TEST_ASSERT(myVect.m_capacity == capacity);
        TEST_ASSERT(sortedCheck(myVect, sorted));

        TEST_ASSERT(myVect.at(0) == 0);
        TEST_ASSERT(myVect.at(1) == 7 * multipliers[index]);
        TEST_ASSERT(myVect.at(2) == 0);
        TEST_ASSERT(myVect.at(3) == 5 * multipliers[index]);
        TEST_ASSERT(myVect.at(4) == 2 * multipliers[index]);
    }
}

void VectorTestSuite::asterisk2()
{
    Vector myVect(5);
    myVect.set(0, 1);
    myVect.set(2, -4);
    myVect.set(3, 2);

    Matrix myMatrix(5, 5);

    myMatrix.set(0, 0, 2);
    myMatrix.set(0, 1, 0);
    myMatrix.set(0, 2, 4);
    myMatrix.set(0, 3, 10);
    myMatrix.set(0, 4, 1);

    myMatrix.set(1, 0, 9);
    myMatrix.set(1, 1, 1);
    myMatrix.set(1, 2, 0);
    myMatrix.set(1, 3, 0);
    myMatrix.set(1, 4, 1);

    myMatrix.set(2, 0, 0);
    myMatrix.set(2, 1, 0);
    myMatrix.set(2, 2, 1);
    myMatrix.set(2, 3, 0);
    myMatrix.set(2, 4, 1);

    myMatrix.set(3, 0, -7);
    myMatrix.set(3, 1, -1);
    myMatrix.set(3, 2, 0);
    myMatrix.set(3, 3, 2);
    myMatrix.set(3, 4, 1);

    myMatrix.set(4, 0, 0);
    myMatrix.set(4, 1, 0);
    myMatrix.set(4, 2, 5);
    myMatrix.set(4, 3, -2);
    myMatrix.set(4, 4, 1);

    //MULTIPLICATION:
    //                    +--+--+--+--+--+
    //                    | 2| 0| 4|10| 1|
    //                    +--+--+--+--+--+
    //                    | 9| 1| 0| 0| 1|
    //                    +--+--+--+--+--+
    //                    | 0| 0| 1| 0| 1|
    //                    +--+--+--+--+--+
    //                    |-7|-1| 0| 2| 1|
    //                    +--+--+--+--+--+
    //                    | 0| 0| 5|-2| 1|
    //                    +--+--+--+--+--+
    //
    // [ 1  0 -4  2  0]   [-12 -2 0 14 -1]

    const unsigned int testSparsityThreshold = (unsigned int) Numerical::round(
        (myVect.m_dimension) * SPARSITY_RATIO);

    myVect = myVect * myMatrix;
    TEST_ASSERT(integrityCheck(myVect));

    TEST_ASSERT(myVect.m_dimension == 5);
    TEST_ASSERT(myVect.m_nonZeros == 4);
    TEST_ASSERT(myVect.m_sparsityThreshold == testSparsityThreshold);
    TEST_ASSERT(myVect.m_sparsityRatio == SPARSITY_RATIO);
    TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - myVect.m_size);
    TEST_ASSERT(myVect.m_capacity >= myVect.m_size);

    if (myVect.m_nonZeros < myVect.m_sparsityThreshold) {
        TEST_ASSERT(myVect.m_vectorType == Vector::SPARSE_VECTOR);
        TEST_ASSERT(sortedCheck(myVect, true, true));
        TEST_ASSERT(myVect.m_size == myVect.m_nonZeros);
    } else {
        TEST_ASSERT(myVect.m_vectorType == Vector::DENSE_VECTOR);
        TEST_ASSERT(sortedCheck(myVect, true));
        TEST_ASSERT(myVect.m_size == myVect.m_dimension);
    }

    TEST_ASSERT(myVect.at(0) == -12);
    TEST_ASSERT(myVect.at(1) == -2);
    TEST_ASSERT(myVect.at(2) == 0);
    TEST_ASSERT(myVect.at(3) == 14);
    TEST_ASSERT(myVect.at(4) == -1);

    //Should switch to sparse

    myMatrix.set(0, 0, 0);
    myMatrix.set(0, 1, 1);
    myMatrix.set(0, 2, 0);
    myMatrix.set(0, 3, 0);
    myMatrix.set(0, 4, 0);

    myMatrix.set(1, 0, 0);
    myMatrix.set(1, 1, 1);
    myMatrix.set(1, 2, 0);
    myMatrix.set(1, 3, 0);
    myMatrix.set(1, 4, 0);

    myMatrix.set(2, 0, 8);
    myMatrix.set(2, 1, -4);
    myMatrix.set(2, 2, 12);
    myMatrix.set(2, 3, 5);
    myMatrix.set(2, 4, 1);

    myMatrix.set(3, 0, 0);
    myMatrix.set(3, 1, 1);
    myMatrix.set(3, 2, 0);
    myMatrix.set(3, 3, 0);
    myMatrix.set(3, 4, 0);

    myMatrix.set(4, 0, 0);
    myMatrix.set(4, 1, 0);
    myMatrix.set(4, 2, 0);
    myMatrix.set(4, 3, 0);
    myMatrix.set(4, 4, -4);

    //MULTIPLICATION:
    //                    +--+--+--+--+--+
    //                    | 0| 1| 0| 0| 0|
    //                    +--+--+--+--+--+
    //                    | 0| 1| 0| 0| 0|
    //                    +--+--+--+--+--+
    //                    | 8|-4|12| 5| 1|
    //                    +--+--+--+--+--+
    //                    | 0| 1| 0| 0| 0|
    //                    +--+--+--+--+--+
    //                    | 0| 0| 0| 0|-4|
    //                    +--+--+--+--+--+
    //
    // [-12 -2 0 14 -1]   [ 0  0  0  0  4]

    myVect = myVect * myMatrix;
    TEST_ASSERT(integrityCheck(myVect));

    TEST_ASSERT(myVect.m_dimension == 5);
    TEST_ASSERT(myVect.m_nonZeros == 1);
    TEST_ASSERT(myVect.m_sparsityThreshold == testSparsityThreshold);
    TEST_ASSERT(myVect.m_sparsityRatio == SPARSITY_RATIO);
    TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - myVect.m_size);
    TEST_ASSERT(myVect.m_capacity >= myVect.m_size);

    if (myVect.m_nonZeros < myVect.m_sparsityThreshold) {
        TEST_ASSERT(myVect.m_vectorType == Vector::SPARSE_VECTOR);
        TEST_ASSERT(sortedCheck(myVect, true, true));
        TEST_ASSERT(myVect.m_size == myVect.m_nonZeros);
    } else {
        TEST_ASSERT(myVect.m_vectorType == Vector::DENSE_VECTOR);
        TEST_ASSERT(sortedCheck(myVect, true));
        TEST_ASSERT(myVect.m_size == myVect.m_dimension);
    }

    TEST_ASSERT(myVect.at(0) == 0);
    TEST_ASSERT(myVect.at(1) == 0);
    TEST_ASSERT(myVect.at(2) == 0);
    TEST_ASSERT(myVect.at(3) == 0);
    TEST_ASSERT(myVect.at(4) == 4);

    //Should switch to dense

    myMatrix.set(0, 0, 0);
    myMatrix.set(0, 1, 1);
    myMatrix.set(0, 2, 0);
    myMatrix.set(0, 3, 0);
    myMatrix.set(0, 4, 0);

    myMatrix.set(1, 0, 0);
    myMatrix.set(1, 1, 1);
    myMatrix.set(1, 2, 0);
    myMatrix.set(1, 3, -8);
    myMatrix.set(1, 4, 0);

    myMatrix.set(2, 0, 8);
    myMatrix.set(2, 1, -4);
    myMatrix.set(2, 2, 12);
    myMatrix.set(2, 3, 5);
    myMatrix.set(2, 4, 1);

    myMatrix.set(3, 0, 0);
    myMatrix.set(3, 1, 1);
    myMatrix.set(3, 2, 0);
    myMatrix.set(3, 3, 0);
    myMatrix.set(3, 4, 0);

    myMatrix.set(4, 0, 0);
    myMatrix.set(4, 1, 2);
    myMatrix.set(4, 2, 0);
    myMatrix.set(4, 3, 0);
    myMatrix.set(4, 4, -4);

    //MULTIPLICATION:
    //                    +--+--+--+--+--+
    //                    | 0| 1| 0| 0| 0|
    //                    +--+--+--+--+--+
    //                    | 0| 1| 0|-8| 0|
    //                    +--+--+--+--+--+
    //                    | 8|-4|12| 5| 1|
    //                    +--+--+--+--+--+
    //                    | 0| 1| 0| 0| 0|
    //                    +--+--+--+--+--+
    //                    | 0| 2| 0| 0|-4|
    //                    +--+--+--+--+--+
    //
    // [ 0  0  0  0  4]   [ 0  8  0  0-16]

    myVect = myVect * myMatrix;
    TEST_ASSERT(integrityCheck(myVect));

    TEST_ASSERT(myVect.m_dimension == 5);
    TEST_ASSERT(myVect.m_nonZeros == 2);
    TEST_ASSERT(myVect.m_sparsityThreshold == testSparsityThreshold);
    TEST_ASSERT(myVect.m_sparsityRatio == SPARSITY_RATIO);
    TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - myVect.m_size);
    TEST_ASSERT(myVect.m_capacity >= myVect.m_size);

    if (myVect.m_nonZeros < myVect.m_sparsityThreshold) {
        TEST_ASSERT(myVect.m_vectorType == Vector::SPARSE_VECTOR);
        TEST_ASSERT(sortedCheck(myVect, true, true));
        TEST_ASSERT(myVect.m_size == myVect.m_nonZeros);
    } else {
        TEST_ASSERT(myVect.m_vectorType == Vector::DENSE_VECTOR);
        TEST_ASSERT(sortedCheck(myVect, true));
        TEST_ASSERT(myVect.m_size == myVect.m_dimension);
    }

    TEST_ASSERT(myVect.at(0) == 0);
    TEST_ASSERT(myVect.at(1) == 8);
    TEST_ASSERT(myVect.at(2) == 0);
    TEST_ASSERT(myVect.at(3) == 0);
    TEST_ASSERT(myVect.at(4) == -16);
}

void VectorTestSuite::equal()
{
    Vector myVect(5);
    myVect.set(1, 3);
    Vector mySecondVect(2);
    mySecondVect.set(1, 3);
    Vector myThirdVect(5);
    myThirdVect.set(2, 5);
    Vector myFourthVect(myThirdVect);

    TEST_ASSERT(myVect == mySecondVect);
    TEST_ASSERT(myThirdVect != mySecondVect);
    TEST_ASSERT(myVect != myThirdVect);
    TEST_ASSERT(myThirdVect == myFourthVect);
}

void VectorTestSuite::sortElements()
{
    Vector myVect(100);
    Vector mySecondVect(100);
    Vector myThirdVect(100);
    Vector myFourthVect(1000);

    //Test selectionSort

    myVect.set(77, 4);
    myVect.set(44, 7);
    myVect.set(91, 23);
    myVect.set(5, 5);
    if (myVect.m_vectorType == Vector::SPARSE_VECTOR) {
        myVect.sortElements();
        TEST_ASSERT(integrityCheck(myVect));
        for (unsigned int index = 0; index < myVect.m_nonZeros - 1; index++) {
            TEST_ASSERT(myVect.m_index[index] < myVect.m_index[index + 1]);
        }
    }
    //Test insertionSort

    mySecondVect.set(77, 4);
    mySecondVect.set(44, 7);
    mySecondVect.set(91, 23);
    mySecondVect.set(5, 5);
    mySecondVect.set(69, 4);
    mySecondVect.set(3, 7);
    mySecondVect.set(1, 23);
    mySecondVect.set(49, 5);
    mySecondVect.set(0, 4);
    mySecondVect.set(81, 7);
    mySecondVect.set(92, 23);
    mySecondVect.set(7, 5);

    //insertion count: 72
    //counting count: 116
    //heap count: 96
    if (mySecondVect.m_vectorType == Vector::SPARSE_VECTOR) {
        mySecondVect.sortElements();
        TEST_ASSERT(integrityCheck(mySecondVect));
        for (unsigned int index = 0; index < mySecondVect.m_nonZeros - 1; index++) {
            TEST_ASSERT(mySecondVect.m_index[index] < mySecondVect.m_index[index + 1]);
        }
    }
    //Test countingSort

    myThirdVect.set(77, 4);
    myThirdVect.set(76, 7);
    myThirdVect.set(75, 23);
    myThirdVect.set(74, 5);
    myThirdVect.set(73, 4);
    myThirdVect.set(72, 7);
    myThirdVect.set(70, 23);
    myThirdVect.set(71, 5);
    myThirdVect.set(69, 4);
    myThirdVect.set(68, 7);
    myThirdVect.set(67, 23);
    myThirdVect.set(66, 5);
    myThirdVect.set(65, 4);
    myThirdVect.set(64, 7);
    myThirdVect.set(63, 23);
    myThirdVect.set(62, 5);
    myThirdVect.set(61, 4);
    myThirdVect.set(60, 7);
    myThirdVect.set(57, 23);
    myThirdVect.set(58, 5);
    myThirdVect.set(56, 4);
    myThirdVect.set(59, 7);
    myThirdVect.set(55, 23);
    myThirdVect.set(54, 5);

    //insertion count: 288
    //counting count: 71
    //heap count: 240
    if (myThirdVect.m_vectorType == Vector::SPARSE_VECTOR) {
        myThirdVect.sortElements();
        TEST_ASSERT(integrityCheck(myThirdVect));
        for (unsigned int index = 0; index < myThirdVect.m_nonZeros - 1; index++) {
            TEST_ASSERT(myThirdVect.m_index[index] < myThirdVect.m_index[index + 1]);
        }
    }
    //Test heapSort

    myFourthVect.set(77, 4);
    myFourthVect.set(44, 7);
    myFourthVect.set(0, 23);
    myFourthVect.set(5, 5);
    myFourthVect.set(69, 4);
    myFourthVect.set(3, 7);
    myFourthVect.set(1, 23);
    myFourthVect.set(49, 5);
    myFourthVect.set(53, 4);
    myFourthVect.set(81, 7);
    myFourthVect.set(92, 23);
    myFourthVect.set(7, 5);
    myFourthVect.set(8, 4);
    myFourthVect.set(9, 7);
    myFourthVect.set(10, 23);
    myFourthVect.set(34, 5);
    myFourthVect.set(24, 4);
    myFourthVect.set(91, 7);
    myFourthVect.set(87, 23);
    myFourthVect.set(999, 5);
    myFourthVect.set(12, 4);
    myFourthVect.set(68, 7);
    myFourthVect.set(89, 23);
    myFourthVect.set(21, 5);

    //insertion count: 288
    //counting count: 1047
    //heap count: 240

    myFourthVect.sortElements();
    TEST_ASSERT(integrityCheck(myFourthVect));
    if (myFourthVect.m_vectorType == Vector::SPARSE_VECTOR) {
        for (unsigned int index = 0; index < myFourthVect.m_nonZeros - 1; index++) {
            TEST_ASSERT(myFourthVect.m_index[index] < myFourthVect.m_index[index + 1]);
        }
    }
}

void VectorTestSuite::reInit()
{
    Vector myVect(5);
    unsigned int testDimensions[] = {0, 1, 2, 3, 4, 5, 10, 20, 50, 100};
    const unsigned int testDimensionCount = sizeof (testDimensions) / sizeof (unsigned int);
    unsigned int dimensionIndex;

    for (dimensionIndex = 0; dimensionIndex < testDimensionCount; dimensionIndex++) {
        const unsigned int actualDimension = testDimensions[dimensionIndex];

        myVect.reInit(actualDimension);
        TEST_ASSERT(integrityCheck(myVect));
        TEST_ASSERT(myVect.m_dimension == actualDimension);

        unsigned int testSparsityThreshold1 = (unsigned int) Numerical::round(
            myVect.m_dimension * SPARSITY_RATIO);
        TEST_ASSERT(myVect.m_sparsityThreshold == testSparsityThreshold1);
        TEST_ASSERT(myVect.m_nonZeros == 0);
        TEST_ASSERT(myVect.m_sparsityRatio == SPARSITY_RATIO);
        TEST_ASSERT(myVect.sm_fullLenghtReferenceCounter == 1);

        if (myVect.m_nonZeros < myVect.m_sparsityThreshold) {
            TEST_ASSERT(myVect.m_capacity == ELBOWROOM);
            TEST_ASSERT(myVect.m_data == myVect.m_dataEnd);
            TEST_ASSERT(sortedCheck(myVect, true));
            TEST_ASSERT(myVect.m_vectorType == Vector::SPARSE_VECTOR);
            TEST_ASSERT(myVect.m_size == 0);
        } else {
            TEST_ASSERT(myVect.m_capacity == myVect.m_dimension + ELBOWROOM);
            TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - myVect.m_dimension);
            TEST_ASSERT(sortedCheck(myVect, true));
            TEST_ASSERT(myVect.m_vectorType == Vector::DENSE_VECTOR);
            TEST_ASSERT(myVect.m_size == myVect.m_dimension);
        }

        unsigned int index;
        for (index = 0; index < actualDimension; index++) {
            TEST_ASSERT(myVect.at(index) == 0);
        }
    }
}

void VectorTestSuite::heap()
{
    unsigned int counter = Vector::sm_fullLenghtReferenceCounter;
    Vector * vec1 = new Vector(10);
    TEST_ASSERT(counter + 1 == Vector::sm_fullLenghtReferenceCounter);
    delete vec1;
    vec1 = 0;
    TEST_ASSERT(counter == Vector::sm_fullLenghtReferenceCounter);
    if (counter == 0) {
        TEST_ASSERT(Vector::sm_fullLengthVector == 0);
    } else {
        TEST_ASSERT(Vector::sm_fullLengthVector != 0);
    }
}

void VectorTestSuite::prepare()
{
    Vector myVect(10);
    myVect.set(1, 4);
    myVect.set(5, 2);

    unsigned int nonZeros;
    unsigned int dimension;

    for (nonZeros = 0; nonZeros < 10; nonZeros++) {
        for (dimension = 0; dimension < 10; dimension++) {
            myVect.prepareForData(nonZeros, nonZeros + dimension * dimension);
            unsigned int index;
            for (index = 0; index < nonZeros; index++) {
                myVect.newNonZero(index + nonZeros + dimension, index);
            }

            unsigned int testSparsityThreshold1 = (unsigned int) Numerical::round(
                myVect.m_dimension * SPARSITY_RATIO);

            TEST_ASSERT(integrityCheck(myVect));
            TEST_ASSERT(myVect.m_dimension == nonZeros + dimension * dimension);

            TEST_ASSERT(myVect.m_sparsityThreshold == testSparsityThreshold1);
            TEST_ASSERT(myVect.m_nonZeros == nonZeros);
            TEST_ASSERT(myVect.m_sparsityRatio == SPARSITY_RATIO);
            TEST_ASSERT(myVect.m_capacity == myVect.m_size + ELBOWROOM);
            TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - myVect.m_size);
            if (myVect.m_nonZeros < myVect.m_sparsityThreshold) {
                TEST_ASSERT(sortedCheck(myVect, false));
                TEST_ASSERT(myVect.m_vectorType == Vector::SPARSE_VECTOR);
                TEST_ASSERT(myVect.m_size == myVect.m_nonZeros);
            } else {
                TEST_ASSERT(sortedCheck(myVect, true));
                TEST_ASSERT(myVect.m_vectorType == Vector::DENSE_VECTOR);
                TEST_ASSERT(myVect.m_size == myVect.m_dimension);
            }
            for (index = 0; index < nonZeros; index++) {
                TEST_ASSERT(myVect.at(index) == index + nonZeros + dimension);
            }
        }
    }

}

void VectorTestSuite::iterators()
{
    Vector myVect(10);

    Vector::Iterator iter1 = myVect.begin();
    Vector::Iterator iter1End = myVect.end();
    unsigned int index;
    if (myVect.m_vectorType == Vector::SPARSE_VECTOR) {
        TEST_ASSERT(iter1 == iter1End);
        TEST_ASSERT(!(iter1 != iter1End));
        TEST_ASSERT(!(iter1 < iter1End));
        TEST_ASSERT(iter1 <= iter1End);
        TEST_ASSERT(iter1 >= iter1End);
        TEST_ASSERT(!(iter1 > iter1End));
    } else {
        for (index = 0; index < myVect.m_dimension; index++, iter1++) {
            TEST_ASSERT(!(iter1 == iter1End));
            TEST_ASSERT(iter1 != iter1End);
            TEST_ASSERT(iter1 <= iter1End);
            TEST_ASSERT(!(iter1 >= iter1End));
            TEST_ASSERT(iter1 < iter1End);
            TEST_ASSERT(!(iter1 > iter1End));
            TEST_ASSERT(*iter1 == 0.0);
            TEST_ASSERT(iter1.getIndex() == index);
        }
        TEST_ASSERT(iter1 == iter1End);
        TEST_ASSERT(!(iter1 != iter1End));
        TEST_ASSERT(!(iter1 < iter1End));
        TEST_ASSERT(iter1 <= iter1End);
        TEST_ASSERT(iter1 >= iter1End);
        TEST_ASSERT(!(iter1 > iter1End));
    }

    Vector::NonzeroIterator itern1 = myVect.beginNonzero();
    Vector::NonzeroIterator itern1End = myVect.endNonzero();
    TEST_ASSERT(itern1 == itern1End);
    TEST_ASSERT(!(itern1 != itern1End));
    TEST_ASSERT(!(itern1 < itern1End));
    TEST_ASSERT(itern1 <= itern1End);
    TEST_ASSERT(itern1 >= itern1End);
    TEST_ASSERT(!(itern1 > itern1End));

    myVect.set(3, 10);
    myVect.set(1, 2);
    myVect.set(8, 3);
    Vector::Iterator iter2 = myVect.begin();
    Vector::Iterator iter2End = myVect.end();
    if (myVect.m_vectorType == Vector::SPARSE_VECTOR) {
        unsigned int count = 0;
        for (; iter2 < iter2End; iter2++, count++) {
            TEST_ASSERT(!(iter2 == iter2End));
            TEST_ASSERT(iter2 != iter2End);
            TEST_ASSERT(iter2 <= iter2End);
            TEST_ASSERT(!(iter2 >= iter2End));
            TEST_ASSERT(iter2 < iter2End);
            TEST_ASSERT(!(iter2 > iter2End));
            if (iter2.getIndex() == 1) {
                TEST_ASSERT(*iter2 == 2.0);
            } else if (iter2.getIndex() == 3) {
                TEST_ASSERT(*iter2 == 10.0);
            } else if (iter2.getIndex() == 8) {
                TEST_ASSERT(*iter2 == 3.0);
            } else {
                TEST_ASSERT(iter2.getIndex() == 1 || iter2.getIndex() == 3 ||
                    iter2.getIndex() == 8);
            }
        }
        TEST_ASSERT(iter2 == iter2End);
        TEST_ASSERT(!(iter2 != iter2End));
        TEST_ASSERT(iter2 <= iter2End);
        TEST_ASSERT(iter2 >= iter2End);
        TEST_ASSERT(!(iter2 < iter2End));
        TEST_ASSERT(!(iter2 > iter2End));
        TEST_ASSERT(count == 3);
    } else {
        for (index = 0; index < myVect.m_dimension; index++, iter2++) {
            TEST_ASSERT(!(iter2 == iter2End));
            TEST_ASSERT(iter2 != iter2End);
            TEST_ASSERT(iter2 <= iter2End);
            TEST_ASSERT(!(iter2 >= iter2End));
            TEST_ASSERT(iter2 < iter2End);
            TEST_ASSERT(!(iter2 > iter2End));
            switch (index) {
                case 1:
                    TEST_ASSERT(*iter2 == 2.0);
                    break;
                case 3:
                    TEST_ASSERT(*iter2 == 10.0);
                    break;
                case 8:
                    TEST_ASSERT(*iter2 == 3.0);
                    break;
                default:
                    TEST_ASSERT(*iter2 == 0.0);
            }

            TEST_ASSERT(iter2.getIndex() == index);
        }
        TEST_ASSERT(iter2 == iter2End);
        TEST_ASSERT(!(iter2 != iter2End));
        TEST_ASSERT(iter2 <= iter2End);
        TEST_ASSERT(iter2 >= iter2End);
        TEST_ASSERT(!(iter2 < iter2End));
        TEST_ASSERT(!(iter2 > iter2End));
    }

    Vector::NonzeroIterator itern2 = myVect.beginNonzero();
    Vector::NonzeroIterator itern2End = myVect.endNonzero();
    if (myVect.m_vectorType == Vector::SPARSE_VECTOR) {
        unsigned int count = 0;
        for (; itern2 < itern2End; itern2++, count++) {
            TEST_ASSERT(!(itern2 == itern2End));
            TEST_ASSERT(itern2 != itern2End);
            TEST_ASSERT(itern2 <= itern2End);
            TEST_ASSERT(!(itern2 >= itern2End));
            TEST_ASSERT(itern2 < itern2End);
            TEST_ASSERT(!(itern2 > itern2End));
            if (itern2.getIndex() == 1) {
                TEST_ASSERT(*itern2 == 2.0);
            } else if (itern2.getIndex() == 3) {
                TEST_ASSERT(*itern2 == 10.0);
            } else if (itern2.getIndex() == 8) {
                TEST_ASSERT(*itern2 == 3.0);
            } else {
                EMIT_ERROR(Illegal nonzero iterator behaviour!);
            }
        }
        TEST_ASSERT(itern2 == itern2End);
        TEST_ASSERT(!(itern2 != itern2End));
        TEST_ASSERT(itern2 <= itern2End);
        TEST_ASSERT(itern2 >= itern2End);
        TEST_ASSERT(!(itern2 < itern2End));
        TEST_ASSERT(!(itern2 > itern2End));
        TEST_ASSERT(count == 3);
    } else {
        for (index = 0; index < myVect.m_nonZeros; index++, itern2++) {
            TEST_ASSERT(!(itern2 == itern2End));
            TEST_ASSERT(itern2 != itern2End);
            TEST_ASSERT(itern2 <= itern2End);
            TEST_ASSERT(!(itern2 >= itern2End));
            TEST_ASSERT(itern2 < itern2End);
            TEST_ASSERT(!(itern2 > itern2End));

            switch (itern2.getIndex()) {
                case 1:
                    TEST_ASSERT(*itern2 == 2.0);
                    break;
                case 3:
                    TEST_ASSERT(*itern2 == 10.0);
                    break;
                case 8:
                    TEST_ASSERT(*itern2 == 3.0);
                    break;
                default:
                    EMIT_ERROR(Illegal nonzero iterator behaviour!);
            }
        }
        TEST_ASSERT(itern2 == itern2End);
        TEST_ASSERT(!(itern2 != itern2End));
        TEST_ASSERT(itern2 <= itern2End);
        TEST_ASSERT(itern2 >= itern2End);
        TEST_ASSERT(!(itern2 < itern2End));
        TEST_ASSERT(!(itern2 > itern2End));
    }
}
