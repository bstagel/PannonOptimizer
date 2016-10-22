#include <sparsevectortest.h>
#include <linalg/sparsevector.h>
#include <linalg/densevector.h>
#include <linalg/indexeddensevector.h>

#define CHECK_FAST_ADD_FUNCTION_POINTERS \
{ \
    TEST_ASSERT_EQUALITY(SparseVector::sm_addDenseToSparse, &SparseVector::addDenseToSparseFast); \
    TEST_ASSERT_EQUALITY(SparseVector::sm_addSparseToSparse, &SparseVector::addSparseToSparseFast); \
    TEST_ASSERT_EQUALITY(SparseVector::sm_addIndexedDenseToSparse, &SparseVector::addIndexedDenseToSparseFast); \
    }

#define CHECK_ABS_ADD_FUNCTION_POINTERS \
{ \
    TEST_ASSERT_EQUALITY(SparseVector::sm_addDenseToSparse, &SparseVector::addDenseToSparseAbs); \
    TEST_ASSERT_EQUALITY(SparseVector::sm_addSparseToSparse, &SparseVector::addSparseToSparseAbs); \
    TEST_ASSERT_EQUALITY(SparseVector::sm_addIndexedDenseToSparse, &SparseVector::addIndexedDenseToSparseAbs); \
    }

#define CHECK_DEFAULT_ADD_FUNCTION_POINTERS \
{ \
    TEST_ASSERT_EQUALITY(SparseVector::sm_addDenseToSparse, &SparseVector::addDenseToSparseAbsRel); \
    TEST_ASSERT_EQUALITY(SparseVector::sm_addSparseToSparse, &SparseVector::addSparseToSparseAbsRel); \
    TEST_ASSERT_EQUALITY(SparseVector::sm_addIndexedDenseToSparse, &SparseVector::addIndexedDenseToSparseAbsRel); \
    }

#define CHECK_UNSTABLE_DOT_PRODUCT_FUNCTION_POINTERS \
{ \
    TEST_ASSERT_EQUALITY(SparseVector::sm_sparseToDenseDotProduct, &SparseVector::dotProductSparseToDenseUnstable); \
    TEST_ASSERT_EQUALITY(SparseVector::sm_sparseToIndexedDenseDotProduct, &SparseVector::dotProductSparseToIndexedDenseUnstable); \
    TEST_ASSERT_EQUALITY(SparseVector::sm_sparseToSparseDotProduct, &SparseVector::dotProductSparseToSparseUnstable); \
    }

#define CHECK_FAST_DOT_PRODUCT_FUNCTION_POINTERS \
{ \
    TEST_ASSERT_EQUALITY(SparseVector::sm_sparseToDenseDotProduct, &SparseVector::dotProductSparseToDenseFast); \
    TEST_ASSERT_EQUALITY(SparseVector::sm_sparseToIndexedDenseDotProduct, &SparseVector::dotProductSparseToIndexedDenseFast); \
    TEST_ASSERT_EQUALITY(SparseVector::sm_sparseToSparseDotProduct, &SparseVector::dotProductSparseToSparseFast); \
    }

#define CHECK_ABS_DOT_PRODUCT_FUNCTION_POINTERS \
{ \
    TEST_ASSERT_EQUALITY(SparseVector::sm_sparseToDenseDotProduct, &SparseVector::dotProductSparseToDenseAbs); \
    TEST_ASSERT_EQUALITY(SparseVector::sm_sparseToIndexedDenseDotProduct, &SparseVector::dotProductSparseToIndexedDenseAbs); \
    TEST_ASSERT_EQUALITY(SparseVector::sm_sparseToSparseDotProduct, &SparseVector::dotProductSparseToSparseAbs); \
    }

#define CHECK_DEFAULT_DOT_PRODUCT_FUNCTION_POINTERS \
{ \
    TEST_ASSERT_EQUALITY(SparseVector::sm_sparseToDenseDotProduct, &SparseVector::dotProductSparseToDenseAbsRel); \
    TEST_ASSERT_EQUALITY(SparseVector::sm_sparseToIndexedDenseDotProduct, &SparseVector::dotProductSparseToIndexedDenseAbsRel); \
    TEST_ASSERT_EQUALITY(SparseVector::sm_sparseToSparseDotProduct, &SparseVector::dotProductSparseToSparseAbsRel); \
    }

#define CHECK_DEFAULT_FUNCTION_POINTERS \
{ \
    CHECK_DEFAULT_ADD_FUNCTION_POINTERS; \
    CHECK_DEFAULT_DOT_PRODUCT_FUNCTION_POINTERS; \
    }

#define CHECK_CLEAN_AUXILIARY_VECTORS \
{ \
    unsigned int index; \
    for (index = 0; index < SparseVector::sm_fullLengthVectorLength; index++) {\
    TEST_ASSERT_EQUALITY(SparseVector::sm_fullLengthVector[index], 0.0); \
    TEST_ASSERT_EQUALITY(SparseVector::sm_indexPointerVector[index], nullptr); \
    } \
    TEST_ASSERT( (SparseVector::sm_fullLengthVectorLength == 0 && \
    SparseVector::sm_fullLengthVector == nullptr && \
    SparseVector::sm_indexPointerVector == nullptr && \
    SparseVector::sm_indexVector == nullptr && \
    SparseVector::sm_removeIndices == nullptr) || \
    (SparseVector::sm_fullLengthVectorLength > 0 && \
    SparseVector::sm_fullLengthVector != nullptr && \
    SparseVector::sm_indexPointerVector != nullptr && \
    SparseVector::sm_indexVector != nullptr && \
    SparseVector::sm_removeIndices != nullptr)); \
    }


SparseVectorTestSuite::SparseVectorTestSuite(const char *name):
    UnitTest(name)
{
    ADD_TEST(SparseVectorTestSuite::init);
    ADD_TEST(SparseVectorTestSuite::copy);
    ADD_TEST(SparseVectorTestSuite::move);
    ADD_TEST(SparseVectorTestSuite::assign);
    ADD_TEST(SparseVectorTestSuite::assignDense);
    ADD_TEST(SparseVectorTestSuite::moveAssign);
    ADD_TEST(SparseVectorTestSuite::createUnitVector);
    ADD_TEST(SparseVectorTestSuite::createVectorFromDenseArray);
    ADD_TEST(SparseVectorTestSuite::createVectorFromSparseArray);
    ADD_TEST(SparseVectorTestSuite::createEtaVector);
    ADD_TEST(SparseVectorTestSuite::at);
    ADD_TEST(SparseVectorTestSuite::arrayOperator);
    ADD_TEST(SparseVectorTestSuite::set);
    ADD_TEST(SparseVectorTestSuite::setNewNonzero);
    ADD_TEST(SparseVectorTestSuite::length);
    ADD_TEST(SparseVectorTestSuite::euclidNorm);
    ADD_TEST(SparseVectorTestSuite::l1Norm);
    ADD_TEST(SparseVectorTestSuite::elementaryFtran);
    //ADD_TEST(SparseVectorTestSuite::addSparseVector);
    ADD_TEST(SparseVectorTestSuite::addDenseVector);
    //ADD_TEST(SparseVectorTestSuite::addIndexedDenseVector);
    ADD_TEST(SparseVectorTestSuite::nonZeros);
    ADD_TEST(SparseVectorTestSuite::scale);
    ADD_TEST(SparseVectorTestSuite::sortIndices);
    ADD_TEST(SparseVectorTestSuite::clear);
    ADD_TEST(SparseVectorTestSuite::setAddMode);
    ADD_TEST(SparseVectorTestSuite::setDotProductMode);
    ADD_TEST(SparseVectorTestSuite::beginNonzero);
    ADD_TEST(SparseVectorTestSuite::endNonzero);
    ADD_TEST(SparseVectorTestSuite::iterator);
}

void SparseVectorTestSuite::init()
{
    SparseVector::_globalRelease();
    SparseVector::_globalInit();

    CHECK_DEFAULT_FUNCTION_POINTERS;
    CHECK_CLEAN_AUXILIARY_VECTORS;

    SparseVector v1(0);
    TEST_ASSERT_EQUALITY(v1.m_length, 0);
    TEST_ASSERT_EQUALITY(v1.m_nonZeros, 0);
    TEST_ASSERT_EQUALITY(v1.m_data, nullptr);
    TEST_ASSERT_EQUALITY(v1.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v1.m_capacity, 0);
    TEST_ASSERT_EQUALITY(SparseVector::sm_fullLengthVectorLength, 0);
    TEST_ASSERT_EQUALITY(SparseVector::sm_fullLengthVector, nullptr);
    TEST_ASSERT_EQUALITY(SparseVector::sm_indexPointerVector, nullptr);
    TEST_ASSERT_EQUALITY(SparseVector::sm_indexVector, nullptr);

    CHECK_DEFAULT_FUNCTION_POINTERS;
    CHECK_CLEAN_AUXILIARY_VECTORS;

    SparseVector v2(1);
    TEST_ASSERT_EQUALITY(v2.m_length, 1);
    TEST_ASSERT_EQUALITY(v2.m_nonZeros, 0);
    TEST_ASSERT_EQUALITY(v2.m_data, nullptr);
    TEST_ASSERT_EQUALITY(v2.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v2.m_capacity, 0);
    TEST_ASSERT_EQUALITY(SparseVector::sm_fullLengthVectorLength, 1);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_fullLengthVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexPointerVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexVector, nullptr);

    CHECK_DEFAULT_FUNCTION_POINTERS;
    CHECK_CLEAN_AUXILIARY_VECTORS;

    SparseVector v3(2);
    TEST_ASSERT_EQUALITY(v3.m_length, 2);
    TEST_ASSERT_EQUALITY(v3.m_nonZeros, 0);
    TEST_ASSERT_EQUALITY(v3.m_data, nullptr);
    TEST_ASSERT_EQUALITY(v3.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v3.m_capacity, 0);
    TEST_ASSERT_EQUALITY(SparseVector::sm_fullLengthVectorLength, 2);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_fullLengthVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexPointerVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexVector, nullptr);

    CHECK_DEFAULT_FUNCTION_POINTERS;
    CHECK_CLEAN_AUXILIARY_VECTORS;

    SparseVector v4(10);
    TEST_ASSERT_EQUALITY(v4.m_length, 10);
    TEST_ASSERT_EQUALITY(v4.m_nonZeros, 0);
    TEST_ASSERT_EQUALITY(v4.m_data, nullptr);
    TEST_ASSERT_EQUALITY(v4.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v4.m_capacity, 0);
    TEST_ASSERT_EQUALITY(SparseVector::sm_fullLengthVectorLength, 10);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_fullLengthVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexPointerVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexVector, nullptr);

    CHECK_DEFAULT_FUNCTION_POINTERS;
    CHECK_CLEAN_AUXILIARY_VECTORS;

    SparseVector v5(5);
    TEST_ASSERT_EQUALITY(v5.m_length, 5);
    TEST_ASSERT_EQUALITY(v5.m_nonZeros, 0);
    TEST_ASSERT_EQUALITY(v5.m_data, nullptr);
    TEST_ASSERT_EQUALITY(v5.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v5.m_capacity, 0);
    TEST_ASSERT_EQUALITY(SparseVector::sm_fullLengthVectorLength, 10);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_fullLengthVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexPointerVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexVector, nullptr);

    CHECK_DEFAULT_FUNCTION_POINTERS;
    CHECK_CLEAN_AUXILIARY_VECTORS;

    SparseVector::setAddMode(Numerical::ADD_ABS);
    SparseVector::setDotProductMode(Numerical::DOT_ABS);

    SparseVector v6(20);
    TEST_ASSERT_EQUALITY(v6.m_length, 20);
    TEST_ASSERT_EQUALITY(v6.m_nonZeros, 0);
    TEST_ASSERT_EQUALITY(v6.m_data, nullptr);
    TEST_ASSERT_EQUALITY(v6.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v6.m_capacity, 0);
    TEST_ASSERT_EQUALITY(SparseVector::sm_fullLengthVectorLength, 20);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_fullLengthVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexPointerVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexVector, nullptr);

    TEST_ASSERT_EQUALITY(SparseVector::sm_sparseToDenseDotProduct, &SparseVector::dotProductSparseToDenseAbs );
    TEST_ASSERT_EQUALITY(SparseVector::sm_sparseToIndexedDenseDotProduct, &SparseVector::dotProductSparseToIndexedDenseAbs );
    TEST_ASSERT_EQUALITY(SparseVector::sm_sparseToSparseDotProduct, &SparseVector::dotProductSparseToSparseAbs);
    TEST_ASSERT_EQUALITY(SparseVector::sm_addDenseToSparse, &SparseVector::addDenseToSparseAbs);
    TEST_ASSERT_EQUALITY(SparseVector::sm_addSparseToSparse, &SparseVector::addSparseToSparseAbs);
    TEST_ASSERT_EQUALITY(SparseVector::sm_addIndexedDenseToSparse, &SparseVector::addIndexedDenseToSparseAbs);

    CHECK_CLEAN_AUXILIARY_VECTORS;

    SparseVector::_globalRelease();
}

void SparseVectorTestSuite::copy()
{
    SparseVector::_globalRelease();
    SparseVector::_globalInit();

    SparseVector v1(0);
    SparseVector v1_c = v1;

    TEST_ASSERT_EQUALITY(v1.m_length, 0);
    TEST_ASSERT_EQUALITY(v1.m_nonZeros, 0);
    TEST_ASSERT_EQUALITY(v1.m_data, nullptr);
    TEST_ASSERT_EQUALITY(v1.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v1.m_capacity, 0);
    TEST_ASSERT_EQUALITY(v1_c.m_length, 0);
    TEST_ASSERT_EQUALITY(v1_c.m_nonZeros, 0);
    TEST_ASSERT_EQUALITY(v1_c.m_data, nullptr);
    TEST_ASSERT_EQUALITY(v1_c.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v1_c.m_capacity, 0);
    TEST_ASSERT_EQUALITY(SparseVector::sm_fullLengthVectorLength, 0);
    TEST_ASSERT_EQUALITY(SparseVector::sm_fullLengthVector, nullptr);
    TEST_ASSERT_EQUALITY(SparseVector::sm_indexPointerVector, nullptr);
    TEST_ASSERT_EQUALITY(SparseVector::sm_indexVector, nullptr);

    CHECK_DEFAULT_FUNCTION_POINTERS;
    CHECK_CLEAN_AUXILIARY_VECTORS;

    SparseVector v2(1);
    SparseVector v2_c = v2;
    TEST_ASSERT_EQUALITY(v2.m_length, 1);
    TEST_ASSERT_EQUALITY(v2.m_nonZeros, 0);
    TEST_ASSERT_EQUALITY(v2.m_data, nullptr);
    TEST_ASSERT_EQUALITY(v2.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v2.m_capacity, 0);
    TEST_ASSERT_EQUALITY(v2_c.m_length, 1);
    TEST_ASSERT_EQUALITY(v2_c.m_nonZeros, 0);
    TEST_ASSERT_EQUALITY(v2_c.m_data, nullptr);
    TEST_ASSERT_EQUALITY(v2_c.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v2_c.m_capacity, 0);
    TEST_ASSERT_EQUALITY(SparseVector::sm_fullLengthVectorLength, 1);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_fullLengthVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexPointerVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexVector, nullptr);

    CHECK_DEFAULT_FUNCTION_POINTERS;
    CHECK_CLEAN_AUXILIARY_VECTORS;

    SparseVector v3(2);
    SparseVector v3_c = v3;
    TEST_ASSERT_EQUALITY(v3.m_length, 2);
    TEST_ASSERT_EQUALITY(v3.m_nonZeros, 0);
    TEST_ASSERT_EQUALITY(v3.m_data, nullptr);
    TEST_ASSERT_EQUALITY(v3.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v3.m_capacity, 0);
    TEST_ASSERT_EQUALITY(v3_c.m_length, 2);
    TEST_ASSERT_EQUALITY(v3_c.m_nonZeros, 0);
    TEST_ASSERT_EQUALITY(v3_c.m_data, nullptr);
    TEST_ASSERT_EQUALITY(v3_c.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v3_c.m_capacity, 0);
    TEST_ASSERT_EQUALITY(SparseVector::sm_fullLengthVectorLength, 2);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_fullLengthVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexPointerVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexVector, nullptr);

    CHECK_DEFAULT_FUNCTION_POINTERS;
    CHECK_CLEAN_AUXILIARY_VECTORS;

    SparseVector v4(10);
    SparseVector v4_c = v4;
    TEST_ASSERT_EQUALITY(v4.m_length, 10);
    TEST_ASSERT_EQUALITY(v4.m_nonZeros, 0);
    TEST_ASSERT_EQUALITY(v4.m_data, nullptr);
    TEST_ASSERT_EQUALITY(v4.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v4.m_capacity, 0);
    TEST_ASSERT_EQUALITY(v4_c.m_length, 10);
    TEST_ASSERT_EQUALITY(v4_c.m_nonZeros, 0);
    TEST_ASSERT_EQUALITY(v4_c.m_data, nullptr);
    TEST_ASSERT_EQUALITY(v4_c.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v4_c.m_capacity, 0);
    TEST_ASSERT_EQUALITY(SparseVector::sm_fullLengthVectorLength, 10);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_fullLengthVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexPointerVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexVector, nullptr);

    CHECK_DEFAULT_FUNCTION_POINTERS;
    CHECK_CLEAN_AUXILIARY_VECTORS;

    SparseVector v5(5);
    SparseVector v5_c = v5;
    TEST_ASSERT_EQUALITY(v5.m_length, 5);
    TEST_ASSERT_EQUALITY(v5.m_nonZeros, 0);
    TEST_ASSERT_EQUALITY(v5.m_data, nullptr);
    TEST_ASSERT_EQUALITY(v5.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v5.m_capacity, 0);
    TEST_ASSERT_EQUALITY(v5_c.m_length, 5);
    TEST_ASSERT_EQUALITY(v5_c.m_nonZeros, 0);
    TEST_ASSERT_EQUALITY(v5_c.m_data, nullptr);
    TEST_ASSERT_EQUALITY(v5_c.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v5_c.m_capacity, 0);
    TEST_ASSERT_EQUALITY(SparseVector::sm_fullLengthVectorLength, 10);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_fullLengthVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexPointerVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexVector, nullptr);

    CHECK_DEFAULT_FUNCTION_POINTERS;
    CHECK_CLEAN_AUXILIARY_VECTORS;

    SparseVector::setAddMode(Numerical::ADD_ABS);
    SparseVector::setDotProductMode(Numerical::DOT_ABS);

    SparseVector v6(20);
    SparseVector v6_c = v6;
    TEST_ASSERT_EQUALITY(v6.m_length, 20);
    TEST_ASSERT_EQUALITY(v6.m_nonZeros, 0);
    TEST_ASSERT_EQUALITY(v6.m_data, nullptr);
    TEST_ASSERT_EQUALITY(v6.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v6.m_capacity, 0);
    TEST_ASSERT_EQUALITY(v6_c.m_length, 20);
    TEST_ASSERT_EQUALITY(v6_c.m_nonZeros, 0);
    TEST_ASSERT_EQUALITY(v6_c.m_data, nullptr);
    TEST_ASSERT_EQUALITY(v6_c.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v6_c.m_capacity, 0);
    TEST_ASSERT_EQUALITY(SparseVector::sm_fullLengthVectorLength, 20);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_fullLengthVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexPointerVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexVector, nullptr);

    TEST_ASSERT_EQUALITY(SparseVector::sm_sparseToDenseDotProduct, &SparseVector::dotProductSparseToDenseAbs);
    TEST_ASSERT_EQUALITY(SparseVector::sm_sparseToIndexedDenseDotProduct, &SparseVector::dotProductSparseToIndexedDenseAbs);
    TEST_ASSERT_EQUALITY(SparseVector::sm_sparseToSparseDotProduct, &SparseVector::dotProductSparseToSparseAbs);
    TEST_ASSERT_EQUALITY(SparseVector::sm_addDenseToSparse, &SparseVector::addDenseToSparseAbs);
    TEST_ASSERT_EQUALITY(SparseVector::sm_addSparseToSparse, &SparseVector::addSparseToSparseAbs);
    TEST_ASSERT_EQUALITY(SparseVector::sm_addIndexedDenseToSparse, &SparseVector::addIndexedDenseToSparseAbs);

    CHECK_CLEAN_AUXILIARY_VECTORS;

    SparseVector v7(1);
    v7.set(0, 4.2);
    SparseVector v7_c = v7;
    TEST_ASSERT_EQUALITY(v7.m_length, 1);
    TEST_ASSERT_EQUALITY(v7.m_nonZeros, 1);
    TEST_ASSERT_INEQUALITY(v7.m_data, nullptr);
    TEST_ASSERT_INEQUALITY(v7.m_indices, nullptr);
    TEST_ASSERT_GREATER_OR_EQUAL(v7.m_capacity, 1);
    TEST_ASSERT_EQUALITY(v7_c.m_length, 1);
    TEST_ASSERT_EQUALITY(v7_c.m_nonZeros, 1);
    TEST_ASSERT_INEQUALITY(v7_c.m_data, nullptr);
    TEST_ASSERT_INEQUALITY(v7_c.m_indices, nullptr);
    TEST_ASSERT_GREATER_OR_EQUAL(v7_c.m_capacity, 1);
    TEST_ASSERT_INEQUALITY(v7.m_data, v7_c.m_data);
    TEST_ASSERT_INEQUALITY(v7.m_indices, v7_c.m_indices);

    TEST_ASSERT_EQUALITY(v7_c.m_data[0], 4.2);
    TEST_ASSERT_EQUALITY(v7_c.m_indices[0], 0);

    CHECK_CLEAN_AUXILIARY_VECTORS;

    SparseVector v8(10);
    v8.set(4, 1.2);
    v8.set(1, 5.6);
    v8.set(9, 1.4);
    SparseVector v8_c = v8;
    TEST_ASSERT_EQUALITY(v8.m_length, 10);
    TEST_ASSERT_EQUALITY(v8.m_nonZeros, 3);
    TEST_ASSERT_INEQUALITY(v8.m_data, nullptr);
    TEST_ASSERT_INEQUALITY(v8.m_indices, nullptr);
    TEST_ASSERT_GREATER_OR_EQUAL(v8.m_capacity, 3);
    TEST_ASSERT_EQUALITY(v8_c.m_length, 10);
    TEST_ASSERT_EQUALITY(v8_c.m_nonZeros, 3);
    TEST_ASSERT_INEQUALITY(v8_c.m_data, nullptr);
    TEST_ASSERT_INEQUALITY(v8_c.m_indices, nullptr);
    TEST_ASSERT_GREATER_OR_EQUAL(v8_c.m_capacity, 3);
    TEST_ASSERT_INEQUALITY(v8.m_data, v8_c.m_data);
    TEST_ASSERT_INEQUALITY(v8.m_indices, v8_c.m_indices);

    TEST_ASSERT_EQUALITY(v8_c.m_data[0], 1.2);
    TEST_ASSERT_EQUALITY(v8_c.m_indices[0], 4);
    TEST_ASSERT_EQUALITY(v8_c.m_data[1], 5.6);
    TEST_ASSERT_EQUALITY(v8_c.m_indices[1], 1);
    TEST_ASSERT_EQUALITY(v8_c.m_data[2], 1.4);
    TEST_ASSERT_EQUALITY(v8_c.m_indices[2], 9);

    CHECK_CLEAN_AUXILIARY_VECTORS;

    SparseVector v9(3);
    v9.set(1, 1.2);
    v9.set(0, 5.6);
    v9.set(2, 1.4);
    SparseVector v9_c = v9;
    TEST_ASSERT_EQUALITY(v9.m_length, 3);
    TEST_ASSERT_EQUALITY(v9.m_nonZeros, 3);
    TEST_ASSERT_INEQUALITY(v9.m_data, nullptr);
    TEST_ASSERT_INEQUALITY(v9.m_indices, nullptr);
    TEST_ASSERT_GREATER_OR_EQUAL(v9.m_capacity, 3);
    TEST_ASSERT_EQUALITY(v9_c.m_length, 3);
    TEST_ASSERT_EQUALITY(v9_c.m_nonZeros, 3);
    TEST_ASSERT_INEQUALITY(v9_c.m_data, nullptr);
    TEST_ASSERT_INEQUALITY(v9_c.m_indices, nullptr);
    TEST_ASSERT_GREATER_OR_EQUAL(v9_c.m_capacity, 3);
    TEST_ASSERT_INEQUALITY(v9.m_data, v8_c.m_data);
    TEST_ASSERT_INEQUALITY(v9.m_indices, v8_c.m_indices);

    TEST_ASSERT_EQUALITY(v9_c.m_data[0], 1.2);
    TEST_ASSERT_EQUALITY(v9_c.m_indices[0], 1);
    TEST_ASSERT_EQUALITY(v9_c.m_data[1], 5.6);
    TEST_ASSERT_EQUALITY(v9_c.m_indices[1], 0);
    TEST_ASSERT_EQUALITY(v9_c.m_data[2], 1.4);
    TEST_ASSERT_EQUALITY(v9_c.m_indices[2], 2);

    CHECK_CLEAN_AUXILIARY_VECTORS;

    SparseVector::_globalRelease();
}

