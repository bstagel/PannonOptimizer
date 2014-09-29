#include <sparsevectortest.h>
#include <linalg/sparsevector.h>

SparseVectorTestSuite::SparseVectorTestSuite(const char *name):
    UnitTest(name)
{
    ADD_TEST(SparseVectorTestSuite::init);
    ADD_TEST(SparseVectorTestSuite::copy);
    ADD_TEST(SparseVectorTestSuite::move);
    ADD_TEST(SparseVectorTestSuite::assign);
    ADD_TEST(SparseVectorTestSuite::moveAssign);
    ADD_TEST(SparseVectorTestSuite::createUnitVector);
    ADD_TEST(SparseVectorTestSuite::createVectorFromDenseArray);
    ADD_TEST(SparseVectorTestSuite::createVectorFromSparseArray);
    ADD_TEST(SparseVectorTestSuite::createEtaByPivotValue);
    ADD_TEST(SparseVectorTestSuite::createEtaByPivotIndex);
    ADD_TEST(SparseVectorTestSuite::at);
    ADD_TEST(SparseVectorTestSuite::arrayOperator);
    ADD_TEST(SparseVectorTestSuite::set);
    ADD_TEST(SparseVectorTestSuite::length);
    ADD_TEST(SparseVectorTestSuite::euclidNorm);
    ADD_TEST(SparseVectorTestSuite::l1Norm);
    ADD_TEST(SparseVectorTestSuite::addSparseVector);
    ADD_TEST(SparseVectorTestSuite::addDenseVector);
    ADD_TEST(SparseVectorTestSuite::addIndexedDenseVector);
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

    TEST_ASSERT(SparseVector::sm_sparseToDenseDotProduct == &SparseVector::dotProductSparseToDenseUnstable );
    TEST_ASSERT(SparseVector::sm_sparseToIndexedDenseDotProduct == &SparseVector::dotProductSparseToIndexedDenseUnstable );
    TEST_ASSERT(SparseVector::sm_sparseToSparseDotProduct == &SparseVector::dotProductSparseToSparseUnstable);
    TEST_ASSERT(SparseVector::sm_addDenseToSparse == &SparseVector::addDenseToSparseFast);
    TEST_ASSERT(SparseVector::sm_addSparseToSparse == &SparseVector::addSparseToSparseFast);
    TEST_ASSERT(SparseVector::sm_addIndexedDenseToSparse == &SparseVector::addIndexedDenseToSparseFast);

    SparseVector v1(0);
    TEST_ASSERT(v1.m_length == 0);
    TEST_ASSERT(v1.m_nonZeros == 0);
    TEST_ASSERT(v1.m_data == nullptr);
    TEST_ASSERT(v1.m_indices == nullptr);
    TEST_ASSERT(v1.m_capacity == 0);
    TEST_ASSERT(SparseVector::sm_fullLengthVectorLength == 0);
    TEST_ASSERT(SparseVector::sm_fullLengthVector == nullptr);
    TEST_ASSERT(SparseVector::sm_indexPointerVector == nullptr);
    TEST_ASSERT(SparseVector::sm_indexVector == nullptr);

    TEST_ASSERT(SparseVector::sm_sparseToDenseDotProduct == &SparseVector::dotProductSparseToDenseUnstable );
    TEST_ASSERT(SparseVector::sm_sparseToIndexedDenseDotProduct == &SparseVector::dotProductSparseToIndexedDenseUnstable );
    TEST_ASSERT(SparseVector::sm_sparseToSparseDotProduct == &SparseVector::dotProductSparseToSparseUnstable);
    TEST_ASSERT(SparseVector::sm_addDenseToSparse == &SparseVector::addDenseToSparseFast);
    TEST_ASSERT(SparseVector::sm_addSparseToSparse == &SparseVector::addSparseToSparseFast);
    TEST_ASSERT(SparseVector::sm_addIndexedDenseToSparse == &SparseVector::addIndexedDenseToSparseFast);

    SparseVector v2(1);
    TEST_ASSERT(v2.m_length == 1);
    TEST_ASSERT(v2.m_nonZeros == 0);
    TEST_ASSERT(v2.m_data == nullptr);
    TEST_ASSERT(v2.m_indices == nullptr);
    TEST_ASSERT(v2.m_capacity == 0);
    TEST_ASSERT(SparseVector::sm_fullLengthVectorLength == 1);
    TEST_ASSERT(SparseVector::sm_fullLengthVector != nullptr);
    TEST_ASSERT(SparseVector::sm_indexPointerVector != nullptr);
    TEST_ASSERT(SparseVector::sm_indexVector != nullptr);

    TEST_ASSERT(SparseVector::sm_sparseToDenseDotProduct == &SparseVector::dotProductSparseToDenseUnstable );
    TEST_ASSERT(SparseVector::sm_sparseToIndexedDenseDotProduct == &SparseVector::dotProductSparseToIndexedDenseUnstable );
    TEST_ASSERT(SparseVector::sm_sparseToSparseDotProduct == &SparseVector::dotProductSparseToSparseUnstable);
    TEST_ASSERT(SparseVector::sm_addDenseToSparse == &SparseVector::addDenseToSparseFast);
    TEST_ASSERT(SparseVector::sm_addSparseToSparse == &SparseVector::addSparseToSparseFast);
    TEST_ASSERT(SparseVector::sm_addIndexedDenseToSparse == &SparseVector::addIndexedDenseToSparseFast);

    SparseVector v3(2);
    TEST_ASSERT(v3.m_length == 2);
    TEST_ASSERT(v3.m_nonZeros == 0);
    TEST_ASSERT(v3.m_data == nullptr);
    TEST_ASSERT(v3.m_indices == nullptr);
    TEST_ASSERT(v3.m_capacity == 0);
    TEST_ASSERT(SparseVector::sm_fullLengthVectorLength == 2);
    TEST_ASSERT(SparseVector::sm_fullLengthVector != nullptr);
    TEST_ASSERT(SparseVector::sm_indexPointerVector != nullptr);
    TEST_ASSERT(SparseVector::sm_indexVector != nullptr);

    TEST_ASSERT(SparseVector::sm_sparseToDenseDotProduct == &SparseVector::dotProductSparseToDenseUnstable );
    TEST_ASSERT(SparseVector::sm_sparseToIndexedDenseDotProduct == &SparseVector::dotProductSparseToIndexedDenseUnstable );
    TEST_ASSERT(SparseVector::sm_sparseToSparseDotProduct == &SparseVector::dotProductSparseToSparseUnstable);
    TEST_ASSERT(SparseVector::sm_addDenseToSparse == &SparseVector::addDenseToSparseFast);
    TEST_ASSERT(SparseVector::sm_addSparseToSparse == &SparseVector::addSparseToSparseFast);
    TEST_ASSERT(SparseVector::sm_addIndexedDenseToSparse == &SparseVector::addIndexedDenseToSparseFast);

    SparseVector v4(10);
    TEST_ASSERT(v4.m_length == 10);
    TEST_ASSERT(v4.m_nonZeros == 0);
    TEST_ASSERT(v4.m_data == nullptr);
    TEST_ASSERT(v4.m_indices == nullptr);
    TEST_ASSERT(v4.m_capacity == 0);
    TEST_ASSERT(SparseVector::sm_fullLengthVectorLength == 10);
    TEST_ASSERT(SparseVector::sm_fullLengthVector != nullptr);
    TEST_ASSERT(SparseVector::sm_indexPointerVector != nullptr);
    TEST_ASSERT(SparseVector::sm_indexVector != nullptr);

    TEST_ASSERT(SparseVector::sm_sparseToDenseDotProduct == &SparseVector::dotProductSparseToDenseUnstable );
    TEST_ASSERT(SparseVector::sm_sparseToIndexedDenseDotProduct == &SparseVector::dotProductSparseToIndexedDenseUnstable );
    TEST_ASSERT(SparseVector::sm_sparseToSparseDotProduct == &SparseVector::dotProductSparseToSparseUnstable);
    TEST_ASSERT(SparseVector::sm_addDenseToSparse == &SparseVector::addDenseToSparseFast);
    TEST_ASSERT(SparseVector::sm_addSparseToSparse == &SparseVector::addSparseToSparseFast);
    TEST_ASSERT(SparseVector::sm_addIndexedDenseToSparse == &SparseVector::addIndexedDenseToSparseFast);

    SparseVector v5(5);
    TEST_ASSERT(v5.m_length == 5);
    TEST_ASSERT(v5.m_nonZeros == 0);
    TEST_ASSERT(v5.m_data == nullptr);
    TEST_ASSERT(v5.m_indices == nullptr);
    TEST_ASSERT(v5.m_capacity == 0);
    TEST_ASSERT(SparseVector::sm_fullLengthVectorLength == 10);
    TEST_ASSERT(SparseVector::sm_fullLengthVector != nullptr);
    TEST_ASSERT(SparseVector::sm_indexPointerVector != nullptr);
    TEST_ASSERT(SparseVector::sm_indexVector != nullptr);

    TEST_ASSERT(SparseVector::sm_sparseToDenseDotProduct == &SparseVector::dotProductSparseToDenseUnstable );
    TEST_ASSERT(SparseVector::sm_sparseToIndexedDenseDotProduct == &SparseVector::dotProductSparseToIndexedDenseUnstable );
    TEST_ASSERT(SparseVector::sm_sparseToSparseDotProduct == &SparseVector::dotProductSparseToSparseUnstable);
    TEST_ASSERT(SparseVector::sm_addDenseToSparse == &SparseVector::addDenseToSparseFast);
    TEST_ASSERT(SparseVector::sm_addSparseToSparse == &SparseVector::addSparseToSparseFast);
    TEST_ASSERT(SparseVector::sm_addIndexedDenseToSparse == &SparseVector::addIndexedDenseToSparseFast);

    SparseVector::setAddMode(Numerical::ADD_ABS);
    SparseVector::setDotProductMode(Numerical::DOT_ABS);

    SparseVector v6(20);
    TEST_ASSERT(v6.m_length == 20);
    TEST_ASSERT(v6.m_nonZeros == 0);
    TEST_ASSERT(v6.m_data == nullptr);
    TEST_ASSERT(v6.m_indices == nullptr);
    TEST_ASSERT(v6.m_capacity == 0);
    TEST_ASSERT(SparseVector::sm_fullLengthVectorLength == 20);
    TEST_ASSERT(SparseVector::sm_fullLengthVector != nullptr);
    TEST_ASSERT(SparseVector::sm_indexPointerVector != nullptr);
    TEST_ASSERT(SparseVector::sm_indexVector != nullptr);

    TEST_ASSERT(SparseVector::sm_sparseToDenseDotProduct == &SparseVector::dotProductSparseToDenseAbs );
    TEST_ASSERT(SparseVector::sm_sparseToIndexedDenseDotProduct == &SparseVector::dotProductSparseToIndexedDenseAbs );
    TEST_ASSERT(SparseVector::sm_sparseToSparseDotProduct == &SparseVector::dotProductSparseToSparseAbs);
    TEST_ASSERT(SparseVector::sm_addDenseToSparse == &SparseVector::addDenseToSparseAbs);
    TEST_ASSERT(SparseVector::sm_addSparseToSparse == &SparseVector::addSparseToSparseAbs);
    TEST_ASSERT(SparseVector::sm_addIndexedDenseToSparse == &SparseVector::addIndexedDenseToSparseAbs);

    SparseVector::_globalRelease();
}

