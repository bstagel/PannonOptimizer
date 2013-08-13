#include "vectortest.h"
#include "testmacros.h"

VectorTestSuite::VectorTestSuite(const char * name) : UnitTest(name)
{
    ADD_TEST(VectorTestSuite::init);
    ADD_TEST(VectorTestSuite::fillAndClear);
    ADD_TEST(VectorTestSuite::set);
    ADD_TEST(VectorTestSuite::change);
    ADD_TEST(VectorTestSuite::setNewNonzero);
    ADD_TEST(VectorTestSuite::insertElement);
    ADD_TEST(VectorTestSuite::append);
    ADD_TEST(VectorTestSuite::removeElement);
    ADD_TEST(VectorTestSuite::resize);
    ADD_TEST(VectorTestSuite::setSparsityRatio);
    ADD_TEST(VectorTestSuite::maxIndex);
    ADD_TEST(VectorTestSuite::absMaxElement);
    ADD_TEST(VectorTestSuite::scaleBy);
    ADD_TEST(VectorTestSuite::scaleElementBy);
    ADD_TEST(VectorTestSuite::scaleByLambdas);
    ADD_TEST(VectorTestSuite::addVector);
    ADD_TEST(VectorTestSuite::dotProduct);
    ADD_TEST(VectorTestSuite::euclidNorm);
    ADD_TEST(VectorTestSuite::elementaryFtran);
    ADD_TEST(VectorTestSuite::elementaryBtran);
    ADD_TEST(VectorTestSuite::plus);
    ADD_TEST(VectorTestSuite::minus);
    ADD_TEST(VectorTestSuite::asterisk);
    ADD_TEST(VectorTestSuite::asterisk2);
    ADD_TEST(VectorTestSuite::equal);
    ADD_TEST(VectorTestSuite::sortElements);
    ADD_TEST(VectorTestSuite::reInit);
}

void VectorTestSuite::init()
{
	Vector myVect(5);
	TEST_ASSERT(myVect.m_dimension == 5);
	TEST_ASSERT(myVect.m_capacity == ELBOWROOM);
	TEST_ASSERT(myVect.m_data == myVect.m_dataEnd);
	TEST_ASSERT(myVect.m_sorted == false);
	TEST_ASSERT(myVect.m_vectorType == 0);
	TEST_ASSERT(myVect.m_size == 0);
	TEST_ASSERT(myVect.m_sparsityThreshold == 2);
	TEST_ASSERT(myVect.m_nonZeros == 0);

	TEST_ASSERT(myVect.at(0) == 0);
	TEST_ASSERT(myVect.at(1) == 0);
	TEST_ASSERT(myVect.at(2) == 0);
	TEST_ASSERT(myVect.at(3) == 0);
	TEST_ASSERT(myVect.at(4) == 0);

	unsigned int myCounter = myVect.sm_fullLenghtReferenceCounter;
	myCounter++;
	Vector myVect2(myVect);
	TEST_ASSERT(myVect2.sm_fullLenghtReferenceCounter == myCounter);

	TEST_ASSERT(myVect2.m_dimension == 5);
	TEST_ASSERT(myVect2.m_capacity == ELBOWROOM);
	TEST_ASSERT(myVect2.m_data == myVect2.m_dataEnd);
	TEST_ASSERT(myVect2.m_sorted == false);
	TEST_ASSERT(myVect2.m_vectorType == 0);
	TEST_ASSERT(myVect2.m_size == 0);
	TEST_ASSERT(myVect2.m_sparsityThreshold == 2);
	TEST_ASSERT(myVect2.nonZeros() == 0);

	TEST_ASSERT(myVect2.at(0) == 0);
	TEST_ASSERT(myVect2.at(1) == 0);
	TEST_ASSERT(myVect2.at(2) == 0);
	TEST_ASSERT(myVect2.at(3) == 0);
	TEST_ASSERT(myVect2.at(4) == 0)
}

void VectorTestSuite::fillAndClear()
{
	Vector myVect(5);

	myVect.fill(7);
	TEST_ASSERT(myVect.m_dimension == 5);
	TEST_ASSERT(myVect.m_capacity == ELBOWROOM + 5);
	TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - 5);
	TEST_ASSERT(myVect.m_sorted == true);
	TEST_ASSERT(myVect.m_vectorType == 1);
	TEST_ASSERT(myVect.m_size == 5);
	TEST_ASSERT(myVect.m_sparsityThreshold == 2);
	TEST_ASSERT(myVect.m_nonZeros == 5);

	TEST_ASSERT(myVect.at(0) == 7);
	TEST_ASSERT(myVect.at(1) == 7);
	TEST_ASSERT(myVect.at(2) == 7);
	TEST_ASSERT(myVect.at(3) == 7);
	TEST_ASSERT(myVect.at(4) == 7);

	myVect.clear();
	TEST_ASSERT(myVect.m_dimension == 5);
	TEST_ASSERT(myVect.m_capacity == ELBOWROOM + 5);
	TEST_ASSERT(myVect.m_data == myVect.m_dataEnd);
	TEST_ASSERT(myVect.m_sorted == false);
	TEST_ASSERT(myVect.m_vectorType == 0);
	TEST_ASSERT(myVect.m_size == 0);
	TEST_ASSERT(myVect.m_sparsityThreshold == 2);
	TEST_ASSERT(myVect.m_nonZeros == 0);

	TEST_ASSERT(myVect.at(0) == 0);
	TEST_ASSERT(myVect.at(1) == 0);
	TEST_ASSERT(myVect.at(2) == 0);
	TEST_ASSERT(myVect.at(3) == 0);
	TEST_ASSERT(myVect.at(4) == 0);
}

void VectorTestSuite::set()
{
	Vector myVect(5);

	myVect.set(0, 4);

	TEST_ASSERT(myVect.m_dimension == 5);
	TEST_ASSERT(myVect.m_capacity == ELBOWROOM);
	TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - 1);
	TEST_ASSERT(myVect.m_sorted == false);
	TEST_ASSERT(myVect.m_vectorType == 0);
	TEST_ASSERT(myVect.m_size == 1);
	TEST_ASSERT(myVect.m_sparsityThreshold == 2);
	TEST_ASSERT(myVect.m_nonZeros == 1);

	TEST_ASSERT(myVect.at(0) == 4);
	TEST_ASSERT(myVect.at(1) == 0);
	TEST_ASSERT(myVect.at(2) == 0);
	TEST_ASSERT(myVect.at(3) == 0);
	TEST_ASSERT(myVect.at(4) == 0);

	myVect.set(2, 3);
	myVect.set(3, 9);

	//Should switch to dense vector

	TEST_ASSERT(myVect.m_dimension == 5);
	TEST_ASSERT(myVect.m_capacity == ELBOWROOM + 5);
	TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - 5);
	TEST_ASSERT(myVect.m_sorted == false);
	TEST_ASSERT(myVect.m_vectorType == 1);
	TEST_ASSERT(myVect.m_size == 5);
	TEST_ASSERT(myVect.m_sparsityThreshold == 2);
	TEST_ASSERT(myVect.m_nonZeros == 3);

	TEST_ASSERT(myVect.at(0) == 4);
	TEST_ASSERT(myVect.at(1) == 0);
	TEST_ASSERT(myVect.at(2) == 3);
	TEST_ASSERT(myVect.at(3) == 9);
	TEST_ASSERT(myVect.at(4) == 0);

	myVect.set(2, 0);
	myVect.set(3, 0);

	//Should switch back to sparse vector

	TEST_ASSERT(myVect.m_dimension == 5);
	TEST_ASSERT(myVect.m_capacity == ELBOWROOM + 1);
	TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - 1);
	TEST_ASSERT(myVect.m_sorted == true);
	TEST_ASSERT(myVect.m_vectorType == 0);
	TEST_ASSERT(myVect.m_size == 1);
	TEST_ASSERT(myVect.m_sparsityThreshold == 2);
	TEST_ASSERT(myVect.m_nonZeros == 1);

	TEST_ASSERT(myVect.at(0) == 4);
	TEST_ASSERT(myVect.at(1) == 0);
	TEST_ASSERT(myVect.at(2) == 0);
	TEST_ASSERT(myVect.at(3) == 0);
	TEST_ASSERT(myVect.at(4) == 0);
}