void SparseVectorTestSuite::move()
{
    SparseVector::_globalRelease();
    SparseVector::_globalInit();

    SparseVector v1(0);
    SparseVector v1_c = std::move(v1);
    TEST_ASSERT_EQUALITY(v1.m_length, 0);
    TEST_ASSERT_EQUALITY(v1.m_nonZeros, 0);
    TEST_ASSERT_EQUALITY(v1.m_data, nullptr);
    TEST_ASSERT_EQUALITY(v1.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v1.m_capacity, 0);
    TEST_ASSERT_EQUALITY(v1_c.m_length, 0);
    TEST_ASSERT_EQUALITY(v1_c.m_nonZeros, 0);
    TEST_ASSERT_EQUALITY(v1_c.m_data, nullptr);
    TEST_ASSERT_EQUALITY(v1_c.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v1_c.m_capacity, 0);
    TEST_ASSERT_EQUALITY(SparseVector::sm_fullLengthVectorLength, 0);
    TEST_ASSERT_EQUALITY(SparseVector::sm_fullLengthVector, nullptr);
    TEST_ASSERT_EQUALITY(SparseVector::sm_indexPointerVector, nullptr);
    TEST_ASSERT_EQUALITY(SparseVector::sm_indexVector, nullptr);

    CHECK_DEFAULT_FUNCTION_POINTERS;
    CHECK_CLEAN_AUXILIARY_VECTORS;

    SparseVector v2(1);
    SparseVector v2_c = std::move(v2);
    TEST_ASSERT_EQUALITY(v2.m_length, 0);
    TEST_ASSERT_EQUALITY(v2.m_nonZeros, 0);
    TEST_ASSERT_EQUALITY(v2.m_data, nullptr);
    TEST_ASSERT_EQUALITY(v2.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v2.m_capacity, 0);
    TEST_ASSERT_EQUALITY(v2_c.m_length, 1);
    TEST_ASSERT_EQUALITY(v2_c.m_nonZeros, 0);
    TEST_ASSERT_EQUALITY(v2_c.m_data, nullptr);
    TEST_ASSERT_EQUALITY(v2_c.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v2_c.m_capacity, 0);
    TEST_ASSERT_EQUALITY(SparseVector::sm_fullLengthVectorLength, 1);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_fullLengthVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexPointerVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexVector, nullptr);

    CHECK_DEFAULT_FUNCTION_POINTERS;
    CHECK_CLEAN_AUXILIARY_VECTORS;

    SparseVector v3(2);
    SparseVector v3_c = std::move(v3);
    TEST_ASSERT_EQUALITY(v3.m_length, 0);
    TEST_ASSERT_EQUALITY(v3.m_nonZeros, 0);
    TEST_ASSERT_EQUALITY(v3.m_data, nullptr);
    TEST_ASSERT_EQUALITY(v3.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v3.m_capacity, 0);
    TEST_ASSERT_EQUALITY(v3_c.m_length, 2);
    TEST_ASSERT_EQUALITY(v3_c.m_nonZeros, 0);
    TEST_ASSERT_EQUALITY(v3_c.m_data, nullptr);
    TEST_ASSERT_EQUALITY(v3_c.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v3_c.m_capacity, 0);
    TEST_ASSERT_EQUALITY(SparseVector::sm_fullLengthVectorLength, 2);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_fullLengthVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexPointerVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexVector, nullptr);

    CHECK_DEFAULT_FUNCTION_POINTERS;
    CHECK_CLEAN_AUXILIARY_VECTORS;

    SparseVector v4(10);
    SparseVector v4_c = std::move(v4);
    TEST_ASSERT_EQUALITY(v4.m_length, 0);
    TEST_ASSERT_EQUALITY(v4.m_nonZeros, 0);
    TEST_ASSERT_EQUALITY(v4.m_data, nullptr);
    TEST_ASSERT_EQUALITY(v4.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v4.m_capacity, 0);
    TEST_ASSERT_EQUALITY(v4_c.m_length, 10);
    TEST_ASSERT_EQUALITY(v4_c.m_nonZeros, 0);
    TEST_ASSERT_EQUALITY(v4_c.m_data, nullptr);
    TEST_ASSERT_EQUALITY(v4_c.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v4_c.m_capacity, 0);
    TEST_ASSERT_EQUALITY(SparseVector::sm_fullLengthVectorLength, 10);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_fullLengthVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexPointerVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexVector, nullptr);

    CHECK_DEFAULT_FUNCTION_POINTERS;
    CHECK_CLEAN_AUXILIARY_VECTORS;

    SparseVector v5(5);
    SparseVector v5_c = std::move(v5);
    TEST_ASSERT_EQUALITY(v5.m_length, 0);
    TEST_ASSERT_EQUALITY(v5.m_nonZeros, 0);
    TEST_ASSERT_EQUALITY(v5.m_data, nullptr);
    TEST_ASSERT_EQUALITY(v5.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v5.m_capacity, 0);
    TEST_ASSERT_EQUALITY(v5_c.m_length, 5);
    TEST_ASSERT_EQUALITY(v5_c.m_nonZeros, 0);
    TEST_ASSERT_EQUALITY(v5_c.m_data, nullptr);
    TEST_ASSERT_EQUALITY(v5_c.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v5_c.m_capacity, 0);
    TEST_ASSERT_EQUALITY(SparseVector::sm_fullLengthVectorLength, 10);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_fullLengthVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexPointerVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexVector, nullptr);

    CHECK_DEFAULT_FUNCTION_POINTERS;
    CHECK_CLEAN_AUXILIARY_VECTORS;

    SparseVector::setAddMode(Numerical::ADD_ABS);
    SparseVector::setDotProductMode(Numerical::DOT_ABS);

    SparseVector v6(20);
    SparseVector v6_c = std::move(v6);
    TEST_ASSERT_EQUALITY(v6.m_length, 0);
    TEST_ASSERT_EQUALITY(v6.m_nonZeros, 0);
    TEST_ASSERT_EQUALITY(v6.m_data, nullptr);
    TEST_ASSERT_EQUALITY(v6.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v6.m_capacity, 0);
    TEST_ASSERT_EQUALITY(v6_c.m_length, 20);
    TEST_ASSERT_EQUALITY(v6_c.m_nonZeros, 0);
    TEST_ASSERT_EQUALITY(v6_c.m_data, nullptr);
    TEST_ASSERT_EQUALITY(v6_c.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v6_c.m_capacity, 0);
    TEST_ASSERT_EQUALITY(SparseVector::sm_fullLengthVectorLength, 20);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_fullLengthVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexPointerVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexVector, nullptr);

    TEST_ASSERT_EQUALITY(SparseVector::sm_sparseToDenseDotProduct, &SparseVector::dotProductSparseToDenseAbs );
    TEST_ASSERT_EQUALITY(SparseVector::sm_sparseToIndexedDenseDotProduct, &SparseVector::dotProductSparseToIndexedDenseAbs );
    TEST_ASSERT_EQUALITY(SparseVector::sm_sparseToSparseDotProduct, &SparseVector::dotProductSparseToSparseAbs);
    TEST_ASSERT_EQUALITY(SparseVector::sm_addDenseToSparse, &SparseVector::addDenseToSparseAbs);
    TEST_ASSERT_EQUALITY(SparseVector::sm_addSparseToSparse, &SparseVector::addSparseToSparseAbs);
    TEST_ASSERT_EQUALITY(SparseVector::sm_addIndexedDenseToSparse, &SparseVector::addIndexedDenseToSparseAbs);

    CHECK_CLEAN_AUXILIARY_VECTORS;

    SparseVector v7(1);
    v7.set(0, 4.2);
    SparseVector v7_c = std::move(v7);
    TEST_ASSERT_EQUALITY(v7.m_length, 0);
    TEST_ASSERT_EQUALITY(v7.m_nonZeros, 0);
    TEST_ASSERT_EQUALITY(v7.m_data, nullptr);
    TEST_ASSERT_EQUALITY(v7.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v7.m_capacity, 0);
    TEST_ASSERT_EQUALITY(v7_c.m_length, 1);
    TEST_ASSERT_EQUALITY(v7_c.m_nonZeros, 1);
    TEST_ASSERT_INEQUALITY(v7_c.m_data, nullptr);
    TEST_ASSERT_INEQUALITY(v7_c.m_indices, nullptr);
    TEST_ASSERT_GREATER_OR_EQUAL(v7_c.m_capacity, 1);
    TEST_ASSERT_INEQUALITY(v7.m_data, v7_c.m_data);
    TEST_ASSERT_INEQUALITY(v7.m_indices, v7_c.m_indices);

    TEST_ASSERT_EQUALITY(v7_c.m_data[0], 4.2);
    TEST_ASSERT_EQUALITY(v7_c.m_indices[0], 0);

    CHECK_CLEAN_AUXILIARY_VECTORS;

    SparseVector v8(10);
    v8.set(4, 1.2);
    v8.set(1, 5.6);
    v8.set(9, 1.4);
    SparseVector v8_c = std::move(v8);
    TEST_ASSERT_EQUALITY(v8.m_length, 0);
    TEST_ASSERT_EQUALITY(v8.m_nonZeros, 0);
    TEST_ASSERT_EQUALITY(v8.m_data, nullptr);
    TEST_ASSERT_EQUALITY(v8.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v8.m_capacity, 0);
    TEST_ASSERT_EQUALITY(v8_c.m_length, 10);
    TEST_ASSERT_EQUALITY(v8_c.m_nonZeros, 3);
    TEST_ASSERT_INEQUALITY(v8_c.m_data, nullptr);
    TEST_ASSERT_INEQUALITY(v8_c.m_indices, nullptr);
    TEST_ASSERT_GREATER_OR_EQUAL(v8_c.m_capacity, 3);
    TEST_ASSERT_INEQUALITY(v8.m_data, v8_c.m_data);
    TEST_ASSERT_INEQUALITY(v8.m_indices, v8_c.m_indices);

    TEST_ASSERT_EQUALITY(v8_c.m_data[0], 1.2);
    TEST_ASSERT_EQUALITY(v8_c.m_indices[0], 4);
    TEST_ASSERT_EQUALITY(v8_c.m_data[1], 5.6);
    TEST_ASSERT_EQUALITY(v8_c.m_indices[1], 1);
    TEST_ASSERT_EQUALITY(v8_c.m_data[2], 1.4);
    TEST_ASSERT_EQUALITY(v8_c.m_indices[2], 9);

    CHECK_CLEAN_AUXILIARY_VECTORS;

    SparseVector v9(3);
    v9.set(1, 1.2);
    v9.set(0, 5.6);
    v9.set(2, 1.4);
    SparseVector v9_c = std::move(v9);
    TEST_ASSERT_EQUALITY(v9.m_length, 0);
    TEST_ASSERT_EQUALITY(v9.m_nonZeros, 0);
    TEST_ASSERT_EQUALITY(v9.m_data, nullptr);
    TEST_ASSERT_EQUALITY(v9.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v9.m_capacity, 0);
    TEST_ASSERT_EQUALITY(v9_c.m_length, 3);
    TEST_ASSERT_EQUALITY(v9_c.m_nonZeros, 3);
    TEST_ASSERT_INEQUALITY(v9_c.m_data, nullptr);
    TEST_ASSERT_INEQUALITY(v9_c.m_indices, nullptr);
    TEST_ASSERT_GREATER_OR_EQUAL(v9_c.m_capacity, 3);
    TEST_ASSERT_INEQUALITY(v9.m_data, v8_c.m_data);
    TEST_ASSERT_INEQUALITY(v9.m_indices, v8_c.m_indices);

    TEST_ASSERT_EQUALITY(v9_c.m_data[0], 1.2);
    TEST_ASSERT_EQUALITY(v9_c.m_indices[0], 1);
    TEST_ASSERT_EQUALITY(v9_c.m_data[1], 5.6);
    TEST_ASSERT_EQUALITY(v9_c.m_indices[1], 0);
    TEST_ASSERT_EQUALITY(v9_c.m_data[2], 1.4);
    TEST_ASSERT_EQUALITY(v9_c.m_indices[2], 2);

    CHECK_CLEAN_AUXILIARY_VECTORS;

    SparseVector::_globalRelease();
}