void SparseVectorTestSuite::copy()
{
    SparseVector::_globalRelease();
    SparseVector::_globalInit();

    SparseVector v1(0);
    SparseVector v1_c = v1;

    TEST_ASSERT(v1.m_length == 0);
    TEST_ASSERT(v1.m_nonZeros == 0);
    TEST_ASSERT(v1.m_data == nullptr);
    TEST_ASSERT(v1.m_indices == nullptr);
    TEST_ASSERT(v1.m_capacity == 0);
    TEST_ASSERT(v1_c.m_length == 0);
    TEST_ASSERT(v1_c.m_nonZeros == 0);
    TEST_ASSERT(v1_c.m_data == nullptr);
    TEST_ASSERT(v1_c.m_indices == nullptr);
    TEST_ASSERT(v1_c.m_capacity == 0);
    TEST_ASSERT(SparseVector::sm_fullLengthVectorLength == 0);
    TEST_ASSERT(SparseVector::sm_fullLengthVector == nullptr);
    TEST_ASSERT(SparseVector::sm_indexPointerVector == nullptr);
    TEST_ASSERT(SparseVector::sm_indexVector == nullptr);

    TEST_ASSERT(SparseVector::sm_sparseToDenseDotProduct == &SparseVector::dotProductSparseToDenseUnstable );
    TEST_ASSERT(SparseVector::sm_sparseToIndexedDenseDotProduct == &SparseVector::dotProductSparseToIndexedDenseUnstable );
    TEST_ASSERT(SparseVector::sm_sparseToSparseDotProduct == &SparseVector::dotProductSparseToSparseUnstable);
    TEST_ASSERT(SparseVector::sm_addDenseToSparse == &SparseVector::addDenseToSparseFast);
    TEST_ASSERT(SparseVector::sm_addSparseToSparse == &SparseVector::addSparseToSparseFast);
    TEST_ASSERT(SparseVector::sm_addIndexedDenseToSparse == &SparseVector::addIndexedDenseToSparseFast);

    SparseVector v2(1);
    SparseVector v2_c = v2;
    TEST_ASSERT(v2.m_length == 1);
    TEST_ASSERT(v2.m_nonZeros == 0);
    TEST_ASSERT(v2.m_data == nullptr);
    TEST_ASSERT(v2.m_indices == nullptr);
    TEST_ASSERT(v2.m_capacity == 0);
    TEST_ASSERT(v2_c.m_length == 1);
    TEST_ASSERT(v2_c.m_nonZeros == 0);
    TEST_ASSERT(v2_c.m_data == nullptr);
    TEST_ASSERT(v2_c.m_indices == nullptr);
    TEST_ASSERT(v2_c.m_capacity == 0);
    TEST_ASSERT(SparseVector::sm_fullLengthVectorLength == 1);
    TEST_ASSERT(SparseVector::sm_fullLengthVector != nullptr);
    TEST_ASSERT(SparseVector::sm_indexPointerVector != nullptr);
    TEST_ASSERT(SparseVector::sm_indexVector != nullptr);

    TEST_ASSERT(SparseVector::sm_sparseToDenseDotProduct == &SparseVector::dotProductSparseToDenseUnstable );
    TEST_ASSERT(SparseVector::sm_sparseToIndexedDenseDotProduct == &SparseVector::dotProductSparseToIndexedDenseUnstable );
    TEST_ASSERT(SparseVector::sm_sparseToSparseDotProduct == &SparseVector::dotProductSparseToSparseUnstable);
    TEST_ASSERT(SparseVector::sm_addDenseToSparse == &SparseVector::addDenseToSparseFast);
    TEST_ASSERT(SparseVector::sm_addSparseToSparse == &SparseVector::addSparseToSparseFast);
    TEST_ASSERT(SparseVector::sm_addIndexedDenseToSparse == &SparseVector::addIndexedDenseToSparseFast);

    SparseVector v3(2);
    SparseVector v3_c = v3;
    TEST_ASSERT(v3.m_length == 2);
    TEST_ASSERT(v3.m_nonZeros == 0);
    TEST_ASSERT(v3.m_data == nullptr);
    TEST_ASSERT(v3.m_indices == nullptr);
    TEST_ASSERT(v3.m_capacity == 0);
    TEST_ASSERT(v3_c.m_length == 2);
    TEST_ASSERT(v3_c.m_nonZeros == 0);
    TEST_ASSERT(v3_c.m_data == nullptr);
    TEST_ASSERT(v3_c.m_indices == nullptr);
    TEST_ASSERT(v3_c.m_capacity == 0);
    TEST_ASSERT(SparseVector::sm_fullLengthVectorLength == 2);
    TEST_ASSERT(SparseVector::sm_fullLengthVector != nullptr);
    TEST_ASSERT(SparseVector::sm_indexPointerVector != nullptr);
    TEST_ASSERT(SparseVector::sm_indexVector != nullptr);

    TEST_ASSERT(SparseVector::sm_sparseToDenseDotProduct == &SparseVector::dotProductSparseToDenseUnstable );
    TEST_ASSERT(SparseVector::sm_sparseToIndexedDenseDotProduct == &SparseVector::dotProductSparseToIndexedDenseUnstable );
    TEST_ASSERT(SparseVector::sm_sparseToSparseDotProduct == &SparseVector::dotProductSparseToSparseUnstable);
    TEST_ASSERT(SparseVector::sm_addDenseToSparse == &SparseVector::addDenseToSparseFast);
    TEST_ASSERT(SparseVector::sm_addSparseToSparse == &SparseVector::addSparseToSparseFast);
    TEST_ASSERT(SparseVector::sm_addIndexedDenseToSparse == &SparseVector::addIndexedDenseToSparseFast);

    SparseVector v4(10);
    SparseVector v4_c = v4;
    TEST_ASSERT(v4.m_length == 10);
    TEST_ASSERT(v4.m_nonZeros == 0);
    TEST_ASSERT(v4.m_data == nullptr);
    TEST_ASSERT(v4.m_indices == nullptr);
    TEST_ASSERT(v4.m_capacity == 0);
    TEST_ASSERT(v4_c.m_length == 10);
    TEST_ASSERT(v4_c.m_nonZeros == 0);
    TEST_ASSERT(v4_c.m_data == nullptr);
    TEST_ASSERT(v4_c.m_indices == nullptr);
    TEST_ASSERT(v4_c.m_capacity == 0);
    TEST_ASSERT(SparseVector::sm_fullLengthVectorLength == 10);
    TEST_ASSERT(SparseVector::sm_fullLengthVector != nullptr);
    TEST_ASSERT(SparseVector::sm_indexPointerVector != nullptr);
    TEST_ASSERT(SparseVector::sm_indexVector != nullptr);

    TEST_ASSERT(SparseVector::sm_sparseToDenseDotProduct == &SparseVector::dotProductSparseToDenseUnstable );
    TEST_ASSERT(SparseVector::sm_sparseToIndexedDenseDotProduct == &SparseVector::dotProductSparseToIndexedDenseUnstable );
    TEST_ASSERT(SparseVector::sm_sparseToSparseDotProduct == &SparseVector::dotProductSparseToSparseUnstable);
    TEST_ASSERT(SparseVector::sm_addDenseToSparse == &SparseVector::addDenseToSparseFast);
    TEST_ASSERT(SparseVector::sm_addSparseToSparse == &SparseVector::addSparseToSparseFast);
    TEST_ASSERT(SparseVector::sm_addIndexedDenseToSparse == &SparseVector::addIndexedDenseToSparseFast);

    SparseVector v5(5);
    SparseVector v5_c = v5;
    TEST_ASSERT(v5.m_length == 5);
    TEST_ASSERT(v5.m_nonZeros == 0);
    TEST_ASSERT(v5.m_data == nullptr);
    TEST_ASSERT(v5.m_indices == nullptr);
    TEST_ASSERT(v5.m_capacity == 0);
    TEST_ASSERT(v5_c.m_length == 5);
    TEST_ASSERT(v5_c.m_nonZeros == 0);
    TEST_ASSERT(v5_c.m_data == nullptr);
    TEST_ASSERT(v5_c.m_indices == nullptr);
    TEST_ASSERT(v5_c.m_capacity == 0);
    TEST_ASSERT(SparseVector::sm_fullLengthVectorLength == 10);
    TEST_ASSERT(SparseVector::sm_fullLengthVector != nullptr);
    TEST_ASSERT(SparseVector::sm_indexPointerVector != nullptr);
    TEST_ASSERT(SparseVector::sm_indexVector != nullptr);

    TEST_ASSERT(SparseVector::sm_sparseToDenseDotProduct == &SparseVector::dotProductSparseToDenseUnstable );
    TEST_ASSERT(SparseVector::sm_sparseToIndexedDenseDotProduct == &SparseVector::dotProductSparseToIndexedDenseUnstable );
    TEST_ASSERT(SparseVector::sm_sparseToSparseDotProduct == &SparseVector::dotProductSparseToSparseUnstable);
    TEST_ASSERT(SparseVector::sm_addDenseToSparse == &SparseVector::addDenseToSparseFast);
    TEST_ASSERT(SparseVector::sm_addSparseToSparse == &SparseVector::addSparseToSparseFast);
    TEST_ASSERT(SparseVector::sm_addIndexedDenseToSparse == &SparseVector::addIndexedDenseToSparseFast);

    SparseVector::setAddMode(Numerical::ADD_ABS);
    SparseVector::setDotProductMode(Numerical::DOT_ABS);

    SparseVector v6(20);
    SparseVector v6_c = v6;
    TEST_ASSERT(v6.m_length == 20);
    TEST_ASSERT(v6.m_nonZeros == 0);
    TEST_ASSERT(v6.m_data == nullptr);
    TEST_ASSERT(v6.m_indices == nullptr);
    TEST_ASSERT(v6.m_capacity == 0);
    TEST_ASSERT(v6_c.m_length == 20);
    TEST_ASSERT(v6_c.m_nonZeros == 0);
    TEST_ASSERT(v6_c.m_data == nullptr);
    TEST_ASSERT(v6_c.m_indices == nullptr);
    TEST_ASSERT(v6_c.m_capacity == 0);
    TEST_ASSERT(SparseVector::sm_fullLengthVectorLength == 20);
    TEST_ASSERT(SparseVector::sm_fullLengthVector != nullptr);
    TEST_ASSERT(SparseVector::sm_indexPointerVector != nullptr);
    TEST_ASSERT(SparseVector::sm_indexVector != nullptr);

    TEST_ASSERT(SparseVector::sm_sparseToDenseDotProduct == &SparseVector::dotProductSparseToDenseAbs );
    TEST_ASSERT(SparseVector::sm_sparseToIndexedDenseDotProduct == &SparseVector::dotProductSparseToIndexedDenseAbs );
    TEST_ASSERT(SparseVector::sm_sparseToSparseDotProduct == &SparseVector::dotProductSparseToSparseAbs);
    TEST_ASSERT(SparseVector::sm_addDenseToSparse == &SparseVector::addDenseToSparseAbs);
    TEST_ASSERT(SparseVector::sm_addSparseToSparse == &SparseVector::addSparseToSparseAbs);
    TEST_ASSERT(SparseVector::sm_addIndexedDenseToSparse == &SparseVector::addIndexedDenseToSparseAbs);


    SparseVector v7(1);
    v7.set(0, 4.2);
    SparseVector v7_c = v7;
    TEST_ASSERT(v7.m_length == 1);
    TEST_ASSERT(v7.m_nonZeros == 1);
    TEST_ASSERT(v7.m_data != nullptr);
    TEST_ASSERT(v7.m_indices != nullptr);
    TEST_ASSERT(v7.m_capacity >= 1);
    TEST_ASSERT(v7_c.m_length == 1);
    TEST_ASSERT(v7_c.m_nonZeros == 1);
    TEST_ASSERT(v7_c.m_data != nullptr);
    TEST_ASSERT(v7_c.m_indices != nullptr);
    TEST_ASSERT(v7_c.m_capacity >= 1);
    TEST_ASSERT(v7.m_data != v7_c.m_data);
    TEST_ASSERT(v7.m_indices != v7_c.m_indices);

    TEST_ASSERT(v7_c.m_data[0] == 4.2);
    TEST_ASSERT(v7_c.m_indices[0] == 0);

    SparseVector v8(10);
    v8.set(4, 1.2);
    v8.set(1, 5.6);
    v8.set(9, 1.4);
    SparseVector v8_c = v8;
    TEST_ASSERT(v8.m_length == 10);
    TEST_ASSERT(v8.m_nonZeros == 3);
    TEST_ASSERT(v8.m_data != nullptr);
    TEST_ASSERT(v8.m_indices != nullptr);
    TEST_ASSERT(v8.m_capacity >= 3);
    TEST_ASSERT(v8_c.m_length == 10);
    TEST_ASSERT(v8_c.m_nonZeros == 3);
    TEST_ASSERT(v8_c.m_data != nullptr);
    TEST_ASSERT(v8_c.m_indices != nullptr);
    TEST_ASSERT(v8_c.m_capacity >= 3);
    TEST_ASSERT(v8.m_data != v8_c.m_data);
    TEST_ASSERT(v8.m_indices != v8_c.m_indices);

    TEST_ASSERT(v8_c.m_data[0] == 1.2);
    TEST_ASSERT(v8_c.m_indices[0] == 4);
    TEST_ASSERT(v8_c.m_data[1] == 5.6);
    TEST_ASSERT(v8_c.m_indices[1] == 1);
    TEST_ASSERT(v8_c.m_data[2] == 1.4);
    TEST_ASSERT(v8_c.m_indices[2] == 9);

    SparseVector v9(3);
    v9.set(1, 1.2);
    v9.set(0, 5.6);
    v9.set(2, 1.4);
    SparseVector v9_c = v9;
    TEST_ASSERT(v9.m_length == 3);
    TEST_ASSERT(v9.m_nonZeros == 3);
    TEST_ASSERT(v9.m_data != nullptr);
    TEST_ASSERT(v9.m_indices != nullptr);
    TEST_ASSERT(v9.m_capacity >= 3);
    TEST_ASSERT(v9_c.m_length == 3);
    TEST_ASSERT(v9_c.m_nonZeros == 3);
    TEST_ASSERT(v9_c.m_data != nullptr);
    TEST_ASSERT(v9_c.m_indices != nullptr);
    TEST_ASSERT(v9_c.m_capacity >= 3);
    TEST_ASSERT(v9.m_data != v8_c.m_data);
    TEST_ASSERT(v9.m_indices != v8_c.m_indices);

    TEST_ASSERT(v9_c.m_data[0] == 1.2);
    TEST_ASSERT(v9_c.m_indices[0] == 1);
    TEST_ASSERT(v9_c.m_data[1] == 5.6);
    TEST_ASSERT(v9_c.m_indices[1] == 0);
    TEST_ASSERT(v9_c.m_data[2] == 1.4);
    TEST_ASSERT(v9_c.m_indices[2] == 2);

    SparseVector::_globalRelease();
}