void VectorTestSuite::change()
{
	Vector myVect(5);

	myVect.set(0, 4);
	myVect.set(2, 3);
	myVect.set(3, 9);
	myVect.change(3, 4);
	myVect.change(0, 0);

	TEST_ASSERT(myVect.m_dimension == 5);
	TEST_ASSERT(myVect.m_capacity == ELBOWROOM + 5);
	TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - 5);
	TEST_ASSERT(myVect.m_sorted == false);
	TEST_ASSERT(myVect.m_vectorType == 1);
	TEST_ASSERT(myVect.m_size == 5);
	TEST_ASSERT(myVect.m_sparsityThreshold == 2);
	TEST_ASSERT(myVect.m_nonZeros == 2);

	TEST_ASSERT(myVect.at(0) == 0);
	TEST_ASSERT(myVect.at(1) == 0);
	TEST_ASSERT(myVect.at(2) == 3);
	TEST_ASSERT(myVect.at(3) == 4);
	TEST_ASSERT(myVect.at(4) == 0);

	myVect.change(2, 0);

	//Should switch to sparse vector

	TEST_ASSERT(myVect.m_dimension == 5);
	TEST_ASSERT(myVect.m_capacity == ELBOWROOM + 1);
	TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - 1);
	TEST_ASSERT(myVect.m_sorted == true);
	TEST_ASSERT(myVect.m_vectorType == 0);
	TEST_ASSERT(myVect.m_size == 1);
	TEST_ASSERT(myVect.m_sparsityThreshold == 2);
	TEST_ASSERT(myVect.m_nonZeros == 1);

	TEST_ASSERT(myVect.at(0) == 0);
	TEST_ASSERT(myVect.at(1) == 0);
	TEST_ASSERT(myVect.at(2) == 0);
	TEST_ASSERT(myVect.at(3) == 4);
	TEST_ASSERT(myVect.at(4) == 0);
}

void VectorTestSuite::setNewNonzero()
{
	Vector myVect(5);

	myVect.setNewNonzero(0, 4);

	TEST_ASSERT(myVect.m_dimension == 5);
	TEST_ASSERT(myVect.m_capacity == ELBOWROOM);
	TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - 1);
	TEST_ASSERT(myVect.m_sorted == false);
	TEST_ASSERT(myVect.m_vectorType == 0);
	TEST_ASSERT(myVect.m_size == 1);
	TEST_ASSERT(myVect.m_sparsityThreshold == 2);
	TEST_ASSERT(myVect.m_nonZeros == 1);

	TEST_ASSERT(myVect.at(0) == 4);
	TEST_ASSERT(myVect.at(1) == 0);
	TEST_ASSERT(myVect.at(2) == 0);
	TEST_ASSERT(myVect.at(3) == 0);
	TEST_ASSERT(myVect.at(4) == 0);

	myVect.setNewNonzero(2, 3);
	myVect.setNewNonzero(3, 9);

	//Should switch to dense vector

	TEST_ASSERT(myVect.m_dimension == 5);
	TEST_ASSERT(myVect.m_capacity == ELBOWROOM + 5);
	TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - 5);
	TEST_ASSERT(myVect.m_sorted == false);
	TEST_ASSERT(myVect.m_vectorType == 1);
	TEST_ASSERT(myVect.m_size == 5);
	TEST_ASSERT(myVect.m_sparsityThreshold == 2);
	TEST_ASSERT(myVect.m_nonZeros == 3);

	TEST_ASSERT(myVect.at(0) == 4);
	TEST_ASSERT(myVect.at(1) == 0);
	TEST_ASSERT(myVect.at(2) == 3);
	TEST_ASSERT(myVect.at(3) == 9);
	TEST_ASSERT(myVect.at(4) == 0);
}

void VectorTestSuite::insertElement()
{
	Vector myVect(5);

	myVect.set(1, 4);
	myVect.insertElement(1, 0);

	TEST_ASSERT(myVect.m_dimension == 6);
	TEST_ASSERT(myVect.m_capacity == ELBOWROOM);
	TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - 1);
	TEST_ASSERT(myVect.m_sorted == false);
	TEST_ASSERT(myVect.m_vectorType == 0);
	TEST_ASSERT(myVect.m_size == 1);
	TEST_ASSERT(myVect.m_sparsityThreshold == 2);
	TEST_ASSERT(myVect.m_nonZeros == 1);

	TEST_ASSERT(myVect.at(0) == 0);
	TEST_ASSERT(myVect.at(1) == 0);
	TEST_ASSERT(myVect.at(2) == 4);
	TEST_ASSERT(myVect.at(3) == 0);
	TEST_ASSERT(myVect.at(4) == 0);
	TEST_ASSERT(myVect.at(5) == 0);

	myVect.insertElement(1, 9);
	myVect.insertElement(4, 5);
	myVect.insertElement(7, 1);

	//Should switch to dense vector

	TEST_ASSERT(myVect.m_dimension == 9);
	TEST_ASSERT(myVect.m_capacity == ELBOWROOM + 9);
	TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - 9);
	TEST_ASSERT(myVect.m_sorted == false);
	TEST_ASSERT(myVect.m_vectorType == 1);
	TEST_ASSERT(myVect.m_size == 9);
	TEST_ASSERT(myVect.m_sparsityThreshold == 3);
	TEST_ASSERT(myVect.m_nonZeros == 4);

	TEST_ASSERT(myVect.at(0) == 0);
	TEST_ASSERT(myVect.at(1) == 9);
	TEST_ASSERT(myVect.at(2) == 0);
	TEST_ASSERT(myVect.at(3) == 4);
	TEST_ASSERT(myVect.at(4) == 5);
	TEST_ASSERT(myVect.at(5) == 0);
	TEST_ASSERT(myVect.at(6) == 0);
	TEST_ASSERT(myVect.at(7) == 1);
	TEST_ASSERT(myVect.at(8) == 0);

	myVect.insertElement(2, 0);
	myVect.insertElement(8, 0);
	myVect.insertElement(8, 0);
	myVect.insertElement(0, 0);

	//Should switch back to sparse vector

	TEST_ASSERT(myVect.m_dimension == 13);
	TEST_ASSERT(myVect.m_capacity == ELBOWROOM + 4);
	TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - 4);
	TEST_ASSERT(myVect.m_sorted == true);
	TEST_ASSERT(myVect.m_vectorType == 0);
	TEST_ASSERT(myVect.m_size == 4);
	TEST_ASSERT(myVect.m_sparsityThreshold == 5);
	TEST_ASSERT(myVect.m_nonZeros == 4);

	TEST_ASSERT(myVect.at(0) == 0);
	TEST_ASSERT(myVect.at(1) == 0);
	TEST_ASSERT(myVect.at(2) == 9);
	TEST_ASSERT(myVect.at(3) == 0);
	TEST_ASSERT(myVect.at(4) == 0);
	TEST_ASSERT(myVect.at(5) == 4);
	TEST_ASSERT(myVect.at(6) == 5);
	TEST_ASSERT(myVect.at(7) == 0);
	TEST_ASSERT(myVect.at(8) == 0);
	TEST_ASSERT(myVect.at(9) == 0);
	TEST_ASSERT(myVect.at(10) == 0);
	TEST_ASSERT(myVect.at(11) == 1);
	TEST_ASSERT(myVect.at(12) == 0);
}