void SparseVectorTestSuite::assign()
{
    SparseVector::_globalRelease();
    SparseVector::_globalInit();

    SparseVector v1(0);
    SparseVector v1_c(10);
    v1_c.set(8, 3.4);
    v1_c = v1;

    TEST_ASSERT_EQUALITY(v1.m_length, 0);
    TEST_ASSERT_EQUALITY(v1.m_nonZeros, 0);
    TEST_ASSERT_EQUALITY(v1.m_data, nullptr);
    TEST_ASSERT_EQUALITY(v1.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v1.m_capacity, 0);
    TEST_ASSERT_EQUALITY(v1_c.m_length, 0);
    TEST_ASSERT_EQUALITY(v1_c.m_nonZeros, 0);
    TEST_ASSERT_EQUALITY(v1_c.m_data, nullptr);
    TEST_ASSERT_EQUALITY(v1_c.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v1_c.m_capacity, 0);
    TEST_ASSERT_EQUALITY(SparseVector::sm_fullLengthVectorLength, 10);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_fullLengthVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexPointerVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexVector, nullptr);

    CHECK_DEFAULT_FUNCTION_POINTERS;
    CHECK_CLEAN_AUXILIARY_VECTORS;

    SparseVector v2(1);
    SparseVector v2_c(10);
    v2_c.set(8, 3.4);
    v2_c = v2;
    TEST_ASSERT_EQUALITY(v2.m_length, 1);
    TEST_ASSERT_EQUALITY(v2.m_nonZeros, 0);
    TEST_ASSERT_EQUALITY(v2.m_data, nullptr);
    TEST_ASSERT_EQUALITY(v2.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v2.m_capacity, 0);
    TEST_ASSERT_EQUALITY(v2_c.m_length, 1);
    TEST_ASSERT_EQUALITY(v2_c.m_nonZeros, 0);
    TEST_ASSERT_EQUALITY(v2_c.m_data, nullptr);
    TEST_ASSERT_EQUALITY(v2_c.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v2_c.m_capacity, 0);
    TEST_ASSERT_EQUALITY(SparseVector::sm_fullLengthVectorLength, 10);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_fullLengthVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexPointerVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexVector, nullptr);

    CHECK_DEFAULT_FUNCTION_POINTERS;
    CHECK_CLEAN_AUXILIARY_VECTORS;

    SparseVector v3(2);
    SparseVector v3_c(10);
    v3_c.set(8, 3.4);
    v3_c = v3;
    TEST_ASSERT_EQUALITY(v3.m_length, 2);
    TEST_ASSERT_EQUALITY(v3.m_nonZeros, 0);
    TEST_ASSERT_EQUALITY(v3.m_data, nullptr);
    TEST_ASSERT_EQUALITY(v3.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v3.m_capacity, 0);
    TEST_ASSERT_EQUALITY(v3_c.m_length, 2);
    TEST_ASSERT_EQUALITY(v3_c.m_nonZeros, 0);
    TEST_ASSERT_EQUALITY(v3_c.m_data, nullptr);
    TEST_ASSERT_EQUALITY(v3_c.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v3_c.m_capacity, 0);
    TEST_ASSERT_EQUALITY(SparseVector::sm_fullLengthVectorLength, 10);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_fullLengthVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexPointerVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexVector, nullptr);

    CHECK_DEFAULT_FUNCTION_POINTERS;
    CHECK_CLEAN_AUXILIARY_VECTORS;

    SparseVector v4(10);
    SparseVector v4_c(10);
    v4_c.set(8, 3.4);
    v4_c = v4;
    TEST_ASSERT_EQUALITY(v4.m_length, 10);
    TEST_ASSERT_EQUALITY(v4.m_nonZeros, 0);
    TEST_ASSERT_EQUALITY(v4.m_data, nullptr);
    TEST_ASSERT_EQUALITY(v4.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v4.m_capacity, 0);
    TEST_ASSERT_EQUALITY(v4_c.m_length, 10);
    TEST_ASSERT_EQUALITY(v4_c.m_nonZeros, 0);
    TEST_ASSERT_EQUALITY(v4_c.m_data, nullptr);
    TEST_ASSERT_EQUALITY(v4_c.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v4_c.m_capacity, 0);
    TEST_ASSERT_EQUALITY(SparseVector::sm_fullLengthVectorLength, 10);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_fullLengthVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexPointerVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexVector, nullptr);

    CHECK_DEFAULT_FUNCTION_POINTERS;
    CHECK_CLEAN_AUXILIARY_VECTORS;

    SparseVector v5(5);
    SparseVector v5_c(10);
    v5_c.set(8, 3.4);
    v5_c = v5;
    TEST_ASSERT_EQUALITY(v5.m_length, 5);
    TEST_ASSERT_EQUALITY(v5.m_nonZeros, 0);
    TEST_ASSERT_EQUALITY(v5.m_data, nullptr);
    TEST_ASSERT_EQUALITY(v5.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v5.m_capacity, 0);
    TEST_ASSERT_EQUALITY(v5_c.m_length, 5);
    TEST_ASSERT_EQUALITY(v5_c.m_nonZeros, 0);
    TEST_ASSERT_EQUALITY(v5_c.m_data, nullptr);
    TEST_ASSERT_EQUALITY(v5_c.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v5_c.m_capacity, 0);
    TEST_ASSERT_EQUALITY(SparseVector::sm_fullLengthVectorLength, 10);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_fullLengthVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexPointerVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexVector, nullptr);

    CHECK_DEFAULT_FUNCTION_POINTERS;
    CHECK_CLEAN_AUXILIARY_VECTORS;

    SparseVector::setAddMode(Numerical::ADD_ABS);
    SparseVector::setDotProductMode(Numerical::DOT_ABS);

    SparseVector v6(20);
    SparseVector v6_c(10);
    v6_c.set(8, 3.4);
    v6_c = v6;
    TEST_ASSERT_EQUALITY(v6.m_length, 20);
    TEST_ASSERT_EQUALITY(v6.m_nonZeros, 0);
    TEST_ASSERT_EQUALITY(v6.m_data, nullptr);
    TEST_ASSERT_EQUALITY(v6.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v6.m_capacity, 0);
    TEST_ASSERT_EQUALITY(v6_c.m_length, 20);
    TEST_ASSERT_EQUALITY(v6_c.m_nonZeros, 0);
    TEST_ASSERT_EQUALITY(v6_c.m_data, nullptr);
    TEST_ASSERT_EQUALITY(v6_c.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v6_c.m_capacity, 0);
    TEST_ASSERT_EQUALITY(SparseVector::sm_fullLengthVectorLength, 20);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_fullLengthVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexPointerVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexVector, nullptr);

    TEST_ASSERT_EQUALITY(SparseVector::sm_sparseToDenseDotProduct, &SparseVector::dotProductSparseToDenseAbs );
    TEST_ASSERT_EQUALITY(SparseVector::sm_sparseToIndexedDenseDotProduct, &SparseVector::dotProductSparseToIndexedDenseAbs );
    TEST_ASSERT_EQUALITY(SparseVector::sm_sparseToSparseDotProduct, &SparseVector::dotProductSparseToSparseAbs);
    TEST_ASSERT_EQUALITY(SparseVector::sm_addDenseToSparse, &SparseVector::addDenseToSparseAbs);
    TEST_ASSERT_EQUALITY(SparseVector::sm_addSparseToSparse, &SparseVector::addSparseToSparseAbs);
    TEST_ASSERT_EQUALITY(SparseVector::sm_addIndexedDenseToSparse, &SparseVector::addIndexedDenseToSparseAbs);

    CHECK_CLEAN_AUXILIARY_VECTORS;

    SparseVector v7(1);
    v7.set(0, 4.2);
    SparseVector v7_c(10);
    v7_c.set(8, 3.4);
    v7_c = v7;
    TEST_ASSERT_EQUALITY(v7.m_length, 1);
    TEST_ASSERT_EQUALITY(v7.m_nonZeros, 1);
    TEST_ASSERT_INEQUALITY(v7.m_data, nullptr);
    TEST_ASSERT_INEQUALITY(v7.m_indices, nullptr);
    TEST_ASSERT_GREATER_OR_EQUAL(v7.m_capacity, 1);
    TEST_ASSERT_EQUALITY(v7_c.m_length, 1);
    TEST_ASSERT_EQUALITY(v7_c.m_nonZeros, 1);
    TEST_ASSERT_INEQUALITY(v7_c.m_data, nullptr);
    TEST_ASSERT_INEQUALITY(v7_c.m_indices, nullptr);
    TEST_ASSERT_GREATER_OR_EQUAL(v7_c.m_capacity, 1);
    TEST_ASSERT_INEQUALITY(v7.m_data, v7_c.m_data);
    TEST_ASSERT_INEQUALITY(v7.m_indices, v7_c.m_indices);

    TEST_ASSERT_EQUALITY(v7_c.m_data[0], 4.2);
    TEST_ASSERT_EQUALITY(v7_c.m_indices[0], 0);

    CHECK_CLEAN_AUXILIARY_VECTORS;

    SparseVector v8(10);
    v8.set(4, 1.2);
    v8.set(1, 5.6);
    v8.set(9, 1.4);
    SparseVector v8_c(10);
    v8_c.set(8, 3.4);
    v8_c = v8;
    TEST_ASSERT_EQUALITY(v8.m_length, 10);
    TEST_ASSERT_EQUALITY(v8.m_nonZeros, 3);
    TEST_ASSERT_INEQUALITY(v8.m_data, nullptr);
    TEST_ASSERT_INEQUALITY(v8.m_indices, nullptr);
    TEST_ASSERT_GREATER_OR_EQUAL(v8.m_capacity, 3);
    TEST_ASSERT_EQUALITY(v8_c.m_length, 10);
    TEST_ASSERT_EQUALITY(v8_c.m_nonZeros, 3);
    TEST_ASSERT_INEQUALITY(v8_c.m_data, nullptr);
    TEST_ASSERT_INEQUALITY(v8_c.m_indices, nullptr);
    TEST_ASSERT_GREATER_OR_EQUAL(v8_c.m_capacity, 3);
    TEST_ASSERT_INEQUALITY(v8.m_data, v8_c.m_data);
    TEST_ASSERT_INEQUALITY(v8.m_indices, v8_c.m_indices);

    TEST_ASSERT_EQUALITY(v8_c.m_data[0], 1.2);
    TEST_ASSERT_EQUALITY(v8_c.m_indices[0], 4);
    TEST_ASSERT_EQUALITY(v8_c.m_data[1], 5.6);
    TEST_ASSERT_EQUALITY(v8_c.m_indices[1], 1);
    TEST_ASSERT_EQUALITY(v8_c.m_data[2], 1.4);
    TEST_ASSERT_EQUALITY(v8_c.m_indices[2], 9);

    CHECK_CLEAN_AUXILIARY_VECTORS;

    SparseVector v9(3);
    v9.set(1, 1.2);
    v9.set(0, 5.6);
    v9.set(2, 1.4);
    SparseVector v9_c(10);
    v9_c.set(8, 3.4);
    v9_c = v9;
    TEST_ASSERT_EQUALITY(v9.m_length, 3);
    TEST_ASSERT_EQUALITY(v9.m_nonZeros, 3);
    TEST_ASSERT_INEQUALITY(v9.m_data, nullptr);
    TEST_ASSERT_INEQUALITY(v9.m_indices, nullptr);
    TEST_ASSERT_GREATER_OR_EQUAL(v9.m_capacity, 3);
    TEST_ASSERT_EQUALITY(v9_c.m_length, 3);
    TEST_ASSERT_EQUALITY(v9_c.m_nonZeros, 3);
    TEST_ASSERT_INEQUALITY(v9_c.m_data, nullptr);
    TEST_ASSERT_INEQUALITY(v9_c.m_indices, nullptr);
    TEST_ASSERT_GREATER_OR_EQUAL(v9_c.m_capacity, 3);
    TEST_ASSERT_INEQUALITY(v9.m_data, v8_c.m_data);
    TEST_ASSERT_INEQUALITY(v9.m_indices, v8_c.m_indices);

    TEST_ASSERT_EQUALITY(v9_c.m_data[0], 1.2);
    TEST_ASSERT_EQUALITY(v9_c.m_indices[0], 1);
    TEST_ASSERT_EQUALITY(v9_c.m_data[1], 5.6);
    TEST_ASSERT_EQUALITY(v9_c.m_indices[1], 0);
    TEST_ASSERT_EQUALITY(v9_c.m_data[2], 1.4);
    TEST_ASSERT_EQUALITY(v9_c.m_indices[2], 2);

    CHECK_CLEAN_AUXILIARY_VECTORS;

    SparseVector::_globalRelease();
}

void SparseVectorTestSuite::assignDense()
{
    SparseVector::_globalRelease();
    SparseVector::_globalInit();

    const Numerical::Double dense1[] = {3, 4, 1, 0, 5, 6, 0, 9};

    DenseVector v1_dense = DenseVector::createVectorFromDenseArray(dense1, STATIC_ARRAY_SIZE(dense1));

    SparseVector v1;
    v1 = v1_dense; // tested function
    TEST_ASSERT_EQUALITY(v1.m_length, STATIC_ARRAY_SIZE(dense1));
    TEST_ASSERT_EQUALITY(v1.m_nonZeros, 6);
    TEST_ASSERT_INEQUALITY(v1.m_data, nullptr);
    TEST_ASSERT_INEQUALITY(v1.m_indices, nullptr);
    TEST_ASSERT_GREATER_OR_EQUAL(v1.m_capacity, 6);

    TEST_ASSERT_EQUALITY(v1.m_data[0], 3);
    TEST_ASSERT_EQUALITY(v1.m_indices[0], 0);
    TEST_ASSERT_EQUALITY(v1.m_data[1], 4);
    TEST_ASSERT_EQUALITY(v1.m_indices[1], 1);
    TEST_ASSERT_EQUALITY(v1.m_data[2], 1);
    TEST_ASSERT_EQUALITY(v1.m_indices[2], 2);
    TEST_ASSERT_EQUALITY(v1.m_data[3], 5);
    TEST_ASSERT_EQUALITY(v1.m_indices[3], 4);
    TEST_ASSERT_EQUALITY(v1.m_data[4], 6);
    TEST_ASSERT_EQUALITY(v1.m_indices[4], 5);
    TEST_ASSERT_EQUALITY(v1.m_data[5], 9);
    TEST_ASSERT_EQUALITY(v1.m_indices[5], 7);

    TEST_ASSERT_EQUALITY(SparseVector::sm_fullLengthVectorLength, STATIC_ARRAY_SIZE(dense1));
    TEST_ASSERT_INEQUALITY(SparseVector::sm_fullLengthVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexPointerVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexVector, nullptr);

    CHECK_DEFAULT_FUNCTION_POINTERS;
    CHECK_CLEAN_AUXILIARY_VECTORS;

    SparseVector::_globalRelease();
    SparseVector::_globalInit();

    const Numerical::Double dense2[] = {0, 0, 0, 0, 0};

    DenseVector v2_dense = DenseVector::createVectorFromDenseArray(dense2, STATIC_ARRAY_SIZE(dense2));

    SparseVector v2;
    v2 = v2_dense; // tested function

    TEST_ASSERT_EQUALITY(v2.m_length, STATIC_ARRAY_SIZE(dense2));
    TEST_ASSERT_EQUALITY(v2.m_nonZeros, 0);
    TEST_ASSERT_EQUALITY(v2.m_data, nullptr);
    TEST_ASSERT_EQUALITY(v2.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v2.m_capacity, 0);

    TEST_ASSERT_EQUALITY(SparseVector::sm_fullLengthVectorLength, STATIC_ARRAY_SIZE(dense2));
    TEST_ASSERT_INEQUALITY(SparseVector::sm_fullLengthVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexPointerVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexVector, nullptr);

    CHECK_DEFAULT_FUNCTION_POINTERS;
    CHECK_CLEAN_AUXILIARY_VECTORS;

    SparseVector::_globalRelease();

}