void SparseVectorTestSuite::move()
{
    SparseVector::_globalRelease();
    SparseVector::_globalInit();

    SparseVector v1(0);
    SparseVector v1_c = std::move(v1);
    TEST_ASSERT(v1.m_length == 0);
    TEST_ASSERT(v1.m_nonZeros == 0);
    TEST_ASSERT(v1.m_data == nullptr);
    TEST_ASSERT(v1.m_indices == nullptr);
    TEST_ASSERT(v1.m_capacity == 0);
    TEST_ASSERT(v1_c.m_length == 0);
    TEST_ASSERT(v1_c.m_nonZeros == 0);
    TEST_ASSERT(v1_c.m_data == nullptr);
    TEST_ASSERT(v1_c.m_indices == nullptr);
    TEST_ASSERT(v1_c.m_capacity == 0);
    TEST_ASSERT(SparseVector::sm_fullLengthVectorLength == 0);
    TEST_ASSERT(SparseVector::sm_fullLengthVector == nullptr);
    TEST_ASSERT(SparseVector::sm_indexPointerVector == nullptr);
    TEST_ASSERT(SparseVector::sm_indexVector == nullptr);

    TEST_ASSERT(SparseVector::sm_sparseToDenseDotProduct == &SparseVector::dotProductSparseToDenseUnstable );
    TEST_ASSERT(SparseVector::sm_sparseToIndexedDenseDotProduct == &SparseVector::dotProductSparseToIndexedDenseUnstable );
    TEST_ASSERT(SparseVector::sm_sparseToSparseDotProduct == &SparseVector::dotProductSparseToSparseUnstable);
    TEST_ASSERT(SparseVector::sm_addDenseToSparse == &SparseVector::addDenseToSparseFast);
    TEST_ASSERT(SparseVector::sm_addSparseToSparse == &SparseVector::addSparseToSparseFast);
    TEST_ASSERT(SparseVector::sm_addIndexedDenseToSparse == &SparseVector::addIndexedDenseToSparseFast);


    SparseVector v2(1);
    SparseVector v2_c = std::move(v2);
    TEST_ASSERT(v2.m_length == 0);
    TEST_ASSERT(v2.m_nonZeros == 0);
    TEST_ASSERT(v2.m_data == nullptr);
    TEST_ASSERT(v2.m_indices == nullptr);
    TEST_ASSERT(v2.m_capacity == 0);
    TEST_ASSERT(v2_c.m_length == 1);
    TEST_ASSERT(v2_c.m_nonZeros == 0);
    TEST_ASSERT(v2_c.m_data == nullptr);
    TEST_ASSERT(v2_c.m_indices == nullptr);
    TEST_ASSERT(v2_c.m_capacity == 0);
    TEST_ASSERT(SparseVector::sm_fullLengthVectorLength == 1);
    TEST_ASSERT(SparseVector::sm_fullLengthVector != nullptr);
    TEST_ASSERT(SparseVector::sm_indexPointerVector != nullptr);
    TEST_ASSERT(SparseVector::sm_indexVector != nullptr);

    TEST_ASSERT(SparseVector::sm_sparseToDenseDotProduct == &SparseVector::dotProductSparseToDenseUnstable );
    TEST_ASSERT(SparseVector::sm_sparseToIndexedDenseDotProduct == &SparseVector::dotProductSparseToIndexedDenseUnstable );
    TEST_ASSERT(SparseVector::sm_sparseToSparseDotProduct == &SparseVector::dotProductSparseToSparseUnstable);
    TEST_ASSERT(SparseVector::sm_addDenseToSparse == &SparseVector::addDenseToSparseFast);
    TEST_ASSERT(SparseVector::sm_addSparseToSparse == &SparseVector::addSparseToSparseFast);
    TEST_ASSERT(SparseVector::sm_addIndexedDenseToSparse == &SparseVector::addIndexedDenseToSparseFast);

    SparseVector v3(2);
    SparseVector v3_c = std::move(v3);
    TEST_ASSERT(v3.m_length == 0);
    TEST_ASSERT(v3.m_nonZeros == 0);
    TEST_ASSERT(v3.m_data == nullptr);
    TEST_ASSERT(v3.m_indices == nullptr);
    TEST_ASSERT(v3.m_capacity == 0);
    TEST_ASSERT(v3_c.m_length == 2);
    TEST_ASSERT(v3_c.m_nonZeros == 0);
    TEST_ASSERT(v3_c.m_data == nullptr);
    TEST_ASSERT(v3_c.m_indices == nullptr);
    TEST_ASSERT(v3_c.m_capacity == 0);
    TEST_ASSERT(SparseVector::sm_fullLengthVectorLength == 2);
    TEST_ASSERT(SparseVector::sm_fullLengthVector != nullptr);
    TEST_ASSERT(SparseVector::sm_indexPointerVector != nullptr);
    TEST_ASSERT(SparseVector::sm_indexVector != nullptr);

    TEST_ASSERT(SparseVector::sm_sparseToDenseDotProduct == &SparseVector::dotProductSparseToDenseUnstable );
    TEST_ASSERT(SparseVector::sm_sparseToIndexedDenseDotProduct == &SparseVector::dotProductSparseToIndexedDenseUnstable );
    TEST_ASSERT(SparseVector::sm_sparseToSparseDotProduct == &SparseVector::dotProductSparseToSparseUnstable);
    TEST_ASSERT(SparseVector::sm_addDenseToSparse == &SparseVector::addDenseToSparseFast);
    TEST_ASSERT(SparseVector::sm_addSparseToSparse == &SparseVector::addSparseToSparseFast);
    TEST_ASSERT(SparseVector::sm_addIndexedDenseToSparse == &SparseVector::addIndexedDenseToSparseFast);

    SparseVector v4(10);
    SparseVector v4_c = std::move(v4);
    TEST_ASSERT(v4.m_length == 0);
    TEST_ASSERT(v4.m_nonZeros == 0);
    TEST_ASSERT(v4.m_data == nullptr);
    TEST_ASSERT(v4.m_indices == nullptr);
    TEST_ASSERT(v4.m_capacity == 0);
    TEST_ASSERT(v4_c.m_length == 10);
    TEST_ASSERT(v4_c.m_nonZeros == 0);
    TEST_ASSERT(v4_c.m_data == nullptr);
    TEST_ASSERT(v4_c.m_indices == nullptr);
    TEST_ASSERT(v4_c.m_capacity == 0);
    TEST_ASSERT(SparseVector::sm_fullLengthVectorLength == 10);
    TEST_ASSERT(SparseVector::sm_fullLengthVector != nullptr);
    TEST_ASSERT(SparseVector::sm_indexPointerVector != nullptr);
    TEST_ASSERT(SparseVector::sm_indexVector != nullptr);

    TEST_ASSERT(SparseVector::sm_sparseToDenseDotProduct == &SparseVector::dotProductSparseToDenseUnstable );
    TEST_ASSERT(SparseVector::sm_sparseToIndexedDenseDotProduct == &SparseVector::dotProductSparseToIndexedDenseUnstable );
    TEST_ASSERT(SparseVector::sm_sparseToSparseDotProduct == &SparseVector::dotProductSparseToSparseUnstable);
    TEST_ASSERT(SparseVector::sm_addDenseToSparse == &SparseVector::addDenseToSparseFast);
    TEST_ASSERT(SparseVector::sm_addSparseToSparse == &SparseVector::addSparseToSparseFast);
    TEST_ASSERT(SparseVector::sm_addIndexedDenseToSparse == &SparseVector::addIndexedDenseToSparseFast);

    SparseVector v5(5);
    SparseVector v5_c = std::move(v5);
    TEST_ASSERT(v5.m_length == 0);
    TEST_ASSERT(v5.m_nonZeros == 0);
    TEST_ASSERT(v5.m_data == nullptr);
    TEST_ASSERT(v5.m_indices == nullptr);
    TEST_ASSERT(v5.m_capacity == 0);
    TEST_ASSERT(v5_c.m_length == 5);
    TEST_ASSERT(v5_c.m_nonZeros == 0);
    TEST_ASSERT(v5_c.m_data == nullptr);
    TEST_ASSERT(v5_c.m_indices == nullptr);
    TEST_ASSERT(v5_c.m_capacity == 0);
    TEST_ASSERT(SparseVector::sm_fullLengthVectorLength == 10);
    TEST_ASSERT(SparseVector::sm_fullLengthVector != nullptr);
    TEST_ASSERT(SparseVector::sm_indexPointerVector != nullptr);
    TEST_ASSERT(SparseVector::sm_indexVector != nullptr);

    TEST_ASSERT(SparseVector::sm_sparseToDenseDotProduct == &SparseVector::dotProductSparseToDenseUnstable );
    TEST_ASSERT(SparseVector::sm_sparseToIndexedDenseDotProduct == &SparseVector::dotProductSparseToIndexedDenseUnstable );
    TEST_ASSERT(SparseVector::sm_sparseToSparseDotProduct == &SparseVector::dotProductSparseToSparseUnstable);
    TEST_ASSERT(SparseVector::sm_addDenseToSparse == &SparseVector::addDenseToSparseFast);
    TEST_ASSERT(SparseVector::sm_addSparseToSparse == &SparseVector::addSparseToSparseFast);
    TEST_ASSERT(SparseVector::sm_addIndexedDenseToSparse == &SparseVector::addIndexedDenseToSparseFast);

    SparseVector::setAddMode(Numerical::ADD_ABS);
    SparseVector::setDotProductMode(Numerical::DOT_ABS);

    SparseVector v6(20);
    SparseVector v6_c = std::move(v6);
    TEST_ASSERT(v6.m_length == 0);
    TEST_ASSERT(v6.m_nonZeros == 0);
    TEST_ASSERT(v6.m_data == nullptr);
    TEST_ASSERT(v6.m_indices == nullptr);
    TEST_ASSERT(v6.m_capacity == 0);
    TEST_ASSERT(v6_c.m_length == 20);
    TEST_ASSERT(v6_c.m_nonZeros == 0);
    TEST_ASSERT(v6_c.m_data == nullptr);
    TEST_ASSERT(v6_c.m_indices == nullptr);
    TEST_ASSERT(v6_c.m_capacity == 0);
    TEST_ASSERT(SparseVector::sm_fullLengthVectorLength == 20);
    TEST_ASSERT(SparseVector::sm_fullLengthVector != nullptr);
    TEST_ASSERT(SparseVector::sm_indexPointerVector != nullptr);
    TEST_ASSERT(SparseVector::sm_indexVector != nullptr);

    TEST_ASSERT(SparseVector::sm_sparseToDenseDotProduct == &SparseVector::dotProductSparseToDenseAbs );
    TEST_ASSERT(SparseVector::sm_sparseToIndexedDenseDotProduct == &SparseVector::dotProductSparseToIndexedDenseAbs );
    TEST_ASSERT(SparseVector::sm_sparseToSparseDotProduct == &SparseVector::dotProductSparseToSparseAbs);
    TEST_ASSERT(SparseVector::sm_addDenseToSparse == &SparseVector::addDenseToSparseAbs);
    TEST_ASSERT(SparseVector::sm_addSparseToSparse == &SparseVector::addSparseToSparseAbs);
    TEST_ASSERT(SparseVector::sm_addIndexedDenseToSparse == &SparseVector::addIndexedDenseToSparseAbs);

    SparseVector v7(1);
    v7.set(0, 4.2);
    SparseVector v7_c = std::move(v7);
    TEST_ASSERT(v7.m_length == 0);
    TEST_ASSERT(v7.m_nonZeros == 0);
    TEST_ASSERT(v7.m_data == nullptr);
    TEST_ASSERT(v7.m_indices == nullptr);
    TEST_ASSERT(v7.m_capacity == 0);
    TEST_ASSERT(v7_c.m_length == 1);
    TEST_ASSERT(v7_c.m_nonZeros == 1);
    TEST_ASSERT(v7_c.m_data != nullptr);
    TEST_ASSERT(v7_c.m_indices != nullptr);
    TEST_ASSERT(v7_c.m_capacity >= 1);
    TEST_ASSERT(v7.m_data != v7_c.m_data);
    TEST_ASSERT(v7.m_indices != v7_c.m_indices);

    TEST_ASSERT(v7_c.m_data[0] == 4.2);
    TEST_ASSERT(v7_c.m_indices[0] == 0);

    SparseVector v8(10);
    v8.set(4, 1.2);
    v8.set(1, 5.6);
    v8.set(9, 1.4);
    SparseVector v8_c = std::move(v8);
    TEST_ASSERT(v8.m_length == 0);
    TEST_ASSERT(v8.m_nonZeros == 0);
    TEST_ASSERT(v8.m_data == nullptr);
    TEST_ASSERT(v8.m_indices == nullptr);
    TEST_ASSERT(v8.m_capacity == 0);
    TEST_ASSERT(v8_c.m_length == 10);
    TEST_ASSERT(v8_c.m_nonZeros == 3);
    TEST_ASSERT(v8_c.m_data != nullptr);
    TEST_ASSERT(v8_c.m_indices != nullptr);
    TEST_ASSERT(v8_c.m_capacity >= 3);
    TEST_ASSERT(v8.m_data != v8_c.m_data);
    TEST_ASSERT(v8.m_indices != v8_c.m_indices);

    TEST_ASSERT(v8_c.m_data[0] == 1.2);
    TEST_ASSERT(v8_c.m_indices[0] == 4);
    TEST_ASSERT(v8_c.m_data[1] == 5.6);
    TEST_ASSERT(v8_c.m_indices[1] == 1);
    TEST_ASSERT(v8_c.m_data[2] == 1.4);
    TEST_ASSERT(v8_c.m_indices[2] == 9);

    SparseVector v9(3);
    v9.set(1, 1.2);
    v9.set(0, 5.6);
    v9.set(2, 1.4);
    SparseVector v9_c = std::move(v9);
    TEST_ASSERT(v9.m_length == 0);
    TEST_ASSERT(v9.m_nonZeros == 0);
    TEST_ASSERT(v9.m_data == nullptr);
    TEST_ASSERT(v9.m_indices == nullptr);
    TEST_ASSERT(v9.m_capacity == 0);
    TEST_ASSERT(v9_c.m_length == 3);
    TEST_ASSERT(v9_c.m_nonZeros == 3);
    TEST_ASSERT(v9_c.m_data != nullptr);
    TEST_ASSERT(v9_c.m_indices != nullptr);
    TEST_ASSERT(v9_c.m_capacity >= 3);
    TEST_ASSERT(v9.m_data != v8_c.m_data);
    TEST_ASSERT(v9.m_indices != v8_c.m_indices);

    TEST_ASSERT(v9_c.m_data[0] == 1.2);
    TEST_ASSERT(v9_c.m_indices[0] == 1);
    TEST_ASSERT(v9_c.m_data[1] == 5.6);
    TEST_ASSERT(v9_c.m_indices[1] == 0);
    TEST_ASSERT(v9_c.m_data[2] == 1.4);
    TEST_ASSERT(v9_c.m_indices[2] == 2);

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

    TEST_ASSERT(v1.m_length == 0);
    TEST_ASSERT(v1.m_nonZeros == 0);
    TEST_ASSERT(v1.m_data == nullptr);
    TEST_ASSERT(v1.m_indices == nullptr);
    TEST_ASSERT(v1.m_capacity == 0);
    TEST_ASSERT(v1_c.m_length == 0);
    TEST_ASSERT(v1_c.m_nonZeros == 0);
    TEST_ASSERT(v1_c.m_data == nullptr);
    TEST_ASSERT(v1_c.m_indices == nullptr);
    TEST_ASSERT(v1_c.m_capacity == 0);
    TEST_ASSERT(SparseVector::sm_fullLengthVectorLength == 10);
    TEST_ASSERT(SparseVector::sm_fullLengthVector != nullptr);
    TEST_ASSERT(SparseVector::sm_indexPointerVector != nullptr);
    TEST_ASSERT(SparseVector::sm_indexVector != nullptr);

    TEST_ASSERT(SparseVector::sm_sparseToDenseDotProduct == &SparseVector::dotProductSparseToDenseUnstable );
    TEST_ASSERT(SparseVector::sm_sparseToIndexedDenseDotProduct == &SparseVector::dotProductSparseToIndexedDenseUnstable );
    TEST_ASSERT(SparseVector::sm_sparseToSparseDotProduct == &SparseVector::dotProductSparseToSparseUnstable);
    TEST_ASSERT(SparseVector::sm_addDenseToSparse == &SparseVector::addDenseToSparseFast);
    TEST_ASSERT(SparseVector::sm_addSparseToSparse == &SparseVector::addSparseToSparseFast);
    TEST_ASSERT(SparseVector::sm_addIndexedDenseToSparse == &SparseVector::addIndexedDenseToSparseFast);

    SparseVector v2(1);
    SparseVector v2_c(10);
    v2_c.set(8, 3.4);
    v2_c = v2;
    TEST_ASSERT(v2.m_length == 1);
    TEST_ASSERT(v2.m_nonZeros == 0);
    TEST_ASSERT(v2.m_data == nullptr);
    TEST_ASSERT(v2.m_indices == nullptr);
    TEST_ASSERT(v2.m_capacity == 0);
    TEST_ASSERT(v2_c.m_length == 1);
    TEST_ASSERT(v2_c.m_nonZeros == 0);
    TEST_ASSERT(v2_c.m_data == nullptr);
    TEST_ASSERT(v2_c.m_indices == nullptr);
    TEST_ASSERT(v2_c.m_capacity == 0);
    TEST_ASSERT(SparseVector::sm_fullLengthVectorLength == 10);
    TEST_ASSERT(SparseVector::sm_fullLengthVector != nullptr);
    TEST_ASSERT(SparseVector::sm_indexPointerVector != nullptr);
    TEST_ASSERT(SparseVector::sm_indexVector != nullptr);

    TEST_ASSERT(SparseVector::sm_sparseToDenseDotProduct == &SparseVector::dotProductSparseToDenseUnstable );
    TEST_ASSERT(SparseVector::sm_sparseToIndexedDenseDotProduct == &SparseVector::dotProductSparseToIndexedDenseUnstable );
    TEST_ASSERT(SparseVector::sm_sparseToSparseDotProduct == &SparseVector::dotProductSparseToSparseUnstable);
    TEST_ASSERT(SparseVector::sm_addDenseToSparse == &SparseVector::addDenseToSparseFast);
    TEST_ASSERT(SparseVector::sm_addSparseToSparse == &SparseVector::addSparseToSparseFast);
    TEST_ASSERT(SparseVector::sm_addIndexedDenseToSparse == &SparseVector::addIndexedDenseToSparseFast);

    SparseVector v3(2);
    SparseVector v3_c(10);
    v3_c.set(8, 3.4);
    v3_c = v3;
    TEST_ASSERT(v3.m_length == 2);
    TEST_ASSERT(v3.m_nonZeros == 0);
    TEST_ASSERT(v3.m_data == nullptr);
    TEST_ASSERT(v3.m_indices == nullptr);
    TEST_ASSERT(v3.m_capacity == 0);
    TEST_ASSERT(v3_c.m_length == 2);
    TEST_ASSERT(v3_c.m_nonZeros == 0);
    TEST_ASSERT(v3_c.m_data == nullptr);
    TEST_ASSERT(v3_c.m_indices == nullptr);
    TEST_ASSERT(v3_c.m_capacity == 0);
    TEST_ASSERT(SparseVector::sm_fullLengthVectorLength == 10);
    TEST_ASSERT(SparseVector::sm_fullLengthVector != nullptr);
    TEST_ASSERT(SparseVector::sm_indexPointerVector != nullptr);
    TEST_ASSERT(SparseVector::sm_indexVector != nullptr);

    TEST_ASSERT(SparseVector::sm_sparseToDenseDotProduct == &SparseVector::dotProductSparseToDenseUnstable );
    TEST_ASSERT(SparseVector::sm_sparseToIndexedDenseDotProduct == &SparseVector::dotProductSparseToIndexedDenseUnstable );
    TEST_ASSERT(SparseVector::sm_sparseToSparseDotProduct == &SparseVector::dotProductSparseToSparseUnstable);
    TEST_ASSERT(SparseVector::sm_addDenseToSparse == &SparseVector::addDenseToSparseFast);
    TEST_ASSERT(SparseVector::sm_addSparseToSparse == &SparseVector::addSparseToSparseFast);
    TEST_ASSERT(SparseVector::sm_addIndexedDenseToSparse == &SparseVector::addIndexedDenseToSparseFast);

    SparseVector v4(10);
    SparseVector v4_c(10);
    v4_c.set(8, 3.4);
    v4_c = v4;
    TEST_ASSERT(v4.m_length == 10);
    TEST_ASSERT(v4.m_nonZeros == 0);
    TEST_ASSERT(v4.m_data == nullptr);
    TEST_ASSERT(v4.m_indices == nullptr);
    TEST_ASSERT(v4.m_capacity == 0);
    TEST_ASSERT(v4_c.m_length == 10);
    TEST_ASSERT(v4_c.m_nonZeros == 0);
    TEST_ASSERT(v4_c.m_data == nullptr);
    TEST_ASSERT(v4_c.m_indices == nullptr);
    TEST_ASSERT(v4_c.m_capacity == 0);
    TEST_ASSERT(SparseVector::sm_fullLengthVectorLength == 10);
    TEST_ASSERT(SparseVector::sm_fullLengthVector != nullptr);
    TEST_ASSERT(SparseVector::sm_indexPointerVector != nullptr);
    TEST_ASSERT(SparseVector::sm_indexVector != nullptr);

    TEST_ASSERT(SparseVector::sm_sparseToDenseDotProduct == &SparseVector::dotProductSparseToDenseUnstable );
    TEST_ASSERT(SparseVector::sm_sparseToIndexedDenseDotProduct == &SparseVector::dotProductSparseToIndexedDenseUnstable );
    TEST_ASSERT(SparseVector::sm_sparseToSparseDotProduct == &SparseVector::dotProductSparseToSparseUnstable);
    TEST_ASSERT(SparseVector::sm_addDenseToSparse == &SparseVector::addDenseToSparseFast);
    TEST_ASSERT(SparseVector::sm_addSparseToSparse == &SparseVector::addSparseToSparseFast);
    TEST_ASSERT(SparseVector::sm_addIndexedDenseToSparse == &SparseVector::addIndexedDenseToSparseFast);

    SparseVector v5(5);
    SparseVector v5_c(10);
    v5_c.set(8, 3.4);
    v5_c = v5;
    TEST_ASSERT(v5.m_length == 5);
    TEST_ASSERT(v5.m_nonZeros == 0);
    TEST_ASSERT(v5.m_data == nullptr);
    TEST_ASSERT(v5.m_indices == nullptr);
    TEST_ASSERT(v5.m_capacity == 0);
    TEST_ASSERT(v5_c.m_length == 5);
    TEST_ASSERT(v5_c.m_nonZeros == 0);
    TEST_ASSERT(v5_c.m_data == nullptr);
    TEST_ASSERT(v5_c.m_indices == nullptr);
    TEST_ASSERT(v5_c.m_capacity == 0);
    TEST_ASSERT(SparseVector::sm_fullLengthVectorLength == 10);
    TEST_ASSERT(SparseVector::sm_fullLengthVector != nullptr);
    TEST_ASSERT(SparseVector::sm_indexPointerVector != nullptr);
    TEST_ASSERT(SparseVector::sm_indexVector != nullptr);

    TEST_ASSERT(SparseVector::sm_sparseToDenseDotProduct == &SparseVector::dotProductSparseToDenseUnstable );
    TEST_ASSERT(SparseVector::sm_sparseToIndexedDenseDotProduct == &SparseVector::dotProductSparseToIndexedDenseUnstable );
    TEST_ASSERT(SparseVector::sm_sparseToSparseDotProduct == &SparseVector::dotProductSparseToSparseUnstable);
    TEST_ASSERT(SparseVector::sm_addDenseToSparse == &SparseVector::addDenseToSparseFast);
    TEST_ASSERT(SparseVector::sm_addSparseToSparse == &SparseVector::addSparseToSparseFast);
    TEST_ASSERT(SparseVector::sm_addIndexedDenseToSparse == &SparseVector::addIndexedDenseToSparseFast);

    SparseVector::setAddMode(Numerical::ADD_ABS);
    SparseVector::setDotProductMode(Numerical::DOT_ABS);

    SparseVector v6(20);
    SparseVector v6_c(10);
    v6_c.set(8, 3.4);
    v6_c = v6;
    TEST_ASSERT(v6.m_length == 20);
    TEST_ASSERT(v6.m_nonZeros == 0);
    TEST_ASSERT(v6.m_data == nullptr);
    TEST_ASSERT(v6.m_indices == nullptr);
    TEST_ASSERT(v6.m_capacity == 0);
    TEST_ASSERT(v6_c.m_length == 20);
    TEST_ASSERT(v6_c.m_nonZeros == 0);
    TEST_ASSERT(v6_c.m_data == nullptr);
    TEST_ASSERT(v6_c.m_indices == nullptr);
    TEST_ASSERT(v6_c.m_capacity == 0);
    TEST_ASSERT(SparseVector::sm_fullLengthVectorLength == 20);
    TEST_ASSERT(SparseVector::sm_fullLengthVector != nullptr);
    TEST_ASSERT(SparseVector::sm_indexPointerVector != nullptr);
    TEST_ASSERT(SparseVector::sm_indexVector != nullptr);

    TEST_ASSERT(SparseVector::sm_sparseToDenseDotProduct == &SparseVector::dotProductSparseToDenseAbs );
    TEST_ASSERT(SparseVector::sm_sparseToIndexedDenseDotProduct == &SparseVector::dotProductSparseToIndexedDenseAbs );
    TEST_ASSERT(SparseVector::sm_sparseToSparseDotProduct == &SparseVector::dotProductSparseToSparseAbs);
    TEST_ASSERT(SparseVector::sm_addDenseToSparse == &SparseVector::addDenseToSparseAbs);
    TEST_ASSERT(SparseVector::sm_addSparseToSparse == &SparseVector::addSparseToSparseAbs);
    TEST_ASSERT(SparseVector::sm_addIndexedDenseToSparse == &SparseVector::addIndexedDenseToSparseAbs);

    SparseVector v7(1);
    v7.set(0, 4.2);
    SparseVector v7_c(10);
    v7_c.set(8, 3.4);
    v7_c = v7;
    TEST_ASSERT(v7.m_length == 1);
    TEST_ASSERT(v7.m_nonZeros == 1);
    TEST_ASSERT(v7.m_data != nullptr);
    TEST_ASSERT(v7.m_indices != nullptr);
    TEST_ASSERT(v7.m_capacity >= 1);
    TEST_ASSERT(v7_c.m_length == 1);
    TEST_ASSERT(v7_c.m_nonZeros == 1);
    TEST_ASSERT(v7_c.m_data != nullptr);
    TEST_ASSERT(v7_c.m_indices != nullptr);
    TEST_ASSERT(v7_c.m_capacity >= 1);
    TEST_ASSERT(v7.m_data != v7_c.m_data);
    TEST_ASSERT(v7.m_indices != v7_c.m_indices);

    TEST_ASSERT(v7_c.m_data[0] == 4.2);
    TEST_ASSERT(v7_c.m_indices[0] == 0);

    SparseVector v8(10);
    v8.set(4, 1.2);
    v8.set(1, 5.6);
    v8.set(9, 1.4);
    SparseVector v8_c(10);
    v8_c.set(8, 3.4);
    v8_c = v8;
    TEST_ASSERT(v8.m_length == 10);
    TEST_ASSERT(v8.m_nonZeros == 3);
    TEST_ASSERT(v8.m_data != nullptr);
    TEST_ASSERT(v8.m_indices != nullptr);
    TEST_ASSERT(v8.m_capacity >= 3);
    TEST_ASSERT(v8_c.m_length == 10);
    TEST_ASSERT(v8_c.m_nonZeros == 3);
    TEST_ASSERT(v8_c.m_data != nullptr);
    TEST_ASSERT(v8_c.m_indices != nullptr);
    TEST_ASSERT(v8_c.m_capacity >= 3);
    TEST_ASSERT(v8.m_data != v8_c.m_data);
    TEST_ASSERT(v8.m_indices != v8_c.m_indices);

    TEST_ASSERT(v8_c.m_data[0] == 1.2);
    TEST_ASSERT(v8_c.m_indices[0] == 4);
    TEST_ASSERT(v8_c.m_data[1] == 5.6);
    TEST_ASSERT(v8_c.m_indices[1] == 1);
    TEST_ASSERT(v8_c.m_data[2] == 1.4);
    TEST_ASSERT(v8_c.m_indices[2] == 9);

    SparseVector v9(3);
    v9.set(1, 1.2);
    v9.set(0, 5.6);
    v9.set(2, 1.4);
    SparseVector v9_c(10);
    v9_c.set(8, 3.4);
    v9_c = v9;
    TEST_ASSERT(v9.m_length == 3);
    TEST_ASSERT(v9.m_nonZeros == 3);
    TEST_ASSERT(v9.m_data != nullptr);
    TEST_ASSERT(v9.m_indices != nullptr);
    TEST_ASSERT(v9.m_capacity >= 3);
    TEST_ASSERT(v9_c.m_length == 3);
    TEST_ASSERT(v9_c.m_nonZeros == 3);
    TEST_ASSERT(v9_c.m_data != nullptr);
    TEST_ASSERT(v9_c.m_indices != nullptr);
    TEST_ASSERT(v9_c.m_capacity >= 3);
    TEST_ASSERT(v9.m_data != v8_c.m_data);
    TEST_ASSERT(v9.m_indices != v8_c.m_indices);

    TEST_ASSERT(v9_c.m_data[0] == 1.2);
    TEST_ASSERT(v9_c.m_indices[0] == 1);
    TEST_ASSERT(v9_c.m_data[1] == 5.6);
    TEST_ASSERT(v9_c.m_indices[1] == 0);
    TEST_ASSERT(v9_c.m_data[2] == 1.4);
    TEST_ASSERT(v9_c.m_indices[2] == 2);

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
    TEST_ASSERT(v1.m_length == 0);
    TEST_ASSERT(v1.m_nonZeros == 0);
    TEST_ASSERT(v1.m_data == nullptr);
    TEST_ASSERT(v1.m_indices == nullptr);
    TEST_ASSERT(v1.m_capacity == 0);
    TEST_ASSERT(v1_c.m_length == 0);
    TEST_ASSERT(v1_c.m_nonZeros == 0);
    TEST_ASSERT(v1_c.m_data == nullptr);
    TEST_ASSERT(v1_c.m_indices == nullptr);
    TEST_ASSERT(v1_c.m_capacity == 0);
    TEST_ASSERT(SparseVector::sm_fullLengthVectorLength == 10);
    TEST_ASSERT(SparseVector::sm_fullLengthVector != nullptr);
    TEST_ASSERT(SparseVector::sm_indexPointerVector != nullptr);
    TEST_ASSERT(SparseVector::sm_indexVector != nullptr);

    TEST_ASSERT(SparseVector::sm_sparseToDenseDotProduct == &SparseVector::dotProductSparseToDenseUnstable );
    TEST_ASSERT(SparseVector::sm_sparseToIndexedDenseDotProduct == &SparseVector::dotProductSparseToIndexedDenseUnstable );
    TEST_ASSERT(SparseVector::sm_sparseToSparseDotProduct == &SparseVector::dotProductSparseToSparseUnstable);
    TEST_ASSERT(SparseVector::sm_addDenseToSparse == &SparseVector::addDenseToSparseFast);
    TEST_ASSERT(SparseVector::sm_addSparseToSparse == &SparseVector::addSparseToSparseFast);
    TEST_ASSERT(SparseVector::sm_addIndexedDenseToSparse == &SparseVector::addIndexedDenseToSparseFast);


    SparseVector v2(1);
    SparseVector v2_c(10);
    v2_c.set(4, 6.4);
    v2_c = std::move(v2);
    TEST_ASSERT(v2.m_length == 0);
    TEST_ASSERT(v2.m_nonZeros == 0);
    TEST_ASSERT(v2.m_data == nullptr);
    TEST_ASSERT(v2.m_indices == nullptr);
    TEST_ASSERT(v2.m_capacity == 0);
    TEST_ASSERT(v2_c.m_length == 1);
    TEST_ASSERT(v2_c.m_nonZeros == 0);
    TEST_ASSERT(v2_c.m_data == nullptr);
    TEST_ASSERT(v2_c.m_indices == nullptr);
    TEST_ASSERT(v2_c.m_capacity == 0);
    TEST_ASSERT(SparseVector::sm_fullLengthVectorLength == 10);
    TEST_ASSERT(SparseVector::sm_fullLengthVector != nullptr);
    TEST_ASSERT(SparseVector::sm_indexPointerVector != nullptr);
    TEST_ASSERT(SparseVector::sm_indexVector != nullptr);

    TEST_ASSERT(SparseVector::sm_sparseToDenseDotProduct == &SparseVector::dotProductSparseToDenseUnstable );
    TEST_ASSERT(SparseVector::sm_sparseToIndexedDenseDotProduct == &SparseVector::dotProductSparseToIndexedDenseUnstable );
    TEST_ASSERT(SparseVector::sm_sparseToSparseDotProduct == &SparseVector::dotProductSparseToSparseUnstable);
    TEST_ASSERT(SparseVector::sm_addDenseToSparse == &SparseVector::addDenseToSparseFast);
    TEST_ASSERT(SparseVector::sm_addSparseToSparse == &SparseVector::addSparseToSparseFast);
    TEST_ASSERT(SparseVector::sm_addIndexedDenseToSparse == &SparseVector::addIndexedDenseToSparseFast);

    SparseVector v3(2);
    SparseVector v3_c(10);
    v3_c.set(4, 6.4);
    v3_c = std::move(v3);
    TEST_ASSERT(v3.m_length == 0);
    TEST_ASSERT(v3.m_nonZeros == 0);
    TEST_ASSERT(v3.m_data == nullptr);
    TEST_ASSERT(v3.m_indices == nullptr);
    TEST_ASSERT(v3.m_capacity == 0);
    TEST_ASSERT(v3_c.m_length == 2);
    TEST_ASSERT(v3_c.m_nonZeros == 0);
    TEST_ASSERT(v3_c.m_data == nullptr);
    TEST_ASSERT(v3_c.m_indices == nullptr);
    TEST_ASSERT(v3_c.m_capacity == 0);
    TEST_ASSERT(SparseVector::sm_fullLengthVectorLength == 10);
    TEST_ASSERT(SparseVector::sm_fullLengthVector != nullptr);
    TEST_ASSERT(SparseVector::sm_indexPointerVector != nullptr);
    TEST_ASSERT(SparseVector::sm_indexVector != nullptr);

    TEST_ASSERT(SparseVector::sm_sparseToDenseDotProduct == &SparseVector::dotProductSparseToDenseUnstable );
    TEST_ASSERT(SparseVector::sm_sparseToIndexedDenseDotProduct == &SparseVector::dotProductSparseToIndexedDenseUnstable );
    TEST_ASSERT(SparseVector::sm_sparseToSparseDotProduct == &SparseVector::dotProductSparseToSparseUnstable);
    TEST_ASSERT(SparseVector::sm_addDenseToSparse == &SparseVector::addDenseToSparseFast);
    TEST_ASSERT(SparseVector::sm_addSparseToSparse == &SparseVector::addSparseToSparseFast);
    TEST_ASSERT(SparseVector::sm_addIndexedDenseToSparse == &SparseVector::addIndexedDenseToSparseFast);

    SparseVector v4(10);
    SparseVector v4_c(10);
    v4_c.set(4, 6.4);
    v4_c = std::move(v4);
    TEST_ASSERT(v4.m_length == 0);
    TEST_ASSERT(v4.m_nonZeros == 0);
    TEST_ASSERT(v4.m_data == nullptr);
    TEST_ASSERT(v4.m_indices == nullptr);
    TEST_ASSERT(v4.m_capacity == 0);
    TEST_ASSERT(v4_c.m_length == 10);
    TEST_ASSERT(v4_c.m_nonZeros == 0);
    TEST_ASSERT(v4_c.m_data == nullptr);
    TEST_ASSERT(v4_c.m_indices == nullptr);
    TEST_ASSERT(v4_c.m_capacity == 0);
    TEST_ASSERT(SparseVector::sm_fullLengthVectorLength == 10);
    TEST_ASSERT(SparseVector::sm_fullLengthVector != nullptr);
    TEST_ASSERT(SparseVector::sm_indexPointerVector != nullptr);
    TEST_ASSERT(SparseVector::sm_indexVector != nullptr);

    TEST_ASSERT(SparseVector::sm_sparseToDenseDotProduct == &SparseVector::dotProductSparseToDenseUnstable );
    TEST_ASSERT(SparseVector::sm_sparseToIndexedDenseDotProduct == &SparseVector::dotProductSparseToIndexedDenseUnstable );
    TEST_ASSERT(SparseVector::sm_sparseToSparseDotProduct == &SparseVector::dotProductSparseToSparseUnstable);
    TEST_ASSERT(SparseVector::sm_addDenseToSparse == &SparseVector::addDenseToSparseFast);
    TEST_ASSERT(SparseVector::sm_addSparseToSparse == &SparseVector::addSparseToSparseFast);
    TEST_ASSERT(SparseVector::sm_addIndexedDenseToSparse == &SparseVector::addIndexedDenseToSparseFast);

    SparseVector v5(5);
    SparseVector v5_c(10);
    v5_c.set(4, 6.4);
    v5_c = std::move(v5);
    TEST_ASSERT(v5.m_length == 0);
    TEST_ASSERT(v5.m_nonZeros == 0);
    TEST_ASSERT(v5.m_data == nullptr);
    TEST_ASSERT(v5.m_indices == nullptr);
    TEST_ASSERT(v5.m_capacity == 0);
    TEST_ASSERT(v5_c.m_length == 5);
    TEST_ASSERT(v5_c.m_nonZeros == 0);
    TEST_ASSERT(v5_c.m_data == nullptr);
    TEST_ASSERT(v5_c.m_indices == nullptr);
    TEST_ASSERT(v5_c.m_capacity == 0);
    TEST_ASSERT(SparseVector::sm_fullLengthVectorLength == 10);
    TEST_ASSERT(SparseVector::sm_fullLengthVector != nullptr);
    TEST_ASSERT(SparseVector::sm_indexPointerVector != nullptr);
    TEST_ASSERT(SparseVector::sm_indexVector != nullptr);

    TEST_ASSERT(SparseVector::sm_sparseToDenseDotProduct == &SparseVector::dotProductSparseToDenseUnstable );
    TEST_ASSERT(SparseVector::sm_sparseToIndexedDenseDotProduct == &SparseVector::dotProductSparseToIndexedDenseUnstable );
    TEST_ASSERT(SparseVector::sm_sparseToSparseDotProduct == &SparseVector::dotProductSparseToSparseUnstable);
    TEST_ASSERT(SparseVector::sm_addDenseToSparse == &SparseVector::addDenseToSparseFast);
    TEST_ASSERT(SparseVector::sm_addSparseToSparse == &SparseVector::addSparseToSparseFast);
    TEST_ASSERT(SparseVector::sm_addIndexedDenseToSparse == &SparseVector::addIndexedDenseToSparseFast);

    SparseVector::setAddMode(Numerical::ADD_ABS);
    SparseVector::setDotProductMode(Numerical::DOT_ABS);

    SparseVector v6(20);
    SparseVector v6_c(10);
    v6_c.set(4, 6.4);
    v6_c = std::move(v6);
    TEST_ASSERT(v6.m_length == 0);
    TEST_ASSERT(v6.m_nonZeros == 0);
    TEST_ASSERT(v6.m_data == nullptr);
    TEST_ASSERT(v6.m_indices == nullptr);
    TEST_ASSERT(v6.m_capacity == 0);
    TEST_ASSERT(v6_c.m_length == 20);
    TEST_ASSERT(v6_c.m_nonZeros == 0);
    TEST_ASSERT(v6_c.m_data == nullptr);
    TEST_ASSERT(v6_c.m_indices == nullptr);
    TEST_ASSERT(v6_c.m_capacity == 0);
    TEST_ASSERT(SparseVector::sm_fullLengthVectorLength == 20);
    TEST_ASSERT(SparseVector::sm_fullLengthVector != nullptr);
    TEST_ASSERT(SparseVector::sm_indexPointerVector != nullptr);
    TEST_ASSERT(SparseVector::sm_indexVector != nullptr);

    TEST_ASSERT(SparseVector::sm_sparseToDenseDotProduct == &SparseVector::dotProductSparseToDenseAbs );
    TEST_ASSERT(SparseVector::sm_sparseToIndexedDenseDotProduct == &SparseVector::dotProductSparseToIndexedDenseAbs );
    TEST_ASSERT(SparseVector::sm_sparseToSparseDotProduct == &SparseVector::dotProductSparseToSparseAbs);
    TEST_ASSERT(SparseVector::sm_addDenseToSparse == &SparseVector::addDenseToSparseAbs);
    TEST_ASSERT(SparseVector::sm_addSparseToSparse == &SparseVector::addSparseToSparseAbs);
    TEST_ASSERT(SparseVector::sm_addIndexedDenseToSparse == &SparseVector::addIndexedDenseToSparseAbs);

    SparseVector v7(1);
    v7.set(0, 4.2);
    SparseVector v7_c(10);
    v7_c.set(4, 6.4);
    v7_c = std::move(v7);
    TEST_ASSERT(v7.m_length == 0);
    TEST_ASSERT(v7.m_nonZeros == 0);
    TEST_ASSERT(v7.m_data == nullptr);
    TEST_ASSERT(v7.m_indices == nullptr);
    TEST_ASSERT(v7.m_capacity == 0);
    TEST_ASSERT(v7_c.m_length == 1);
    TEST_ASSERT(v7_c.m_nonZeros == 1);
    TEST_ASSERT(v7_c.m_data != nullptr);
    TEST_ASSERT(v7_c.m_indices != nullptr);
    TEST_ASSERT(v7_c.m_capacity >= 1);
    TEST_ASSERT(v7.m_data != v7_c.m_data);
    TEST_ASSERT(v7.m_indices != v7_c.m_indices);

    TEST_ASSERT(v7_c.m_data[0] == 4.2);
    TEST_ASSERT(v7_c.m_indices[0] == 0);

    SparseVector v8(10);
    v8.set(4, 1.2);
    v8.set(1, 5.6);
    v8.set(9, 1.4);
    SparseVector v8_c(10);
    v8_c.set(3, 6.4);
    v8_c = std::move(v8);
    TEST_ASSERT(v8.m_length == 0);
    TEST_ASSERT(v8.m_nonZeros == 0);
    TEST_ASSERT(v8.m_data == nullptr);
    TEST_ASSERT(v8.m_indices == nullptr);
    TEST_ASSERT(v8.m_capacity == 0);
    TEST_ASSERT(v8_c.m_length == 10);
    TEST_ASSERT(v8_c.m_nonZeros == 3);
    TEST_ASSERT(v8_c.m_data != nullptr);
    TEST_ASSERT(v8_c.m_indices != nullptr);
    TEST_ASSERT(v8_c.m_capacity >= 3);
    TEST_ASSERT(v8.m_data != v8_c.m_data);
    TEST_ASSERT(v8.m_indices != v8_c.m_indices);

    TEST_ASSERT(v8_c.m_data[0] == 1.2);
    TEST_ASSERT(v8_c.m_indices[0] == 4);
    TEST_ASSERT(v8_c.m_data[1] == 5.6);
    TEST_ASSERT(v8_c.m_indices[1] == 1);
    TEST_ASSERT(v8_c.m_data[2] == 1.4);
    TEST_ASSERT(v8_c.m_indices[2] == 9);

    SparseVector v9(3);
    v9.set(1, 1.2);
    v9.set(0, 5.6);
    v9.set(2, 1.4);
    SparseVector v9_c(10);
    v9_c.set(4, 6.4);
    v9_c = std::move(v9);
    TEST_ASSERT(v9.m_length == 0);
    TEST_ASSERT(v9.m_nonZeros == 0);
    TEST_ASSERT(v9.m_data == nullptr);
    TEST_ASSERT(v9.m_indices == nullptr);
    TEST_ASSERT(v9.m_capacity == 0);
    TEST_ASSERT(v9_c.m_length == 3);
    TEST_ASSERT(v9_c.m_nonZeros == 3);
    TEST_ASSERT(v9_c.m_data != nullptr);
    TEST_ASSERT(v9_c.m_indices != nullptr);
    TEST_ASSERT(v9_c.m_capacity >= 3);
    TEST_ASSERT(v9.m_data != v8_c.m_data);
    TEST_ASSERT(v9.m_indices != v8_c.m_indices);

    TEST_ASSERT(v9_c.m_data[0] == 1.2);
    TEST_ASSERT(v9_c.m_indices[0] == 1);
    TEST_ASSERT(v9_c.m_data[1] == 5.6);
    TEST_ASSERT(v9_c.m_indices[1] == 0);
    TEST_ASSERT(v9_c.m_data[2] == 1.4);
    TEST_ASSERT(v9_c.m_indices[2] == 2);

    SparseVector::_globalRelease();
}