void VectorTestSuite::append()
{
	Vector myVect(5);

	myVect.set(1, 4);
	myVect.append(0);

	TEST_ASSERT(myVect.m_dimension == 6);
	TEST_ASSERT(myVect.m_capacity == ELBOWROOM);
	TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - 1);
	TEST_ASSERT(myVect.m_sorted == false);
	TEST_ASSERT(myVect.m_vectorType == 0);
	TEST_ASSERT(myVect.m_size == 1);
	TEST_ASSERT(myVect.m_sparsityThreshold == 2);
	TEST_ASSERT(myVect.m_nonZeros == 1);

	TEST_ASSERT(myVect.at(0) == 0);
	TEST_ASSERT(myVect.at(1) == 4);
	TEST_ASSERT(myVect.at(2) == 0);
	TEST_ASSERT(myVect.at(3) == 0);
	TEST_ASSERT(myVect.at(4) == 0);
	TEST_ASSERT(myVect.at(5) == 0);

	myVect.append(5);
	myVect.append(4);
	myVect.append(2);

	//Should switch to dense vector

	TEST_ASSERT(myVect.m_dimension == 9);
	TEST_ASSERT(myVect.m_capacity == ELBOWROOM + 9);
	TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - 9);
	TEST_ASSERT(myVect.m_sorted == false);
	TEST_ASSERT(myVect.m_vectorType == 1);
	TEST_ASSERT(myVect.m_size == 9);
	TEST_ASSERT(myVect.m_sparsityThreshold == 3);
	TEST_ASSERT(myVect.m_nonZeros == 4);

	TEST_ASSERT(myVect.at(0) == 0);
	TEST_ASSERT(myVect.at(1) == 4);
	TEST_ASSERT(myVect.at(2) == 0);
	TEST_ASSERT(myVect.at(3) == 0);
	TEST_ASSERT(myVect.at(4) == 0);
	TEST_ASSERT(myVect.at(5) == 0);
	TEST_ASSERT(myVect.at(6) == 5);
	TEST_ASSERT(myVect.at(7) == 4);
	TEST_ASSERT(myVect.at(8) == 2);

	myVect.append(0);
	myVect.append(0);
	myVect.append(0);
	myVect.append(0);

	//Should switch back to sparse

	TEST_ASSERT(myVect.m_dimension == 13);
	TEST_ASSERT(myVect.m_capacity == ELBOWROOM + 4);
	TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - 4);
	TEST_ASSERT(myVect.m_sorted == true);
	TEST_ASSERT(myVect.m_vectorType == 0);
	TEST_ASSERT(myVect.m_size == 4);
	TEST_ASSERT(myVect.m_sparsityThreshold == 5);
	TEST_ASSERT(myVect.m_nonZeros == 4);

	TEST_ASSERT(myVect.at(0) == 0);
	TEST_ASSERT(myVect.at(1) == 4);
	TEST_ASSERT(myVect.at(2) == 0);
	TEST_ASSERT(myVect.at(3) == 0);
	TEST_ASSERT(myVect.at(4) == 0);
	TEST_ASSERT(myVect.at(5) == 0);
	TEST_ASSERT(myVect.at(6) == 5);
	TEST_ASSERT(myVect.at(7) == 4);
	TEST_ASSERT(myVect.at(8) == 2);
	TEST_ASSERT(myVect.at(9) == 0);
	TEST_ASSERT(myVect.at(10) == 0);
	TEST_ASSERT(myVect.at(11) == 0);
	TEST_ASSERT(myVect.at(12) == 0);
}

void VectorTestSuite::removeElement()
{
	Vector myVect(14);

	myVect.set(2, 9);
	myVect.set(5, 4);
	myVect.set(6, 5);
	myVect.set(11, 1);

	//Vector: [0, 0, 9, 0, 0, 4, 5, 0, 0, 0, 0, 1, 0, 0]

	myVect.removeElement(3);

	TEST_ASSERT(myVect.m_dimension == 13);
	TEST_ASSERT(myVect.m_capacity == ELBOWROOM);
	TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - 4);
	TEST_ASSERT(myVect.m_sorted == false);
	TEST_ASSERT(myVect.m_vectorType == 0);
	TEST_ASSERT(myVect.m_size == 4);
	TEST_ASSERT(myVect.m_sparsityThreshold == 5);
	TEST_ASSERT(myVect.m_nonZeros == 4);

	TEST_ASSERT(myVect.at(0) == 0);
	TEST_ASSERT(myVect.at(1) == 0);
	TEST_ASSERT(myVect.at(2) == 9);
	TEST_ASSERT(myVect.at(3) == 0);
	TEST_ASSERT(myVect.at(4) == 4);
	TEST_ASSERT(myVect.at(5) == 5);
	TEST_ASSERT(myVect.at(6) == 0);
	TEST_ASSERT(myVect.at(7) == 0);
	TEST_ASSERT(myVect.at(8) == 0);
	TEST_ASSERT(myVect.at(9) == 0);
	TEST_ASSERT(myVect.at(10) == 1);
	TEST_ASSERT(myVect.at(11) == 0);
	TEST_ASSERT(myVect.at(12) == 0);

	myVect.removeElement(11);
	myVect.removeElement(7);
	myVect.removeElement(7);
	myVect.removeElement(0);

	//Should switch to dense

	TEST_ASSERT(myVect.m_dimension == 9);
	TEST_ASSERT(myVect.m_capacity == ELBOWROOM + 9);
	TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - 9);
	TEST_ASSERT(myVect.m_sorted == false);
	TEST_ASSERT(myVect.m_vectorType == 1);
	TEST_ASSERT(myVect.m_size == 9);
	TEST_ASSERT(myVect.m_sparsityThreshold == 3);
	TEST_ASSERT(myVect.m_nonZeros == 4);

	TEST_ASSERT(myVect.at(0) == 0);
	TEST_ASSERT(myVect.at(1) == 9);
	TEST_ASSERT(myVect.at(2) == 0);
	TEST_ASSERT(myVect.at(3) == 4);
	TEST_ASSERT(myVect.at(4) == 5);
	TEST_ASSERT(myVect.at(5) == 0);
	TEST_ASSERT(myVect.at(6) == 0);
	TEST_ASSERT(myVect.at(7) == 1);
	TEST_ASSERT(myVect.at(8) == 0);

	myVect.removeElement(3);
	myVect.removeElement(6);
	myVect.removeElement(3);

	//Should switch back to sparse

	TEST_ASSERT(myVect.m_dimension == 6);
	TEST_ASSERT(myVect.m_capacity == ELBOWROOM + 1);
	TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - 1);
	TEST_ASSERT(myVect.m_sorted == true);
	TEST_ASSERT(myVect.m_vectorType == 0);
	TEST_ASSERT(myVect.m_size == 1);
	TEST_ASSERT(myVect.m_sparsityThreshold == 2);
	TEST_ASSERT(myVect.m_nonZeros == 1);

	TEST_ASSERT(myVect.at(0) == 0);
	TEST_ASSERT(myVect.at(1) == 9);
	TEST_ASSERT(myVect.at(2) == 0);
	TEST_ASSERT(myVect.at(3) == 0);
	TEST_ASSERT(myVect.at(4) == 0);
	TEST_ASSERT(myVect.at(5) == 0);
}

void VectorTestSuite::resize()
{
	Vector myVect(8);
	myVect.set(1, 3);
	myVect.set(0, 7);
	myVect.resize(9);

	TEST_ASSERT(myVect.m_dimension == 9);
	TEST_ASSERT(myVect.m_capacity == ELBOWROOM);
	TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - 2);
	TEST_ASSERT(myVect.m_sorted == false);
	TEST_ASSERT(myVect.m_vectorType == 0);
	TEST_ASSERT(myVect.m_size == 2);
	TEST_ASSERT(myVect.m_sparsityThreshold == 3);
	TEST_ASSERT(myVect.m_nonZeros == 2);

	TEST_ASSERT(myVect.at(0) == 7);
	TEST_ASSERT(myVect.at(1) == 3);
	TEST_ASSERT(myVect.at(2) == 0);
	TEST_ASSERT(myVect.at(3) == 0);
	TEST_ASSERT(myVect.at(4) == 0);
	TEST_ASSERT(myVect.at(5) == 0);
	TEST_ASSERT(myVect.at(6) == 0);
	TEST_ASSERT(myVect.at(7) == 0);
	TEST_ASSERT(myVect.at(8) == 0);

	myVect.resize(2);

	//Should switch to dense

	TEST_ASSERT(myVect.m_dimension == 2);
	TEST_ASSERT(myVect.m_capacity == ELBOWROOM + 2);
	TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - 2);
	TEST_ASSERT(myVect.m_sorted == false);
	TEST_ASSERT(myVect.m_vectorType == 1);
	TEST_ASSERT(myVect.m_size == 2);
	TEST_ASSERT(myVect.m_sparsityThreshold == 1);
	TEST_ASSERT(myVect.m_nonZeros == 2);

	TEST_ASSERT(myVect.at(0) == 7);
	TEST_ASSERT(myVect.at(1) == 3);

	myVect.resize(9);

	//Should switch back to sparse

	TEST_ASSERT(myVect.m_dimension == 9);
	TEST_ASSERT(myVect.m_capacity == ELBOWROOM + 2);
	TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - 2);
	TEST_ASSERT(myVect.m_sorted == true);
	TEST_ASSERT(myVect.m_vectorType == 0);
	TEST_ASSERT(myVect.m_size == 2);
	TEST_ASSERT(myVect.m_sparsityThreshold == 3);
	TEST_ASSERT(myVect.m_nonZeros == 2);

	TEST_ASSERT(myVect.at(0) == 7);
	TEST_ASSERT(myVect.at(1) == 3);
	TEST_ASSERT(myVect.at(2) == 0);
	TEST_ASSERT(myVect.at(3) == 0);
	TEST_ASSERT(myVect.at(4) == 0);
	TEST_ASSERT(myVect.at(5) == 0);
	TEST_ASSERT(myVect.at(6) == 0);
	TEST_ASSERT(myVect.at(7) == 0);
	TEST_ASSERT(myVect.at(8) == 0);
}