void SparseVectorTestSuite::moveAssign()
{
    SparseVector::_globalRelease();
    SparseVector::_globalInit();

    SparseVector v1(0);
    SparseVector v1_c(10);
    v1_c.set(4, 6.4);
    v1_c = std::move(v1);
    TEST_ASSERT_EQUALITY(v1.m_length, 0);
    TEST_ASSERT_EQUALITY(v1.m_nonZeros, 0);
    TEST_ASSERT_EQUALITY(v1.m_data, nullptr);
    TEST_ASSERT_EQUALITY(v1.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v1.m_capacity, 0);
    TEST_ASSERT_EQUALITY(v1_c.m_length, 0);
    TEST_ASSERT_EQUALITY(v1_c.m_nonZeros, 0);
    TEST_ASSERT_EQUALITY(v1_c.m_data, nullptr);
    TEST_ASSERT_EQUALITY(v1_c.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v1_c.m_capacity, 0);
    TEST_ASSERT_EQUALITY(SparseVector::sm_fullLengthVectorLength, 10);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_fullLengthVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexPointerVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexVector, nullptr);

    CHECK_DEFAULT_FUNCTION_POINTERS;
    CHECK_CLEAN_AUXILIARY_VECTORS;

    SparseVector v2(1);
    SparseVector v2_c(10);
    v2_c.set(4, 6.4);
    v2_c = std::move(v2);
    TEST_ASSERT_EQUALITY(v2.m_length, 0);
    TEST_ASSERT_EQUALITY(v2.m_nonZeros, 0);
    TEST_ASSERT_EQUALITY(v2.m_data, nullptr);
    TEST_ASSERT_EQUALITY(v2.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v2.m_capacity, 0);
    TEST_ASSERT_EQUALITY(v2_c.m_length, 1);
    TEST_ASSERT_EQUALITY(v2_c.m_nonZeros, 0);
    TEST_ASSERT_EQUALITY(v2_c.m_data, nullptr);
    TEST_ASSERT_EQUALITY(v2_c.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v2_c.m_capacity, 0);
    TEST_ASSERT_EQUALITY(SparseVector::sm_fullLengthVectorLength, 10);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_fullLengthVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexPointerVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexVector, nullptr);

    CHECK_DEFAULT_FUNCTION_POINTERS;
    CHECK_CLEAN_AUXILIARY_VECTORS;

    SparseVector v3(2);
    SparseVector v3_c(10);
    v3_c.set(4, 6.4);
    v3_c = std::move(v3);
    TEST_ASSERT_EQUALITY(v3.m_length, 0);
    TEST_ASSERT_EQUALITY(v3.m_nonZeros, 0);
    TEST_ASSERT_EQUALITY(v3.m_data, nullptr);
    TEST_ASSERT_EQUALITY(v3.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v3.m_capacity, 0);
    TEST_ASSERT_EQUALITY(v3_c.m_length, 2);
    TEST_ASSERT_EQUALITY(v3_c.m_nonZeros, 0);
    TEST_ASSERT_EQUALITY(v3_c.m_data, nullptr);
    TEST_ASSERT_EQUALITY(v3_c.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v3_c.m_capacity, 0);
    TEST_ASSERT_EQUALITY(SparseVector::sm_fullLengthVectorLength, 10);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_fullLengthVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexPointerVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexVector, nullptr);

    CHECK_DEFAULT_FUNCTION_POINTERS;
    CHECK_CLEAN_AUXILIARY_VECTORS;

    SparseVector v4(10);
    SparseVector v4_c(10);
    v4_c.set(4, 6.4);
    v4_c = std::move(v4);
    TEST_ASSERT_EQUALITY(v4.m_length, 0);
    TEST_ASSERT_EQUALITY(v4.m_nonZeros, 0);
    TEST_ASSERT_EQUALITY(v4.m_data, nullptr);
    TEST_ASSERT_EQUALITY(v4.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v4.m_capacity, 0);
    TEST_ASSERT_EQUALITY(v4_c.m_length, 10);
    TEST_ASSERT_EQUALITY(v4_c.m_nonZeros, 0);
    TEST_ASSERT_EQUALITY(v4_c.m_data, nullptr);
    TEST_ASSERT_EQUALITY(v4_c.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v4_c.m_capacity, 0);
    TEST_ASSERT_EQUALITY(SparseVector::sm_fullLengthVectorLength, 10);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_fullLengthVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexPointerVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexVector, nullptr);

    CHECK_DEFAULT_FUNCTION_POINTERS;
    CHECK_CLEAN_AUXILIARY_VECTORS;

    SparseVector v5(5);
    SparseVector v5_c(10);
    v5_c.set(4, 6.4);
    v5_c = std::move(v5);
    TEST_ASSERT_EQUALITY(v5.m_length, 0);
    TEST_ASSERT_EQUALITY(v5.m_nonZeros, 0);
    TEST_ASSERT_EQUALITY(v5.m_data, nullptr);
    TEST_ASSERT_EQUALITY(v5.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v5.m_capacity, 0);
    TEST_ASSERT_EQUALITY(v5_c.m_length, 5);
    TEST_ASSERT_EQUALITY(v5_c.m_nonZeros, 0);
    TEST_ASSERT_EQUALITY(v5_c.m_data, nullptr);
    TEST_ASSERT_EQUALITY(v5_c.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v5_c.m_capacity, 0);
    TEST_ASSERT_EQUALITY(SparseVector::sm_fullLengthVectorLength, 10);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_fullLengthVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexPointerVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexVector, nullptr);

    CHECK_DEFAULT_FUNCTION_POINTERS;
    CHECK_CLEAN_AUXILIARY_VECTORS;

    SparseVector::setAddMode(Numerical::ADD_ABS);
    SparseVector::setDotProductMode(Numerical::DOT_ABS);

    SparseVector v6(20);
    SparseVector v6_c(10);
    v6_c.set(4, 6.4);
    v6_c = std::move(v6);
    TEST_ASSERT_EQUALITY(v6.m_length, 0);
    TEST_ASSERT_EQUALITY(v6.m_nonZeros, 0);
    TEST_ASSERT_EQUALITY(v6.m_data, nullptr);
    TEST_ASSERT_EQUALITY(v6.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v6.m_capacity, 0);
    TEST_ASSERT_EQUALITY(v6_c.m_length, 20);
    TEST_ASSERT_EQUALITY(v6_c.m_nonZeros, 0);
    TEST_ASSERT_EQUALITY(v6_c.m_data, nullptr);
    TEST_ASSERT_EQUALITY(v6_c.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v6_c.m_capacity, 0);
    TEST_ASSERT_EQUALITY(SparseVector::sm_fullLengthVectorLength, 20);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_fullLengthVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexPointerVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexVector, nullptr);

    TEST_ASSERT_EQUALITY(SparseVector::sm_sparseToDenseDotProduct, &SparseVector::dotProductSparseToDenseAbs );
    TEST_ASSERT_EQUALITY(SparseVector::sm_sparseToIndexedDenseDotProduct, &SparseVector::dotProductSparseToIndexedDenseAbs );
    TEST_ASSERT_EQUALITY(SparseVector::sm_sparseToSparseDotProduct, &SparseVector::dotProductSparseToSparseAbs);
    TEST_ASSERT_EQUALITY(SparseVector::sm_addDenseToSparse, &SparseVector::addDenseToSparseAbs);
    TEST_ASSERT_EQUALITY(SparseVector::sm_addSparseToSparse, &SparseVector::addSparseToSparseAbs);
    TEST_ASSERT_EQUALITY(SparseVector::sm_addIndexedDenseToSparse, &SparseVector::addIndexedDenseToSparseAbs);

    CHECK_CLEAN_AUXILIARY_VECTORS;

    SparseVector v7(1);
    v7.set(0, 4.2);
    SparseVector v7_c(10);
    v7_c.set(4, 6.4);
    v7_c = std::move(v7);
    TEST_ASSERT_EQUALITY(v7.m_length, 0);
    TEST_ASSERT_EQUALITY(v7.m_nonZeros, 0);
    TEST_ASSERT_EQUALITY(v7.m_data, nullptr);
    TEST_ASSERT_EQUALITY(v7.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v7.m_capacity, 0);
    TEST_ASSERT_EQUALITY(v7_c.m_length, 1);
    TEST_ASSERT_EQUALITY(v7_c.m_nonZeros, 1);
    TEST_ASSERT_INEQUALITY(v7_c.m_data, nullptr);
    TEST_ASSERT_INEQUALITY(v7_c.m_indices, nullptr);
    TEST_ASSERT_GREATER_OR_EQUAL(v7_c.m_capacity, 1);
    TEST_ASSERT_INEQUALITY(v7.m_data, v7_c.m_data);
    TEST_ASSERT_INEQUALITY(v7.m_indices, v7_c.m_indices);

    TEST_ASSERT_EQUALITY(v7_c.m_data[0], 4.2);
    TEST_ASSERT_EQUALITY(v7_c.m_indices[0], 0);

    TEST_ASSERT_EQUALITY(SparseVector::sm_fullLengthVectorLength, 20);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_fullLengthVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexPointerVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexVector, nullptr);

    TEST_ASSERT_EQUALITY(SparseVector::sm_sparseToDenseDotProduct, &SparseVector::dotProductSparseToDenseAbs );
    TEST_ASSERT_EQUALITY(SparseVector::sm_sparseToIndexedDenseDotProduct, &SparseVector::dotProductSparseToIndexedDenseAbs );
    TEST_ASSERT_EQUALITY(SparseVector::sm_sparseToSparseDotProduct, &SparseVector::dotProductSparseToSparseAbs);
    TEST_ASSERT_EQUALITY(SparseVector::sm_addDenseToSparse, &SparseVector::addDenseToSparseAbs);
    TEST_ASSERT_EQUALITY(SparseVector::sm_addSparseToSparse, &SparseVector::addSparseToSparseAbs);
    TEST_ASSERT_EQUALITY(SparseVector::sm_addIndexedDenseToSparse, &SparseVector::addIndexedDenseToSparseAbs);

    CHECK_CLEAN_AUXILIARY_VECTORS;

    SparseVector v8(10);
    v8.set(4, 1.2);
    v8.set(1, 5.6);
    v8.set(9, 1.4);
    SparseVector v8_c(10);
    v8_c.set(3, 6.4);
    v8_c = std::move(v8);
    TEST_ASSERT_EQUALITY(v8.m_length, 0);
    TEST_ASSERT_EQUALITY(v8.m_nonZeros, 0);
    TEST_ASSERT_EQUALITY(v8.m_data, nullptr);
    TEST_ASSERT_EQUALITY(v8.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v8.m_capacity, 0);
    TEST_ASSERT_EQUALITY(v8_c.m_length, 10);
    TEST_ASSERT_EQUALITY(v8_c.m_nonZeros, 3);
    TEST_ASSERT_INEQUALITY(v8_c.m_data, nullptr);
    TEST_ASSERT_INEQUALITY(v8_c.m_indices, nullptr);
    TEST_ASSERT_GREATER_OR_EQUAL(v8_c.m_capacity, 3);
    TEST_ASSERT_INEQUALITY(v8.m_data, v8_c.m_data);
    TEST_ASSERT_INEQUALITY(v8.m_indices, v8_c.m_indices);

    TEST_ASSERT_EQUALITY(v8_c.m_data[0], 1.2);
    TEST_ASSERT_EQUALITY(v8_c.m_indices[0], 4);
    TEST_ASSERT_EQUALITY(v8_c.m_data[1], 5.6);
    TEST_ASSERT_EQUALITY(v8_c.m_indices[1], 1);
    TEST_ASSERT_EQUALITY(v8_c.m_data[2], 1.4);
    TEST_ASSERT_EQUALITY(v8_c.m_indices[2], 9);

    TEST_ASSERT_EQUALITY(SparseVector::sm_fullLengthVectorLength, 20);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_fullLengthVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexPointerVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexVector, nullptr);

    TEST_ASSERT_EQUALITY(SparseVector::sm_sparseToDenseDotProduct, &SparseVector::dotProductSparseToDenseAbs );
    TEST_ASSERT_EQUALITY(SparseVector::sm_sparseToIndexedDenseDotProduct, &SparseVector::dotProductSparseToIndexedDenseAbs );
    TEST_ASSERT_EQUALITY(SparseVector::sm_sparseToSparseDotProduct, &SparseVector::dotProductSparseToSparseAbs);
    TEST_ASSERT_EQUALITY(SparseVector::sm_addDenseToSparse, &SparseVector::addDenseToSparseAbs);
    TEST_ASSERT_EQUALITY(SparseVector::sm_addSparseToSparse, &SparseVector::addSparseToSparseAbs);
    TEST_ASSERT_EQUALITY(SparseVector::sm_addIndexedDenseToSparse, &SparseVector::addIndexedDenseToSparseAbs);

    CHECK_CLEAN_AUXILIARY_VECTORS;

    SparseVector v9(3);
    v9.set(1, 1.2);
    v9.set(0, 5.6);
    v9.set(2, 1.4);
    SparseVector v9_c(10);
    v9_c.set(4, 6.4);
    v9_c = std::move(v9);
    TEST_ASSERT_EQUALITY(v9.m_length, 0);
    TEST_ASSERT_EQUALITY(v9.m_nonZeros, 0);
    TEST_ASSERT_EQUALITY(v9.m_data, nullptr);
    TEST_ASSERT_EQUALITY(v9.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v9.m_capacity, 0);
    TEST_ASSERT_EQUALITY(v9_c.m_length, 3);
    TEST_ASSERT_EQUALITY(v9_c.m_nonZeros, 3);
    TEST_ASSERT_INEQUALITY(v9_c.m_data, nullptr);
    TEST_ASSERT_INEQUALITY(v9_c.m_indices, nullptr);
    TEST_ASSERT_GREATER_OR_EQUAL(v9_c.m_capacity, 3);
    TEST_ASSERT_INEQUALITY(v9.m_data, v8_c.m_data);
    TEST_ASSERT_INEQUALITY(v9.m_indices, v8_c.m_indices);

    TEST_ASSERT_EQUALITY(v9_c.m_data[0], 1.2);
    TEST_ASSERT_EQUALITY(v9_c.m_indices[0], 1);
    TEST_ASSERT_EQUALITY(v9_c.m_data[1], 5.6);
    TEST_ASSERT_EQUALITY(v9_c.m_indices[1], 0);
    TEST_ASSERT_EQUALITY(v9_c.m_data[2], 1.4);
    TEST_ASSERT_EQUALITY(v9_c.m_indices[2], 2);

    TEST_ASSERT_EQUALITY(SparseVector::sm_fullLengthVectorLength, 20);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_fullLengthVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexPointerVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexVector, nullptr);

    TEST_ASSERT_EQUALITY(SparseVector::sm_sparseToDenseDotProduct, &SparseVector::dotProductSparseToDenseAbs );
    TEST_ASSERT_EQUALITY(SparseVector::sm_sparseToIndexedDenseDotProduct, &SparseVector::dotProductSparseToIndexedDenseAbs );
    TEST_ASSERT_EQUALITY(SparseVector::sm_sparseToSparseDotProduct, &SparseVector::dotProductSparseToSparseAbs);
    TEST_ASSERT_EQUALITY(SparseVector::sm_addDenseToSparse, &SparseVector::addDenseToSparseAbs);
    TEST_ASSERT_EQUALITY(SparseVector::sm_addSparseToSparse, &SparseVector::addSparseToSparseAbs);
    TEST_ASSERT_EQUALITY(SparseVector::sm_addIndexedDenseToSparse, &SparseVector::addIndexedDenseToSparseAbs);

    CHECK_CLEAN_AUXILIARY_VECTORS;

    SparseVector::_globalRelease();
}

void SparseVectorTestSuite::createUnitVector()
{
    SparseVector::_globalRelease();
    SparseVector::_globalInit();

    SparseVector v1 = SparseVector::createUnitVector(1, 0);
    TEST_ASSERT_EQUALITY(v1.m_length, 1);
    TEST_ASSERT_EQUALITY(v1.m_nonZeros, 1);
    TEST_ASSERT_INEQUALITY(v1.m_data, nullptr);
    TEST_ASSERT_INEQUALITY(v1.m_indices, nullptr);
    TEST_ASSERT_GREATER_OR_EQUAL(v1.m_capacity, 1);

    TEST_ASSERT_EQUALITY(v1.m_data[0], 1);
    TEST_ASSERT_EQUALITY(v1.m_indices[0], 0);

    TEST_ASSERT_EQUALITY(SparseVector::sm_fullLengthVectorLength, 1);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_fullLengthVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexPointerVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexVector, nullptr);

    CHECK_DEFAULT_FUNCTION_POINTERS;
    CHECK_CLEAN_AUXILIARY_VECTORS;

    SparseVector v2 = SparseVector::createUnitVector(2, 0);
    TEST_ASSERT_EQUALITY(v2.m_length, 2);
    TEST_ASSERT_EQUALITY(v2.m_nonZeros, 1);
    TEST_ASSERT_INEQUALITY(v2.m_data, nullptr);
    TEST_ASSERT_INEQUALITY(v2.m_indices, nullptr);
    TEST_ASSERT_GREATER_OR_EQUAL(v2.m_capacity, 1);

    TEST_ASSERT_EQUALITY(v2.m_data[0], 1);
    TEST_ASSERT_EQUALITY(v2.m_indices[0], 0);

    TEST_ASSERT_EQUALITY(SparseVector::sm_fullLengthVectorLength, 2);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_fullLengthVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexPointerVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexVector, nullptr);

    CHECK_DEFAULT_FUNCTION_POINTERS;
    CHECK_CLEAN_AUXILIARY_VECTORS;

    SparseVector v3 = SparseVector::createUnitVector(2, 1);
    TEST_ASSERT_EQUALITY(v3.m_length, 2);
    TEST_ASSERT_EQUALITY(v3.m_nonZeros, 1);
    TEST_ASSERT_INEQUALITY(v3.m_data, nullptr);
    TEST_ASSERT_INEQUALITY(v3.m_indices, nullptr);
    TEST_ASSERT_GREATER_OR_EQUAL(v3.m_capacity, 1);

    TEST_ASSERT_EQUALITY(v3.m_data[0], 1);
    TEST_ASSERT_EQUALITY(v3.m_indices[0], 1);

    TEST_ASSERT_EQUALITY(SparseVector::sm_fullLengthVectorLength, 2);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_fullLengthVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexPointerVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexVector, nullptr);

    CHECK_DEFAULT_FUNCTION_POINTERS;
    CHECK_CLEAN_AUXILIARY_VECTORS;

    SparseVector v4 = SparseVector::createUnitVector(10, 0);
    TEST_ASSERT_EQUALITY(v4.m_length, 10);
    TEST_ASSERT_EQUALITY(v4.m_nonZeros, 1);
    TEST_ASSERT_INEQUALITY(v4.m_data, nullptr);
    TEST_ASSERT_INEQUALITY(v4.m_indices, nullptr);
    TEST_ASSERT_GREATER_OR_EQUAL(v4.m_capacity, 1);

    TEST_ASSERT_EQUALITY(v4.m_data[0], 1);
    TEST_ASSERT_EQUALITY(v4.m_indices[0], 0);

    TEST_ASSERT_EQUALITY(SparseVector::sm_fullLengthVectorLength, 10);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_fullLengthVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexPointerVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexVector, nullptr);

    CHECK_DEFAULT_FUNCTION_POINTERS;
    CHECK_CLEAN_AUXILIARY_VECTORS;

    SparseVector v5 = SparseVector::createUnitVector(10, 4);
    TEST_ASSERT_EQUALITY(v5.m_length, 10);
    TEST_ASSERT_EQUALITY(v5.m_nonZeros, 1);
    TEST_ASSERT_INEQUALITY(v5.m_data, nullptr);
    TEST_ASSERT_INEQUALITY(v5.m_indices, nullptr);
    TEST_ASSERT_GREATER_OR_EQUAL(v5.m_capacity, 1);

    TEST_ASSERT_EQUALITY(v5.m_data[0], 1);
    TEST_ASSERT_EQUALITY(v5.m_indices[0], 4);

    TEST_ASSERT_EQUALITY(SparseVector::sm_fullLengthVectorLength, 10);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_fullLengthVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexPointerVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexVector, nullptr);

    CHECK_DEFAULT_FUNCTION_POINTERS;
    CHECK_CLEAN_AUXILIARY_VECTORS;

    SparseVector v6 = SparseVector::createUnitVector(10, 9);
    TEST_ASSERT_EQUALITY(v6.m_length, 10);
    TEST_ASSERT_EQUALITY(v6.m_nonZeros, 1);
    TEST_ASSERT_INEQUALITY(v6.m_data, nullptr);
    TEST_ASSERT_INEQUALITY(v6.m_indices, nullptr);
    TEST_ASSERT_GREATER_OR_EQUAL(v6.m_capacity, 1);

    TEST_ASSERT_EQUALITY(v6.m_data[0], 1);
    TEST_ASSERT_EQUALITY(v6.m_indices[0], 9);

    TEST_ASSERT_EQUALITY(SparseVector::sm_fullLengthVectorLength, 10);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_fullLengthVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexPointerVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexVector, nullptr);

    CHECK_DEFAULT_FUNCTION_POINTERS;
    CHECK_CLEAN_AUXILIARY_VECTORS;

    SparseVector::_globalRelease();
}

void SparseVectorTestSuite::createVectorFromDenseArray()
{
    SparseVector::_globalRelease();
    SparseVector::_globalInit();

    const Numerical::Double dense1[] = {3, 4, 1, 0, 5, 6, 0, 9};

    SparseVector v1 = SparseVector::createVectorFromDenseArray(
                dense1,
                STATIC_ARRAY_SIZE(dense1));
    TEST_ASSERT_EQUALITY(v1.m_length, STATIC_ARRAY_SIZE(dense1));
    TEST_ASSERT_EQUALITY(v1.m_nonZeros, 6);
    TEST_ASSERT_INEQUALITY(v1.m_data, nullptr);
    TEST_ASSERT_INEQUALITY(v1.m_indices, nullptr);
    TEST_ASSERT_GREATER_OR_EQUAL(v1.m_capacity, 6);

    TEST_ASSERT_EQUALITY(v1.m_data[0], 3);
    TEST_ASSERT_EQUALITY(v1.m_indices[0], 0);
    TEST_ASSERT_EQUALITY(v1.m_data[1], 4);
    TEST_ASSERT_EQUALITY(v1.m_indices[1], 1);
    TEST_ASSERT_EQUALITY(v1.m_data[2], 1);
    TEST_ASSERT_EQUALITY(v1.m_indices[2], 2);
    TEST_ASSERT_EQUALITY(v1.m_data[3], 5);
    TEST_ASSERT_EQUALITY(v1.m_indices[3], 4);
    TEST_ASSERT_EQUALITY(v1.m_data[4], 6);
    TEST_ASSERT_EQUALITY(v1.m_indices[4], 5);
    TEST_ASSERT_EQUALITY(v1.m_data[5], 9);
    TEST_ASSERT_EQUALITY(v1.m_indices[5], 7);

    TEST_ASSERT_EQUALITY(SparseVector::sm_fullLengthVectorLength, STATIC_ARRAY_SIZE(dense1));
    TEST_ASSERT_INEQUALITY(SparseVector::sm_fullLengthVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexPointerVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexVector, nullptr);

    CHECK_DEFAULT_FUNCTION_POINTERS;
    CHECK_CLEAN_AUXILIARY_VECTORS;

    SparseVector::_globalRelease();
    SparseVector::_globalInit();

    const Numerical::Double dense2[] = {0, 0, 0, 0, 0};

    SparseVector v2 = SparseVector::createVectorFromDenseArray(
                dense2,
                STATIC_ARRAY_SIZE(dense2));
    TEST_ASSERT_EQUALITY(v2.m_length, STATIC_ARRAY_SIZE(dense2));
    TEST_ASSERT_EQUALITY(v2.m_nonZeros, 0);
    TEST_ASSERT_EQUALITY(v2.m_data, nullptr);
    TEST_ASSERT_EQUALITY(v2.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v2.m_capacity, 0);

    TEST_ASSERT_EQUALITY(SparseVector::sm_fullLengthVectorLength, STATIC_ARRAY_SIZE(dense2));
    TEST_ASSERT_INEQUALITY(SparseVector::sm_fullLengthVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexPointerVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexVector, nullptr);

    CHECK_DEFAULT_FUNCTION_POINTERS;
    CHECK_CLEAN_AUXILIARY_VECTORS;

    SparseVector::_globalRelease();
}