void SparseVectorTestSuite::createUnitVector()
{
    SparseVector::_globalRelease();
    SparseVector::_globalInit();

    SparseVector v1 = SparseVector::createUnitVector(1, 0);
    TEST_ASSERT(v1.m_length == 1);
    TEST_ASSERT(v1.m_nonZeros == 1);
    TEST_ASSERT(v1.m_data != nullptr);
    TEST_ASSERT(v1.m_indices != nullptr);
    TEST_ASSERT(v1.m_capacity >= 1);

    TEST_ASSERT(v1.m_data[0] == 1);
    TEST_ASSERT(v1.m_indices[0] == 0);

    SparseVector v2 = SparseVector::createUnitVector(2, 0);
    TEST_ASSERT(v2.m_length == 2);
    TEST_ASSERT(v2.m_nonZeros == 1);
    TEST_ASSERT(v2.m_data != nullptr);
    TEST_ASSERT(v2.m_indices != nullptr);
    TEST_ASSERT(v2.m_capacity >= 1);

    TEST_ASSERT(v2.m_data[0] == 1);
    TEST_ASSERT(v2.m_indices[0] == 0);

    SparseVector v3 = SparseVector::createUnitVector(2, 1);
    TEST_ASSERT(v3.m_length == 2);
    TEST_ASSERT(v3.m_nonZeros == 1);
    TEST_ASSERT(v3.m_data != nullptr);
    TEST_ASSERT(v3.m_indices != nullptr);
    TEST_ASSERT(v3.m_capacity >= 1);

    TEST_ASSERT(v3.m_data[0] == 1);
    TEST_ASSERT(v3.m_indices[0] == 1);

    SparseVector v4 = SparseVector::createUnitVector(10, 0);
    TEST_ASSERT(v4.m_length == 10);
    TEST_ASSERT(v4.m_nonZeros == 1);
    TEST_ASSERT(v4.m_data != nullptr);
    TEST_ASSERT(v4.m_indices != nullptr);
    TEST_ASSERT(v4.m_capacity >= 1);

    TEST_ASSERT(v4.m_data[0] == 1);
    TEST_ASSERT(v4.m_indices[0] == 0);

    SparseVector v5 = SparseVector::createUnitVector(10, 4);
    TEST_ASSERT(v5.m_length == 10);
    TEST_ASSERT(v5.m_nonZeros == 1);
    TEST_ASSERT(v5.m_data != nullptr);
    TEST_ASSERT(v5.m_indices != nullptr);
    TEST_ASSERT(v5.m_capacity >= 1);

    TEST_ASSERT(v5.m_data[0] == 1);
    TEST_ASSERT(v5.m_indices[0] == 4);

    SparseVector v6 = SparseVector::createUnitVector(10, 9);
    TEST_ASSERT(v6.m_length == 10);
    TEST_ASSERT(v6.m_nonZeros == 1);
    TEST_ASSERT(v6.m_data != nullptr);
    TEST_ASSERT(v6.m_indices != nullptr);
    TEST_ASSERT(v6.m_capacity >= 1);

    TEST_ASSERT(v6.m_data[0] == 1);
    TEST_ASSERT(v6.m_indices[0] == 9);

    SparseVector::_globalRelease();
}