void VectorTestSuite::setSparsityRatio() {
	Vector myVect(10);
	myVect.set(3, 8);
	myVect.set(7, -2);
	myVect.set(5, 6.76);
	myVect.setSparsityRatio(0.55);

	TEST_ASSERT(myVect.m_dimension == 10);
	TEST_ASSERT(myVect.m_capacity == ELBOWROOM);
	TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - 3);
	TEST_ASSERT(myVect.m_sorted == false);
	TEST_ASSERT(myVect.m_vectorType == 0);
	TEST_ASSERT(myVect.m_size == 3);
	TEST_ASSERT(myVect.m_sparsityThreshold == 6);
	TEST_ASSERT(myVect.m_nonZeros == 3);

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

	myVect.setSparsityRatio(0.20);

	//Should switch to dense

	TEST_ASSERT(myVect.m_dimension == 10);
	TEST_ASSERT(myVect.m_capacity == ELBOWROOM + 10);
	TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - 10);
	TEST_ASSERT(myVect.m_sorted == false);
	TEST_ASSERT(myVect.m_vectorType == 1);
	TEST_ASSERT(myVect.m_size == 10);
	TEST_ASSERT(myVect.m_sparsityThreshold == 2);
	TEST_ASSERT(myVect.m_nonZeros == 3);

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

	myVect.setSparsityRatio(0.94);

	//Should switch back to sparse

	TEST_ASSERT(myVect.m_dimension == 10);
	TEST_ASSERT(myVect.m_capacity == ELBOWROOM + 3);
	TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - 3);
	TEST_ASSERT(myVect.m_sorted == true);
	TEST_ASSERT(myVect.m_vectorType == 0);
	TEST_ASSERT(myVect.m_size == 3);
	TEST_ASSERT(myVect.m_sparsityThreshold == 9);
	TEST_ASSERT(myVect.m_nonZeros == 3);

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
}

void VectorTestSuite::maxIndex() {
	Vector myVect(5);
	myVect.set(3, 99);

	//In case of sparse vector

	TEST_ASSERT(myVect.maxIndex() == 3);

	myVect.set(2, 5);
	myVect.set(0, -7);

	//In case of dense vector

	TEST_ASSERT(myVect.maxIndex() == 4);
}

void VectorTestSuite::absMaxElement() {
	Vector myVect(8);

	TEST_ASSERT(myVect.absMaxElement() == 0);

	myVect.set(2, 6);

	TEST_ASSERT(myVect.absMaxElement() == 6);

	myVect.set(0, -11.7);

	TEST_ASSERT(myVect.absMaxElement() == 11.7);

	//In case of dense vector

	myVect.set(6, 5);
	myVect.set(5, -3);

	TEST_ASSERT(myVect.absMaxElement() == 11.7);
}

void VectorTestSuite::scaleBy() {
	Vector myVect(5);
	myVect.set(1, 7);

	myVect.scaleBy(-1);

	TEST_ASSERT(myVect.m_dimension == 5);
	TEST_ASSERT(myVect.m_capacity == ELBOWROOM);
	TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - 1);
	TEST_ASSERT(myVect.m_sorted == false);
	TEST_ASSERT(myVect.m_vectorType == 0);
	TEST_ASSERT(myVect.m_size == 1);
	TEST_ASSERT(myVect.m_sparsityThreshold == 2);
	TEST_ASSERT(myVect.m_nonZeros == 1);

	TEST_ASSERT(myVect.at(0) == 0);
	TEST_ASSERT(myVect.at(1) == -7);
	TEST_ASSERT(myVect.at(2) == 0);
	TEST_ASSERT(myVect.at(3) == 0);
	TEST_ASSERT(myVect.at(4) == 0);

	myVect.set(3, 5);
	myVect.set(4, 2);

	myVect.scaleBy(-4);

	TEST_ASSERT(myVect.m_dimension == 5);
	TEST_ASSERT(myVect.m_capacity == ELBOWROOM + 5);
	TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - 5);
	TEST_ASSERT(myVect.m_sorted == false);
	TEST_ASSERT(myVect.m_vectorType == 1);
	TEST_ASSERT(myVect.m_size == 5);
	TEST_ASSERT(myVect.m_sparsityThreshold == 2);
	TEST_ASSERT(myVect.m_nonZeros == 3);

	TEST_ASSERT(myVect.at(0) == 0);
	TEST_ASSERT(myVect.at(1) == 28);
	TEST_ASSERT(myVect.at(2) == 0);
	TEST_ASSERT(myVect.at(3) == -20);
	TEST_ASSERT(myVect.at(4) == -8);

	myVect.scaleBy(0);

	//Should switch to sparse

	TEST_ASSERT(myVect.m_dimension == 5);
	TEST_ASSERT(myVect.m_capacity == ELBOWROOM);
	TEST_ASSERT(myVect.m_data == myVect.m_dataEnd);
	TEST_ASSERT(myVect.m_sorted == false);
	TEST_ASSERT(myVect.m_vectorType == 0);
	TEST_ASSERT(myVect.m_size == 0);
	TEST_ASSERT(myVect.m_sparsityThreshold == 2);
	TEST_ASSERT(myVect.m_nonZeros == 0);

	TEST_ASSERT(myVect.at(0) == 0);
	TEST_ASSERT(myVect.at(1) == 0);
	TEST_ASSERT(myVect.at(2) == 0);
	TEST_ASSERT(myVect.at(3) == 0);
	TEST_ASSERT(myVect.at(4) == 0);
}

void VectorTestSuite::scaleElementBy() {
	Vector myVect(7);
	myVect.set(3, -3);
	myVect.scaleElementBy(3, -2);

	TEST_ASSERT(myVect.at(0) == 0);
	TEST_ASSERT(myVect.at(1) == 0);
	TEST_ASSERT(myVect.at(2) == 0);
	TEST_ASSERT(myVect.at(3) == 6);
	TEST_ASSERT(myVect.at(4) == 0);
	TEST_ASSERT(myVect.at(5) == 0);
	TEST_ASSERT(myVect.at(6) == 0);

	myVect.set(2, 4);
	myVect.set(5, -6);
	myVect.set(6, 6);
	myVect.scaleElementBy(3, -2);

	TEST_ASSERT(myVect.at(0) == 0);
	TEST_ASSERT(myVect.at(1) == 0);
	TEST_ASSERT(myVect.at(2) == 4);
	TEST_ASSERT(myVect.at(3) == -12);
	TEST_ASSERT(myVect.at(4) == 0);
	TEST_ASSERT(myVect.at(5) == -6);
	TEST_ASSERT(myVect.at(6) == 6);

	myVect.scaleElementBy(2, 0);
	myVect.scaleElementBy(5, 0);
	myVect.scaleElementBy(6, 0);

	//Should switch to sparse

	TEST_ASSERT(myVect.m_dimension == 7);
	TEST_ASSERT(myVect.m_capacity == ELBOWROOM + 1);
	TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - 1);
	TEST_ASSERT(myVect.m_sorted == true);
	TEST_ASSERT(myVect.m_vectorType == 0);
	TEST_ASSERT(myVect.m_size == 1);
	TEST_ASSERT(myVect.m_sparsityThreshold == 2);
	TEST_ASSERT(myVect.m_nonZeros == 1);

	TEST_ASSERT(myVect.at(0) == 0);
	TEST_ASSERT(myVect.at(1) == 0);
	TEST_ASSERT(myVect.at(2) == 0);
	TEST_ASSERT(myVect.at(3) == -12);
	TEST_ASSERT(myVect.at(4) == 0);
	TEST_ASSERT(myVect.at(5) == 0);
	TEST_ASSERT(myVect.at(6) == 0);
}