void SparseVectorTestSuite::createVectorFromSparseArray()
{
    SparseVector::_globalRelease();
    SparseVector::_globalInit();

    const Numerical::Double data1[] = {3, 4, 1, 5, 6, 9};
    const unsigned int indices1[] =   {0, 1, 2, 4, 5, 7};

    SparseVector v1 = SparseVector::createVectorFromSparseArray(data1,
                                                                indices1,
                                                                STATIC_ARRAY_SIZE(data1),
                                                                10);

    TEST_ASSERT_EQUALITY(v1.m_length, 10);
    TEST_ASSERT_EQUALITY(v1.m_nonZeros, 6);
    TEST_ASSERT_INEQUALITY(v1.m_data, nullptr);
    TEST_ASSERT_INEQUALITY(v1.m_indices, nullptr);
    TEST_ASSERT_GREATER_OR_EQUAL(v1.m_capacity, 6);

    TEST_ASSERT_EQUALITY(v1.m_data[0], 3);
    TEST_ASSERT_EQUALITY(v1.m_indices[0], 0);
    TEST_ASSERT_EQUALITY(v1.m_data[1], 4);
    TEST_ASSERT_EQUALITY(v1.m_indices[1], 1);
    TEST_ASSERT_EQUALITY(v1.m_data[2], 1);
    TEST_ASSERT_EQUALITY(v1.m_indices[2], 2);
    TEST_ASSERT_EQUALITY(v1.m_data[3], 5);
    TEST_ASSERT_EQUALITY(v1.m_indices[3], 4);
    TEST_ASSERT_EQUALITY(v1.m_data[4], 6);
    TEST_ASSERT_EQUALITY(v1.m_indices[4], 5);
    TEST_ASSERT_EQUALITY(v1.m_data[5], 9);
    TEST_ASSERT_EQUALITY(v1.m_indices[5], 7);

    TEST_ASSERT_EQUALITY(SparseVector::sm_fullLengthVectorLength, 10);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_fullLengthVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexPointerVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexVector, nullptr);

    CHECK_DEFAULT_FUNCTION_POINTERS;
    CHECK_CLEAN_AUXILIARY_VECTORS;

    SparseVector::_globalRelease();
    SparseVector::_globalInit();

    SparseVector v2 = SparseVector::createVectorFromSparseArray(nullptr,
                                                                nullptr,
                                                                0,
                                                                20);
    TEST_ASSERT_EQUALITY(v2.m_length, 20);
    TEST_ASSERT_EQUALITY(v2.m_nonZeros, 0);
    TEST_ASSERT_EQUALITY(v2.m_data, nullptr);
    TEST_ASSERT_EQUALITY(v2.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v2.m_capacity, 0);

    TEST_ASSERT_EQUALITY(SparseVector::sm_fullLengthVectorLength, 20);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_fullLengthVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexPointerVector, nullptr);
    TEST_ASSERT_INEQUALITY(SparseVector::sm_indexVector, nullptr);

    CHECK_DEFAULT_FUNCTION_POINTERS;
    CHECK_CLEAN_AUXILIARY_VECTORS;

    SparseVector::_globalRelease();
}

void SparseVectorTestSuite::createEtaVector()
{
    SparseVector::_globalRelease();
    SparseVector::_globalInit();

    const Numerical::Double data1[] = {1.4, 6.7, -0.1, 3.0, 0, -2.4};

    SparseVector vec = SparseVector::createVectorFromDenseArray(data1, 6);

    SparseVector * eta = nullptr;
    try {
        eta = SparseVector::createEtaVector(vec, 4.0, 3);
        TEST_ASSERT_EQUALITY(eta->m_length, 6);
        TEST_ASSERT_EQUALITY(eta->m_nonZeros, 5);
        TEST_ASSERT_INEQUALITY(eta->m_data, nullptr);
        TEST_ASSERT_INEQUALITY(eta->m_indices, nullptr);
        TEST_ASSERT_GREATER_OR_EQUAL(eta->m_capacity, 5);

        TEST_ASSERT_EQUALITY(eta->m_data[0], -1.4 / 4.0);
        TEST_ASSERT_EQUALITY(eta->m_indices[0], 0);
        TEST_ASSERT_EQUALITY(eta->m_data[1], -6.7 / 4.0);
        TEST_ASSERT_EQUALITY(eta->m_indices[1], 1);
        TEST_ASSERT_EQUALITY(eta->m_data[2], 0.1 / 4.0);
        TEST_ASSERT_EQUALITY(eta->m_indices[2], 2);
        TEST_ASSERT_EQUALITY(eta->m_data[3], 1.0 / 4.0);
        TEST_ASSERT_EQUALITY(eta->m_indices[3], 3);
        TEST_ASSERT_EQUALITY(eta->m_data[4], 2.4 / 4.0);
        TEST_ASSERT_EQUALITY(eta->m_indices[4], 5);

    } catch (...) {
        EMIT_ERROR("Invalid exception in the SparseVector::createEtaVector");
    }

    delete eta;
    eta = nullptr;

    try {
        eta = SparseVector::createEtaVector(vec, 0.0, 3);
        EMIT_ERROR("An exception was excepted after SparseVector::createEtaVector");
        delete eta;
    } catch ( const NumericalException & ) {

    } catch (...) {
        EMIT_ERROR("Invalid exception in the SparseVector::createEtaVector");
    }

    eta = nullptr;

    CHECK_CLEAN_AUXILIARY_VECTORS;


    SparseVector::_globalRelease();
}

void SparseVectorTestSuite::at()
{
    SparseVector::_globalRelease();
    SparseVector::_globalInit();

    unsigned int index;
    unsigned int size;

    for (size = 0; size < 15; size++) {
        SparseVector v1(size);
        for (index = 0; index < 10; index++) {
            if (index < size) {
                TEST_ASSERT_EQUALITY(v1.at(index), 0.0);


                //TEST_ASSERT_EQUALITY(v1.at(index), 1.0);
                //TEST_ASSERT_INEQUALITY(v1.at(index), 0.0);
            } else {
                try {
                    v1.at(index);
                    EMIT_ERROR("An exception was excepted after SparseVector::at");
                } catch (InvalidIndexException & ex) {
                    TEST_ASSERT_EQUALITY(ex.getMax(), 0);
                    TEST_ASSERT_EQUALITY(ex.getMin(), 0);
                    TEST_ASSERT_EQUALITY(ex.getValue(), index);
                } catch (...) {
                    EMIT_ERROR("Invalid exception in the SparseVector::at");
                }
            }
        }
    }

    Numerical::Double data1[] = {0.0, 5.6, -4.0, 1.0, 0.0, 0.0, 1.0};
    SparseVector v2 = SparseVector::createVectorFromDenseArray(data1, STATIC_ARRAY_SIZE(data1));

    for (index = 0; index < STATIC_ARRAY_SIZE(data1) * 2; index++) {
        if (index < STATIC_ARRAY_SIZE(data1)) {
            TEST_ASSERT_EQUALITY(v2.at(index), data1[index]);
        } else {
            try {
                v2.at(index);
                EMIT_ERROR("An exception was excepted after SparseVector::at");
            } catch (InvalidIndexException & ex) {
                TEST_ASSERT_EQUALITY(ex.getMax(), 0);
                TEST_ASSERT_EQUALITY(ex.getMin(), 0);
                TEST_ASSERT_EQUALITY(ex.getValue(), index);
            } catch (...) {
                EMIT_ERROR("Invalid exception in the SparseVector::at");
            }
        }
    }

    Numerical::Double data2[] = {4.3, 1.1, -3.4};
    unsigned int indices[] = {5, 1, 7};
    SparseVector v3 = SparseVector::createVectorFromSparseArray(data2, indices, STATIC_ARRAY_SIZE(data2), 10);

    TEST_ASSERT_EQUALITY(v3.at(0), 0.0);
    TEST_ASSERT_EQUALITY(v3.at(1), 1.1);
    TEST_ASSERT_EQUALITY(v3.at(2), 0.0);
    TEST_ASSERT_EQUALITY(v3.at(3), 0.0);
    TEST_ASSERT_EQUALITY(v3.at(4), 0.0);
    TEST_ASSERT_EQUALITY(v3.at(5), 4.3);
    TEST_ASSERT_EQUALITY(v3.at(6), 0.0);
    TEST_ASSERT_EQUALITY(v3.at(7), -3.4);
    TEST_ASSERT_EQUALITY(v3.at(8), 0.0);
    TEST_ASSERT_EQUALITY(v3.at(9), 0.0);

    for (index = 10; index < 15; index++) {
        try {
            v3.at(index);
            EMIT_ERROR("An exception was excepted after SparseVector::at");
        } catch (InvalidIndexException & ex) {
            TEST_ASSERT_EQUALITY(ex.getMax(), 0);
            TEST_ASSERT_EQUALITY(ex.getMin(), 0);
            TEST_ASSERT_EQUALITY(ex.getValue(), index);
        } catch (...) {
            EMIT_ERROR("Invalid exception in the SparseVector::at");
        }
    }

    SparseVector::_globalRelease();
}

void SparseVectorTestSuite::arrayOperator()
{
    SparseVector::_globalRelease();
    SparseVector::_globalInit();

    unsigned int index;
    unsigned int size;

    for (size = 1; size < 10; size++) {
        SparseVector v1(size);
        for (index = 0; index < size; index++) {
            TEST_ASSERT_EQUALITY(v1[index], 0.0);
        }
    }

    Numerical::Double data1[] = {0.0, 5.6, -4.0, 1.0, 0.0, 0.0, 1.0};
    SparseVector v2 = SparseVector::createVectorFromDenseArray(data1, STATIC_ARRAY_SIZE(data1));

    for (index = 0; index < STATIC_ARRAY_SIZE(data1); index++) {
        TEST_ASSERT_EQUALITY(v2.at(index), data1[index]);
    }

    Numerical::Double data2[] = {4.3, 1.1, -3.4};
    unsigned int indices[] = {5, 1, 7};
    SparseVector v3 = SparseVector::createVectorFromSparseArray(data2, indices, STATIC_ARRAY_SIZE(data2), 10);

    TEST_ASSERT_EQUALITY(v3[0], 0.0);
    TEST_ASSERT_EQUALITY(v3[1], 1.1);
    TEST_ASSERT_EQUALITY(v3[2], 0.0);
    TEST_ASSERT_EQUALITY(v3[3], 0.0);
    TEST_ASSERT_EQUALITY(v3[4], 0.0);
    TEST_ASSERT_EQUALITY(v3[5], 4.3);
    TEST_ASSERT_EQUALITY(v3[6], 0.0);
    TEST_ASSERT_EQUALITY(v3[7], -3.4);
    TEST_ASSERT_EQUALITY(v3[8], 0.0);
    TEST_ASSERT_EQUALITY(v3[9], 0.0);

    SparseVector::_globalRelease();
}

void SparseVectorTestSuite::set()
{
    SparseVector::_globalRelease();
    SparseVector::_globalInit();

    SparseVector v1(1);

    v1.set(0, 4.5);

    TEST_ASSERT_GREATER_OR_EQUAL(v1.m_capacity, 1);
    TEST_ASSERT_INEQUALITY(v1.m_data, nullptr);
    TEST_ASSERT_INEQUALITY(v1.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v1.m_length, 1);
    TEST_ASSERT_EQUALITY(v1.m_nonZeros, 1);
    TEST_ASSERT_EQUALITY(v1.m_data[0], 4.5);
    TEST_ASSERT_EQUALITY(v1.m_indices[0], 0);

    CHECK_CLEAN_AUXILIARY_VECTORS;
    CHECK_DEFAULT_FUNCTION_POINTERS;

    v1.set(0, 0.0);

    TEST_ASSERT_GREATER_OR_EQUAL(v1.m_capacity, 0);
    if (v1.m_capacity == 0) {
        TEST_ASSERT_EQUALITY(v1.m_data, nullptr);
        TEST_ASSERT_EQUALITY(v1.m_indices, nullptr);
    } else {
        TEST_ASSERT_INEQUALITY(v1.m_data, nullptr);
        TEST_ASSERT_INEQUALITY(v1.m_indices, nullptr);
    }
    TEST_ASSERT_EQUALITY(v1.m_length, 1);
    TEST_ASSERT_EQUALITY(v1.m_nonZeros, 0);

    CHECK_CLEAN_AUXILIARY_VECTORS;
    CHECK_DEFAULT_FUNCTION_POINTERS;

    SparseVector v2(5);

    v2.set(3, 3.0);
    TEST_ASSERT_GREATER_OR_EQUAL(v2.m_capacity, 1);
    TEST_ASSERT_INEQUALITY(v2.m_data, nullptr);
    TEST_ASSERT_INEQUALITY(v2.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v2.m_length, 5);
    TEST_ASSERT_EQUALITY(v2.m_nonZeros, 1);
    TEST_ASSERT_EQUALITY(v2.m_data[0], 3.0);
    TEST_ASSERT_EQUALITY(v2.m_indices[0], 3);

    CHECK_CLEAN_AUXILIARY_VECTORS;
    CHECK_DEFAULT_FUNCTION_POINTERS;

    v2.set(1, 1.0);
    TEST_ASSERT_GREATER_OR_EQUAL(v2.m_capacity, 2);
    TEST_ASSERT_INEQUALITY(v2.m_data, nullptr);
    TEST_ASSERT_INEQUALITY(v2.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v2.m_length, 5);
    TEST_ASSERT_EQUALITY(v2.m_nonZeros, 2);
    TEST_ASSERT_EQUALITY(v2.m_data[0], 3.0);
    TEST_ASSERT_EQUALITY(v2.m_indices[0], 3);
    TEST_ASSERT_EQUALITY(v2.m_data[1], 1.0);
    TEST_ASSERT_EQUALITY(v2.m_indices[1], 1);

    CHECK_CLEAN_AUXILIARY_VECTORS;
    CHECK_DEFAULT_FUNCTION_POINTERS;

    v2.set(4, 4.0);
    TEST_ASSERT_GREATER_OR_EQUAL(v2.m_capacity, 3);
    TEST_ASSERT_INEQUALITY(v2.m_data, nullptr);
    TEST_ASSERT_INEQUALITY(v2.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v2.m_length, 5);
    TEST_ASSERT_EQUALITY(v2.m_nonZeros, 3);
    TEST_ASSERT_EQUALITY(v2.m_data[0], 3.0);
    TEST_ASSERT_EQUALITY(v2.m_indices[0], 3);
    TEST_ASSERT_EQUALITY(v2.m_data[1], 1.0);
    TEST_ASSERT_EQUALITY(v2.m_indices[1], 1);
    TEST_ASSERT_EQUALITY(v2.m_data[2], 4.0);
    TEST_ASSERT_EQUALITY(v2.m_indices[2], 4);

    CHECK_CLEAN_AUXILIARY_VECTORS;
    CHECK_DEFAULT_FUNCTION_POINTERS;

    v2.set(3, 3.2);
    TEST_ASSERT_GREATER_OR_EQUAL(v2.m_capacity, 3);
    TEST_ASSERT_INEQUALITY(v2.m_data, nullptr);
    TEST_ASSERT_INEQUALITY(v2.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v2.m_length, 5);
    TEST_ASSERT_EQUALITY(v2.m_nonZeros, 3);
    TEST_ASSERT_EQUALITY(v2.m_data[0], 3.2);
    TEST_ASSERT_EQUALITY(v2.m_indices[0], 3);
    TEST_ASSERT_EQUALITY(v2.m_data[1], 1.0);
    TEST_ASSERT_EQUALITY(v2.m_indices[1], 1);
    TEST_ASSERT_EQUALITY(v2.m_data[2], 4.0);
    TEST_ASSERT_EQUALITY(v2.m_indices[2], 4);

    CHECK_CLEAN_AUXILIARY_VECTORS;
    CHECK_DEFAULT_FUNCTION_POINTERS;

    v2.set(2, 0.0);
    TEST_ASSERT_GREATER_OR_EQUAL(v2.m_capacity, 3);
    TEST_ASSERT_INEQUALITY(v2.m_data, nullptr);
    TEST_ASSERT_INEQUALITY(v2.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v2.m_length, 5);
    TEST_ASSERT_EQUALITY(v2.m_nonZeros, 3);
    TEST_ASSERT_EQUALITY(v2.m_data[0], 3.2);
    TEST_ASSERT_EQUALITY(v2.m_indices[0], 3);
    TEST_ASSERT_EQUALITY(v2.m_data[1], 1.0);
    TEST_ASSERT_EQUALITY(v2.m_indices[1], 1);
    TEST_ASSERT_EQUALITY(v2.m_data[2], 4.0);
    TEST_ASSERT_EQUALITY(v2.m_indices[2], 4);

    CHECK_CLEAN_AUXILIARY_VECTORS;
    CHECK_DEFAULT_FUNCTION_POINTERS;

    v2.set(2, 2.0);
    TEST_ASSERT_GREATER_OR_EQUAL(v2.m_capacity, 4);
    TEST_ASSERT_INEQUALITY(v2.m_data, nullptr);
    TEST_ASSERT_INEQUALITY(v2.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v2.m_length, 5);
    TEST_ASSERT_EQUALITY(v2.m_nonZeros, 4);
    TEST_ASSERT_EQUALITY(v2.m_data[0], 3.2);
    TEST_ASSERT_EQUALITY(v2.m_indices[0], 3);
    TEST_ASSERT_EQUALITY(v2.m_data[1], 1.0);
    TEST_ASSERT_EQUALITY(v2.m_indices[1], 1);
    TEST_ASSERT_EQUALITY(v2.m_data[2], 4.0);
    TEST_ASSERT_EQUALITY(v2.m_indices[2], 4);
    TEST_ASSERT_EQUALITY(v2.m_data[3], 2.0);
    TEST_ASSERT_EQUALITY(v2.m_indices[3], 2);

    CHECK_CLEAN_AUXILIARY_VECTORS;
    CHECK_DEFAULT_FUNCTION_POINTERS;

    v2.set(1, 0.0);
    TEST_ASSERT_GREATER_OR_EQUAL(v2.m_capacity, 3);
    TEST_ASSERT_INEQUALITY(v2.m_data, nullptr);
    TEST_ASSERT_INEQUALITY(v2.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v2.m_length, 5);
    TEST_ASSERT_EQUALITY(v2.m_nonZeros, 3);
    TEST_ASSERT_EQUALITY(v2.m_data[0], 3.2);
    TEST_ASSERT_EQUALITY(v2.m_indices[0], 3);
    TEST_ASSERT_EQUALITY(v2.m_data[1], 2.0);
    TEST_ASSERT_EQUALITY(v2.m_indices[1], 2);
    TEST_ASSERT_EQUALITY(v2.m_data[2], 4.0);
    TEST_ASSERT_EQUALITY(v2.m_indices[2], 4);

    CHECK_CLEAN_AUXILIARY_VECTORS;
    CHECK_DEFAULT_FUNCTION_POINTERS;

    v2.set(1, 1.0);
    TEST_ASSERT_GREATER_OR_EQUAL(v2.m_capacity, 4);
    TEST_ASSERT_INEQUALITY(v2.m_data, nullptr);
    TEST_ASSERT_INEQUALITY(v2.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v2.m_length, 5);
    TEST_ASSERT_EQUALITY(v2.m_nonZeros, 4);
    TEST_ASSERT_EQUALITY(v2.m_data[0], 3.2);
    TEST_ASSERT_EQUALITY(v2.m_indices[0], 3);
    TEST_ASSERT_EQUALITY(v2.m_data[1], 2.0);
    TEST_ASSERT_EQUALITY(v2.m_indices[1], 2);
    TEST_ASSERT_EQUALITY(v2.m_data[2], 4.0);
    TEST_ASSERT_EQUALITY(v2.m_indices[2], 4);
    TEST_ASSERT_EQUALITY(v2.m_data[3], 1.0);
    TEST_ASSERT_EQUALITY(v2.m_indices[3], 1);

    CHECK_CLEAN_AUXILIARY_VECTORS;
    CHECK_DEFAULT_FUNCTION_POINTERS;

    v2.set(0, 0.5);
    TEST_ASSERT_GREATER_OR_EQUAL(v2.m_capacity, 5);
    TEST_ASSERT_INEQUALITY(v2.m_data, nullptr);
    TEST_ASSERT_INEQUALITY(v2.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v2.m_length, 5);
    TEST_ASSERT_EQUALITY(v2.m_nonZeros, 5);
    TEST_ASSERT_EQUALITY(v2.m_data[0], 3.2);
    TEST_ASSERT_EQUALITY(v2.m_indices[0], 3);
    TEST_ASSERT_EQUALITY(v2.m_data[1], 2.0);
    TEST_ASSERT_EQUALITY(v2.m_indices[1], 2);
    TEST_ASSERT_EQUALITY(v2.m_data[2], 4.0);
    TEST_ASSERT_EQUALITY(v2.m_indices[2], 4);
    TEST_ASSERT_EQUALITY(v2.m_data[3], 1.0);
    TEST_ASSERT_EQUALITY(v2.m_indices[3], 1);
    TEST_ASSERT_EQUALITY(v2.m_data[4], 0.5);
    TEST_ASSERT_EQUALITY(v2.m_indices[4], 0);

    CHECK_CLEAN_AUXILIARY_VECTORS;
    CHECK_DEFAULT_FUNCTION_POINTERS;

    v2.set(2, 2.5);
    TEST_ASSERT_GREATER_OR_EQUAL(v2.m_capacity, 5);
    TEST_ASSERT_INEQUALITY(v2.m_data, nullptr);
    TEST_ASSERT_INEQUALITY(v2.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v2.m_length, 5);
    TEST_ASSERT_EQUALITY(v2.m_nonZeros, 5);
    TEST_ASSERT_EQUALITY(v2.m_data[0], 3.2);
    TEST_ASSERT_EQUALITY(v2.m_indices[0], 3);
    TEST_ASSERT_EQUALITY(v2.m_data[1], 2.5);
    TEST_ASSERT_EQUALITY(v2.m_indices[1], 2);
    TEST_ASSERT_EQUALITY(v2.m_data[2], 4.0);
    TEST_ASSERT_EQUALITY(v2.m_indices[2], 4);
    TEST_ASSERT_EQUALITY(v2.m_data[3], 1.0);
    TEST_ASSERT_EQUALITY(v2.m_indices[3], 1);
    TEST_ASSERT_EQUALITY(v2.m_data[4], 0.5);
    TEST_ASSERT_EQUALITY(v2.m_indices[4], 0);

    CHECK_CLEAN_AUXILIARY_VECTORS;
    CHECK_DEFAULT_FUNCTION_POINTERS;

    v2.set(3, 0.0);
    TEST_ASSERT_GREATER_OR_EQUAL(v2.m_capacity, 4);
    TEST_ASSERT_INEQUALITY(v2.m_data, nullptr);
    TEST_ASSERT_INEQUALITY(v2.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v2.m_length, 5);
    TEST_ASSERT_EQUALITY(v2.m_nonZeros, 4);
    TEST_ASSERT_EQUALITY(v2.m_data[0], 0.5);
    TEST_ASSERT_EQUALITY(v2.m_indices[0], 0);
    TEST_ASSERT_EQUALITY(v2.m_data[1], 2.5);
    TEST_ASSERT_EQUALITY(v2.m_indices[1], 2);
    TEST_ASSERT_EQUALITY(v2.m_data[2], 4.0);
    TEST_ASSERT_EQUALITY(v2.m_indices[2], 4);
    TEST_ASSERT_EQUALITY(v2.m_data[3], 1.0);
    TEST_ASSERT_EQUALITY(v2.m_indices[3], 1);

    CHECK_CLEAN_AUXILIARY_VECTORS;
    CHECK_DEFAULT_FUNCTION_POINTERS;

    v2.set(3, 0.0);
    TEST_ASSERT_GREATER_OR_EQUAL(v2.m_capacity, 4);
    TEST_ASSERT_INEQUALITY(v2.m_data, nullptr);
    TEST_ASSERT_INEQUALITY(v2.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v2.m_length, 5);
    TEST_ASSERT_EQUALITY(v2.m_nonZeros, 4);
    TEST_ASSERT_EQUALITY(v2.m_data[0], 0.5);
    TEST_ASSERT_EQUALITY(v2.m_indices[0], 0);
    TEST_ASSERT_EQUALITY(v2.m_data[1], 2.5);
    TEST_ASSERT_EQUALITY(v2.m_indices[1], 2);
    TEST_ASSERT_EQUALITY(v2.m_data[2], 4.0);
    TEST_ASSERT_EQUALITY(v2.m_indices[2], 4);
    TEST_ASSERT_EQUALITY(v2.m_data[3], 1.0);
    TEST_ASSERT_EQUALITY(v2.m_indices[3], 1);

    CHECK_CLEAN_AUXILIARY_VECTORS;
    CHECK_DEFAULT_FUNCTION_POINTERS;

    v2.set(1, 0.0);
    TEST_ASSERT_GREATER_OR_EQUAL(v2.m_capacity, 3);
    TEST_ASSERT_INEQUALITY(v2.m_data, nullptr);
    TEST_ASSERT_INEQUALITY(v2.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v2.m_length, 5);
    TEST_ASSERT_EQUALITY(v2.m_nonZeros, 3);
    TEST_ASSERT_EQUALITY(v2.m_data[0], 0.5);
    TEST_ASSERT_EQUALITY(v2.m_indices[0], 0);
    TEST_ASSERT_EQUALITY(v2.m_data[1], 2.5);
    TEST_ASSERT_EQUALITY(v2.m_indices[1], 2);
    TEST_ASSERT_EQUALITY(v2.m_data[2], 4.0);
    TEST_ASSERT_EQUALITY(v2.m_indices[2], 4);

    CHECK_CLEAN_AUXILIARY_VECTORS;
    CHECK_DEFAULT_FUNCTION_POINTERS;

    v2.set(2, 0.0);
    TEST_ASSERT_GREATER_OR_EQUAL(v2.m_capacity, 2);
    TEST_ASSERT_INEQUALITY(v2.m_data, nullptr);
    TEST_ASSERT_INEQUALITY(v2.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v2.m_length, 5);
    TEST_ASSERT_EQUALITY(v2.m_nonZeros, 2);
    TEST_ASSERT_EQUALITY(v2.m_data[0], 0.5);
    TEST_ASSERT_EQUALITY(v2.m_indices[0], 0);
    TEST_ASSERT_EQUALITY(v2.m_data[1], 4.0);
    TEST_ASSERT_EQUALITY(v2.m_indices[1], 4);

    CHECK_CLEAN_AUXILIARY_VECTORS;
    CHECK_DEFAULT_FUNCTION_POINTERS;

    v2.set(0, 0.0);
    TEST_ASSERT_GREATER_OR_EQUAL(v2.m_capacity, 1);
    TEST_ASSERT_INEQUALITY(v2.m_data, nullptr);
    TEST_ASSERT_INEQUALITY(v2.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v2.m_length, 5);
    TEST_ASSERT_EQUALITY(v2.m_nonZeros, 1);
    TEST_ASSERT_EQUALITY(v2.m_data[0], 4.0);
    TEST_ASSERT_EQUALITY(v2.m_indices[0], 4);

    CHECK_CLEAN_AUXILIARY_VECTORS;
    CHECK_DEFAULT_FUNCTION_POINTERS;

    v2.set(0, 0.0);
    TEST_ASSERT_GREATER_OR_EQUAL(v2.m_capacity, 1);
    TEST_ASSERT_INEQUALITY(v2.m_data, nullptr);
    TEST_ASSERT_INEQUALITY(v2.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v2.m_length, 5);
    TEST_ASSERT_EQUALITY(v2.m_nonZeros, 1);
    TEST_ASSERT_EQUALITY(v2.m_data[0], 4.0);
    TEST_ASSERT_EQUALITY(v2.m_indices[0], 4);

    CHECK_CLEAN_AUXILIARY_VECTORS;
    CHECK_DEFAULT_FUNCTION_POINTERS;

    v2.set(4, 0.0);
    TEST_ASSERT_GREATER_OR_EQUAL(v2.m_capacity, 0);
    if (v2.m_capacity == 0) {
        TEST_ASSERT_EQUALITY(v2.m_data, nullptr);
        TEST_ASSERT_EQUALITY(v2.m_indices, nullptr);
    } else {
        TEST_ASSERT_INEQUALITY(v2.m_data, nullptr);
        TEST_ASSERT_INEQUALITY(v2.m_indices, nullptr);
    }
    TEST_ASSERT_EQUALITY(v2.m_length, 5);
    TEST_ASSERT_EQUALITY(v2.m_nonZeros, 0);

    CHECK_CLEAN_AUXILIARY_VECTORS;
    CHECK_DEFAULT_FUNCTION_POINTERS;

    SparseVector::_globalRelease();
}