void SparseVectorTestSuite::createVectorFromDenseArray()
{
    SparseVector::_globalRelease();
    SparseVector::_globalInit();

    SparseVector::_globalRelease();
}

void SparseVectorTestSuite::createVectorFromSparseArray()
{
    SparseVector::_globalRelease();
    SparseVector::_globalInit();

    SparseVector::_globalRelease();
}

void SparseVectorTestSuite::createEtaByPivotValue()
{
    SparseVector::_globalRelease();
    SparseVector::_globalInit();

    SparseVector::_globalRelease();
}

void SparseVectorTestSuite::createEtaByPivotIndex()
{
    SparseVector::_globalRelease();
    SparseVector::_globalInit();

    SparseVector::_globalRelease();
}

void SparseVectorTestSuite::at()
{
    SparseVector::_globalRelease();
    SparseVector::_globalInit();

    SparseVector::_globalRelease();
}

void SparseVectorTestSuite::arrayOperator()
{
    SparseVector::_globalRelease();
    SparseVector::_globalInit();

    SparseVector::_globalRelease();
}

void SparseVectorTestSuite::set()
{
    SparseVector::_globalRelease();
    SparseVector::_globalInit();

    SparseVector::_globalRelease();
}

void SparseVectorTestSuite::length()
{
    SparseVector::_globalRelease();
    SparseVector::_globalInit();

    SparseVector::_globalRelease();
}

void SparseVectorTestSuite::euclidNorm()
{
    SparseVector::_globalRelease();
    SparseVector::_globalInit();

    SparseVector::_globalRelease();
}

void SparseVectorTestSuite::l1Norm()
{
    SparseVector::_globalRelease();
    SparseVector::_globalInit();

    SparseVector::_globalRelease();
}

void SparseVectorTestSuite::addSparseVector()
{
    SparseVector::_globalRelease();
    SparseVector::_globalInit();

    SparseVector::_globalRelease();
}

void SparseVectorTestSuite::addDenseVector()
{
    SparseVector::_globalRelease();
    SparseVector::_globalInit();

    SparseVector::_globalRelease();
}

void SparseVectorTestSuite::addIndexedDenseVector()
{
    SparseVector::_globalRelease();
    SparseVector::_globalInit();

    SparseVector::_globalRelease();
}

void SparseVectorTestSuite::nonZeros()
{
    SparseVector::_globalRelease();
    SparseVector::_globalInit();

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