void VectorTestSuite::scaleByLambdas() {
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

	myVect.scaleByLambdas(myLambdas);

	TEST_ASSERT(myVect.at(0) == -9);
	TEST_ASSERT(myVect.at(1) == -8);
	TEST_ASSERT(myVect.at(2) == 0);
	TEST_ASSERT(myVect.at(3) == 0);
	TEST_ASSERT(myVect.at(4) == 4);
	TEST_ASSERT(myVect.at(5) == 20);

	myLambdas[0] = 0;
	myLambdas[1] = 0;
	myLambdas[2] = 3;
	myLambdas[3] = -11;
	myLambdas[4] = 0;
	myLambdas[5] = 0;

	myVect.scaleByLambdas(myLambdas);

	//Should switch to sparse

	TEST_ASSERT(myVect.m_dimension == 6);
	TEST_ASSERT(myVect.m_capacity == ELBOWROOM);
	TEST_ASSERT(myVect.m_data == myVect.m_dataEnd);
	TEST_ASSERT(myVect.m_sorted == true);
	TEST_ASSERT(myVect.m_vectorType == 0);
	TEST_ASSERT(myVect.m_size == 0);
	TEST_ASSERT(myVect.m_sparsityThreshold == 2);
	TEST_ASSERT(myVect.m_nonZeros == 0);

	TEST_ASSERT(myVect.at(0) == 0);
	TEST_ASSERT(myVect.at(1) == 0);
	TEST_ASSERT(myVect.at(2) == 0);
	TEST_ASSERT(myVect.at(3) == 0);
	TEST_ASSERT(myVect.at(4) == 0);
	TEST_ASSERT(myVect.at(5) == 0);
}

void VectorTestSuite::addVector() {
	//Add dense to dense

	Numerical::Double myLambda = 2;

	Vector myVect(5);
	myVect.set(0, 4);
	myVect.set(2, 2);
	myVect.set(3, 5);

	Vector myVectAdd(5);
	myVectAdd.set(1, 1);
	myVectAdd.set(2, -2);
	myVectAdd.set(3, 2);

	//myVect: [4, 0, 2, 5, 0]
	//myVectAdd: [0, 1, -2, 2, 0]
	//myLambda * myVectAdd: [0, 2, -4, 4, 0]
	//Sum: [4, 2, -2, 9, 0]

	myVect.addVector(myLambda, myVectAdd);

	TEST_ASSERT(myVect.m_dimension == 5);
	TEST_ASSERT(myVect.m_capacity == ELBOWROOM + 5);
	TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - 5);
	TEST_ASSERT(myVect.m_sorted == false);
	TEST_ASSERT(myVect.m_vectorType == 1);
	TEST_ASSERT(myVect.m_size == 5);
	TEST_ASSERT(myVect.m_sparsityThreshold == 2);
	TEST_ASSERT(myVect.m_nonZeros == 4);

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

	myVect.addVector(myLambda, myVectAdd);

	TEST_ASSERT(myVect.m_dimension == 5);
	TEST_ASSERT(myVect.m_capacity == ELBOWROOM + 1);
	TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - 1);
	TEST_ASSERT(myVect.m_sorted == true);
	TEST_ASSERT(myVect.m_vectorType == 0);
	TEST_ASSERT(myVect.m_size == 1);
	TEST_ASSERT(myVect.m_sparsityThreshold == 2);
	TEST_ASSERT(myVect.m_nonZeros == 1);

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

	myVect.addVector(myLambda, myVectAdd);

	TEST_ASSERT(myVect.m_dimension == 5);
	TEST_ASSERT(myVect.m_capacity == ELBOWROOM + 1);
	TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - 1);
	TEST_ASSERT(myVect.m_sorted == true);
	TEST_ASSERT(myVect.m_vectorType == 0);
	TEST_ASSERT(myVect.m_size == 1);
	TEST_ASSERT(myVect.m_sparsityThreshold == 2);
	TEST_ASSERT(myVect.m_nonZeros == 1);

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

	//myVect: [0, 0, 0, -9, 0]
	//myVectAdd: [3, 0, 0, 3, 3]
	//myLambda * myVectAdd: [3, 0, 0, 3, 3]
	//Sum: [3, 0, 0, -6, 3]

	myVect.addVector(myLambda, myVectAdd);

	TEST_ASSERT(myVect.m_dimension == 5);
	TEST_ASSERT(myVect.m_capacity == ELBOWROOM + 5);
	TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - 5);
	TEST_ASSERT(myVect.m_sorted == false);
	TEST_ASSERT(myVect.m_vectorType == 1);
	TEST_ASSERT(myVect.m_size == 5);
	TEST_ASSERT(myVect.m_sparsityThreshold == 2);
	TEST_ASSERT(myVect.m_nonZeros == 3);

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

	myVect.addVector(myLambda, myVectAdd);

	TEST_ASSERT(myVect.m_dimension == 5);
	TEST_ASSERT(myVect.m_capacity == ELBOWROOM + 5);
	TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - 5);
	TEST_ASSERT(myVect.m_sorted == false);
	TEST_ASSERT(myVect.m_vectorType == 1);
	TEST_ASSERT(myVect.m_size == 5);
	TEST_ASSERT(myVect.m_sparsityThreshold == 2);
	TEST_ASSERT(myVect.m_nonZeros == 2);

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

	myVect.addVector(myLambda, myVectAdd);

	TEST_ASSERT(myVect.m_dimension == 5);
	TEST_ASSERT(myVect.m_capacity == ELBOWROOM + 1);
	TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - 1);
	TEST_ASSERT(myVect.m_sorted == true);
	TEST_ASSERT(myVect.m_vectorType == 0);
	TEST_ASSERT(myVect.m_size == 1);
	TEST_ASSERT(myVect.m_sparsityThreshold == 2);
	TEST_ASSERT(myVect.m_nonZeros == 1);

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

	myVect.addVector(myLambda, myVectAdd);

	TEST_ASSERT(myVect.m_dimension == 5);
	TEST_ASSERT(myVect.m_capacity == ELBOWROOM + 1);
	TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - 1);
	TEST_ASSERT(myVect.m_sorted == true);
	TEST_ASSERT(myVect.m_vectorType == 0);
	TEST_ASSERT(myVect.m_size == 1);
	TEST_ASSERT(myVect.m_sparsityThreshold == 2);
	TEST_ASSERT(myVect.m_nonZeros == 1);

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

	myVect.addVector(myLambda, myVectAdd);

	TEST_ASSERT(myVect.m_dimension == 5);
	TEST_ASSERT(myVect.m_capacity == ELBOWROOM + 5);
	TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - 5);
	TEST_ASSERT(myVect.m_sorted == false);
	TEST_ASSERT(myVect.m_vectorType == 1);
	TEST_ASSERT(myVect.m_size == 5);
	TEST_ASSERT(myVect.m_sparsityThreshold == 2);
	TEST_ASSERT(myVect.m_nonZeros == 2);

	TEST_ASSERT(myVect.at(0) == 3);
	TEST_ASSERT(myVect.at(1) == 4);
	TEST_ASSERT(myVect.at(2) == 0);
	TEST_ASSERT(myVect.at(3) == 0);
	TEST_ASSERT(myVect.at(4) == 0);
}

void VectorTestSuite::dotProduct() {

	//Two sparse vectors

	Vector myVect(5);
	myVect.set(3, 6);
	Vector mySecondVect(myVect);

	//myVect: [0, 0, 0, 6, 0]
	//mySecondVect: [0, 0, 0, 6, 0]
	//Dot product: 6 * 6 = 36

	TEST_ASSERT(myVect.dotProduct(mySecondVect) == 36);

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

	//Different vector types

	mySecondVect.set(0, 0);
	mySecondVect.set(3, 0);
	mySecondVect.set(4, 0);

	//myVect: [0, 2, 5, 6, 7]
	//mySecondVect: [0, 5, 0, 0, 0]
	//Dot product: 2 * 5 = 10

	TEST_ASSERT(myVect.dotProduct(mySecondVect) == 10);
}