void SparseVectorTestSuite::setNewNonzero()
{
    SparseVector::_globalRelease();
    SparseVector::_globalInit();

    SparseVector v(5);

    v.setNewNonzero(3, 3.2);

    TEST_ASSERT_GREATER_OR_EQUAL(v.m_capacity, 1);
    TEST_ASSERT_INEQUALITY(v.m_data, nullptr);
    TEST_ASSERT_INEQUALITY(v.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v.m_length, 5);
    TEST_ASSERT_EQUALITY(v.m_nonZeros, 1);
    TEST_ASSERT_EQUALITY(v.m_data[0], 3.2);
    TEST_ASSERT_EQUALITY(v.m_indices[0], 3);

    CHECK_CLEAN_AUXILIARY_VECTORS;
    CHECK_DEFAULT_FUNCTION_POINTERS;

    v.setNewNonzero(2, 2.5);

    TEST_ASSERT_GREATER_OR_EQUAL(v.m_capacity, 2);
    TEST_ASSERT_INEQUALITY(v.m_data, nullptr);
    TEST_ASSERT_INEQUALITY(v.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v.m_length, 5);
    TEST_ASSERT_EQUALITY(v.m_nonZeros, 2);
    TEST_ASSERT_EQUALITY(v.m_data[0], 3.2);
    TEST_ASSERT_EQUALITY(v.m_indices[0], 3);
    TEST_ASSERT_EQUALITY(v.m_data[1], 2.5);
    TEST_ASSERT_EQUALITY(v.m_indices[1], 2);

    CHECK_CLEAN_AUXILIARY_VECTORS;
    CHECK_DEFAULT_FUNCTION_POINTERS;

    v.setNewNonzero(4, 4.0);

    TEST_ASSERT_GREATER_OR_EQUAL(v.m_capacity, 3);
    TEST_ASSERT_INEQUALITY(v.m_data, nullptr);
    TEST_ASSERT_INEQUALITY(v.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v.m_length, 5);
    TEST_ASSERT_EQUALITY(v.m_nonZeros, 3);
    TEST_ASSERT_EQUALITY(v.m_data[0], 3.2);
    TEST_ASSERT_EQUALITY(v.m_indices[0], 3);
    TEST_ASSERT_EQUALITY(v.m_data[1], 2.5);
    TEST_ASSERT_EQUALITY(v.m_indices[1], 2);
    TEST_ASSERT_EQUALITY(v.m_data[2], 4.0);
    TEST_ASSERT_EQUALITY(v.m_indices[2], 4);

    CHECK_CLEAN_AUXILIARY_VECTORS;
    CHECK_DEFAULT_FUNCTION_POINTERS;

    v.setNewNonzero(1, 1.0);

    TEST_ASSERT_GREATER_OR_EQUAL(v.m_capacity, 4);
    TEST_ASSERT_INEQUALITY(v.m_data, nullptr);
    TEST_ASSERT_INEQUALITY(v.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v.m_length, 5);
    TEST_ASSERT_EQUALITY(v.m_nonZeros, 4);
    TEST_ASSERT_EQUALITY(v.m_data[0], 3.2);
    TEST_ASSERT_EQUALITY(v.m_indices[0], 3);
    TEST_ASSERT_EQUALITY(v.m_data[1], 2.5);
    TEST_ASSERT_EQUALITY(v.m_indices[1], 2);
    TEST_ASSERT_EQUALITY(v.m_data[2], 4.0);
    TEST_ASSERT_EQUALITY(v.m_indices[2], 4);
    TEST_ASSERT_EQUALITY(v.m_data[3], 1.0);
    TEST_ASSERT_EQUALITY(v.m_indices[3], 1);

    CHECK_CLEAN_AUXILIARY_VECTORS;
    CHECK_DEFAULT_FUNCTION_POINTERS;

    v.setNewNonzero(0, 0.5);

    TEST_ASSERT_GREATER_OR_EQUAL(v.m_capacity, 5);
    TEST_ASSERT_INEQUALITY(v.m_data, nullptr);
    TEST_ASSERT_INEQUALITY(v.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v.m_length, 5);
    TEST_ASSERT_EQUALITY(v.m_nonZeros, 5);
    TEST_ASSERT_EQUALITY(v.m_data[0], 3.2);
    TEST_ASSERT_EQUALITY(v.m_indices[0], 3);
    TEST_ASSERT_EQUALITY(v.m_data[1], 2.5);
    TEST_ASSERT_EQUALITY(v.m_indices[1], 2);
    TEST_ASSERT_EQUALITY(v.m_data[2], 4.0);
    TEST_ASSERT_EQUALITY(v.m_indices[2], 4);
    TEST_ASSERT_EQUALITY(v.m_data[3], 1.0);
    TEST_ASSERT_EQUALITY(v.m_indices[3], 1);
    TEST_ASSERT_EQUALITY(v.m_data[4], 0.5);
    TEST_ASSERT_EQUALITY(v.m_indices[4], 0);

    CHECK_CLEAN_AUXILIARY_VECTORS;
    CHECK_DEFAULT_FUNCTION_POINTERS;

    SparseVector::_globalRelease();
}

void SparseVectorTestSuite::length()
{
    SparseVector::_globalRelease();
    SparseVector::_globalInit();

    SparseVector v1;

    TEST_ASSERT_EQUALITY(v1.length(), 0);

    SparseVector v2(10);

    TEST_ASSERT_EQUALITY(v2.length(), 10);

    v2.set(4, 6.4);
    v2.set(1, 1.3);

    TEST_ASSERT_EQUALITY(v2.length(), 10);

    SparseVector::_globalRelease();
}

void SparseVectorTestSuite::nonZeros()
{
    SparseVector::_globalRelease();
    SparseVector::_globalInit();

    SparseVector v1;
    TEST_ASSERT_EQUALITY(v1.nonZeros(), 0);

    SparseVector v2(10);
    TEST_ASSERT_EQUALITY(v2.nonZeros(), 0);

    v2.set(3, 1);
    TEST_ASSERT_EQUALITY(v2.nonZeros(), 1);

    v2.set(3, 0);
    TEST_ASSERT_EQUALITY(v2.nonZeros(), 0);

    v2.set(5, 0);
    TEST_ASSERT_EQUALITY(v2.nonZeros(), 0);

    v2.set(6, 3);
    TEST_ASSERT_EQUALITY(v2.nonZeros(), 1);

    v2.set(3, 3);
    TEST_ASSERT_EQUALITY(v2.nonZeros(), 2);

    v2.set(1, 3);
    TEST_ASSERT_EQUALITY(v2.nonZeros(), 3);

    SparseVector::_globalRelease();
}

void SparseVectorTestSuite::euclidNorm()
{
    SparseVector::_globalRelease();
    SparseVector::_globalInit();

    SparseVector v1;
    TEST_ASSERT_EQUALITY(v1.euclidNorm(), 0);

    SparseVector v2(5);
    TEST_ASSERT_EQUALITY(v2.euclidNorm(), 0);

    v2.set(0, 1);
    TEST_ASSERT_EQUALITY(v2.euclidNorm(), 1);

    v2.set(0, -1);
    TEST_ASSERT_EQUALITY(v2.euclidNorm(), 1);

    v2.set(1, 2);
    TEST_ASSERT_EQUALITY(v2.euclidNorm(), Numerical::sqrt(1 + 4));

    v2.set(2, -3);
    TEST_ASSERT_EQUALITY(v2.euclidNorm(), Numerical::sqrt(1 + 4 + 9));

    v2.set(3, 4);
    TEST_ASSERT_EQUALITY(v2.euclidNorm(), Numerical::sqrt(1 + 4 + 9 + 16));

    v2.set(4, -5);
    TEST_ASSERT_EQUALITY(v2.euclidNorm(), Numerical::sqrt(1 + 4 + 9 + 16 + 25));

    SparseVector::_globalRelease();
}

void SparseVectorTestSuite::l1Norm()
{
    SparseVector::_globalRelease();
    SparseVector::_globalInit();

    SparseVector v1;
    TEST_ASSERT_EQUALITY(v1.l1Norm(), 0);

    SparseVector v2(5);
    TEST_ASSERT_EQUALITY(v2.l1Norm(), 0);

    v2.set(0, 1);
    TEST_ASSERT_EQUALITY(v2.l1Norm(), 1);

    v2.set(0, -1);
    TEST_ASSERT_EQUALITY(v2.l1Norm(), 1);

    v2.set(1, 2);
    TEST_ASSERT_EQUALITY(v2.l1Norm(), 1 + 2);

    v2.set(2, -3);
    TEST_ASSERT_EQUALITY(v2.l1Norm(), 1 + 2 + 3);

    v2.set(3, 4);
    TEST_ASSERT_EQUALITY(v2.l1Norm(), 1 + 2 + 3 + 4);

    v2.set(4, -5);
    TEST_ASSERT_EQUALITY(v2.l1Norm(), 1 + 2 + 3 + 4 + 5);


    SparseVector::_globalRelease();
}

void SparseVectorTestSuite::elementaryFtran()
{
    SparseVector::_globalRelease();
    SparseVector::_globalInit();

    SparseVector v1(5), eta1(5);
    v1.elementaryFtran(eta1, 0);

    TEST_ASSERT_EQUALITY(v1.m_capacity, 0);
    TEST_ASSERT_EQUALITY(v1.m_data, nullptr);
    TEST_ASSERT_EQUALITY(v1.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v1.m_length, 5);
    TEST_ASSERT_EQUALITY(v1.m_nonZeros, 0);

    CHECK_CLEAN_AUXILIARY_VECTORS;
    CHECK_DEFAULT_FUNCTION_POINTERS;

    eta1.set(0, 1);
    eta1.set(1, 2);
    eta1.set(2, 3);
    eta1.set(3, 4);
    eta1.set(4, 5);

    v1.elementaryFtran(eta1, 1);

    TEST_ASSERT_EQUALITY(v1.m_capacity, 0);
    TEST_ASSERT_EQUALITY(v1.m_data, nullptr);
    TEST_ASSERT_EQUALITY(v1.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v1.m_length, 5);
    TEST_ASSERT_EQUALITY(v1.m_nonZeros, 0);

    CHECK_CLEAN_AUXILIARY_VECTORS;
    CHECK_DEFAULT_FUNCTION_POINTERS;

    v1.set(0, 1);
    v1.set(1, 0);
    v1.set(2, 3);
    v1.set(3, 4);
    v1.set(4, 5);

    v1.elementaryFtran(eta1, 1);

    TEST_ASSERT_GREATER_OR_EQUAL(v1.m_capacity, 4);
    TEST_ASSERT_INEQUALITY(v1.m_data, nullptr);
    TEST_ASSERT_INEQUALITY(v1.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v1.m_length, 5);
    TEST_ASSERT_EQUALITY(v1.m_nonZeros, 4);

    TEST_ASSERT_EQUALITY(v1[0], 1);
    TEST_ASSERT_EQUALITY(v1[1], 0);
    TEST_ASSERT_EQUALITY(v1[2], 3);
    TEST_ASSERT_EQUALITY(v1[3], 4);
    TEST_ASSERT_EQUALITY(v1[4], 5);

    CHECK_CLEAN_AUXILIARY_VECTORS;
    CHECK_DEFAULT_FUNCTION_POINTERS;

    v1.elementaryFtran(eta1, 2);

    TEST_ASSERT_GREATER_OR_EQUAL(v1.m_capacity, 5);
    TEST_ASSERT_INEQUALITY(v1.m_data, nullptr);
    TEST_ASSERT_INEQUALITY(v1.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v1.m_length, 5);
    TEST_ASSERT_EQUALITY(v1.m_nonZeros, 5);

    TEST_ASSERT_EQUALITY(v1[0], 1 + 3 * 1);
    TEST_ASSERT_EQUALITY(v1[1], 0 + 3 * 2);
    TEST_ASSERT_EQUALITY(v1[2], 3 * 3);
    TEST_ASSERT_EQUALITY(v1[3], 4 + 3 * 4);
    TEST_ASSERT_EQUALITY(v1[4], 5 + 3 * 5);

    CHECK_CLEAN_AUXILIARY_VECTORS;
    CHECK_DEFAULT_FUNCTION_POINTERS;

    SparseVector::_globalRelease();
}