void VectorTestSuite::euclidNorm() {

	//Case of sparse vector

	Vector myVect(5);
	myVect.set(1, 4);

	TEST_ASSERT(myVect.euclidNorm() == 4);

	//Case of dense vector

	myVect.set(3, 3);
	myVect.set(4, 12);

	//myVect: [0, 4, 0, 3, 12]
	//16 (4^2) + 9 (3^2) + 144 (12^2) = 169 (13^2)

	TEST_ASSERT(myVect.euclidNorm() == 13);
}
void VectorTestSuite::elementaryFtran() {

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

	myVect.elementaryFtran(myEta, myPivot);

	TEST_ASSERT(myVect.m_dimension == 5);
	TEST_ASSERT(myVect.m_capacity == ELBOWROOM + 5);
	TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - 5);
	TEST_ASSERT(myVect.m_sorted == false);
	TEST_ASSERT(myVect.m_vectorType == 1);
	TEST_ASSERT(myVect.m_size == 5);
	TEST_ASSERT(myVect.m_sparsityThreshold == 2);
	TEST_ASSERT(myVect.m_nonZeros == 5);

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

	TEST_ASSERT(myVect.m_dimension == 5);
	TEST_ASSERT(myVect.m_capacity == ELBOWROOM + 1);
	TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - 1);
	TEST_ASSERT(myVect.m_sorted == true);
	TEST_ASSERT(myVect.m_vectorType == 0);
	TEST_ASSERT(myVect.m_size == 1);
	TEST_ASSERT(myVect.m_sparsityThreshold == 2);
	TEST_ASSERT(myVect.m_nonZeros == 1);

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

	TEST_ASSERT(myVect.m_dimension == 5);
	TEST_ASSERT(myVect.m_capacity == ELBOWROOM + 5);
	TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - 5);
	TEST_ASSERT(myVect.m_sorted == false);
	TEST_ASSERT(myVect.m_vectorType == 1);
	TEST_ASSERT(myVect.m_size == 5);
	TEST_ASSERT(myVect.m_sparsityThreshold == 2);
	TEST_ASSERT(myVect.m_nonZeros == 5);

	TEST_ASSERT(myVect.at(0) == 6);
	TEST_ASSERT(myVect.at(1) == -15);
	TEST_ASSERT(myVect.at(2) == 3);
	TEST_ASSERT(myVect.at(3) == 0);
	TEST_ASSERT(myVect.at(4) == 3);
}

void VectorTestSuite::elementaryBtran() {

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

	TEST_ASSERT(myVect.m_dimension == 5);
	TEST_ASSERT(myVect.m_capacity == ELBOWROOM + 5);
	TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - 5);
	TEST_ASSERT(myVect.m_sorted == false);
	TEST_ASSERT(myVect.m_vectorType == 1);
	TEST_ASSERT(myVect.m_size == 5);
	TEST_ASSERT(myVect.m_sparsityThreshold == 2);
	TEST_ASSERT(myVect.m_nonZeros == 2);

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

	TEST_ASSERT(myVect.m_dimension == 5);
	TEST_ASSERT(myVect.m_capacity == ELBOWROOM + 1);
	TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - 1);
	TEST_ASSERT(myVect.m_sorted == true);
	TEST_ASSERT(myVect.m_vectorType == 0);
	TEST_ASSERT(myVect.m_size == 1);
	TEST_ASSERT(myVect.m_sparsityThreshold == 2);
	TEST_ASSERT(myVect.m_nonZeros == 1);

	TEST_ASSERT(myVect.at(0) == 0);
	TEST_ASSERT(myVect.at(1) == 0);
	TEST_ASSERT(myVect.at(2) == 27);
	TEST_ASSERT(myVect.at(3) == 0);
	TEST_ASSERT(myVect.at(4) == 0);
}

void VectorTestSuite::plus() {

	//Add dense to dense

	Vector myVect(5);
	myVect.set(0, 4);
	myVect.set(2, 2);
	myVect.set(3, 5);

	Vector mySecondVect(5);
	mySecondVect.set(1, 2);
	mySecondVect.set(2, -4);
	mySecondVect.set(3, 4);

	//myVect: [4, 0, 2, 5, 0]
	//mySecondVect: [0, 2, -4, 4, 0]
	//Sum: [4, 2, -2, 9, 0]

	myVect = myVect + mySecondVect;

	TEST_ASSERT(myVect.m_dimension == 5);
	TEST_ASSERT(myVect.m_capacity == ELBOWROOM + 5);
	TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - 5);
	TEST_ASSERT(myVect.m_sorted == false);
	TEST_ASSERT(myVect.m_vectorType == 1);
	TEST_ASSERT(myVect.m_size == 5);
	TEST_ASSERT(myVect.m_sparsityThreshold == 2);
	TEST_ASSERT(myVect.m_nonZeros == 4);

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

	myVect = myVect + mySecondVect;

	TEST_ASSERT(myVect.m_dimension == 5);
	TEST_ASSERT(myVect.m_capacity == ELBOWROOM + 1);
	TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - 1);
	TEST_ASSERT(myVect.m_sorted == true);
	TEST_ASSERT(myVect.m_vectorType == 0);
	TEST_ASSERT(myVect.m_size == 1);
	TEST_ASSERT(myVect.m_sparsityThreshold == 2);
	TEST_ASSERT(myVect.m_nonZeros == 1);

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

	myVect = myVect + mySecondVect;

	TEST_ASSERT(myVect.m_dimension == 5);
	TEST_ASSERT(myVect.m_capacity == ELBOWROOM + 1);
	TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - 1);
	TEST_ASSERT(myVect.m_sorted == true);
	TEST_ASSERT(myVect.m_vectorType == 0);
	TEST_ASSERT(myVect.m_size == 1);
	TEST_ASSERT(myVect.m_sparsityThreshold == 2);
	TEST_ASSERT(myVect.m_nonZeros == 1);

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

	myVect = myVect + mySecondVect;

	TEST_ASSERT(myVect.m_dimension == 5);
	TEST_ASSERT(myVect.m_capacity == ELBOWROOM + 5);
	TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - 5);
	TEST_ASSERT(myVect.m_sorted == false);
	TEST_ASSERT(myVect.m_vectorType == 1);
	TEST_ASSERT(myVect.m_size == 5);
	TEST_ASSERT(myVect.m_sparsityThreshold == 2);
	TEST_ASSERT(myVect.m_nonZeros == 3);

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

	myVect = myVect + mySecondVect;

	TEST_ASSERT(myVect.m_dimension == 5);
	TEST_ASSERT(myVect.m_capacity == ELBOWROOM + 5);
	TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - 5);
	TEST_ASSERT(myVect.m_sorted == false);
	TEST_ASSERT(myVect.m_vectorType == 1);
	TEST_ASSERT(myVect.m_size == 5);
	TEST_ASSERT(myVect.m_sparsityThreshold == 2);
	TEST_ASSERT(myVect.m_nonZeros == 2);

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

	myVect = myVect + mySecondVect;

	TEST_ASSERT(myVect.m_dimension == 5);
	TEST_ASSERT(myVect.m_capacity == ELBOWROOM + 1);
	TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - 1);
	TEST_ASSERT(myVect.m_sorted == true);
	TEST_ASSERT(myVect.m_vectorType == 0);
	TEST_ASSERT(myVect.m_size == 1);
	TEST_ASSERT(myVect.m_sparsityThreshold == 2);
	TEST_ASSERT(myVect.m_nonZeros == 1);

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
	mySecondVect.set(4, 0);

	//myVect: [3, 0, 0, 0, 0]
	//mySecondVect: [0, 0, 0, 0, 0]
	//Sum: [3, 0, 0, 0, 0]

	myVect = myVect + mySecondVect;

	TEST_ASSERT(myVect.m_dimension == 5);
	TEST_ASSERT(myVect.m_capacity == ELBOWROOM + 1);
	TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - 1);
	TEST_ASSERT(myVect.m_sorted == true);
	TEST_ASSERT(myVect.m_vectorType == 0);
	TEST_ASSERT(myVect.m_size == 1);
	TEST_ASSERT(myVect.m_sparsityThreshold == 2);
	TEST_ASSERT(myVect.m_nonZeros == 1);

	TEST_ASSERT(myVect.at(0) == 3);
	TEST_ASSERT(myVect.at(1) == 0);
	TEST_ASSERT(myVect.at(2) == 0);
	TEST_ASSERT(myVect.at(3) == 0);
	TEST_ASSERT(myVect.at(4) == 0);

	//Should switch to dense

	mySecondVect.set(0, 0);
	mySecondVect.set(1, 4);
	mySecondVect.set(2, 0);
	mySecondVect.set(3, 0);
	mySecondVect.set(4, 0);

	//myVect: [3, 0, 0, 0, 0]
	//mySecondVect: [0, 4, 0, 0, 0]
	//Sum: [3, 4, 0, 0, 0]

	myVect = myVect + mySecondVect;

	TEST_ASSERT(myVect.m_dimension == 5);
	TEST_ASSERT(myVect.m_capacity == ELBOWROOM + 5);
	TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - 5);
	TEST_ASSERT(myVect.m_sorted == false);
	TEST_ASSERT(myVect.m_vectorType == 1);
	TEST_ASSERT(myVect.m_size == 5);
	TEST_ASSERT(myVect.m_sparsityThreshold == 2);
	TEST_ASSERT(myVect.m_nonZeros == 2);

	TEST_ASSERT(myVect.at(0) == 3);
	TEST_ASSERT(myVect.at(1) == 4);
	TEST_ASSERT(myVect.at(2) == 0);
	TEST_ASSERT(myVect.at(3) == 0);
	TEST_ASSERT(myVect.at(4) == 0);
}

void VectorTestSuite::minus() {

	//Subtract dense from dense

	Vector myVect(5);
	myVect.set(0, 4);
	myVect.set(2, 2);
	myVect.set(3, 5);

	Vector mySecondVect(5);
	mySecondVect.set(1, -2);
	mySecondVect.set(2, 4);
	mySecondVect.set(3, -4);

	//myVect: [4, 0, 2, 5, 0]
	//mySecondVect: [0, -2, 4, -4, 0]
	//Difference: [4, 2, -2, 9, 0]

	myVect = myVect - mySecondVect;

	TEST_ASSERT(myVect.m_dimension == 5);
	TEST_ASSERT(myVect.m_capacity == ELBOWROOM + 5);
	TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - 5);
	TEST_ASSERT(myVect.m_sorted == false);
	TEST_ASSERT(myVect.m_vectorType == 1);
	TEST_ASSERT(myVect.m_size == 5);
	TEST_ASSERT(myVect.m_sparsityThreshold == 2);
	TEST_ASSERT(myVect.m_nonZeros == 4);

	TEST_ASSERT(myVect.at(0) == 4);
	TEST_ASSERT(myVect.at(1) == 2);
	TEST_ASSERT(myVect.at(2) == -2);
	TEST_ASSERT(myVect.at(3) == 9);
	TEST_ASSERT(myVect.at(4) == 0);

	//Should switch to sparse

	mySecondVect.set(1, 2);
	mySecondVect.set(2, -2);
	mySecondVect.set(3, 9);

	//myVect: [4, 2, -2, 9, 0]
	//mySecondVect: [0, 2, -2, 9, 0]
	//Difference: [4, 0, 0, 0, 0]

	myVect = myVect - mySecondVect;

	TEST_ASSERT(myVect.m_dimension == 5);
	TEST_ASSERT(myVect.m_capacity == ELBOWROOM + 1);
	TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - 1);
	TEST_ASSERT(myVect.m_sorted == true);
	TEST_ASSERT(myVect.m_vectorType == 0);
	TEST_ASSERT(myVect.m_size == 1);
	TEST_ASSERT(myVect.m_sparsityThreshold == 2);
	TEST_ASSERT(myVect.m_nonZeros == 1);

	TEST_ASSERT(myVect.at(0) == 4);
	TEST_ASSERT(myVect.at(1) == 0);
	TEST_ASSERT(myVect.at(2) == 0);
	TEST_ASSERT(myVect.at(3) == 0);
	TEST_ASSERT(myVect.at(4) == 0);

	//Subtract dense from sparse

	mySecondVect.set(0, 4);
	mySecondVect.set(1, 0);
	mySecondVect.set(2, 0);
	mySecondVect.set(3, 9);
	mySecondVect.set(4, 0);

	//myVect: [4, 0, 0, 0, 0]
	//mySecondVect: [4, 0, 0, 9, 0]
	//Difference: [0, 0, 0, -9, 0]

	myVect = myVect - mySecondVect;

	TEST_ASSERT(myVect.m_dimension == 5);
	TEST_ASSERT(myVect.m_capacity == ELBOWROOM + 1);
	TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - 1);
	TEST_ASSERT(myVect.m_sorted == true);
	TEST_ASSERT(myVect.m_vectorType == 0);
	TEST_ASSERT(myVect.m_size == 1);
	TEST_ASSERT(myVect.m_sparsityThreshold == 2);
	TEST_ASSERT(myVect.m_nonZeros == 1);

	TEST_ASSERT(myVect.at(0) == 0);
	TEST_ASSERT(myVect.at(1) == 0);
	TEST_ASSERT(myVect.at(2) == 0);
	TEST_ASSERT(myVect.at(3) == -9);
	TEST_ASSERT(myVect.at(4) == 0);

	//Should switch to dense

	mySecondVect.set(0, -3);
	mySecondVect.set(1, 0);
	mySecondVect.set(2, 0);
	mySecondVect.set(3, -3);
	mySecondVect.set(4, -3);

	//myVect: [0, 0, 0, -9, 0]
	//mySecondVect: [-3, 0, 0, -3, -3]
	//Difference: [3, 0, 0, -6, 3]

	myVect = myVect - mySecondVect;

	TEST_ASSERT(myVect.m_dimension == 5);
	TEST_ASSERT(myVect.m_capacity == ELBOWROOM + 5);
	TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - 5);
	TEST_ASSERT(myVect.m_sorted == false);
	TEST_ASSERT(myVect.m_vectorType == 1);
	TEST_ASSERT(myVect.m_size == 5);
	TEST_ASSERT(myVect.m_sparsityThreshold == 2);
	TEST_ASSERT(myVect.m_nonZeros == 3);

	TEST_ASSERT(myVect.at(0) == 3);
	TEST_ASSERT(myVect.at(1) == 0);
	TEST_ASSERT(myVect.at(2) == 0);
	TEST_ASSERT(myVect.at(3) == -6);
	TEST_ASSERT(myVect.at(4) == 3);

	//Subtract sparse from dense

	mySecondVect.set(0, 0);
	mySecondVect.set(1, 0);
	mySecondVect.set(2, 0);
	mySecondVect.set(3, -6);
	mySecondVect.set(4, 0);

	//myVect: [3, 0, 0, -6, 3]
	//mySecondVect: [0, 0, 0, -6, 0]
	//Difference: [3, 0, 0, 0, 3]

	myVect = myVect - mySecondVect;

	TEST_ASSERT(myVect.m_dimension == 5);
	TEST_ASSERT(myVect.m_capacity == ELBOWROOM + 5);
	TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - 5);
	TEST_ASSERT(myVect.m_sorted == false);
	TEST_ASSERT(myVect.m_vectorType == 1);
	TEST_ASSERT(myVect.m_size == 5);
	TEST_ASSERT(myVect.m_sparsityThreshold == 2);
	TEST_ASSERT(myVect.m_nonZeros == 2);

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
	mySecondVect.set(4, 3);

	//myVect: [3, 0, 0, 0, 3]
	//mySecondVect: [0, 0, 0, 0, 3]
	//Difference: [3, 0, 0, 0, 0]

	myVect = myVect - mySecondVect;

	TEST_ASSERT(myVect.m_dimension == 5);
	TEST_ASSERT(myVect.m_capacity == ELBOWROOM + 1);
	TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - 1);
	TEST_ASSERT(myVect.m_sorted == true);
	TEST_ASSERT(myVect.m_vectorType == 0);
	TEST_ASSERT(myVect.m_size == 1);
	TEST_ASSERT(myVect.m_sparsityThreshold == 2);
	TEST_ASSERT(myVect.m_nonZeros == 1);

	TEST_ASSERT(myVect.at(0) == 3);
	TEST_ASSERT(myVect.at(1) == 0);
	TEST_ASSERT(myVect.at(2) == 0);
	TEST_ASSERT(myVect.at(3) == 0);
	TEST_ASSERT(myVect.at(4) == 0);

	//Subtract sparse from sparse

	mySecondVect.set(0, 0);
	mySecondVect.set(1, 0);
	mySecondVect.set(2, 0);
	mySecondVect.set(3, 0);
	mySecondVect.set(4, 0);

	//myVect: [3, 0, 0, 0, 0]
	//mySecondVect: [0, 0, 0, 0, 0]
	//Difference: [3, 0, 0, 0, 0]

	myVect = myVect - mySecondVect;

	TEST_ASSERT(myVect.m_dimension == 5);
	TEST_ASSERT(myVect.m_capacity == ELBOWROOM + 1);
	TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - 1);
	TEST_ASSERT(myVect.m_sorted == true);
	TEST_ASSERT(myVect.m_vectorType == 0);
	TEST_ASSERT(myVect.m_size == 1);
	TEST_ASSERT(myVect.m_sparsityThreshold == 2);
	TEST_ASSERT(myVect.m_nonZeros == 1);

	TEST_ASSERT(myVect.at(0) == 3);
	TEST_ASSERT(myVect.at(1) == 0);
	TEST_ASSERT(myVect.at(2) == 0);
	TEST_ASSERT(myVect.at(3) == 0);
	TEST_ASSERT(myVect.at(4) == 0);

	//Should switch to dense

	mySecondVect.set(0, 0);
	mySecondVect.set(1, -4);
	mySecondVect.set(2, 0);
	mySecondVect.set(3, 0);
	mySecondVect.set(4, 0);

	//myVect: [3, 0, 0, 0, 0]
	//mySecondVect: [0, -4, 0, 0, 0]
	//Difference: [3, 4, 0, 0, 0]

	myVect = myVect - mySecondVect;

	TEST_ASSERT(myVect.m_dimension == 5);
	TEST_ASSERT(myVect.m_capacity == ELBOWROOM + 5);
	TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - 5);
	TEST_ASSERT(myVect.m_sorted == false);
	TEST_ASSERT(myVect.m_vectorType == 1);
	TEST_ASSERT(myVect.m_size == 5);
	TEST_ASSERT(myVect.m_sparsityThreshold == 2);
	TEST_ASSERT(myVect.m_nonZeros == 2);

	TEST_ASSERT(myVect.at(0) == 3);
	TEST_ASSERT(myVect.at(1) == 4);
	TEST_ASSERT(myVect.at(2) == 0);
	TEST_ASSERT(myVect.at(3) == 0);
	TEST_ASSERT(myVect.at(4) == 0);
}