void SparseVectorTestSuite::addSparseVector()
{
    SparseVector::_globalRelease();
    SparseVector::_globalInit();

    SparseVector::setAddMode(Numerical::ADD_FAST);

    int lambda;

    for (lambda = -2; lambda <= 2; lambda++) {
        testWithPermutationPair<Numerical::Double, Numerical::Double>({1, 2, 3, 4},
        {5, 6, 7, 8},
                                                                      [lambda, this](const Numerical::Double * array1,
                                                                      unsigned int size1,
                                                                      const Numerical::Double * array2,
                                                                      unsigned int size2)->void {

            SparseVector v1 = SparseVector::createVectorFromDenseArray(array1, size1);
            SparseVector v2 = SparseVector::createVectorFromDenseArray(array2, size2);
            v1.addVector(lambda, v2);

            unsigned int i;
            unsigned int nonZeros = 0;
            for (i = 0; i < size1; i++) {
                Numerical::Double result = array1[i] + lambda * array2[i];
                if (result != 0.0) {
                    nonZeros++;
                }

                TEST_ASSERT_EQUALITY(result, v1[i]);
            }

            TEST_ASSERT_GREATER_OR_EQUAL( v1.m_capacity, nonZeros );
            if (v1.m_capacity == 0) {
                TEST_ASSERT_EQUALITY(v1.m_data, nullptr);
                TEST_ASSERT_EQUALITY(v1.m_indices, nullptr);
            } else {
                TEST_ASSERT_INEQUALITY(v1.m_data, nullptr);
                TEST_ASSERT_INEQUALITY(v1.m_indices, nullptr);
            }
            TEST_ASSERT_EQUALITY(v1.m_length, size1);
            TEST_ASSERT_EQUALITY(v1.m_nonZeros, nonZeros);

            CHECK_CLEAN_AUXILIARY_VECTORS;
            CHECK_DEFAULT_DOT_PRODUCT_FUNCTION_POINTERS;
            CHECK_FAST_ADD_FUNCTION_POINTERS;
        });
    }

    unsigned int pattern1;
    unsigned int pattern2;

    lambda = 1;
    for (pattern1 = 0; pattern1 <= 7; pattern1++) {
        for (pattern2 = 0; pattern2 <= 7; pattern2++) {
            testWithPermutationPair<Numerical::Double, Numerical::Double>({1, 2, 3}, {4, 5, 6},
                                                                          [pattern1, pattern2, lambda, this]
                                                                          (const Numerical::Double * array1,
                                                                          unsigned int size1,
                                                                          const Numerical::Double * array2,
                                                                          unsigned int size2)->void {
                unsigned int indices1[] = {0, 1, 2};
                Numerical::Double data1[] = {array1[0], array1[1], array1[2]};

                std::pair<unsigned int, Numerical::Double> pairs1[] =
                {{0, array1[0]}, {1, array1[1]}, {2, array1[2]}};

                do {
                    indices1[0] = pairs1[0].first; indices1[1] = pairs1[1].first;
                    indices1[2] = pairs1[2].first;
                    data1[0] = pairs1[0].second; data1[1] = pairs1[1].second;
                    data1[2] = pairs1[2].second;

                    unsigned int indices2[] = {0, 1, 2};
                    Numerical::Double data2[] = {array2[0], array2[1], array2[2]};
                    std::pair<unsigned int, Numerical::Double> pairs2[] =
                    {{0, array2[0]}, {1, array2[1]}, {2, array2[2]}};


                    do {
                        indices2[0] = pairs2[0].first; indices2[1] = pairs2[1].first;
                        indices2[2] = pairs2[2].first;
                        data2[0] = pairs2[0].second; data2[1] = pairs2[1].second;
                        data2[2] = pairs2[2].second;

                        SparseVector v1 = SparseVector::createVectorFromSparseArray(data1, indices1, size1, size1);
                        SparseVector v2 = SparseVector::createVectorFromSparseArray(data2, indices2, size2, size2);

                        unsigned int i;
                        for (i = 0; i < size1; i++) {
                            if ((pattern1 & (1 << i)) == 0) {
                                v1.set(i, 0);
                            }
                            if ((pattern2 & (1 << i)) == 0) {
                                v2.set(i, 0);
                            }
                        }

                        v1.addVector(lambda, v2);

                        unsigned int nonZeros = 0;
                        for (i = 0; i < size1; i++) {
                            Numerical::Double result = ((pattern1 & (1 << i)) != 0) * array1[i]+
                                    lambda * array2[i] * ((pattern2 & (1 << i)) != 0);
                            if (result != 0.0) {
                                nonZeros++;
                            }

                            TEST_ASSERT_EQUALITY(result, v1[i]);
                        }

                        TEST_ASSERT_GREATER_OR_EQUAL( v1.m_capacity, nonZeros );
                        if (v1.m_capacity == 0) {
                            TEST_ASSERT_EQUALITY(v1.m_data, nullptr);
                            TEST_ASSERT_EQUALITY(v1.m_indices, nullptr);
                        } else {
                            TEST_ASSERT_INEQUALITY(v1.m_data, nullptr);
                            TEST_ASSERT_INEQUALITY(v1.m_indices, nullptr);
                        }
                        TEST_ASSERT_EQUALITY(v1.m_length, size1);
                        TEST_ASSERT_EQUALITY(v1.m_nonZeros, nonZeros);

                        CHECK_CLEAN_AUXILIARY_VECTORS;
                        CHECK_DEFAULT_DOT_PRODUCT_FUNCTION_POINTERS;
                        CHECK_FAST_ADD_FUNCTION_POINTERS;
                    } while(std::next_permutation(pairs2, pairs2 + 3));
                } while (std::next_permutation(pairs1, pairs1 + 3));
            });
        }
    }

    testWithPermutationPair<std::pair<unsigned int, Numerical::Double>,
            std::pair<unsigned int, Numerical::Double>>({{0, 1},
                                                         {1, 0.001},
                                                         {2, 10000},
                                                         {3, 4}},
    {{0, -1.1},
     {1, -0.0011},
     {2, -10000.1},
     {3, 4}},
                                                        [lambda, this](const std::pair<unsigned int, Numerical::Double> * array1,
                                                        unsigned int size1,
                                                        const std::pair<unsigned int, Numerical::Double> * array2,
                                                        unsigned int size2)->void {
        Numerical::Double oldAbsTolerance = Numerical::AbsoluteTolerance;
        Numerical::Double oldRelTolerance = Numerical::RelativeTolerance;

        Numerical::AbsoluteTolerance = 0.01;
        Numerical::RelativeTolerance = 0.04;

        unsigned int i;
        std::vector<unsigned int> indices1(size1);
        std::vector<unsigned int> indices2(size2);
        std::vector<Numerical::Double> data1(size1);
        std::vector<Numerical::Double> data2(size2);
        for (i = 0; i < size1; i++) {
            indices1[i] = array1[i].first;
            data1[i] = array1[i].second;
            indices2[i] = array2[i].first;
            data2[i] = array2[i].second;
        }

        SparseVector v1 = SparseVector::createVectorFromSparseArray(data1.data(), indices1.data(), size1, size1);
        SparseVector v2 = SparseVector::createVectorFromSparseArray(data2.data(), indices2.data(), size2, size2);
        SparseVector v0 = v1;

        SparseVector::setAddMode(Numerical::ADD_FAST);

        v1.addVector(lambda, v2);

        unsigned int nonZeros = 0;
        for (i = 0; i < size1; i++) {
            Numerical::Double result = v0[i] + lambda * v2[i];
            if (result != 0.0) {
                nonZeros++;
            }

            TEST_ASSERT_EQUALITY(result, v1[i]);
        }

        TEST_ASSERT_GREATER_OR_EQUAL( v1.m_capacity, nonZeros );
        if (v1.m_capacity == 0) {
            TEST_ASSERT_EQUALITY(v1.m_data, nullptr);
            TEST_ASSERT_EQUALITY(v1.m_indices, nullptr);
        } else {
            TEST_ASSERT_INEQUALITY(v1.m_data, nullptr);
            TEST_ASSERT_INEQUALITY(v1.m_indices, nullptr);
        }
        TEST_ASSERT_EQUALITY(v1.m_length, size1);
        TEST_ASSERT_EQUALITY(v1.m_nonZeros, nonZeros);

        CHECK_CLEAN_AUXILIARY_VECTORS;
        CHECK_DEFAULT_DOT_PRODUCT_FUNCTION_POINTERS;
        CHECK_FAST_ADD_FUNCTION_POINTERS;

        v1 = v0;

        SparseVector::setAddMode(Numerical::ADD_ABS);

        v1.addVector(lambda, v2);

        nonZeros = 0;
        for (i = 0; i < size1; i++) {
            Numerical::Double result = Numerical::stableAddAbs(v0[i], lambda * v2[i]);
            if (result != 0.0) {
                nonZeros++;
            }

            TEST_ASSERT_EQUALITY(result, v1[i]);
            //  std::cout << v1[i] << std::endl;
        }
        //std::cin.get();

        TEST_ASSERT_GREATER_OR_EQUAL( v1.m_capacity, nonZeros );
        if (v1.m_capacity == 0) {
            TEST_ASSERT_EQUALITY(v1.m_data, nullptr);
            TEST_ASSERT_EQUALITY(v1.m_indices, nullptr);
        } else {
            TEST_ASSERT_INEQUALITY(v1.m_data, nullptr);
            TEST_ASSERT_INEQUALITY(v1.m_indices, nullptr);
        }
        TEST_ASSERT_EQUALITY(v1.m_length, size1);
        TEST_ASSERT_EQUALITY(v1.m_nonZeros, nonZeros);

        CHECK_CLEAN_AUXILIARY_VECTORS;
        CHECK_DEFAULT_DOT_PRODUCT_FUNCTION_POINTERS;
        CHECK_ABS_ADD_FUNCTION_POINTERS;

        v1 = v0;

        SparseVector::setAddMode(Numerical::ADD_ABS_REL);

        v1.addVector(lambda, v2);

        nonZeros = 0;
        for (i = 0; i < size1; i++) {
            Numerical::Double result = Numerical::stableAdd(v0[i], lambda * v2[i]);
            if (result != 0.0) {
                nonZeros++;
            }

            TEST_ASSERT_EQUALITY(result, v1[i]);
        }

        TEST_ASSERT_GREATER_OR_EQUAL( v1.m_capacity, nonZeros );
        if (v1.m_capacity == 0) {
            TEST_ASSERT_EQUALITY(v1.m_data, nullptr);
            TEST_ASSERT_EQUALITY(v1.m_indices, nullptr);
        } else {
            TEST_ASSERT_INEQUALITY(v1.m_data, nullptr);
            TEST_ASSERT_INEQUALITY(v1.m_indices, nullptr);
        }
        TEST_ASSERT_EQUALITY(v1.m_length, size1);
        TEST_ASSERT_EQUALITY(v1.m_nonZeros, nonZeros);

        CHECK_CLEAN_AUXILIARY_VECTORS;
        CHECK_DEFAULT_DOT_PRODUCT_FUNCTION_POINTERS;
        CHECK_DEFAULT_ADD_FUNCTION_POINTERS;


        Numerical::AbsoluteTolerance = oldAbsTolerance;
        Numerical::RelativeTolerance = oldRelTolerance;

    });

    SparseVector::_globalRelease();
}

void SparseVectorTestSuite::addDenseVector()
{
    SparseVector::_globalRelease();
    SparseVector::_globalInit();

    SparseVector::setAddMode(Numerical::ADD_FAST);

    int lambda;

    for (lambda = -2; lambda <= 2; lambda++) {
        testWithPermutationPair<Numerical::Double, Numerical::Double>({1, 2, 3, 4},
        {5, 6, 7, 8},
                                                                      [lambda, this](const Numerical::Double * array1,
                                                                      unsigned int size1,
                                                                      const Numerical::Double * array2,
                                                                      unsigned int size2)->void {

            SparseVector v1 = SparseVector::createVectorFromDenseArray(array1, size1);
            DenseVector v2 = DenseVector::createVectorFromDenseArray(array2, size2);
            v1.addVector(lambda, v2);

            unsigned int i;
            unsigned int nonZeros = 0;
            for (i = 0; i < size1; i++) {
                Numerical::Double result = array1[i] + lambda * array2[i];
                if (result != 0.0) {
                    nonZeros++;
                }

                TEST_ASSERT_EQUALITY(result, v1[i]);
            }

            TEST_ASSERT_GREATER_OR_EQUAL( v1.m_capacity, nonZeros );
            if (v1.m_capacity == 0) {
                TEST_ASSERT_EQUALITY(v1.m_data, nullptr);
                TEST_ASSERT_EQUALITY(v1.m_indices, nullptr);
            } else {
                TEST_ASSERT_INEQUALITY(v1.m_data, nullptr);
                TEST_ASSERT_INEQUALITY(v1.m_indices, nullptr);
            }
            TEST_ASSERT_EQUALITY(v1.m_length, size1);
            TEST_ASSERT_EQUALITY(v1.m_nonZeros, nonZeros);

            CHECK_CLEAN_AUXILIARY_VECTORS;
            CHECK_DEFAULT_DOT_PRODUCT_FUNCTION_POINTERS;
            CHECK_FAST_ADD_FUNCTION_POINTERS;
        });
    }

    unsigned int pattern1;
    unsigned int pattern2;

    lambda = 1;
    for (pattern1 = 0; pattern1 <= 7; pattern1++) {
        for (pattern2 = 0; pattern2 <= 7; pattern2++) {
            testWithPermutationPair<Numerical::Double, Numerical::Double>({1, 2, 3}, {5, 6, 7},
                                                                          [pattern1, pattern2, lambda, this]
                                                                          (const Numerical::Double * array1,
                                                                          unsigned int size1,
                                                                          const Numerical::Double * array2,
                                                                          unsigned int size2)->void {
                unsigned int i;
                std::vector<unsigned int> indices1(size1);
                std::vector<Numerical::Double> data1(size1);
                std::vector<std::pair<unsigned int, Numerical::Double>> pairs1(size1);
                for (i = 0; i < size1; i++) {
                    indices1[i] = i;
                    data1[i] = array1[i];
                    pairs1[i].first = i;
                    pairs1[i].second = array1[i];
                }

                do {
                    for (i = 0; i < size1; i++) {
                        indices1[i] = pairs1[i].first;
                        data1[i] = pairs1[i].second;
                    }

                    SparseVector v1 = SparseVector::createVectorFromSparseArray(data1.data(), indices1.data(), size1, size1);
                    DenseVector v2 = DenseVector::createVectorFromDenseArray(array2, size2);

                    //std::cout << std::endl;
                    for (i = 0; i < size1; i++) {
                        if ((pattern1 & (1 << i)) == 0) {
                           // std::cout << "v1.set(" << i << ", 0); (" << v1.m_nonZeros << ") ";
                            v1.set(i, 0);
                        }
                        if ((pattern2 & (1 << i)) == 0) {
                            v2.set(i, 0);
                        }
                    }

                    v1.addVector(lambda, v2);

                    unsigned int nonZeros = 0;
                    for (i = 0; i < size1; i++) {
                        Numerical::Double a = ((pattern1 & (1 << i)) != 0) * array1[i];
                        Numerical::Double b = array2[i] * ((pattern2 & (1 << i)) != 0);
                        Numerical::Double result = a + lambda * b;
                        if (result != 0.0) {
                            nonZeros++;
                        }

                        TEST_ASSERT_EQUALITY(result, v1[i]);
                    }

                    TEST_ASSERT_GREATER_OR_EQUAL( v1.m_capacity, nonZeros );
                    if (v1.m_capacity == 0) {
                        TEST_ASSERT_EQUALITY(v1.m_data, nullptr);
                        TEST_ASSERT_EQUALITY(v1.m_indices, nullptr);
                    } else {
                        TEST_ASSERT_INEQUALITY(v1.m_data, nullptr);
                        TEST_ASSERT_INEQUALITY(v1.m_indices, nullptr);
                    }
                    TEST_ASSERT_EQUALITY(v1.m_length, size1);
                    TEST_ASSERT_EQUALITY(v1.m_nonZeros, nonZeros);

                    CHECK_CLEAN_AUXILIARY_VECTORS;
                    CHECK_DEFAULT_DOT_PRODUCT_FUNCTION_POINTERS;
                    CHECK_FAST_ADD_FUNCTION_POINTERS;
                } while (std::next_permutation(pairs1.begin(), pairs1.end()));
            });
        }
    }

    testWithPermutationPair<std::pair<unsigned int, Numerical::Double>,
            std::pair<unsigned int, Numerical::Double>>({{0, 1},
                                                         {1, 0.001},
                                                         {2, 10000},
                                                         {3, 4}},
    {{0, -1.1},
     {1, -0.0011},
     {2, -10000.1},
     {3, 4}},
                                                        [lambda, this](const std::pair<unsigned int, Numerical::Double> * array1,
                                                        unsigned int size1,
                                                        const std::pair<unsigned int, Numerical::Double> * array2,
                                                        unsigned int size2)->void {
        Numerical::Double oldAbsTolerance = Numerical::AbsoluteTolerance;
        Numerical::Double oldRelTolerance = Numerical::RelativeTolerance;

        Numerical::AbsoluteTolerance = 0.01;
        Numerical::RelativeTolerance = 0.04;

        unsigned int i;
        std::vector<unsigned int> indices1(size1);
        std::vector<unsigned int> indices2(size2);
        std::vector<Numerical::Double> data1(size1);
        std::vector<Numerical::Double> data2(size2);
        for (i = 0; i < size1; i++) {
            indices1[i] = array1[i].first;
            data1[i] = array1[i].second;
            indices2[i] = array2[i].first;
            data2[i] = array2[i].second;
        }

        SparseVector v1 = SparseVector::createVectorFromSparseArray(data1.data(), indices1.data(), size1, size1);
        DenseVector v2 = DenseVector::createVectorFromDenseArray(data2.data(), size2);
        SparseVector v0 = v1;

        SparseVector::setAddMode(Numerical::ADD_FAST);

        v1.addVector(lambda, v2);

        unsigned int nonZeros = 0;
        for (i = 0; i < size1; i++) {
            Numerical::Double result = v0[i] + lambda * v2[i];
            if (result != 0.0) {
                nonZeros++;
            }

            TEST_ASSERT_EQUALITY(result, v1[i]);
        }

        TEST_ASSERT_GREATER_OR_EQUAL( v1.m_capacity, nonZeros );
        if (v1.m_capacity == 0) {
            TEST_ASSERT_EQUALITY(v1.m_data, nullptr);
            TEST_ASSERT_EQUALITY(v1.m_indices, nullptr);
        } else {
            TEST_ASSERT_INEQUALITY(v1.m_data, nullptr);
            TEST_ASSERT_INEQUALITY(v1.m_indices, nullptr);
        }
        TEST_ASSERT_EQUALITY(v1.m_length, size1);
        TEST_ASSERT_EQUALITY(v1.m_nonZeros, nonZeros);

        CHECK_CLEAN_AUXILIARY_VECTORS;
        CHECK_DEFAULT_DOT_PRODUCT_FUNCTION_POINTERS;
        CHECK_FAST_ADD_FUNCTION_POINTERS;

        v1 = v0;

        SparseVector::setAddMode(Numerical::ADD_ABS);

        v1.addVector(lambda, v2);

        nonZeros = 0;
        for (i = 0; i < size1; i++) {
            Numerical::Double result = Numerical::stableAddAbs(v0[i], lambda * v2[i]);
            if (result != 0.0) {
                nonZeros++;
            }

            TEST_ASSERT_EQUALITY(result, v1[i]);
            //  std::cout << v1[i] << std::endl;
        }
        //std::cin.get();

        TEST_ASSERT_GREATER_OR_EQUAL( v1.m_capacity, nonZeros );
        if (v1.m_capacity == 0) {
            TEST_ASSERT_EQUALITY(v1.m_data, nullptr);
            TEST_ASSERT_EQUALITY(v1.m_indices, nullptr);
        } else {
            TEST_ASSERT_INEQUALITY(v1.m_data, nullptr);
            TEST_ASSERT_INEQUALITY(v1.m_indices, nullptr);
        }
        TEST_ASSERT_EQUALITY(v1.m_length, size1);
        TEST_ASSERT_EQUALITY(v1.m_nonZeros, nonZeros);

        CHECK_CLEAN_AUXILIARY_VECTORS;
        CHECK_DEFAULT_DOT_PRODUCT_FUNCTION_POINTERS;
        CHECK_ABS_ADD_FUNCTION_POINTERS;

        v1 = v0;

        SparseVector::setAddMode(Numerical::ADD_ABS_REL);

        v1.addVector(lambda, v2);

        nonZeros = 0;
        for (i = 0; i < size1; i++) {
            Numerical::Double result = Numerical::stableAdd(v0[i], lambda * v2[i]);
            if (result != 0.0) {
                nonZeros++;
            }

            TEST_ASSERT_EQUALITY(result, v1[i]);
            if (result != v1[i]) {
                std::cout << "i = " << i << std::endl;
                std::cout << v0[i] << " + " << lambda * v2[i] << std::endl;
                std::cout << v0[0] << " " << v0[1] << " " << v0[2] << " " << v0[3] << " + ";
                std::cout << v2[0] << " " << v2[1] << " " << v2[2] << " " << v2[3] << " = ";
                std::cout << v1[0] << " " << v1[1] << " " << v1[2] << " " << v1[3] << std::endl;
                std::cin.get();
            }
        }

        TEST_ASSERT_GREATER_OR_EQUAL( v1.m_capacity, nonZeros );
        if (v1.m_capacity == 0) {
            TEST_ASSERT_EQUALITY(v1.m_data, nullptr);
            TEST_ASSERT_EQUALITY(v1.m_indices, nullptr);
        } else {
            TEST_ASSERT_INEQUALITY(v1.m_data, nullptr);
            TEST_ASSERT_INEQUALITY(v1.m_indices, nullptr);
        }
        TEST_ASSERT_EQUALITY(v1.m_length, size1);
        TEST_ASSERT_EQUALITY(v1.m_nonZeros, nonZeros);

        CHECK_CLEAN_AUXILIARY_VECTORS;
        CHECK_DEFAULT_DOT_PRODUCT_FUNCTION_POINTERS;
        CHECK_DEFAULT_ADD_FUNCTION_POINTERS;


        Numerical::AbsoluteTolerance = oldAbsTolerance;
        Numerical::RelativeTolerance = oldRelTolerance;

    });

    SparseVector::_globalRelease();
}

void SparseVectorTestSuite::addIndexedDenseVector()
{
    SparseVector::_globalRelease();
    SparseVector::_globalInit();

    SparseVector::setAddMode(Numerical::ADD_FAST);

    int lambda;

    for (lambda = -2; lambda <= 2; lambda++) {
        testWithPermutationPair<Numerical::Double, Numerical::Double>({1, 2, 3, 4},
        {5, 6, 7, 8},
                                                                      [lambda, this](const Numerical::Double * array1,
                                                                      unsigned int size1,
                                                                      const Numerical::Double * array2,
                                                                      unsigned int size2)->void {

            SparseVector v1 = SparseVector::createVectorFromDenseArray(array1, size1);
            IndexedDenseVector v2 = IndexedDenseVector::createVectorFromDenseArray(array2, size2);

            SparseVector v0 = v1;

          //  DEBUG_MODE = true;

            v1.addVector(lambda, v2);

            unsigned int i;
            unsigned int nonZeros = 0;
            for (i = 0; i < size1; i++) {
                Numerical::Double result = array1[i] + lambda * array2[i];
                if (result != 0.0) {
                    nonZeros++;
                }

                TEST_ASSERT_EQUALITY(result, v1[i]);
                if (result != v1[i]) {
                    std::cout << v0[0] << " " << v0[1] << " " << v0[2] << " " << v0[3] << " + ";
                    std::cout << lambda*v2[0] << " " << lambda*v2[1] << " " << lambda*v2[2] << " " << lambda*v2[3] << " = ";
                    std::cout << v1[0] << " " << v1[1] << " " << v1[2] << " " << v1[3] << std::endl;
                    std::cin.get();
                }
            }

            TEST_ASSERT_GREATER_OR_EQUAL( v1.m_capacity, nonZeros );
            if (v1.m_capacity == 0) {
                TEST_ASSERT_EQUALITY(v1.m_data, nullptr);
                TEST_ASSERT_EQUALITY(v1.m_indices, nullptr);
            } else {
                TEST_ASSERT_INEQUALITY(v1.m_data, nullptr);
                TEST_ASSERT_INEQUALITY(v1.m_indices, nullptr);
            }
            TEST_ASSERT_EQUALITY(v1.m_length, size1);
            TEST_ASSERT_EQUALITY(v1.m_nonZeros, nonZeros);

            CHECK_CLEAN_AUXILIARY_VECTORS;
            CHECK_DEFAULT_DOT_PRODUCT_FUNCTION_POINTERS;
            CHECK_FAST_ADD_FUNCTION_POINTERS;
        });
    }

    unsigned int pattern1;
    unsigned int pattern2;

    lambda = 1;
    for (pattern1 = 0; pattern1 <= 7; pattern1++) {
        for (pattern2 = 0; pattern2 <= 7; pattern2++) {
            testWithPermutationPair<Numerical::Double, Numerical::Double>({1, 2, 3}, {4, 5, 6},
                                                                          [pattern1, pattern2, lambda, this]
                                                                          (const Numerical::Double * array1,
                                                                          unsigned int size1,
                                                                          const Numerical::Double * array2,
                                                                          unsigned int size2)->void {
                unsigned int indices1[] = {0, 1, 2};
                Numerical::Double data1[] = {array1[0], array1[1], array1[2]};

                std::pair<unsigned int, Numerical::Double> pairs1[] =
                {{0, array1[0]}, {1, array1[1]}, {2, array1[2]}};

                do {
                    indices1[0] = pairs1[0].first; indices1[1] = pairs1[1].first;
                    indices1[2] = pairs1[2].first;
                    data1[0] = pairs1[0].second; data1[1] = pairs1[1].second;
                    data1[2] = pairs1[2].second;

                    unsigned int indices2[] = {0, 1, 2};
                    Numerical::Double data2[] = {array2[0], array2[1], array2[2]};
                    std::pair<unsigned int, Numerical::Double> pairs2[] =
                    {{0, array2[0]}, {1, array2[1]}, {2, array2[2]}};


                    do {
                        indices2[0] = pairs2[0].first; indices2[1] = pairs2[1].first;
                        indices2[2] = pairs2[2].first;
                        data2[0] = pairs2[0].second; data2[1] = pairs2[1].second;
                        data2[2] = pairs2[2].second;

                        static unsigned int _counter = 0;
                        _counter++;

                        if (_counter == 1299) {
                            //DEBUG_MODE = true;
                            std::cout << std::endl;
                            std::cout << "ARRAY: " << array2[0] << " " << array2[1] << " " << array2[2] << std::endl;
                            std::cout << "indices: " << indices2[0] << " " << indices2[1] << " " << indices2[2] << std::endl;
                            std::cout << "data: " << data2[0] << " " << data2[1] << " " << data2[2] << std::endl;
                        }

                        SparseVector v1 = SparseVector::createVectorFromSparseArray(data1, indices1, size1, size1);
                        IndexedDenseVector v2 = IndexedDenseVector::createVectorFromSparseArray(data2, indices2, size2, size2);

                        unsigned int i;
                        for (i = 0; i < size1; i++) {
                            if ((pattern1 & (1 << i)) == 0) {
                                v1.set(i, 0);
                            }
                            if ((pattern2 & (1 << i)) == 0) {
                                v2.set(i, 0);
                            }
                        }

                        //std::cout << "counter = " << _counter << std::endl;

                        SparseVector v0 = v1;

                        v1.addVector(lambda, v2);

                        unsigned int nonZeros = 0;
                        for (i = 0; i < size1; i++) {
                            Numerical::Double result = ((pattern1 & (1 << i)) != 0) * array1[i]+
                                    lambda * array2[i] * ((pattern2 & (1 << i)) != 0);
                            if (result != 0.0) {
                                nonZeros++;
                            }

                            TEST_ASSERT_EQUALITY(result, v1[i]);
                            if (result != v1[i]) {
                                std::cout << "i = " << i << std::endl;
                                std::cout << v0[0] << " " << v0[1] << " " << v0[2] << " + ";
                                std::cout << v2[0] << " " << v2[1] << " " << v2[2] << " = ";
                                std::cout << v1[0] << " " << v1[1] << " " << v1[2] << std::endl;

                                std::cin.get();
                            }
                        }

                        TEST_ASSERT_GREATER_OR_EQUAL( v1.m_capacity, nonZeros );
                        if (v1.m_capacity == 0) {
                            TEST_ASSERT_EQUALITY(v1.m_data, nullptr);
                            TEST_ASSERT_EQUALITY(v1.m_indices, nullptr);
                        } else {
                            TEST_ASSERT_INEQUALITY(v1.m_data, nullptr);
                            TEST_ASSERT_INEQUALITY(v1.m_indices, nullptr);
                        }
                        TEST_ASSERT_EQUALITY(v1.m_length, size1);
                        TEST_ASSERT_EQUALITY(v1.m_nonZeros, nonZeros);

                        CHECK_CLEAN_AUXILIARY_VECTORS;
                        CHECK_DEFAULT_DOT_PRODUCT_FUNCTION_POINTERS;
                        CHECK_FAST_ADD_FUNCTION_POINTERS;
                    } while(std::next_permutation(pairs2, pairs2 + 3));
                } while (std::next_permutation(pairs1, pairs1 + 3));
            });
        }
    }

    testWithPermutationPair<std::pair<unsigned int, Numerical::Double>,
            std::pair<unsigned int, Numerical::Double>>({{0, 1},
                                                         {1, 0.001},
                                                         {2, 10000},
                                                         {3, 4}},
    {{0, -1.1},
     {1, -0.0011},
     {2, -10000.1},
     {3, 4}},
                                                        [lambda, this](const std::pair<unsigned int, Numerical::Double> * array1,
                                                        unsigned int size1,
                                                        const std::pair<unsigned int, Numerical::Double> * array2,
                                                        unsigned int size2)->void {
        Numerical::Double oldAbsTolerance = Numerical::AbsoluteTolerance;
        Numerical::Double oldRelTolerance = Numerical::RelativeTolerance;

        Numerical::AbsoluteTolerance = 0.01;
        Numerical::RelativeTolerance = 0.04;

        unsigned int i;
        std::vector<unsigned int> indices1(size1);
        std::vector<unsigned int> indices2(size2);
        std::vector<Numerical::Double> data1(size1);
        std::vector<Numerical::Double> data2(size2);
        for (i = 0; i < size1; i++) {
            indices1[i] = array1[i].first;
            data1[i] = array1[i].second;
            indices2[i] = array2[i].first;
            data2[i] = array2[i].second;
        }

        SparseVector v1 = SparseVector::createVectorFromSparseArray(data1.data(), indices1.data(), size1, size1);
        IndexedDenseVector v2 = IndexedDenseVector::createVectorFromSparseArray(data2.data(), indices2.data(), size2, size2);
        SparseVector v0 = v1;

        SparseVector::setAddMode(Numerical::ADD_FAST);

        v1.addVector(lambda, v2);

        unsigned int nonZeros = 0;
        for (i = 0; i < size1; i++) {
            Numerical::Double result = v0[i] + lambda * v2[i];
            if (result != 0.0) {
                nonZeros++;
            }

            TEST_ASSERT_EQUALITY(result, v1[i]);
        }

        TEST_ASSERT_GREATER_OR_EQUAL( v1.m_capacity, nonZeros );
        if (v1.m_capacity == 0) {
            TEST_ASSERT_EQUALITY(v1.m_data, nullptr);
            TEST_ASSERT_EQUALITY(v1.m_indices, nullptr);
        } else {
            TEST_ASSERT_INEQUALITY(v1.m_data, nullptr);
            TEST_ASSERT_INEQUALITY(v1.m_indices, nullptr);
        }
        TEST_ASSERT_EQUALITY(v1.m_length, size1);
        TEST_ASSERT_EQUALITY(v1.m_nonZeros, nonZeros);

        CHECK_CLEAN_AUXILIARY_VECTORS;
        CHECK_DEFAULT_DOT_PRODUCT_FUNCTION_POINTERS;
        CHECK_FAST_ADD_FUNCTION_POINTERS;

        v1 = v0;

        SparseVector::setAddMode(Numerical::ADD_ABS);

        v1.addVector(lambda, v2);

        nonZeros = 0;
        for (i = 0; i < size1; i++) {
            Numerical::Double result = Numerical::stableAddAbs(v0[i], lambda * v2[i]);
            if (result != 0.0) {
                nonZeros++;
            }

            TEST_ASSERT_EQUALITY(result, v1[i]);
            //  std::cout << v1[i] << std::endl;
        }
        //std::cin.get();

        TEST_ASSERT_GREATER_OR_EQUAL( v1.m_capacity, nonZeros );
        if (v1.m_capacity == 0) {
            TEST_ASSERT_EQUALITY(v1.m_data, nullptr);
            TEST_ASSERT_EQUALITY(v1.m_indices, nullptr);
        } else {
            TEST_ASSERT_INEQUALITY(v1.m_data, nullptr);
            TEST_ASSERT_INEQUALITY(v1.m_indices, nullptr);
        }
        TEST_ASSERT_EQUALITY(v1.m_length, size1);
        TEST_ASSERT_EQUALITY(v1.m_nonZeros, nonZeros);

        CHECK_CLEAN_AUXILIARY_VECTORS;
        CHECK_DEFAULT_DOT_PRODUCT_FUNCTION_POINTERS;
        CHECK_ABS_ADD_FUNCTION_POINTERS;

        v1 = v0;

        SparseVector::setAddMode(Numerical::ADD_ABS_REL);

        v1.addVector(lambda, v2);

        nonZeros = 0;
        for (i = 0; i < size1; i++) {
            Numerical::Double result = Numerical::stableAdd(v0[i], lambda * v2[i]);
            if (result != 0.0) {
                nonZeros++;
            }

            TEST_ASSERT_EQUALITY(result, v1[i]);
        }

        TEST_ASSERT_GREATER_OR_EQUAL( v1.m_capacity, nonZeros );
        if (v1.m_capacity == 0) {
            TEST_ASSERT_EQUALITY(v1.m_data, nullptr);
            TEST_ASSERT_EQUALITY(v1.m_indices, nullptr);
        } else {
            TEST_ASSERT_INEQUALITY(v1.m_data, nullptr);
            TEST_ASSERT_INEQUALITY(v1.m_indices, nullptr);
        }
        TEST_ASSERT_EQUALITY(v1.m_length, size1);
        TEST_ASSERT_EQUALITY(v1.m_nonZeros, nonZeros);

        CHECK_CLEAN_AUXILIARY_VECTORS;
        CHECK_DEFAULT_DOT_PRODUCT_FUNCTION_POINTERS;
        CHECK_DEFAULT_ADD_FUNCTION_POINTERS;


        Numerical::AbsoluteTolerance = oldAbsTolerance;
        Numerical::RelativeTolerance = oldRelTolerance;

    });

    SparseVector::_globalRelease();
}

void SparseVectorTestSuite::scale()
{
    SparseVector::_globalRelease();
    SparseVector::_globalInit();

    SparseVector::_globalRelease();
}

void SparseVectorTestSuite::sortIndices()
{
    SparseVector::_globalRelease();
    SparseVector::_globalInit();

    SparseVector::_globalRelease();
}

void SparseVectorTestSuite::clear()
{
    SparseVector::_globalRelease();
    SparseVector::_globalInit();

    SparseVector v1;
    v1.clear();
    TEST_ASSERT_EQUALITY(v1.m_capacity, 0);
    TEST_ASSERT_EQUALITY(v1.m_data, nullptr);
    TEST_ASSERT_EQUALITY(v1.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v1.m_length, 0);
    TEST_ASSERT_EQUALITY(v1.m_nonZeros, 0);

    CHECK_CLEAN_AUXILIARY_VECTORS;
    CHECK_DEFAULT_FUNCTION_POINTERS;

    SparseVector v2(5);
    v2.clear();
    TEST_ASSERT_EQUALITY(v2.m_capacity, 0);
    TEST_ASSERT_EQUALITY(v2.m_data, nullptr);
    TEST_ASSERT_EQUALITY(v2.m_indices, nullptr);
    TEST_ASSERT_EQUALITY(v2.m_length, 5);
    TEST_ASSERT_EQUALITY(v2.m_nonZeros, 0);

    CHECK_CLEAN_AUXILIARY_VECTORS;
    CHECK_DEFAULT_FUNCTION_POINTERS;

    v2.set(4, 3);
    v2.set(0, 3.4);

    v2.clear();
    TEST_ASSERT_GREATER_OR_EQUAL(v2.m_capacity, 0);
    if (v2.m_capacity > 0) {
        TEST_ASSERT_INEQUALITY(v2.m_data, nullptr);
        TEST_ASSERT_INEQUALITY(v2.m_indices, nullptr);
    } else {
        TEST_ASSERT_EQUALITY(v2.m_data, nullptr);
        TEST_ASSERT_EQUALITY(v2.m_indices, nullptr);
    }
    TEST_ASSERT_EQUALITY(v2.m_length, 5);
    TEST_ASSERT_EQUALITY(v2.m_nonZeros, 0);

    CHECK_CLEAN_AUXILIARY_VECTORS;
    CHECK_DEFAULT_FUNCTION_POINTERS;

    SparseVector::_globalRelease();
}

void SparseVectorTestSuite::setAddMode()
{
    SparseVector::_globalRelease();
    SparseVector::_globalInit();

    SparseVector::_globalRelease();
}

void SparseVectorTestSuite::setDotProductMode()
{
    SparseVector::_globalRelease();
    SparseVector::_globalInit();

    SparseVector::_globalRelease();
}

void SparseVectorTestSuite::beginNonzero()
{
    SparseVector::_globalRelease();
    SparseVector::_globalInit();

    SparseVector::_globalRelease();
}

void SparseVectorTestSuite::endNonzero()
{
    SparseVector::_globalRelease();
    SparseVector::_globalInit();

    SparseVector::_globalRelease();
}

void SparseVectorTestSuite::iterator()
{
    SparseVector::_globalRelease();
    SparseVector::_globalInit();

    SparseVector::_globalRelease();
}