void VectorTestSuite::asterisk() {
	Vector myVect(5);
	myVect.set(1, -7);
	myVect.set(3, 5);
	myVect.set(4, 2);

	myVect = myVect*(-4);

	TEST_ASSERT(myVect.m_dimension == 5);
	TEST_ASSERT(myVect.m_capacity == ELBOWROOM + 5);
	TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - 5);
	TEST_ASSERT(myVect.m_sorted == false);
	TEST_ASSERT(myVect.m_vectorType == 1);
	TEST_ASSERT(myVect.m_size == 5);
	TEST_ASSERT(myVect.m_sparsityThreshold == 2);
	TEST_ASSERT(myVect.m_nonZeros == 3);

	TEST_ASSERT(myVect.at(0) == 0);
	TEST_ASSERT(myVect.at(1) == 28);
	TEST_ASSERT(myVect.at(2) == 0);
	TEST_ASSERT(myVect.at(3) == -20);
	TEST_ASSERT(myVect.at(4) == -8);

	myVect = myVect*0;

	//Should switch to sparse

	TEST_ASSERT(myVect.m_dimension == 5);
	TEST_ASSERT(myVect.m_capacity == ELBOWROOM);
	TEST_ASSERT(myVect.m_data == myVect.m_dataEnd);
	TEST_ASSERT(myVect.m_sorted == false);
	TEST_ASSERT(myVect.m_vectorType == 0);
	TEST_ASSERT(myVect.m_size == 0);
	TEST_ASSERT(myVect.m_sparsityThreshold == 2);
	TEST_ASSERT(myVect.m_nonZeros == 0);

	TEST_ASSERT(myVect.at(0) == 0);
	TEST_ASSERT(myVect.at(1) == 0);
	TEST_ASSERT(myVect.at(2) == 0);
	TEST_ASSERT(myVect.at(3) == 0);
	TEST_ASSERT(myVect.at(4) == 0);
}

void VectorTestSuite::asterisk2() {
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

	myVect = myVect * myMatrix;

	TEST_ASSERT(myVect.m_dimension == 5);
	TEST_ASSERT(myVect.m_capacity == ELBOWROOM + 5);
	TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - 5);
	TEST_ASSERT(myVect.m_sorted == false);
	TEST_ASSERT(myVect.m_vectorType == 1);
	TEST_ASSERT(myVect.m_size == 5);
	TEST_ASSERT(myVect.m_sparsityThreshold == 2);
	TEST_ASSERT(myVect.m_nonZeros == 4);

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

	TEST_ASSERT(myVect.m_dimension == 5);
	TEST_ASSERT(myVect.m_capacity == ELBOWROOM);
	TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - 1);
	TEST_ASSERT(myVect.m_sorted == false);
	TEST_ASSERT(myVect.m_vectorType == 0);
	TEST_ASSERT(myVect.m_size == 1);
	TEST_ASSERT(myVect.m_sparsityThreshold == 2);
	TEST_ASSERT(myVect.m_nonZeros == 1);

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

	TEST_ASSERT(myVect.m_dimension == 5);
	TEST_ASSERT(myVect.m_capacity == ELBOWROOM + 5);
	TEST_ASSERT(myVect.m_data == myVect.m_dataEnd - 5);
	TEST_ASSERT(myVect.m_sorted == false);
	TEST_ASSERT(myVect.m_vectorType == 1);
	TEST_ASSERT(myVect.m_size == 5);
	TEST_ASSERT(myVect.m_sparsityThreshold == 2);
	TEST_ASSERT(myVect.m_nonZeros == 2);

	TEST_ASSERT(myVect.at(0) == 0);
	TEST_ASSERT(myVect.at(1) == 8);
	TEST_ASSERT(myVect.at(2) == 0);
	TEST_ASSERT(myVect.at(3) == 0);
	TEST_ASSERT(myVect.at(4) == -16);
}

void VectorTestSuite::equal() {
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

void VectorTestSuite::sortElements() {
	Vector myVect(100);
	Vector mySecondVect(100);
	Vector myThirdVect(100);
	Vector myFourthVect(1000);

	//Test selectionSort

	myVect.set(77, 4);
	myVect.set(44, 7);
	myVect.set(91, 23);
	myVect.set(5, 5);

	myVect.sortElements();

    for (unsigned int index = 0; index < myVect.m_nonZeros - 1; index++) {
        TEST_ASSERT(myVect.m_index[index] < myVect.m_index[index + 1]);
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

	mySecondVect.sortElements();

    for (unsigned int index = 0; index < mySecondVect.m_nonZeros - 1; index++) {
        TEST_ASSERT(mySecondVect.m_index[index] < mySecondVect.m_index[index + 1]);
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

	myThirdVect.sortElements();

    for (unsigned int index = 0; index < myThirdVect.m_nonZeros - 1; index++) {
        TEST_ASSERT(myThirdVect.m_index[index] < myThirdVect.m_index[index + 1]);
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

    for (unsigned int index = 0; index < myFourthVect.m_nonZeros - 1; index++) {
        TEST_ASSERT(myFourthVect.m_index[index] < myFourthVect.m_index[index + 1]);
    }
}

void VectorTestSuite::reInit() {
	Vector myVect(5);

	myVect.set(4, 7);
	myVect.set(2, -8);
	myVect.set(1, 3);

	myVect.reInit(8, 8 + ELBOWROOM);

	TEST_ASSERT(myVect.m_dimension == 8);
	TEST_ASSERT(myVect.m_capacity == ELBOWROOM + 8);
	TEST_ASSERT(myVect.m_data == myVect.m_dataEnd);
	TEST_ASSERT(myVect.m_sorted == false);
	TEST_ASSERT(myVect.m_vectorType == 0);
	TEST_ASSERT(myVect.m_size == 0);
	TEST_ASSERT(myVect.m_sparsityThreshold == 3);
	TEST_ASSERT(myVect.m_nonZeros == 0);

	TEST_ASSERT(myVect.at(0) == 0);
	TEST_ASSERT(myVect.at(1) == 0);
	TEST_ASSERT(myVect.at(2) == 0);
	TEST_ASSERT(myVect.at(3) == 0);
	TEST_ASSERT(myVect.at(4) == 0);
	TEST_ASSERT(myVect.at(5) == 0);
	TEST_ASSERT(myVect.at(6) == 0);
	TEST_ASSERT(myVect.at(7) == 0);
}
