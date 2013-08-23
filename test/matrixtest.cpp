#include "matrixtest.h"
#include "testmacros.h"

MatrixTestSuite::MatrixTestSuite(const char * name) : UnitTest(name)
{
    ADD_TEST_EXTRA(MatrixTestSuite::equal, MatrixTester);
    ADD_TEST_EXTRA(MatrixTestSuite::copy, MatrixTester);
    ADD_TEST_EXTRA(MatrixTestSuite::assign, MatrixTester);
    ADD_TEST_EXTRA(MatrixTestSuite::transposed, MatrixTester);
    ADD_TEST_EXTRA(MatrixTestSuite::transpose, MatrixTester);
    ADD_TEST_EXTRA(MatrixTestSuite::add, MatrixTester);
    ADD_TEST_EXTRA(MatrixTestSuite::addToThis, MatrixTester);
    ADD_TEST_EXTRA(MatrixTestSuite::subtract, MatrixTester);
    ADD_TEST_EXTRA(MatrixTestSuite::subtractFromThis, MatrixTester);
    ADD_TEST_EXTRA(MatrixTestSuite::multipleByVector, MatrixTester);
    ADD_TEST_EXTRA(MatrixTestSuite::multipleByDouble, MatrixTester);
    ADD_TEST_EXTRA(MatrixTestSuite::multipleByMatrix_1, MatrixTester);
    ADD_TEST_EXTRA(MatrixTestSuite::multipleByMatrix_2, MatrixTester);
    ADD_TEST_EXTRA(MatrixTestSuite::multipleByDiagonalMatrix, MatrixTester);
    ADD_TEST_EXTRA(MatrixTestSuite::inverse, MatrixTester);
    ADD_TEST_EXTRA(MatrixTestSuite::invert, MatrixTester);
    ADD_TEST_EXTRA(MatrixTestSuite::diagonalMatrix, MatrixTester);
    ADD_TEST_EXTRA(MatrixTestSuite::diagonalMatrix_Inverse, MatrixTester);
    ADD_TEST_EXTRA(MatrixTestSuite::diagonalMatrix_Invert, MatrixTester);
    ADD_TEST_EXTRA(MatrixTestSuite::rowVector, MatrixTester);
    ADD_TEST_EXTRA(MatrixTestSuite::columnVector, MatrixTester);
    ADD_TEST_EXTRA(MatrixTestSuite::comprehensive_1, MatrixTester);
    ADD_TEST_EXTRA(MatrixTestSuite::constructor, MatrixTester);
    ADD_TEST_EXTRA(MatrixTestSuite::setGet, MatrixTester);
    ADD_TEST_EXTRA(MatrixTestSuite::columnWise, MatrixTester);
    ADD_TEST_EXTRA(MatrixTestSuite::rowWise, MatrixTester);
    ADD_TEST_EXTRA(MatrixTestSuite::clear, MatrixTester);
    ADD_TEST_EXTRA(MatrixTestSuite::resize, MatrixTester);
    ADD_TEST_EXTRA(MatrixTestSuite::scale, MatrixTester);
    ADD_TEST_EXTRA(MatrixTestSuite::scaleRow, MatrixTester);
    ADD_TEST_EXTRA(MatrixTestSuite::scaleColumn, MatrixTester);
    ADD_TEST_EXTRA(MatrixTestSuite::prepend, MatrixTester);
    ADD_TEST_EXTRA(MatrixTestSuite::prependEmpty, MatrixTester);
    ADD_TEST_EXTRA(MatrixTestSuite::prependEmpty2, MatrixTester);
    ADD_TEST_EXTRA(MatrixTestSuite::append, MatrixTester);
    ADD_TEST_EXTRA(MatrixTestSuite::appendEmpty, MatrixTester);
    ADD_TEST_EXTRA(MatrixTestSuite::appendEmpty2, MatrixTester);
    ADD_TEST_EXTRA(MatrixTestSuite::insertColumn, MatrixTester);
    ADD_TEST_EXTRA(MatrixTestSuite::insertEmptyColumn, MatrixTester);
    ADD_TEST_EXTRA(MatrixTestSuite::insertRow, MatrixTester);
    ADD_TEST_EXTRA(MatrixTestSuite::insertEmptyRow, MatrixTester);
    ADD_TEST_EXTRA(MatrixTestSuite::removeRow, MatrixTester);
    ADD_TEST_EXTRA(MatrixTestSuite::removeColumn, MatrixTester);
    ADD_TEST_EXTRA(MatrixTestSuite::nonZerosAndDensity, MatrixTester);
    ADD_TEST_EXTRA(MatrixTestSuite::newNonzero, MatrixTester);
    ADD_TEST_EXTRA(MatrixTestSuite::addScalarMatrix, MatrixTester);
    ADD_TEST_EXTRA(MatrixTestSuite::sortElements, MatrixTester);
    ADD_TEST_EXTRA(MatrixTestSuite::determinant, MatrixTester);
    ADD_TEST_EXTRA(MatrixTestSuite::reInit, MatrixTester);
    ADD_TEST_EXTRA(MatrixTestSuite::gaussian, MatrixTester);
}

void MatrixTestSuite::equal()
{
    Matrix m1(3, 4);
    m1.set(0, 0, 1);
    m1.set(0, 1, 2);
    m1.set(0, 2, 3);
    m1.set(0, 3, 4);
    m1.set(1, 0, 5);
    m1.set(1, 1, 6);
    m1.set(1, 2, 7);
    m1.set(1, 3, 8);
    m1.set(2, 0, 9);
    m1.set(2, 1, 10);
    m1.set(2, 2, 11);
    m1.set(2, 3, 12);

    Matrix m2(3, 4);
    m2.set(0, 0, 1);
    m2.set(0, 1, 2);
    m2.set(0, 2, 3);
    m2.set(0, 3, 4);
    m2.set(1, 0, 5);
    m2.set(1, 1, 6);
    m2.set(1, 2, 7);
    m2.set(1, 3, 8);
    m2.set(2, 0, 9);
    m2.set(2, 1, 10);
    m2.set(2, 2, 11);
    m2.set(2, 3, 12);

    TEST_ASSERT(m1 == m2);

    Matrix m3(2, 5);
    TEST_ASSERT(m1 != m3);

    Matrix m4(3, 4);
    m4.set(0, 0, 1);
    m4.set(0, 1, 2);
    m4.set(0, 2, 3);
    m4.set(0, 3, 4);
    m4.set(1, 0, 55);
    m4.set(1, 1, 6);
    m4.set(1, 2, 7);
    m4.set(1, 3, 8);
    m4.set(2, 0, 9);
    m4.set(2, 1, 10);
    m4.set(2, 2, 11);
    m4.set(2, 3, 12);

    TEST_ASSERT(m1 != m4);

}

void MatrixTestSuite::copy()
{
    Matrix m(3, 4);
    m.set(0, 0, 1);
    m.set(0, 1, 2);
    m.set(0, 2, 3);
    m.set(0, 3, 4);
    m.set(1, 0, 5);
    m.set(1, 1, 6);
    m.set(1, 2, 7);
    m.set(1, 3, 8);
    m.set(2, 0, 9);
    m.set(2, 1, 10);
    m.set(2, 2, 11);
    m.set(2, 3, 12);

    Matrix copy_m = m;
    TEST_ASSERT(3 == copy_m.rowCount());
    TEST_ASSERT(4 == copy_m.columnCount());
    TEST_ASSERT(1 == copy_m.get(0, 0));
    TEST_ASSERT(2 == copy_m.get(0, 1));
    TEST_ASSERT(3 == copy_m.get(0, 2));
    TEST_ASSERT(4 == copy_m.get(0, 3));
    TEST_ASSERT(5 == copy_m.get(1, 0));
    TEST_ASSERT(6 == copy_m.get(1, 1));
    TEST_ASSERT(7 == copy_m.get(1, 2));
    TEST_ASSERT(8 == copy_m.get(1, 3));
    TEST_ASSERT(9 == copy_m.get(2, 0));
    TEST_ASSERT(10 == copy_m.get(2, 1));
    TEST_ASSERT(11 == copy_m.get(2, 2));
    TEST_ASSERT(12 == copy_m.get(2, 3));

    Vector v1 = copy_m.column(0);
    TEST_ASSERT(3 == v1.length());
    TEST_ASSERT(1 == v1.at(0));
    TEST_ASSERT(5 == v1.at(1));
    TEST_ASSERT(9 == v1.at(2));

    Vector v2 = copy_m.column(1);
    TEST_ASSERT(3 == v2.length());
    TEST_ASSERT(2 == v2.at(0));
    TEST_ASSERT(6 == v2.at(1));
    TEST_ASSERT(10 == v2.at(2));

    Vector v3 = copy_m.column(2);
    TEST_ASSERT(3 == v3.length());
    TEST_ASSERT(3 == v3.at(0));
    TEST_ASSERT(7 == v3.at(1));
    TEST_ASSERT(11 == v3.at(2));

    Vector v4 = copy_m.column(3);
    TEST_ASSERT(3 == v4.length());
    TEST_ASSERT(4 == v4.at(0));
    TEST_ASSERT(8 == v4.at(1));
    TEST_ASSERT(12 == v4.at(2));

    Vector v5 = copy_m.row(0);
    TEST_ASSERT(4 == v5.length());
    TEST_ASSERT(1 == v5.at(0));
    TEST_ASSERT(2 == v5.at(1));
    TEST_ASSERT(3 == v5.at(2));
    TEST_ASSERT(4 == v5.at(3));

    Vector v6 = copy_m.row(1);
    TEST_ASSERT(4 == v6.length());
    TEST_ASSERT(5 == v6.at(0));
    TEST_ASSERT(6 == v6.at(1));
    TEST_ASSERT(7 == v6.at(2));
    TEST_ASSERT(8 == v6.at(3));

    Vector v7 = copy_m.row(2);
    TEST_ASSERT(4 == v7.length());
    TEST_ASSERT(9 == v7.at(0));
    TEST_ASSERT(10 == v7.at(1));
    TEST_ASSERT(11 == v7.at(2));
    TEST_ASSERT(12 == v7.at(3));
}

void MatrixTestSuite::assign()
{
    Matrix m(3, 4);
    m.set(0, 0, 1);
    m.set(0, 1, 2);
    m.set(0, 2, 3);
    m.set(0, 3, 4);
    m.set(1, 0, 5);
    m.set(1, 1, 6);
    m.set(1, 2, 7);
    m.set(1, 3, 8);
    m.set(2, 0, 9);
    m.set(2, 1, 10);
    m.set(2, 2, 11);
    m.set(2, 3, 12);

    Matrix copy_m;
    copy_m = m;
    ASSERT_EQ(3, copy_m.rowCount());
    ASSERT_EQ(4, copy_m.columnCount());
    ASSERT_EQ(1, copy_m.get(0, 0));
    ASSERT_EQ(2, copy_m.get(0, 1));
    ASSERT_EQ(3, copy_m.get(0, 2));
    ASSERT_EQ(4, copy_m.get(0, 3));
    ASSERT_EQ(5, copy_m.get(1, 0));
    ASSERT_EQ(6, copy_m.get(1, 1));
    ASSERT_EQ(7, copy_m.get(1, 2));
    ASSERT_EQ(8, copy_m.get(1, 3));
    ASSERT_EQ(9, copy_m.get(2, 0));
    ASSERT_EQ(10, copy_m.get(2, 1));
    ASSERT_EQ(11, copy_m.get(2, 2));
    ASSERT_EQ(12, copy_m.get(2, 3));

    Vector v1 = copy_m.column(0);
    ASSERT_EQ(3, v1.length());
    ASSERT_EQ(1, v1.at(0));
    ASSERT_EQ(5, v1.at(1));
    ASSERT_EQ(9, v1.at(2));

    Vector v2 = copy_m.column(1);
    ASSERT_EQ(3, v2.length());
    ASSERT_EQ(2, v2.at(0));
    ASSERT_EQ(6, v2.at(1));
    ASSERT_EQ(10, v2.at(2));

    Vector v3 = copy_m.column(2);
    ASSERT_EQ(3, v3.length());
    ASSERT_EQ(3, v3.at(0));
    ASSERT_EQ(7, v3.at(1));
    ASSERT_EQ(11, v3.at(2));

    Vector v4 = copy_m.column(3);
    ASSERT_EQ(3, v4.length());
    ASSERT_EQ(4, v4.at(0));
    ASSERT_EQ(8, v4.at(1));
    ASSERT_EQ(12, v4.at(2));

    Vector v5 = copy_m.row(0);
    ASSERT_EQ(4, v5.length());
    ASSERT_EQ(1, v5.at(0));
    ASSERT_EQ(2, v5.at(1));
    ASSERT_EQ(3, v5.at(2));
    ASSERT_EQ(4, v5.at(3));

    Vector v6 = copy_m.row(1);
    ASSERT_EQ(4, v6.length());
    ASSERT_EQ(5, v6.at(0));
    ASSERT_EQ(6, v6.at(1));
    ASSERT_EQ(7, v6.at(2));
    ASSERT_EQ(8, v6.at(3));

    Vector v7 = copy_m.row(2);
    ASSERT_EQ(4, v7.length());
    ASSERT_EQ(9, v7.at(0));
    ASSERT_EQ(10, v7.at(1));
    ASSERT_EQ(11, v7.at(2));
    ASSERT_EQ(12, v7.at(3));

    copy_m = copy_m;
    TEST_ASSERT(3 == copy_m.rowCount());
    TEST_ASSERT(4 == copy_m.columnCount());
    TEST_ASSERT(1 == copy_m.get(0, 0));
    TEST_ASSERT(2 == copy_m.get(0, 1));
    TEST_ASSERT(3 == copy_m.get(0, 2));
    TEST_ASSERT(4 == copy_m.get(0, 3));
    TEST_ASSERT(5 == copy_m.get(1, 0));
    TEST_ASSERT(6 == copy_m.get(1, 1));
    TEST_ASSERT(7 == copy_m.get(1, 2));
    TEST_ASSERT(8 == copy_m.get(1, 3));
    TEST_ASSERT(9 == copy_m.get(2, 0));
    TEST_ASSERT(10 == copy_m.get(2, 1));
    TEST_ASSERT(11 == copy_m.get(2, 2));
    TEST_ASSERT(12 == copy_m.get(2, 3));

    Vector v8 = copy_m.column(0);
    TEST_ASSERT(3 == v8.length());
    TEST_ASSERT(1 == v8.at(0));
    TEST_ASSERT(5 == v8.at(1));
    TEST_ASSERT(9 == v8.at(2));

    Vector v9 = copy_m.column(1);
    TEST_ASSERT(3 == v9.length());
    TEST_ASSERT(2 == v9.at(0));
    TEST_ASSERT(6 == v9.at(1));
    TEST_ASSERT(10 == v9.at(2));

    Vector v10 = copy_m.column(2);
    TEST_ASSERT(3 == v10.length());
    TEST_ASSERT(3 == v10.at(0));
    TEST_ASSERT(7 == v10.at(1));
    TEST_ASSERT(11 == v10.at(2));

    Vector v11 = copy_m.column(3);
    TEST_ASSERT(3 == v11.length());
    TEST_ASSERT(4 == v11.at(0));
    TEST_ASSERT(8 == v11.at(1));
    TEST_ASSERT(12 == v11.at(2));

    Vector v12 = copy_m.row(0);
    TEST_ASSERT(4 == v12.length());
    TEST_ASSERT(1 == v12.at(0));
    TEST_ASSERT(2 == v12.at(1));
    TEST_ASSERT(3 == v12.at(2));
    TEST_ASSERT(4 == v12.at(3));

    Vector v13 = copy_m.row(1);
    TEST_ASSERT(4 == v13.length());
    TEST_ASSERT(5 == v13.at(0));
    TEST_ASSERT(6 == v13.at(1));
    TEST_ASSERT(7 == v13.at(2));
    TEST_ASSERT(8 == v13.at(3));

    Vector v14 = copy_m.row(2);
    TEST_ASSERT(4 == v14.length());
    TEST_ASSERT(9 == v14.at(0));
    TEST_ASSERT(10 == v14.at(1));
    TEST_ASSERT(11 == v14.at(2));
    TEST_ASSERT(12 == v14.at(3));
}

void MatrixTestSuite::transposed()
{
    Matrix m(3, 4);
    m.set(0, 0, 1);
    m.set(0, 1, 2);
    m.set(0, 2, 3);
    m.set(0, 3, 4);
    m.set(1, 0, 5);
    m.set(1, 1, 6);
    m.set(1, 2, 7);
    m.set(1, 3, 8);
    m.set(2, 0, 9);
    m.set(2, 1, 10);
    m.set(2, 2, 11);
    m.set(2, 3, 12);

    Matrix mT = m.transposed();
    ASSERT_EQ(4, mT.rowCount());
    ASSERT_EQ(3, mT.columnCount());
    ASSERT_EQ(1, mT.get(0, 0));
    ASSERT_EQ(5, mT.get(0, 1));
    ASSERT_EQ(9, mT.get(0, 2));
    ASSERT_EQ(2, mT.get(1, 0));
    ASSERT_EQ(6, mT.get(1, 1));
    ASSERT_EQ(10, mT.get(1, 2));
    ASSERT_EQ(3, mT.get(2, 0));
    ASSERT_EQ(7, mT.get(2, 1));
    ASSERT_EQ(11, mT.get(2, 2));
    ASSERT_EQ(4, mT.get(3, 0));
    ASSERT_EQ(8, mT.get(3, 1));
    ASSERT_EQ(12, mT.get(3, 2));

    Vector v1 = mT.row(0);
    ASSERT_EQ(3, v1.length());
    ASSERT_EQ(1, v1.at(0));
    ASSERT_EQ(5, v1.at(1));
    ASSERT_EQ(9, v1.at(2));

    Vector v2 = mT.row(1);
    ASSERT_EQ(3, v2.length());
    ASSERT_EQ(2, v2.at(0));
    ASSERT_EQ(6, v2.at(1));
    ASSERT_EQ(10, v2.at(2));

    Vector v3 = mT.row(2);
    ASSERT_EQ(3, v3.length());
    ASSERT_EQ(3, v3.at(0));
    ASSERT_EQ(7, v3.at(1));
    ASSERT_EQ(11, v3.at(2));

    Vector v4 = mT.row(3);
    ASSERT_EQ(3, v4.length());
    ASSERT_EQ(4, v4.at(0));
    ASSERT_EQ(8, v4.at(1));
    ASSERT_EQ(12, v4.at(2));

    Vector v5 = mT.column(0);
    ASSERT_EQ(4, v5.length());
    ASSERT_EQ(1, v5.at(0));
    ASSERT_EQ(2, v5.at(1));
    ASSERT_EQ(3, v5.at(2));
    ASSERT_EQ(4, v5.at(3));

    Vector v6 = mT.column(1);
    ASSERT_EQ(4, v6.length());
    ASSERT_EQ(5, v6.at(0));
    ASSERT_EQ(6, v6.at(1));
    ASSERT_EQ(7, v6.at(2));
    ASSERT_EQ(8, v6.at(3));

    Vector v7 = mT.column(2);
    ASSERT_EQ(4, v7.length());
    ASSERT_EQ(9, v7.at(0));
    ASSERT_EQ(10, v7.at(1));
    ASSERT_EQ(11, v7.at(2));
    ASSERT_EQ(12, v7.at(3));

}

void MatrixTestSuite::transpose()
{
    Matrix m(3, 4);
    m.set(0, 0, 1);
    m.set(0, 1, 2);
    m.set(0, 2, 3);
    m.set(0, 3, 4);
    m.set(1, 0, 5);
    m.set(1, 1, 6);
    m.set(1, 2, 7);
    m.set(1, 3, 8);
    m.set(2, 0, 9);
    m.set(2, 1, 10);
    m.set(2, 2, 11);
    m.set(2, 3, 12);

    m.transpose();
    ASSERT_EQ(4, m.rowCount());
    ASSERT_EQ(3, m.columnCount());
    ASSERT_EQ(1, m.get(0, 0));
    ASSERT_EQ(5, m.get(0, 1));
    ASSERT_EQ(9, m.get(0, 2));
    ASSERT_EQ(2, m.get(1, 0));
    ASSERT_EQ(6, m.get(1, 1));
    ASSERT_EQ(10, m.get(1, 2));
    ASSERT_EQ(3, m.get(2, 0));
    ASSERT_EQ(7, m.get(2, 1));
    ASSERT_EQ(11, m.get(2, 2));
    ASSERT_EQ(4, m.get(3, 0));
    ASSERT_EQ(8, m.get(3, 1));
    ASSERT_EQ(12, m.get(3, 2));

    Vector v1 = m.row(0);
    ASSERT_EQ(3, v1.length());
    ASSERT_EQ(1, v1.at(0));
    ASSERT_EQ(5, v1.at(1));
    ASSERT_EQ(9, v1.at(2));

    Vector v2 = m.row(1);
    ASSERT_EQ(3, v2.length());
    ASSERT_EQ(2, v2.at(0));
    ASSERT_EQ(6, v2.at(1));
    ASSERT_EQ(10, v2.at(2));

    Vector v3 = m.row(2);
    ASSERT_EQ(3, v3.length());
    ASSERT_EQ(3, v3.at(0));
    ASSERT_EQ(7, v3.at(1));
    ASSERT_EQ(11, v3.at(2));

    Vector v4 = m.row(3);
    ASSERT_EQ(3, v4.length());
    ASSERT_EQ(4, v4.at(0));
    ASSERT_EQ(8, v4.at(1));
    ASSERT_EQ(12, v4.at(2));

    Vector v5 = m.column(0);
    ASSERT_EQ(4, v5.length());
    ASSERT_EQ(1, v5.at(0));
    ASSERT_EQ(2, v5.at(1));
    ASSERT_EQ(3, v5.at(2));
    ASSERT_EQ(4, v5.at(3));

    Vector v6 = m.column(1);
    ASSERT_EQ(4, v6.length());
    ASSERT_EQ(5, v6.at(0));
    ASSERT_EQ(6, v6.at(1));
    ASSERT_EQ(7, v6.at(2));
    ASSERT_EQ(8, v6.at(3));

    Vector v7 = m.column(2);
    ASSERT_EQ(4, v7.length());
    ASSERT_EQ(9, v7.at(0));
    ASSERT_EQ(10, v7.at(1));
    ASSERT_EQ(11, v7.at(2));
    ASSERT_EQ(12, v7.at(3));

}

void MatrixTestSuite::add()
{
    Matrix m1(3, 4);
    m1.set(0, 0, 1);
    m1.set(0, 1, 2);
    m1.set(0, 2, 3);
    m1.set(0, 3, 4);
    m1.set(1, 0, 5);
    m1.set(1, 1, 6);
    m1.set(1, 2, 7);
    m1.set(1, 3, 8);
    m1.set(2, 0, 9);
    m1.set(2, 1, 10);
    m1.set(2, 2, 11);
    m1.set(2, 3, 12);

    Matrix m2(3, 4);
    m2.set(0, 0, 24);
    m2.set(0, 1, 23);
    m2.set(0, 2, 22);
    m2.set(0, 3, 21);
    m2.set(1, 0, 20);
    m2.set(1, 1, 19);
    m2.set(1, 2, 18);
    m2.set(1, 3, 17);
    m2.set(2, 0, 16);
    m2.set(2, 1, 15);
    m2.set(2, 2, 14);
    m2.set(2, 3, 13);

    Matrix result = m1 + m2;
    ASSERT_EQ(3, result.rowCount());
    ASSERT_EQ(4, result.columnCount());
    ASSERT_EQ(25, result.get(0, 0));
    ASSERT_EQ(25, result.get(0, 1));
    ASSERT_EQ(25, result.get(0, 2));
    ASSERT_EQ(25, result.get(0, 3));
    ASSERT_EQ(25, result.get(1, 0));
    ASSERT_EQ(25, result.get(1, 1));
    ASSERT_EQ(25, result.get(1, 2));
    ASSERT_EQ(25, result.get(1, 3));
    ASSERT_EQ(25, result.get(2, 0));
    ASSERT_EQ(25, result.get(2, 1));
    ASSERT_EQ(25, result.get(2, 2));
    ASSERT_EQ(25, result.get(2, 3));

    Vector v1 = result.row(0);
    ASSERT_EQ(4, v1.length());
    ASSERT_EQ(25, v1.at(0));
    ASSERT_EQ(25, v1.at(1));
    ASSERT_EQ(25, v1.at(2));
    ASSERT_EQ(25, v1.at(3));

    Vector v2 = result.row(1);
    ASSERT_EQ(4, v2.length());
    ASSERT_EQ(25, v2.at(0));
    ASSERT_EQ(25, v2.at(1));
    ASSERT_EQ(25, v2.at(2));
    ASSERT_EQ(25, v2.at(3));

    Vector v3 = result.row(2);
    ASSERT_EQ(4, v3.length());
    ASSERT_EQ(25, v3.at(0));
    ASSERT_EQ(25, v3.at(1));
    ASSERT_EQ(25, v3.at(2));
    ASSERT_EQ(25, v3.at(3));

    Vector v4 = result.column(0);
    ASSERT_EQ(3, v4.length());
    ASSERT_EQ(25, v4.at(0));
    ASSERT_EQ(25, v4.at(1));
    ASSERT_EQ(25, v4.at(2));

    Vector v5 = result.column(1);
    ASSERT_EQ(3, v5.length());
    ASSERT_EQ(25, v5.at(0));
    ASSERT_EQ(25, v5.at(1));
    ASSERT_EQ(25, v5.at(2));

    Vector v6 = result.column(2);
    ASSERT_EQ(3, v6.length());
    ASSERT_EQ(25, v6.at(0));
    ASSERT_EQ(25, v6.at(1));
    ASSERT_EQ(25, v6.at(2));

    Vector v7 = result.column(3);
    ASSERT_EQ(3, v7.length());
    ASSERT_EQ(25, v7.at(0));
    ASSERT_EQ(25, v7.at(1));
    ASSERT_EQ(25, v7.at(2));

}

void MatrixTestSuite::addToThis()
{
    Matrix result(3, 4);
    result.set(0, 0, 1);
    result.set(0, 1, 2);
    result.set(0, 2, 3);
    result.set(0, 3, 4);
    result.set(1, 0, 5);
    result.set(1, 1, 6);
    result.set(1, 2, 7);
    result.set(1, 3, 8);
    result.set(2, 0, 9);
    result.set(2, 1, 10);
    result.set(2, 2, 11);
    result.set(2, 3, 12);

    Matrix m2(3, 4);
    m2.set(0, 0, 24);
    m2.set(0, 1, 23);
    m2.set(0, 2, 22);
    m2.set(0, 3, 21);
    m2.set(1, 0, 20);
    m2.set(1, 1, 19);
    m2.set(1, 2, 18);
    m2.set(1, 3, 17);
    m2.set(2, 0, 16);
    m2.set(2, 1, 15);
    m2.set(2, 2, 14);
    m2.set(2, 3, 13);

    result += m2;
    ASSERT_EQ(3, result.rowCount());
    ASSERT_EQ(4, result.columnCount());
    ASSERT_EQ(25, result.get(0, 0));
    ASSERT_EQ(25, result.get(0, 1));
    ASSERT_EQ(25, result.get(0, 2));
    ASSERT_EQ(25, result.get(0, 3));
    ASSERT_EQ(25, result.get(1, 0));
    ASSERT_EQ(25, result.get(1, 1));
    ASSERT_EQ(25, result.get(1, 2));
    ASSERT_EQ(25, result.get(1, 3));
    ASSERT_EQ(25, result.get(2, 0));
    ASSERT_EQ(25, result.get(2, 1));
    ASSERT_EQ(25, result.get(2, 2));
    ASSERT_EQ(25, result.get(2, 3));

    Vector v1 = result.row(0);
    ASSERT_EQ(4, v1.length());
    ASSERT_EQ(25, v1.at(0));
    ASSERT_EQ(25, v1.at(1));
    ASSERT_EQ(25, v1.at(2));
    ASSERT_EQ(25, v1.at(3));

    Vector v2 = result.row(1);
    ASSERT_EQ(4, v2.length());
    ASSERT_EQ(25, v2.at(0));
    ASSERT_EQ(25, v2.at(1));
    ASSERT_EQ(25, v2.at(2));
    ASSERT_EQ(25, v2.at(3));

    Vector v3 = result.row(2);
    ASSERT_EQ(4, v3.length());
    ASSERT_EQ(25, v3.at(0));
    ASSERT_EQ(25, v3.at(1));
    ASSERT_EQ(25, v3.at(2));
    ASSERT_EQ(25, v3.at(3));

    Vector v4 = result.column(0);
    ASSERT_EQ(3, v4.length());
    ASSERT_EQ(25, v4.at(0));
    ASSERT_EQ(25, v4.at(1));
    ASSERT_EQ(25, v4.at(2));

    Vector v5 = result.column(1);
    ASSERT_EQ(3, v5.length());
    ASSERT_EQ(25, v5.at(0));
    ASSERT_EQ(25, v5.at(1));
    ASSERT_EQ(25, v5.at(2));

    Vector v6 = result.column(2);
    ASSERT_EQ(3, v6.length());
    ASSERT_EQ(25, v6.at(0));
    ASSERT_EQ(25, v6.at(1));
    ASSERT_EQ(25, v6.at(2));

    Vector v7 = result.column(3);
    ASSERT_EQ(3, v7.length());
    ASSERT_EQ(25, v7.at(0));
    ASSERT_EQ(25, v7.at(1));
    ASSERT_EQ(25, v7.at(2));

}

void MatrixTestSuite::subtract()
{
    Matrix m1(3, 4);
    m1.set(0, 0, 1);
    m1.set(0, 1, 2);
    m1.set(0, 2, 3);
    m1.set(0, 3, 4);
    m1.set(1, 0, 5);
    m1.set(1, 1, 6);
    m1.set(1, 2, 7);
    m1.set(1, 3, 8);
    m1.set(2, 0, 9);
    m1.set(2, 1, 10);
    m1.set(2, 2, 11);
    m1.set(2, 3, 12);

    Matrix m2(3, 4);
    m2.set(0, 0, 24);
    m2.set(0, 1, 23);
    m2.set(0, 2, 22);
    m2.set(0, 3, 21);
    m2.set(1, 0, 20);
    m2.set(1, 1, 19);
    m2.set(1, 2, 18);
    m2.set(1, 3, 17);
    m2.set(2, 0, 16);
    m2.set(2, 1, 15);
    m2.set(2, 2, 14);
    m2.set(2, 3, 13);

    Matrix result = m1 - m2;
    ASSERT_EQ(3, result.rowCount());
    ASSERT_EQ(4, result.columnCount());
    ASSERT_EQ(-23, result.get(0, 0));
    ASSERT_EQ(-21, result.get(0, 1));
    ASSERT_EQ(-19, result.get(0, 2));
    ASSERT_EQ(-17, result.get(0, 3));
    ASSERT_EQ(-15, result.get(1, 0));
    ASSERT_EQ(-13, result.get(1, 1));
    ASSERT_EQ(-11, result.get(1, 2));
    ASSERT_EQ(-9, result.get(1, 3));
    ASSERT_EQ(-7, result.get(2, 0));
    ASSERT_EQ(-5, result.get(2, 1));
    ASSERT_EQ(-3, result.get(2, 2));
    ASSERT_EQ(-1, result.get(2, 3));

    Vector v1 = result.row(0);
    ASSERT_EQ(4, v1.length());
    ASSERT_EQ(-23, v1.at(0));
    ASSERT_EQ(-21, v1.at(1));
    ASSERT_EQ(-19, v1.at(2));
    ASSERT_EQ(-17, v1.at(3));

    Vector v2 = result.row(1);
    ASSERT_EQ(4, v2.length());
    ASSERT_EQ(-15, v2.at(0));
    ASSERT_EQ(-13, v2.at(1));
    ASSERT_EQ(-11, v2.at(2));
    ASSERT_EQ(-9, v2.at(3));

    Vector v3 = result.row(2);
    ASSERT_EQ(4, v3.length());
    ASSERT_EQ(-7, v3.at(0));
    ASSERT_EQ(-5, v3.at(1));
    ASSERT_EQ(-3, v3.at(2));
    ASSERT_EQ(-1, v3.at(3));

    Vector v4 = result.column(0);
    ASSERT_EQ(3, v4.length());
    ASSERT_EQ(-23, v4.at(0));
    ASSERT_EQ(-15, v4.at(1));
    ASSERT_EQ(-7, v4.at(2));

    Vector v5 = result.column(1);
    ASSERT_EQ(3, v5.length());
    ASSERT_EQ(-21, v5.at(0));
    ASSERT_EQ(-13, v5.at(1));
    ASSERT_EQ(-5, v5.at(2));

    Vector v6 = result.column(2);
    ASSERT_EQ(3, v6.length());
    ASSERT_EQ(-19, v6.at(0));
    ASSERT_EQ(-11, v6.at(1));
    ASSERT_EQ(-3, v6.at(2));

    Vector v7 = result.column(3);
    ASSERT_EQ(3, v7.length());
    ASSERT_EQ(-17, v7.at(0));
    ASSERT_EQ(-9, v7.at(1));
    ASSERT_EQ(-1, v7.at(2));

}

void MatrixTestSuite::subtractFromThis()
{
    Matrix result(3, 4);
    result.set(0, 0, 1);
    result.set(0, 1, 2);
    result.set(0, 2, 3);
    result.set(0, 3, 4);
    result.set(1, 0, 5);
    result.set(1, 1, 6);
    result.set(1, 2, 7);
    result.set(1, 3, 8);
    result.set(2, 0, 9);
    result.set(2, 1, 10);
    result.set(2, 2, 11);
    result.set(2, 3, 12);

    Matrix m2(3, 4);
    m2.set(0, 0, 24);
    m2.set(0, 1, 23);
    m2.set(0, 2, 22);
    m2.set(0, 3, 21);
    m2.set(1, 0, 20);
    m2.set(1, 1, 19);
    m2.set(1, 2, 18);
    m2.set(1, 3, 17);
    m2.set(2, 0, 16);
    m2.set(2, 1, 15);
    m2.set(2, 2, 14);
    m2.set(2, 3, 13);

    result -= m2;
    ASSERT_EQ(3, result.rowCount());
    ASSERT_EQ(4, result.columnCount());
    ASSERT_EQ(-23, result.get(0, 0));
    ASSERT_EQ(-21, result.get(0, 1));
    ASSERT_EQ(-19, result.get(0, 2));
    ASSERT_EQ(-17, result.get(0, 3));
    ASSERT_EQ(-15, result.get(1, 0));
    ASSERT_EQ(-13, result.get(1, 1));
    ASSERT_EQ(-11, result.get(1, 2));
    ASSERT_EQ(-9, result.get(1, 3));
    ASSERT_EQ(-7, result.get(2, 0));
    ASSERT_EQ(-5, result.get(2, 1));
    ASSERT_EQ(-3, result.get(2, 2));
    ASSERT_EQ(-1, result.get(2, 3));


    Vector v1 = result.row(0);
    ASSERT_EQ(4, v1.length());
    ASSERT_EQ(-23, v1.at(0));
    ASSERT_EQ(-21, v1.at(1));
    ASSERT_EQ(-19, v1.at(2));
    ASSERT_EQ(-17, v1.at(3));

    Vector v2 = result.row(1);
    ASSERT_EQ(4, v2.length());
    ASSERT_EQ(-15, v2.at(0));
    ASSERT_EQ(-13, v2.at(1));
    ASSERT_EQ(-11, v2.at(2));
    ASSERT_EQ(-9, v2.at(3));

    Vector v3 = result.row(2);
    ASSERT_EQ(4, v3.length());
    ASSERT_EQ(-7, v3.at(0));
    ASSERT_EQ(-5, v3.at(1));
    ASSERT_EQ(-3, v3.at(2));
    ASSERT_EQ(-1, v3.at(3));

    Vector v4 = result.column(0);
    ASSERT_EQ(3, v4.length());
    ASSERT_EQ(-23, v4.at(0));
    ASSERT_EQ(-15, v4.at(1));
    ASSERT_EQ(-7, v4.at(2));

    Vector v5 = result.column(1);
    ASSERT_EQ(3, v5.length());
    ASSERT_EQ(-21, v5.at(0));
    ASSERT_EQ(-13, v5.at(1));
    ASSERT_EQ(-5, v5.at(2));

    Vector v6 = result.column(2);
    ASSERT_EQ(3, v6.length());
    ASSERT_EQ(-19, v6.at(0));
    ASSERT_EQ(-11, v6.at(1));
    ASSERT_EQ(-3, v6.at(2));

    Vector v7 = result.column(3);
    ASSERT_EQ(3, v7.length());
    ASSERT_EQ(-17, v7.at(0));
    ASSERT_EQ(-9, v7.at(1));
    ASSERT_EQ(-1, v7.at(2));
}

void MatrixTestSuite::multipleByVector()
{
    Matrix m(3, 4);
    m.set(0, 0, 1);
    m.set(0, 1, 2);
    m.set(0, 2, 3);
    m.set(0, 3, 4);
    m.set(1, 0, 5);
    m.set(1, 1, 6);
    m.set(1, 2, 7);
    m.set(1, 3, 8);
    m.set(2, 0, 9);
    m.set(2, 1, 10);
    m.set(2, 2, 11);
    m.set(2, 3, 12);

    Vector v(4);
    v.set(0, 1);
    v.set(1, 2);
    v.set(2, 3);
    v.set(3, 4);

    Vector result = m*v;
    ASSERT_EQ(3, result.length());
    ASSERT_EQ(30, result.at(0));
    ASSERT_EQ(70, result.at(1));
    ASSERT_EQ(110, result.at(2));
}

void MatrixTestSuite::multipleByDouble()
{
    Matrix m(2, 3);
    m.set(0, 0, 2);
    m.set(0, 1, 4);
    m.set(0, 2, 6);
    m.set(1, 0, 8);
    m.set(1, 1, 10);
    m.set(1, 2, 12);

    Matrix result = 0.5 * m;
    ASSERT_EQ(3, result.columnCount());
    ASSERT_EQ(2, result.rowCount());
    ASSERT_EQ(1, result.get(0, 0));
    ASSERT_EQ(2, result.get(0, 1));
    ASSERT_EQ(3, result.get(0, 2));
    ASSERT_EQ(4, result.get(1, 0));
    ASSERT_EQ(5, result.get(1, 1));
    ASSERT_EQ(6, result.get(1, 2));

    Vector v1 = result.row(0);
    ASSERT_EQ(3, v1.length());
    ASSERT_EQ(1, v1.at(0));
    ASSERT_EQ(2, v1.at(1));
    ASSERT_EQ(3, v1.at(2));

    Vector v2 = result.row(1);
    ASSERT_EQ(3, v2.length());
    ASSERT_EQ(4, v2.at(0));
    ASSERT_EQ(5, v2.at(1));
    ASSERT_EQ(6, v2.at(2));

    Vector v3 = result.column(0);
    ASSERT_EQ(2, v3.length());
    ASSERT_EQ(1, v3.at(0));
    ASSERT_EQ(4, v3.at(1));

    Vector v4 = result.column(1);
    ASSERT_EQ(2, v4.length());
    ASSERT_EQ(2, v4.at(0));
    ASSERT_EQ(5, v4.at(1));

    Vector v5 = result.column(2);
    ASSERT_EQ(2, v5.length());
    ASSERT_EQ(3, v5.at(0));
    ASSERT_EQ(6, v5.at(1));
}

void MatrixTestSuite::multipleByMatrix_1()
{
    Matrix m1(3, 4);
    m1.set(0, 0, 1);
    m1.set(0, 1, 2);
    m1.set(0, 2, 3);
    m1.set(0, 3, 4);
    m1.set(1, 0, 5);
    m1.set(1, 1, 6);
    m1.set(1, 2, 7);
    m1.set(1, 3, 8);
    m1.set(2, 0, 9);
    m1.set(2, 1, 10);
    m1.set(2, 2, 11);
    m1.set(2, 3, 12);

    Matrix m2(4, 3);
    m2.set(0, 0, 24);
    m2.set(0, 1, 23);
    m2.set(0, 2, 22);
    m2.set(1, 0, 21);
    m2.set(1, 1, 20);
    m2.set(1, 2, 19);
    m2.set(2, 0, 18);
    m2.set(2, 1, 17);
    m2.set(2, 2, 16);
    m2.set(3, 0, 15);
    m2.set(3, 1, 14);
    m2.set(3, 2, 13);

    Matrix result = m1*m2;

    ASSERT_EQ(3, result.rowCount());
    ASSERT_EQ(3, result.columnCount());
    ASSERT_EQ(180, result.get(0, 0));
    ASSERT_EQ(170, result.get(0, 1));
    ASSERT_EQ(160, result.get(0, 2));
    ASSERT_EQ(492, result.get(1, 0));
    ASSERT_EQ(466, result.get(1, 1));
    ASSERT_EQ(440, result.get(1, 2));
    ASSERT_EQ(804, result.get(2, 0));
    ASSERT_EQ(762, result.get(2, 1));
    ASSERT_EQ(720, result.get(2, 2));

    Vector v1 = result.row(0);
    ASSERT_EQ(3, v1.length());
    ASSERT_EQ(180, v1.at(0));
    ASSERT_EQ(170, v1.at(1));
    ASSERT_EQ(160, v1.at(2));

    Vector v2 = result.row(1);
    ASSERT_EQ(3, v2.length());
    ASSERT_EQ(492, v2.at(0));
    ASSERT_EQ(466, v2.at(1));
    ASSERT_EQ(440, v2.at(2));

    Vector v3 = result.row(2);
    ASSERT_EQ(3, v3.length());
    ASSERT_EQ(804, v3.at(0));
    ASSERT_EQ(762, v3.at(1));
    ASSERT_EQ(720, v3.at(2));

    Vector v4 = result.column(0);
    ASSERT_EQ(3, v4.length());
    ASSERT_EQ(180, v4.at(0));
    ASSERT_EQ(492, v4.at(1));
    ASSERT_EQ(804, v4.at(2));

    Vector v5 = result.column(1);
    ASSERT_EQ(3, v5.length());
    ASSERT_EQ(170, v5.at(0));
    ASSERT_EQ(466, v5.at(1));
    ASSERT_EQ(762, v5.at(2));

    Vector v6 = result.column(2);
    ASSERT_EQ(3, v6.length());
    ASSERT_EQ(160, v6.at(0));
    ASSERT_EQ(440, v6.at(1));
    ASSERT_EQ(720, v6.at(2));

}

void MatrixTestSuite::multipleByMatrix_2()
{
    Matrix m3(3, 4);
    m3.set(0, 0, 1);
    m3.set(0, 1, 2);
    m3.set(0, 2, 3);
    m3.set(0, 3, 4);
    m3.set(1, 0, 5);
    m3.set(1, 1, 6);
    m3.set(1, 2, 7);
    m3.set(1, 3, 8);
    m3.set(2, 0, 9);
    m3.set(2, 1, 10);
    m3.set(2, 2, 11);
    m3.set(2, 3, 12);

    Matrix m4(4, 1);
    m4.set(0, 0, 24);
    m4.set(1, 0, 21);
    m4.set(2, 0, 18);
    m4.set(3, 0, 15);

    Matrix result2 = m3*m4;

    ASSERT_EQ(3, result2.rowCount());
    ASSERT_EQ(1, result2.columnCount());
    ASSERT_EQ(180, result2.get(0, 0));
    ASSERT_EQ(492, result2.get(1, 0));
    ASSERT_EQ(804, result2.get(2, 0));

    Vector v1 = result2.row(0);
    ASSERT_EQ(1, v1.length());
    ASSERT_EQ(180, v1.at(0));

    Vector v2 = result2.row(1);
    ASSERT_EQ(1, v2.length());
    ASSERT_EQ(492, v2.at(0));

    Vector v3 = result2.row(2);
    ASSERT_EQ(1, v3.length());
    ASSERT_EQ(804, v3.at(0));

    Vector v4 = result2.column(0);
    ASSERT_EQ(3, v4.length());
    ASSERT_EQ(180, v4.at(0));
    ASSERT_EQ(492, v4.at(1));
    ASSERT_EQ(804, v4.at(2));

}

void MatrixTestSuite::multipleByDiagonalMatrix()
{
    Vector v(3);
    v.set(0, 1);
    v.set(1, 2);
    v.set(2, 3);

    Matrix d = Matrix::createDiagonalMatrix(v);

    Matrix m(3, 2);
    m.set(0, 0, 1);
    m.set(0, 1, 2);
    m.set(1, 0, 3);
    m.set(1, 1, 4);
    m.set(2, 0, 5);
    m.set(2, 1, 6);

    Matrix result1 = d * m;

    ASSERT_EQ(2, result1.columnCount());
    ASSERT_EQ(3, result1.rowCount());
    ASSERT_EQ(1, result1.get(0, 0));
    ASSERT_EQ(2, result1.get(0, 1));
    ASSERT_EQ(6, result1.get(1, 0));
    ASSERT_EQ(8, result1.get(1, 1));
    ASSERT_EQ(15, result1.get(2, 0));
    ASSERT_EQ(18, result1.get(2, 1));

    Vector v1 = result1.row(0);
    ASSERT_EQ(2, v1.length());
    ASSERT_EQ(1, v1.at(0));
    ASSERT_EQ(2, v1.at(1));

    Vector v2 = result1.row(1);
    ASSERT_EQ(2, v2.length());
    ASSERT_EQ(6, v2.at(0));
    ASSERT_EQ(8, v2.at(1));

    Vector v3 = result1.row(2);
    ASSERT_EQ(2, v3.length());
    ASSERT_EQ(15, v3.at(0));
    ASSERT_EQ(18, v3.at(1));

    Vector v4 = result1.column(0);
    ASSERT_EQ(3, v4.length());
    ASSERT_EQ(1, v4.at(0));
    ASSERT_EQ(6, v4.at(1));
    ASSERT_EQ(15, v4.at(2));

    Vector v5 = result1.column(1);
    ASSERT_EQ(3, v5.length());
    ASSERT_EQ(2, v5.at(0));
    ASSERT_EQ(8, v5.at(1));
    ASSERT_EQ(18, v5.at(2));

    Matrix m2(2, 3);
    m2.set(0, 0, 1);
    m2.set(1, 0, 2);
    m2.set(0, 1, 3);
    m2.set(1, 1, 4);
    m2.set(0, 2, 5);
    m2.set(1, 2, 6);

    Matrix result2 = m2 * d;

    ASSERT_EQ(3, result2.columnCount());
    ASSERT_EQ(2, result2.rowCount());
    ASSERT_EQ(1, result2.get(0, 0));
    ASSERT_EQ(2, result2.get(1, 0));
    ASSERT_EQ(6, result2.get(0, 1));
    ASSERT_EQ(8, result2.get(1, 1));
    ASSERT_EQ(15, result2.get(0, 2));
    ASSERT_EQ(18, result2.get(1, 2));

    Vector v6 = result2.column(0);
    ASSERT_EQ(2, v6.length());
    ASSERT_EQ(1, v6.at(0));
    ASSERT_EQ(2, v6.at(1));

    Vector v7 = result2.column(1);
    ASSERT_EQ(2, v7.length());
    ASSERT_EQ(6, v7.at(0));
    ASSERT_EQ(8, v7.at(1));

    Vector v8 = result2.column(2);
    ASSERT_EQ(2, v8.length());
    ASSERT_EQ(15, v8.at(0));
    ASSERT_EQ(18, v8.at(1));

    Vector v9 = result2.row(0);
    ASSERT_EQ(3, v9.length());
    ASSERT_EQ(1, v9.at(0));
    ASSERT_EQ(6, v9.at(1));
    ASSERT_EQ(15, v9.at(2));

    Vector v10 = result2.row(1);
    ASSERT_EQ(3, v10.length());
    ASSERT_EQ(2, v10.at(0));
    ASSERT_EQ(8, v10.at(1));
    ASSERT_EQ(18, v10.at(2));

}

void MatrixTestSuite::inverse()
{
    Matrix m(3, 3);
    m.set(0, 0, 1);
    m.set(0, 1, 3);
    m.set(0, 2, 3);
    m.set(1, 0, 1);
    m.set(1, 1, 4);
    m.set(1, 2, 3);
    m.set(2, 0, 1);
    m.set(2, 1, 3);
    m.set(2, 2, 4);

    Matrix inv = m.inverse();
    ASSERT_EQ(3, 3);
    ASSERT_EQ(7, inv.get(0, 0));
    ASSERT_EQ(-3, inv.get(0, 1));
    ASSERT_EQ(-3, inv.get(0, 2));

    ASSERT_EQ(-1, inv.get(1, 0));
    ASSERT_EQ(1, inv.get(1, 1));
    ASSERT_EQ(0, inv.get(1, 2));

    ASSERT_EQ(-1, inv.get(2, 0));
    ASSERT_EQ(0, inv.get(2, 1));
    ASSERT_EQ(1, inv.get(2, 2));

    Vector v1 = inv.row(0);
    ASSERT_EQ(3, v1.length());
    ASSERT_EQ(7, v1.at(0));
    ASSERT_EQ(-3, v1.at(1));
    ASSERT_EQ(-3, v1.at(2));

    Vector v2 = inv.row(1);
    ASSERT_EQ(3, v2.length());
    ASSERT_EQ(-1, v2.at(0));
    ASSERT_EQ(1, v2.at(1));
    ASSERT_EQ(0, v2.at(2));

    Vector v3 = inv.row(2);
    ASSERT_EQ(3, v3.length());
    ASSERT_EQ(-1, v3.at(0));
    ASSERT_EQ(0, v3.at(1));
    ASSERT_EQ(1, v3.at(2));

    Vector v4 = inv.column(0);
    ASSERT_EQ(3, v4.length());
    ASSERT_EQ(7, v4.at(0));
    ASSERT_EQ(-1, v4.at(1));
    ASSERT_EQ(-1, v4.at(2));

    Vector v5 = inv.column(1);
    ASSERT_EQ(3, v5.length());
    ASSERT_EQ(-3, v5.at(0));
    ASSERT_EQ(1, v5.at(1));
    ASSERT_EQ(0, v5.at(2));

    Vector v6 = inv.column(2);
    ASSERT_EQ(3, v6.length());
    ASSERT_EQ(-3, v6.at(0));
    ASSERT_EQ(0, v6.at(1));
    ASSERT_EQ(1, v6.at(2));

}

void MatrixTestSuite::invert()
{
    Matrix m(3, 3);
    m.set(0, 0, 1);
    m.set(0, 1, 3);
    m.set(0, 2, 3);
    m.set(1, 0, 1);
    m.set(1, 1, 4);
    m.set(1, 2, 3);
    m.set(2, 0, 1);
    m.set(2, 1, 3);
    m.set(2, 2, 4);

    Matrix m0 = m;
    m.invert();
    ASSERT_EQ(3, 3);
    ASSERT_EQ(7, m.get(0, 0));
    ASSERT_EQ(-3, m.get(0, 1));
    ASSERT_EQ(-3, m.get(0, 2));

    ASSERT_EQ(-1, m.get(1, 0));
    ASSERT_EQ(1, m.get(1, 1));
    ASSERT_EQ(0, m.get(1, 2));

    ASSERT_EQ(-1, m.get(2, 0));
    ASSERT_EQ(0, m.get(2, 1));
    ASSERT_EQ(1, m.get(2, 2));

    Vector v1 = m.row(0);
    ASSERT_EQ(3, v1.length());
    ASSERT_EQ(7, v1.at(0));
    ASSERT_EQ(-3, v1.at(1));
    ASSERT_EQ(-3, v1.at(2));

    Vector v2 = m.row(1);
    ASSERT_EQ(3, v2.length());
    ASSERT_EQ(-1, v2.at(0));
    ASSERT_EQ(1, v2.at(1));
    ASSERT_EQ(0, v2.at(2));

    Vector v3 = m.row(2);
    ASSERT_EQ(3, v3.length());
    ASSERT_EQ(-1, v3.at(0));
    ASSERT_EQ(0, v3.at(1));
    ASSERT_EQ(1, v3.at(2));

    Vector v4 = m.column(0);
    ASSERT_EQ(3, v4.length());
    ASSERT_EQ(7, v4.at(0));
    ASSERT_EQ(-1, v4.at(1));
    ASSERT_EQ(-1, v4.at(2));

    Vector v5 = m.column(1);
    ASSERT_EQ(3, v5.length());
    ASSERT_EQ(-3, v5.at(0));
    ASSERT_EQ(1, v5.at(1));
    ASSERT_EQ(0, v5.at(2));

    Vector v6 = m.column(2);
    ASSERT_EQ(3, v6.length());
    ASSERT_EQ(-3, v6.at(0));
    ASSERT_EQ(0, v6.at(1));
    ASSERT_EQ(1, v6.at(2));
}

void MatrixTestSuite::diagonalMatrix()
{
    Vector v(3);
    v.set(0, 1);
    v.set(1, 2);
    v.set(2, 3);

    Matrix m = Matrix::createDiagonalMatrix(v);
    ASSERT_EQ(3, m.rowCount());
    ASSERT_EQ(3, m.columnCount());
    ASSERT_EQ(1, m.get(0, 0));
    ASSERT_EQ(0, m.get(0, 1));
    ASSERT_EQ(0, m.get(0, 2));
    ASSERT_EQ(0, m.get(1, 0));
    ASSERT_EQ(2, m.get(1, 1));
    ASSERT_EQ(0, m.get(1, 2));
    ASSERT_EQ(0, m.get(2, 0));
    ASSERT_EQ(0, m.get(2, 1));
    ASSERT_EQ(3, m.get(2, 2));

    Vector v1 = m.column(0);
    ASSERT_EQ(3, v1.length());
    ASSERT_EQ(1, v1.at(0));
    ASSERT_EQ(0, v1.at(1));
    ASSERT_EQ(0, v1.at(2));

    Vector v2 = m.column(1);
    ASSERT_EQ(3, v2.length());
    ASSERT_EQ(0, v2.at(0));
    ASSERT_EQ(2, v2.at(1));
    ASSERT_EQ(0, v2.at(2));

    Vector v3 = m.column(2);
    ASSERT_EQ(3, v3.length());
    ASSERT_EQ(0, v3.at(0));
    ASSERT_EQ(0, v3.at(1));
    ASSERT_EQ(3, v3.at(2));

    Vector v4 = m.row(0);
    ASSERT_EQ(3, v4.length());
    ASSERT_EQ(1, v4.at(0));
    ASSERT_EQ(0, v4.at(1));
    ASSERT_EQ(0, v4.at(2));

    Vector v5 = m.row(1);
    ASSERT_EQ(3, v5.length());
    ASSERT_EQ(0, v5.at(0));
    ASSERT_EQ(2, v5.at(1));
    ASSERT_EQ(0, v5.at(2));

    Vector v6 = m.row(2);
    ASSERT_EQ(3, v6.length());
    ASSERT_EQ(0, v6.at(0));
    ASSERT_EQ(0, v6.at(1));
    ASSERT_EQ(3, v6.at(2));

}

void MatrixTestSuite::diagonalMatrix_Inverse()
{
    Vector v(3);
    v.set(0, 1);
    v.set(1, 2);
    v.set(2, 3);

    Matrix m = Matrix::createDiagonalMatrix(v);
    Matrix inverted_m = m.inverse();
    ASSERT_EQ(3, inverted_m.rowCount());
    ASSERT_EQ(3, inverted_m.columnCount());
    ASSERT_EQ(1 / 1., inverted_m.get(0, 0));
    ASSERT_EQ(0, inverted_m.get(0, 1));
    ASSERT_EQ(0, inverted_m.get(0, 2));
    ASSERT_EQ(0, inverted_m.get(1, 0));
    ASSERT_EQ(1 / 2., inverted_m.get(1, 1));
    ASSERT_EQ(0, inverted_m.get(1, 2));
    ASSERT_EQ(0, inverted_m.get(2, 0));
    ASSERT_EQ(0, inverted_m.get(2, 1));
    ASSERT_EQ(1 / 3., inverted_m.get(2, 2));

    Vector v1 = inverted_m.row(0);
    ASSERT_EQ(3, v1.length());
    ASSERT_EQ(1, v1.at(0));
    ASSERT_EQ(0, v1.at(1));
    ASSERT_EQ(0, v1.at(2));

    Vector v2 = inverted_m.row(1);
    ASSERT_EQ(3, v2.length());
    ASSERT_EQ(0, v2.at(0));
    ASSERT_EQ(1 / 2., v2.at(1));
    ASSERT_EQ(0, v2.at(2));

    Vector v3 = inverted_m.row(2);
    ASSERT_EQ(3, v3.length());
    ASSERT_EQ(0, v3.at(0));
    ASSERT_EQ(0, v3.at(1));
    ASSERT_EQ(1 / 3., v3.at(2));

    Vector v4 = inverted_m.column(0);
    ASSERT_EQ(3, v4.length());
    ASSERT_EQ(1, v4.at(0));
    ASSERT_EQ(0, v4.at(1));
    ASSERT_EQ(0, v4.at(2));

    Vector v5 = inverted_m.column(1);
    ASSERT_EQ(3, v5.length());
    ASSERT_EQ(0, v5.at(0));
    ASSERT_EQ(1 / 2., v5.at(1));
    ASSERT_EQ(0, v5.at(2));

    Vector v6 = inverted_m.column(2);
    ASSERT_EQ(3, v6.length());
    ASSERT_EQ(0, v6.at(0));
    ASSERT_EQ(0, v6.at(1));
    ASSERT_EQ(1 / 3., v6.at(2));
}

void MatrixTestSuite::diagonalMatrix_Invert()
{
    Vector v(3);
    v.set(0, 1);
    v.set(1, 2);
    v.set(2, 3);

    Matrix m = Matrix::createDiagonalMatrix(v);
    m.invert();
    ASSERT_EQ(3, m.rowCount());
    ASSERT_EQ(3, m.columnCount());
    ASSERT_EQ(1 / 1., m.get(0, 0));
    ASSERT_EQ(0, m.get(0, 1));
    ASSERT_EQ(0, m.get(0, 2));
    ASSERT_EQ(0, m.get(1, 0));
    ASSERT_EQ(1 / 2., m.get(1, 1));
    ASSERT_EQ(0, m.get(1, 2));
    ASSERT_EQ(0, m.get(2, 0));
    ASSERT_EQ(0, m.get(2, 1));
    ASSERT_EQ(1 / 3., m.get(2, 2));

    Vector v1 = m.row(0);
    ASSERT_EQ(3, v1.length());
    ASSERT_EQ(1, v1.at(0));
    ASSERT_EQ(0, v1.at(1));
    ASSERT_EQ(0, v1.at(2));

    Vector v2 = m.row(1);
    ASSERT_EQ(3, v2.length());
    ASSERT_EQ(0, v2.at(0));
    ASSERT_EQ(1 / 2., v2.at(1));
    ASSERT_EQ(0, v2.at(2));

    Vector v3 = m.row(2);
    ASSERT_EQ(3, v3.length());
    ASSERT_EQ(0, v3.at(0));
    ASSERT_EQ(0, v3.at(1));
    ASSERT_EQ(1 / 3., v3.at(2));

    Vector v4 = m.column(0);
    ASSERT_EQ(3, v4.length());
    ASSERT_EQ(1, v4.at(0));
    ASSERT_EQ(0, v4.at(1));
    ASSERT_EQ(0, v4.at(2));

    Vector v5 = m.column(1);
    ASSERT_EQ(3, v5.length());
    ASSERT_EQ(0, v5.at(0));
    ASSERT_EQ(1 / 2., v5.at(1));
    ASSERT_EQ(0, v5.at(2));

    Vector v6 = m.column(2);
    ASSERT_EQ(3, v6.length());
    ASSERT_EQ(0, v6.at(0));
    ASSERT_EQ(0, v6.at(1));
    ASSERT_EQ(1 / 3., v6.at(2));

}

void MatrixTestSuite::rowVector()
{
    Vector v(3);
    v.set(0, 1);
    v.set(1, 2);
    v.set(2, 3);

    Matrix rowVector = Matrix::createRowMatrix(v);

    ASSERT_EQ(1, rowVector.rowCount());
    ASSERT_EQ(3, rowVector.columnCount());
    ASSERT_EQ(1, rowVector.get(0, 0));
    ASSERT_EQ(2, rowVector.get(0, 1));
    ASSERT_EQ(3, rowVector.get(0, 2));

    Vector v1 = rowVector.row(0);
    ASSERT_EQ(3, v1.length());
    ASSERT_EQ(1, v1.at(0));
    ASSERT_EQ(2, v1.at(1));
    ASSERT_EQ(3, v1.at(2));

    Vector v2 = rowVector.column(0);
    ASSERT_EQ(1, v2.length());
    ASSERT_EQ(1, v2.at(0));

    Vector v3 = rowVector.column(1);
    ASSERT_EQ(1, v3.length());
    ASSERT_EQ(2, v3.at(0));

    Vector v4 = rowVector.column(2);
    ASSERT_EQ(1, v4.length());
    ASSERT_EQ(3, v4.at(0));

}

void MatrixTestSuite::columnVector()
{
    Vector v(3);
    v.set(0, 1);
    v.set(1, 2);
    v.set(2, 3);

    Matrix columnVector = Matrix::createColumnMatrix(v);

    ASSERT_EQ(3, columnVector.rowCount());
    ASSERT_EQ(1, columnVector.columnCount());
    ASSERT_EQ(1, columnVector.get(0, 0));
    ASSERT_EQ(2, columnVector.get(1, 0));
    ASSERT_EQ(3, columnVector.get(2, 0));

    Vector v1 = columnVector.column(0);
    ASSERT_EQ(3, v1.length());
    ASSERT_EQ(1, v1.at(0));
    ASSERT_EQ(2, v1.at(1));
    ASSERT_EQ(3, v1.at(2));

    Vector v2 = columnVector.row(0);
    ASSERT_EQ(1, v2.length());
    ASSERT_EQ(1, v2.at(0));

    Vector v3 = columnVector.row(1);
    ASSERT_EQ(1, v3.length());
    ASSERT_EQ(2, v3.at(0));

    Vector v4 = columnVector.row(2);
    ASSERT_EQ(1, v4.length());
    ASSERT_EQ(3, v4.at(0));
}

void MatrixTestSuite::comprehensive_1()
{
    Vector vDelta(4);
    vDelta.set(0, 1);
    vDelta.set(1, 2);
    vDelta.set(2, 3);
    vDelta.set(3, 4);

    Matrix M(4, 4);
    M.set(0, 0, 1);
    M.set(0, 1, 2);
    M.set(0, 2, 3);
    M.set(0, 3, 4);
    M.set(1, 0, 5);
    M.set(1, 1, 6);
    M.set(1, 2, 7);
    M.set(1, 3, 8);
    M.set(2, 0, 9);
    M.set(2, 1, 10);
    M.set(2, 2, 11);
    M.set(2, 3, 12);
    M.set(3, 0, 13);
    M.set(3, 1, 14);
    M.set(3, 2, 15);
    M.set(3, 3, 16);

    Matrix delta = Matrix::createColumnMatrix(vDelta);

    Matrix result = (delta.transposed() * M * delta);
    ASSERT_EQ(1, result.rowCount());
    ASSERT_EQ(1, result.columnCount());
    ASSERT_EQ(1100, result.get(0, 0));

    Vector v1 = result.row(0);
    ASSERT_EQ(1, v1.length());
    ASSERT_EQ(1100, v1.at(0));

    Vector v2 = result.column(0);
    ASSERT_EQ(1, v2.length());
    ASSERT_EQ(1100, v2.at(0));

}

// Forgive me my lord for all these tester code lines...

void MatrixTestSuite::constructor()
{
    Matrix m1(3, 4);

    ASSERT_EQ(3, m1.rowCount());
    ASSERT_EQ(4, m1.columnCount());
    ASSERT_EQ(0.0, m1.get(0, 0));
    ASSERT_EQ(0.0, m1.get(1, 0));
    ASSERT_EQ(0.0, m1.get(2, 0));

    ASSERT_EQ(0.0, m1.get(0, 1));
    ASSERT_EQ(0.0, m1.get(1, 1));
    ASSERT_EQ(0.0, m1.get(2, 1));

    ASSERT_EQ(0.0, m1.get(0, 2));
    ASSERT_EQ(0.0, m1.get(1, 2));
    ASSERT_EQ(0.0, m1.get(2, 2));

    ASSERT_EQ(0.0, m1.get(0, 3));
    ASSERT_EQ(0.0, m1.get(1, 3));
    ASSERT_EQ(0.0, m1.get(2, 3));

    Vector v1 = m1.row(0);
    ASSERT_EQ(4, v1.length());
    ASSERT_EQ(0, v1.at(0));
    ASSERT_EQ(0, v1.at(1));
    ASSERT_EQ(0, v1.at(2));
    ASSERT_EQ(0, v1.at(3));

    Vector v2 = m1.row(1);
    ASSERT_EQ(4, v2.length());
    ASSERT_EQ(0, v2.at(0));
    ASSERT_EQ(0, v2.at(1));
    ASSERT_EQ(0, v2.at(2));
    ASSERT_EQ(0, v2.at(3));

    Vector v3 = m1.row(2);
    ASSERT_EQ(4, v3.length());
    ASSERT_EQ(0, v3.at(0));
    ASSERT_EQ(0, v3.at(1));
    ASSERT_EQ(0, v3.at(2));
    ASSERT_EQ(0, v3.at(3));

    Vector v4 = m1.column(0);
    ASSERT_EQ(3, v4.length());
    ASSERT_EQ(0, v4.at(0));
    ASSERT_EQ(0, v4.at(1));
    ASSERT_EQ(0, v4.at(2));

    Vector v5 = m1.column(1);
    ASSERT_EQ(3, v5.length());
    ASSERT_EQ(0, v5.at(0));
    ASSERT_EQ(0, v5.at(1));
    ASSERT_EQ(0, v5.at(2));

    Vector v6 = m1.column(2);
    ASSERT_EQ(3, v6.length());
    ASSERT_EQ(0, v6.at(0));
    ASSERT_EQ(0, v6.at(1));
    ASSERT_EQ(0, v6.at(2));

    Vector v7 = m1.column(3);
    ASSERT_EQ(3, v7.length());
    ASSERT_EQ(0, v7.at(0));
    ASSERT_EQ(0, v7.at(1));
    ASSERT_EQ(0, v7.at(2));

    Matrix m2(2);
    ASSERT_EQ(2, m2.rowCount());
    ASSERT_EQ(2, m2.columnCount());

    ASSERT_EQ(0.0, m2.get(0, 0));
    ASSERT_EQ(0.0, m2.get(1, 0));
    ASSERT_EQ(0.0, m2.get(0, 1));
    ASSERT_EQ(0.0, m2.get(1, 1));

    Vector v8 = m2.row(0);
    ASSERT_EQ(2, v8.length());
    ASSERT_EQ(0, v8.at(0));
    ASSERT_EQ(0, v8.at(1));

    Vector v9 = m2.row(1);
    ASSERT_EQ(2, v9.length());
    ASSERT_EQ(0, v9.at(0));
    ASSERT_EQ(0, v9.at(1));

    Vector v10 = m2.column(0);
    ASSERT_EQ(2, v10.length());
    ASSERT_EQ(0, v10.at(0));
    ASSERT_EQ(0, v10.at(1));

    Vector v11 = m2.column(1);
    ASSERT_EQ(2, v11.length());
    ASSERT_EQ(0, v11.at(0));
    ASSERT_EQ(0, v11.at(1));

    Matrix m3;
    ASSERT_EQ(0, m3.rowCount());
    ASSERT_EQ(0, m3.columnCount());

}

void MatrixTestSuite::setGet()
{
    Matrix m1(3, 4);
    m1.set(0, 0, 1);
    m1.set(0, 1, 2);
    m1.set(0, 2, 3);
    m1.set(0, 3, 4);
    m1.set(1, 0, 5);
    m1.set(1, 1, 6);
    m1.set(1, 2, 7);
    m1.set(1, 3, 8);
    m1.set(2, 0, 9);
    m1.set(2, 1, 10);
    m1.set(2, 2, 11);
    m1.set(2, 3, 12);

    ASSERT_EQ(1, m1.get(0, 0));
    ASSERT_EQ(2, m1.get(0, 1));
    ASSERT_EQ(3, m1.get(0, 2));
    ASSERT_EQ(4, m1.get(0, 3));

    ASSERT_EQ(5, m1.get(1, 0));
    ASSERT_EQ(6, m1.get(1, 1));
    ASSERT_EQ(7, m1.get(1, 2));
    ASSERT_EQ(8, m1.get(1, 3));

    ASSERT_EQ(9, m1.get(2, 0));
    ASSERT_EQ(10, m1.get(2, 1));
    ASSERT_EQ(11, m1.get(2, 2));
    ASSERT_EQ(12, m1.get(2, 3));
}

void MatrixTestSuite::columnWise()
{
    Matrix m1(3, 4);
    m1.set(0, 0, 1);
    m1.set(0, 1, 2);
    m1.set(0, 2, 3);
    m1.set(0, 3, 4);
    m1.set(1, 0, 5);
    m1.set(1, 1, 6);
    m1.set(1, 2, 7);
    m1.set(1, 3, 8);
    m1.set(2, 0, 9);
    m1.set(2, 1, 10);
    m1.set(2, 2, 11);
    m1.set(2, 3, 12);

    Vector v1 = m1.column(0);

    ASSERT_EQ(3, v1.length());
    ASSERT_EQ(1, v1.at(0));
    ASSERT_EQ(5, v1.at(1));
    ASSERT_EQ(9, v1.at(2));

    Vector v2 = m1.column(1);

    ASSERT_EQ(3, v2.length());
    ASSERT_EQ(2, v2.at(0));
    ASSERT_EQ(6, v2.at(1));
    ASSERT_EQ(10, v2.at(2));

    Vector v3 = m1.column(2);

    ASSERT_EQ(3, v3.length());
    ASSERT_EQ(3, v3.at(0));
    ASSERT_EQ(7, v3.at(1));
    ASSERT_EQ(11, v3.at(2));

    Vector v4 = m1.column(3);

    ASSERT_EQ(3, v4.length());
    ASSERT_EQ(4, v4.at(0));
    ASSERT_EQ(8, v4.at(1));
    ASSERT_EQ(12, v4.at(2));

}

void MatrixTestSuite::rowWise()
{
    Matrix m1(3, 4);
    m1.set(0, 0, 1);
    m1.set(0, 1, 2);
    m1.set(0, 2, 3);
    m1.set(0, 3, 4);
    m1.set(1, 0, 5);
    m1.set(1, 1, 6);
    m1.set(1, 2, 7);
    m1.set(1, 3, 8);
    m1.set(2, 0, 9);
    m1.set(2, 1, 10);
    m1.set(2, 2, 11);
    m1.set(2, 3, 12);

    Vector v1 = m1.row(0);

    ASSERT_EQ(4, v1.length());
    ASSERT_EQ(1, v1.at(0));
    ASSERT_EQ(2, v1.at(1));
    ASSERT_EQ(3, v1.at(2));
    ASSERT_EQ(4, v1.at(3));

    Vector v2 = m1.row(1);

    ASSERT_EQ(4, v2.length());
    ASSERT_EQ(5, v2.at(0));
    ASSERT_EQ(6, v2.at(1));
    ASSERT_EQ(7, v2.at(2));
    ASSERT_EQ(8, v2.at(3));

    Vector v3 = m1.row(2);

    ASSERT_EQ(4, v3.length());
    ASSERT_EQ(9, v3.at(0));
    ASSERT_EQ(10, v3.at(1));
    ASSERT_EQ(11, v3.at(2));
    ASSERT_EQ(12, v3.at(3));

}

void MatrixTestSuite::clear()
{
    Matrix m(2, 2);
    m.clear();

    ASSERT_EQ(0, m.rowCount());
    ASSERT_EQ(0, m.columnCount());
}

void MatrixTestSuite::resize()
{
    Matrix m(2, 3);
    m.set(0, 0, 1);
    m.set(0, 1, 2);
    m.set(0, 2, 3);

    m.set(1, 0, 11);
    m.set(1, 1, 12);
    m.set(1, 2, 13);

    m.resize(3, 4);

    ASSERT_EQ(3, m.rowCount());
    ASSERT_EQ(4, m.columnCount());

    ASSERT_EQ(1, m.get(0, 0));
    ASSERT_EQ(2, m.get(0, 1));
    ASSERT_EQ(3, m.get(0, 2));
    ASSERT_EQ(0.0, m.get(0, 3));

    ASSERT_EQ(11, m.get(1, 0));
    ASSERT_EQ(12, m.get(1, 1));
    ASSERT_EQ(13, m.get(1, 2));
    ASSERT_EQ(0.0, m.get(1, 3));

    ASSERT_EQ(0.0, m.get(2, 0));
    ASSERT_EQ(0.0, m.get(2, 1));
    ASSERT_EQ(0.0, m.get(2, 2));
    ASSERT_EQ(0.0, m.get(2, 3));

    Vector v1 = m.column(0);

    ASSERT_EQ(3, v1.length());
    ASSERT_EQ(1, v1.at(0));
    ASSERT_EQ(11, v1.at(1));
    ASSERT_EQ(0, v1.at(2));

    Vector v2 = m.column(1);

    ASSERT_EQ(3, v2.length());
    ASSERT_EQ(2, v2.at(0));
    ASSERT_EQ(12, v2.at(1));
    ASSERT_EQ(0, v2.at(2));

    Vector v3 = m.column(2);

    ASSERT_EQ(3, v3.length());
    ASSERT_EQ(3, v3.at(0));
    ASSERT_EQ(13, v3.at(1));
    ASSERT_EQ(0, v3.at(2));

    Vector v4 = m.column(3);

    ASSERT_EQ(3, v4.length());
    ASSERT_EQ(0, v4.at(0));
    ASSERT_EQ(0, v4.at(1));
    ASSERT_EQ(0, v4.at(2));

    Vector v5 = m.row(0);

    ASSERT_EQ(4, v5.length());
    ASSERT_EQ(1, v5.at(0));
    ASSERT_EQ(2, v5.at(1));
    ASSERT_EQ(3, v5.at(2));
    ASSERT_EQ(0, v5.at(3));

    Vector v6 = m.row(1);

    ASSERT_EQ(4, v6.length());
    ASSERT_EQ(11, v6.at(0));
    ASSERT_EQ(12, v6.at(1));
    ASSERT_EQ(13, v6.at(2));
    ASSERT_EQ(0, v6.at(3));

    Vector v7 = m.row(2);

    ASSERT_EQ(4, v7.length());
    ASSERT_EQ(0, v7.at(0));
    ASSERT_EQ(0, v7.at(1));
    ASSERT_EQ(0, v7.at(2));
    ASSERT_EQ(0, v7.at(3));

    m.resize(3, 2);

    ASSERT_EQ(3, m.rowCount());
    ASSERT_EQ(2, m.columnCount());

    ASSERT_EQ(1, m.get(0, 0));
    ASSERT_EQ(2, m.get(0, 1));

    ASSERT_EQ(11, m.get(1, 0));
    ASSERT_EQ(12, m.get(1, 1));

    ASSERT_EQ(0, m.get(2, 0));
    ASSERT_EQ(0, m.get(2, 1));

    Vector v8 = m.row(0);
    ASSERT_EQ(2, v8.length());
    ASSERT_EQ(1, v8.at(0));
    ASSERT_EQ(2, v8.at(1));

    Vector v9 = m.row(1);
    ASSERT_EQ(2, v9.length());
    ASSERT_EQ(11, v9.at(0));
    ASSERT_EQ(12, v9.at(1));

    Vector v10 = m.row(2);
    ASSERT_EQ(2, v10.length());
    ASSERT_EQ(0, v10.at(0));
    ASSERT_EQ(0, v10.at(1));

    Vector v11 = m.column(0);
    ASSERT_EQ(3, v11.length());
    ASSERT_EQ(1, v11.at(0));
    ASSERT_EQ(11, v11.at(1));
    ASSERT_EQ(0, v11.at(2));

    Vector v12 = m.column(1);
    ASSERT_EQ(3, v12.length());
    ASSERT_EQ(2, v12.at(0));
    ASSERT_EQ(12, v12.at(1));
    ASSERT_EQ(0, v12.at(2));

    m.resize(1, 4);

    ASSERT_EQ(1, m.rowCount());
    ASSERT_EQ(4, m.columnCount());

    ASSERT_EQ(1, m.get(0, 0));
    ASSERT_EQ(2, m.get(0, 1));
    ASSERT_EQ(0, m.get(0, 2));
    ASSERT_EQ(0, m.get(0, 3));

    Vector v13 = m.row(0);
    ASSERT_EQ(4, v13.length());
    ASSERT_EQ(1, v13.at(0));
    ASSERT_EQ(2, v13.at(1));
    ASSERT_EQ(0, v13.at(2));
    ASSERT_EQ(0, v13.at(3));

    Vector v14 = m.column(0);
    ASSERT_EQ(1, v14.length());
    ASSERT_EQ(1, v14.at(0));

    Vector v15 = m.column(1);
    ASSERT_EQ(1, v15.length());
    ASSERT_EQ(2, v15.at(0));

    Vector v16 = m.column(2);
    ASSERT_EQ(1, v16.length());
    ASSERT_EQ(0, v16.at(0));

    Vector v17 = m.column(3);
    ASSERT_EQ(1, v17.length());
    ASSERT_EQ(0, v17.at(0));
}

void MatrixTestSuite::scale()
{
    Matrix m(3, 4);
    m.set(0, 0, 1);
    m.set(0, 1, 2);
    m.set(0, 2, 3);
    m.set(0, 3, 4);
    m.set(1, 0, 5);
    m.set(1, 1, 6);
    m.set(1, 2, 7);
    m.set(1, 3, 8);
    m.set(2, 0, 9);
    m.set(2, 1, 10);
    m.set(2, 2, 11);
    m.set(2, 3, 12);

    m.scale(10);
    ASSERT_EQ(10, m.get(0, 0));
    ASSERT_EQ(20, m.get(0, 1));
    ASSERT_EQ(30, m.get(0, 2));
    ASSERT_EQ(40, m.get(0, 3));

    ASSERT_EQ(50, m.get(1, 0));
    ASSERT_EQ(60, m.get(1, 1));
    ASSERT_EQ(70, m.get(1, 2));
    ASSERT_EQ(80, m.get(1, 3));

    ASSERT_EQ(90, m.get(2, 0));
    ASSERT_EQ(100, m.get(2, 1));
    ASSERT_EQ(110, m.get(2, 2));
    ASSERT_EQ(120, m.get(2, 3));

    Vector v1 = m.column(0);

    ASSERT_EQ(10, v1.at(0));
    ASSERT_EQ(50, v1.at(1));
    ASSERT_EQ(90, v1.at(2));

    Vector v2 = m.column(1);

    ASSERT_EQ(20, v2.at(0));
    ASSERT_EQ(60, v2.at(1));
    ASSERT_EQ(100, v2.at(2));

    Vector v3 = m.column(2);

    ASSERT_EQ(30, v3.at(0));
    ASSERT_EQ(70, v3.at(1));
    ASSERT_EQ(110, v3.at(2));

    Vector v4 = m.column(3);

    ASSERT_EQ(40, v4.at(0));
    ASSERT_EQ(80, v4.at(1));
    ASSERT_EQ(120, v4.at(2));

    Vector v5 = m.row(0);

    ASSERT_EQ(10, v5.at(0));
    ASSERT_EQ(20, v5.at(1));
    ASSERT_EQ(30, v5.at(2));
    ASSERT_EQ(40, v5.at(3));

    Vector v6 = m.row(1);

    ASSERT_EQ(50, v6.at(0));
    ASSERT_EQ(60, v6.at(1));
    ASSERT_EQ(70, v6.at(2));
    ASSERT_EQ(80, v6.at(3));

    Vector v7 = m.row(2);

    ASSERT_EQ(90, v7.at(0));
    ASSERT_EQ(100, v7.at(1));
    ASSERT_EQ(110, v7.at(2));
    ASSERT_EQ(120, v7.at(3));

    m.scale(0.0);
    ASSERT_EQ(0, m.get(0, 0));
    ASSERT_EQ(0, m.get(0, 1));
    ASSERT_EQ(0, m.get(0, 2));
    ASSERT_EQ(0, m.get(0, 3));

    ASSERT_EQ(0, m.get(1, 0));
    ASSERT_EQ(0, m.get(1, 1));
    ASSERT_EQ(0, m.get(1, 2));
    ASSERT_EQ(0, m.get(1, 3));

    ASSERT_EQ(0, m.get(2, 0));
    ASSERT_EQ(0, m.get(2, 1));
    ASSERT_EQ(0, m.get(2, 2));
    ASSERT_EQ(0, m.get(2, 3));

    Vector v8 = m.column(0);

    ASSERT_EQ(0, v8.at(0));
    ASSERT_EQ(0, v8.at(1));
    ASSERT_EQ(0, v8.at(2));

    Vector v9 = m.column(1);

    ASSERT_EQ(0, v9.at(0));
    ASSERT_EQ(0, v9.at(1));
    ASSERT_EQ(0, v9.at(2));

    Vector v10 = m.column(2);

    ASSERT_EQ(0, v10.at(0));
    ASSERT_EQ(0, v10.at(1));
    ASSERT_EQ(0, v10.at(2));

    Vector v11 = m.column(3);

    ASSERT_EQ(0, v11.at(0));
    ASSERT_EQ(0, v11.at(1));
    ASSERT_EQ(0, v11.at(2));

    Vector v12 = m.row(0);

    ASSERT_EQ(0, v12.at(0));
    ASSERT_EQ(0, v12.at(1));
    ASSERT_EQ(0, v12.at(2));
    ASSERT_EQ(0, v12.at(3));

    Vector v13 = m.row(1);

    ASSERT_EQ(0, v13.at(0));
    ASSERT_EQ(0, v13.at(1));
    ASSERT_EQ(0, v13.at(2));
    ASSERT_EQ(0, v13.at(3));

    Vector v14 = m.row(2);

    ASSERT_EQ(0, v14.at(0));
    ASSERT_EQ(0, v14.at(1));
    ASSERT_EQ(0, v14.at(2));
    ASSERT_EQ(0, v14.at(3));
}

void MatrixTestSuite::scaleRow()
{
    Matrix m(3, 4);
    m.set(0, 0, 1);
    m.set(0, 1, 2);
    m.set(0, 2, 3);
    m.set(0, 3, 4);
    m.set(1, 0, 5);
    m.set(1, 1, 6);
    m.set(1, 2, 7);
    m.set(1, 3, 8);
    m.set(2, 0, 9);
    m.set(2, 1, 10);
    m.set(2, 2, 11);
    m.set(2, 3, 12);

    m.scaleRow(1, 10);
    ASSERT_EQ(1, m.get(0, 0));
    ASSERT_EQ(2, m.get(0, 1));
    ASSERT_EQ(3, m.get(0, 2));
    ASSERT_EQ(4, m.get(0, 3));

    ASSERT_EQ(50, m.get(1, 0));
    ASSERT_EQ(60, m.get(1, 1));
    ASSERT_EQ(70, m.get(1, 2));
    ASSERT_EQ(80, m.get(1, 3));

    ASSERT_EQ(9, m.get(2, 0));
    ASSERT_EQ(10, m.get(2, 1));
    ASSERT_EQ(11, m.get(2, 2));
    ASSERT_EQ(12, m.get(2, 3));

    Vector v1 = m.column(0);

    ASSERT_EQ(1, v1.at(0));
    ASSERT_EQ(50, v1.at(1));
    ASSERT_EQ(9, v1.at(2));

    Vector v2 = m.column(1);

    ASSERT_EQ(2, v2.at(0));
    ASSERT_EQ(60, v2.at(1));
    ASSERT_EQ(10, v2.at(2));

    Vector v3 = m.column(2);

    ASSERT_EQ(3, v3.at(0));
    ASSERT_EQ(70, v3.at(1));
    ASSERT_EQ(11, v3.at(2));

    Vector v4 = m.column(3);

    ASSERT_EQ(4, v4.at(0));
    ASSERT_EQ(80, v4.at(1));
    ASSERT_EQ(12, v4.at(2));

    Vector v5 = m.row(0);

    ASSERT_EQ(1, v5.at(0));
    ASSERT_EQ(2, v5.at(1));
    ASSERT_EQ(3, v5.at(2));
    ASSERT_EQ(4, v5.at(3));

    Vector v6 = m.row(1);

    ASSERT_EQ(50, v6.at(0));
    ASSERT_EQ(60, v6.at(1));
    ASSERT_EQ(70, v6.at(2));
    ASSERT_EQ(80, v6.at(3));

    Vector v7 = m.row(2);

    ASSERT_EQ(9, v7.at(0));
    ASSERT_EQ(10, v7.at(1));
    ASSERT_EQ(11, v7.at(2));
    ASSERT_EQ(12, v7.at(3));

    m.scaleRow(2, 0);
    ASSERT_EQ(1, m.get(0, 0));
    ASSERT_EQ(2, m.get(0, 1));
    ASSERT_EQ(3, m.get(0, 2));
    ASSERT_EQ(4, m.get(0, 3));

    ASSERT_EQ(50, m.get(1, 0));
    ASSERT_EQ(60, m.get(1, 1));
    ASSERT_EQ(70, m.get(1, 2));
    ASSERT_EQ(80, m.get(1, 3));

    ASSERT_EQ(0, m.get(2, 0));
    ASSERT_EQ(0, m.get(2, 1));
    ASSERT_EQ(0, m.get(2, 2));
    ASSERT_EQ(0, m.get(2, 3));

    Vector v8 = m.column(0);

    ASSERT_EQ(1, v8.at(0));
    ASSERT_EQ(50, v8.at(1));
    ASSERT_EQ(0, v8.at(2));

    Vector v9 = m.column(1);

    ASSERT_EQ(2, v9.at(0));
    ASSERT_EQ(60, v9.at(1));
    ASSERT_EQ(0, v9.at(2));

    Vector v10 = m.column(2);

    ASSERT_EQ(3, v10.at(0));
    ASSERT_EQ(70, v10.at(1));
    ASSERT_EQ(0, v10.at(2));

    Vector v11 = m.column(3);

    ASSERT_EQ(4, v11.at(0));
    ASSERT_EQ(80, v11.at(1));
    ASSERT_EQ(0, v11.at(2));

    Vector v12 = m.row(0);

    ASSERT_EQ(1, v12.at(0));
    ASSERT_EQ(2, v12.at(1));
    ASSERT_EQ(3, v12.at(2));
    ASSERT_EQ(4, v12.at(3));

    Vector v13 = m.row(1);

    ASSERT_EQ(50, v13.at(0));
    ASSERT_EQ(60, v13.at(1));
    ASSERT_EQ(70, v13.at(2));
    ASSERT_EQ(80, v13.at(3));

    Vector v14 = m.row(2);

    ASSERT_EQ(0, v14.at(0));
    ASSERT_EQ(0, v14.at(1));
    ASSERT_EQ(0, v14.at(2));
    ASSERT_EQ(0, v14.at(3));
}

void MatrixTestSuite::scaleColumn()
{
    Matrix m(3, 4);
    m.set(0, 0, 1);
    m.set(0, 1, 2);
    m.set(0, 2, 3);
    m.set(0, 3, 4);
    m.set(1, 0, 5);
    m.set(1, 1, 6);
    m.set(1, 2, 7);
    m.set(1, 3, 8);
    m.set(2, 0, 9);
    m.set(2, 1, 10);
    m.set(2, 2, 11);
    m.set(2, 3, 12);

    m.scaleColumn(2, 10);

    ASSERT_EQ(1, m.get(0, 0));
    ASSERT_EQ(2, m.get(0, 1));
    ASSERT_EQ(30, m.get(0, 2));
    ASSERT_EQ(4, m.get(0, 3));

    ASSERT_EQ(5, m.get(1, 0));
    ASSERT_EQ(6, m.get(1, 1));
    ASSERT_EQ(70, m.get(1, 2));
    ASSERT_EQ(8, m.get(1, 3));

    ASSERT_EQ(9, m.get(2, 0));
    ASSERT_EQ(10, m.get(2, 1));
    ASSERT_EQ(110, m.get(2, 2));
    ASSERT_EQ(12, m.get(2, 3));

    Vector v1 = m.column(0);

    ASSERT_EQ(1, v1.at(0));
    ASSERT_EQ(5, v1.at(1));
    ASSERT_EQ(9, v1.at(2));

    Vector v2 = m.column(1);

    ASSERT_EQ(2, v2.at(0));
    ASSERT_EQ(6, v2.at(1));
    ASSERT_EQ(10, v2.at(2));

    Vector v3 = m.column(2);

    ASSERT_EQ(30, v3.at(0));
    ASSERT_EQ(70, v3.at(1));
    ASSERT_EQ(110, v3.at(2));

    Vector v4 = m.column(3);

    ASSERT_EQ(4, v4.at(0));
    ASSERT_EQ(8, v4.at(1));
    ASSERT_EQ(12, v4.at(2));

    Vector v5 = m.row(0);

    ASSERT_EQ(1, v5.at(0));
    ASSERT_EQ(2, v5.at(1));
    ASSERT_EQ(30, v5.at(2));
    ASSERT_EQ(4, v5.at(3));

    Vector v6 = m.row(1);

    ASSERT_EQ(5, v6.at(0));
    ASSERT_EQ(6, v6.at(1));
    ASSERT_EQ(70, v6.at(2));
    ASSERT_EQ(8, v6.at(3));

    Vector v7 = m.row(2);

    ASSERT_EQ(9, v7.at(0));
    ASSERT_EQ(10, v7.at(1));
    ASSERT_EQ(110, v7.at(2));
    ASSERT_EQ(12, v7.at(3));

    m.scaleColumn(1, 0);
    ASSERT_EQ(1, m.get(0, 0));
    ASSERT_EQ(0, m.get(0, 1));
    ASSERT_EQ(30, m.get(0, 2));
    ASSERT_EQ(4, m.get(0, 3));

    ASSERT_EQ(5, m.get(1, 0));
    ASSERT_EQ(0, m.get(1, 1));
    ASSERT_EQ(70, m.get(1, 2));
    ASSERT_EQ(8, m.get(1, 3));

    ASSERT_EQ(9, m.get(2, 0));
    ASSERT_EQ(0, m.get(2, 1));
    ASSERT_EQ(110, m.get(2, 2));
    ASSERT_EQ(12, m.get(2, 3));

    Vector v8 = m.column(0);

    ASSERT_EQ(1, v8.at(0));
    ASSERT_EQ(5, v8.at(1));
    ASSERT_EQ(9, v8.at(2));

    Vector v9 = m.column(1);

    ASSERT_EQ(0, v9.at(0));
    ASSERT_EQ(0, v9.at(1));
    ASSERT_EQ(0, v9.at(2));

    Vector v10 = m.column(2);

    ASSERT_EQ(30, v10.at(0));
    ASSERT_EQ(70, v10.at(1));
    ASSERT_EQ(110, v10.at(2));

    Vector v11 = m.column(3);

    ASSERT_EQ(4, v11.at(0));
    ASSERT_EQ(8, v11.at(1));
    ASSERT_EQ(12, v11.at(2));

    Vector v12 = m.row(0);

    ASSERT_EQ(1, v12.at(0));
    ASSERT_EQ(0, v12.at(1));
    ASSERT_EQ(30, v12.at(2));
    ASSERT_EQ(4, v12.at(3));

    Vector v13 = m.row(1);

    ASSERT_EQ(5, v13.at(0));
    ASSERT_EQ(0, v13.at(1));
    ASSERT_EQ(70, v13.at(2));
    ASSERT_EQ(8, v13.at(3));

    Vector v14 = m.row(2);

    ASSERT_EQ(9, v14.at(0));
    ASSERT_EQ(0, v14.at(1));
    ASSERT_EQ(110, v14.at(2));
    ASSERT_EQ(12, v14.at(3));
}

void MatrixTestSuite::prepend()
{
    Matrix m(2, 2);
    m.set(0, 0, 11);
    m.set(0, 1, 12);
    m.set(1, 0, 21);
    m.set(1, 1, 22);

    Vector vi1(2);
    vi1.set(0, 1);
    vi1.set(1, 2);

    //m.show();
    m.prependColumn(vi1);

    ASSERT_EQ(2, m.rowCount());
    ASSERT_EQ(3, m.columnCount());
    ASSERT_EQ(1, m.get(0, 0));
    ASSERT_EQ(2, m.get(1, 0));
    ASSERT_EQ(11, m.get(0, 1));
    ASSERT_EQ(12, m.get(0, 2));
    ASSERT_EQ(21, m.get(1, 1));
    ASSERT_EQ(22, m.get(1, 2));

    Vector v1 = m.column(0);
    ASSERT_EQ(2, v1.length());
    ASSERT_EQ(1, v1.at(0));
    ASSERT_EQ(2, v1.at(1));

    Vector v2 = m.column(1);
    ASSERT_EQ(2, v2.length());
    ASSERT_EQ(11, v2.at(0));
    ASSERT_EQ(21, v2.at(1));

    Vector v3 = m.column(2);
    ASSERT_EQ(2, v3.length());
    ASSERT_EQ(12, v3.at(0));
    ASSERT_EQ(22, v3.at(1));

    Vector v4 = m.row(0);
    //v4.show();
    //std::cin.get();
    ASSERT_EQ(3, v4.length());
    ASSERT_EQ(1, v4.at(0));
    ASSERT_EQ(11, v4.at(1));
    ASSERT_EQ(12, v4.at(2));

    Vector v5 = m.row(1);
    ASSERT_EQ(3, v5.length());
    ASSERT_EQ(2, v5.at(0));
    ASSERT_EQ(21, v5.at(1));
    ASSERT_EQ(22, v5.at(2));

    Vector vr1(3);
    vr1.set(0, 100);
    vr1.set(1, 200);
    vr1.set(2, 300);

    m.prependRow(vr1);

    ASSERT_EQ(3, m.rowCount());
    ASSERT_EQ(3, m.columnCount());
    ASSERT_EQ(100, m.get(0, 0));
    ASSERT_EQ(1, m.get(1, 0));
    ASSERT_EQ(2, m.get(2, 0));
    ASSERT_EQ(200, m.get(0, 1));
    ASSERT_EQ(11, m.get(1, 1));
    ASSERT_EQ(12, m.get(1, 2));
    ASSERT_EQ(21, m.get(2, 1));
    ASSERT_EQ(22, m.get(2, 2));
    ASSERT_EQ(300, m.get(0, 2));

    Vector v6 = m.column(0);
    ASSERT_EQ(3, v6.length());
    ASSERT_EQ(100, v6.at(0));
    ASSERT_EQ(1, v6.at(1));
    ASSERT_EQ(2, v6.at(2));

    Vector v7 = m.column(1);
    ASSERT_EQ(3, v7.length());
    ASSERT_EQ(200, v7.at(0));
    ASSERT_EQ(11, v7.at(1));
    ASSERT_EQ(21, v7.at(2));

    Vector v8 = m.column(2);
    ASSERT_EQ(3, v8.length());
    ASSERT_EQ(300, v8.at(0));
    ASSERT_EQ(12, v8.at(1));
    ASSERT_EQ(22, v8.at(2));

    Vector v9 = m.row(0);
    ASSERT_EQ(3, v9.length());
    ASSERT_EQ(100, v9.at(0));
    ASSERT_EQ(200, v9.at(1));
    ASSERT_EQ(300, v9.at(2));

    Vector v10 = m.row(1);
    ASSERT_EQ(3, v10.length());
    ASSERT_EQ(1, v10.at(0));
    ASSERT_EQ(11, v10.at(1));
    ASSERT_EQ(12, v10.at(2));

    Vector v11 = m.row(2);
    ASSERT_EQ(3, v11.length());
    ASSERT_EQ(2, v11.at(0));
    ASSERT_EQ(21, v11.at(1));
    ASSERT_EQ(22, v11.at(2));

    Vector vi2(3);
    vi2.set(0, 1);
    vi2.set(1, 0);
    vi2.set(2, 3);

    m.prependColumn(vi2);

    ASSERT_EQ(3, m.rowCount());
    ASSERT_EQ(4, m.columnCount());
    ASSERT_EQ(1, m.get(0, 0));
    ASSERT_EQ(100, m.get(0, 1));
    ASSERT_EQ(200, m.get(0, 2));
    ASSERT_EQ(300, m.get(0, 3));
    ASSERT_EQ(0, m.get(1, 0));
    ASSERT_EQ(1, m.get(1, 1));
    ASSERT_EQ(11, m.get(1, 2));
    ASSERT_EQ(12, m.get(1, 3));
    ASSERT_EQ(3, m.get(2, 0));
    ASSERT_EQ(2, m.get(2, 1));
    ASSERT_EQ(21, m.get(2, 2));
    ASSERT_EQ(22, m.get(2, 3));

    Vector v12 = m.column(0);
    ASSERT_EQ(3, v12.length());
    ASSERT_EQ(1, v12.at(0));
    ASSERT_EQ(0, v12.at(1));
    ASSERT_EQ(3, v12.at(2));

    Vector v13 = m.column(1);
    ASSERT_EQ(3, v13.length());
    ASSERT_EQ(100, v13.at(0));
    ASSERT_EQ(1, v13.at(1));
    ASSERT_EQ(2, v13.at(2));

    Vector v14 = m.column(2);
    ASSERT_EQ(3, v14.length());
    ASSERT_EQ(200, v14.at(0));
    ASSERT_EQ(11, v14.at(1));
    ASSERT_EQ(21, v14.at(2));

    Vector v15 = m.column(3);
    ASSERT_EQ(3, v15.length());
    ASSERT_EQ(300, v15.at(0));
    ASSERT_EQ(12, v15.at(1));
    ASSERT_EQ(22, v15.at(2));

    Vector v16 = m.row(0);
    ASSERT_EQ(4, v16.length());
    ASSERT_EQ(1, v16.at(0));
    ASSERT_EQ(100, v16.at(1));
    ASSERT_EQ(200, v16.at(2));
    ASSERT_EQ(300, v16.at(3));

    Vector v17 = m.row(1);
    ASSERT_EQ(4, v17.length());
    ASSERT_EQ(0, v17.at(0));
    ASSERT_EQ(1, v17.at(1));
    ASSERT_EQ(11, v17.at(2));
    ASSERT_EQ(12, v17.at(3));

    Vector v18 = m.row(2);
    ASSERT_EQ(4, v18.length());
    ASSERT_EQ(3, v18.at(0));
    ASSERT_EQ(2, v18.at(1));
    ASSERT_EQ(21, v18.at(2));
    ASSERT_EQ(22, v18.at(3));

    Vector vr2(4);
    vr2.set(0, 1);
    vr2.set(1, 0);
    vr2.set(2, 1);
    vr2.set(3, 0);
    m.prependRow(vr2);

    ASSERT_EQ(4, m.rowCount());
    ASSERT_EQ(4, m.columnCount());
    ASSERT_EQ(1, m.get(0, 0));
    ASSERT_EQ(0, m.get(0, 1));
    ASSERT_EQ(1, m.get(0, 2));
    ASSERT_EQ(0, m.get(0, 3));
    ASSERT_EQ(1, m.get(1, 0));
    ASSERT_EQ(100, m.get(1, 1));
    ASSERT_EQ(200, m.get(1, 2));
    ASSERT_EQ(300, m.get(1, 3));
    ASSERT_EQ(0, m.get(2, 0));
    ASSERT_EQ(1, m.get(2, 1));
    ASSERT_EQ(11, m.get(2, 2));
    ASSERT_EQ(12, m.get(2, 3));
    ASSERT_EQ(3, m.get(3, 0));
    ASSERT_EQ(2, m.get(3, 1));
    ASSERT_EQ(21, m.get(3, 2));
    ASSERT_EQ(22, m.get(3, 3));

    Vector v19 = m.column(0);
    ASSERT_EQ(4, v19.length());
    ASSERT_EQ(1, v19.at(0));
    ASSERT_EQ(1, v19.at(1));
    ASSERT_EQ(0, v19.at(2));
    ASSERT_EQ(3, v19.at(3));

    Vector v20 = m.column(1);
    ASSERT_EQ(4, v20.length());
    ASSERT_EQ(0, v20.at(0));
    ASSERT_EQ(100, v20.at(1));
    ASSERT_EQ(1, v20.at(2));
    ASSERT_EQ(2, v20.at(3));

    Vector v21 = m.column(2);
    ASSERT_EQ(4, v21.length());
    ASSERT_EQ(1, v21.at(0));
    ASSERT_EQ(200, v21.at(1));
    ASSERT_EQ(11, v21.at(2));
    ASSERT_EQ(21, v21.at(3));

    Vector v22 = m.column(3);
    ASSERT_EQ(4, v22.length());
    ASSERT_EQ(0, v22.at(0));
    ASSERT_EQ(300, v22.at(1));
    ASSERT_EQ(12, v22.at(2));
    ASSERT_EQ(22, v22.at(3));

    Vector v23 = m.row(0);
    ASSERT_EQ(4, v23.length());
    ASSERT_EQ(1, v23.at(0));
    ASSERT_EQ(0, v23.at(1));
    ASSERT_EQ(1, v23.at(2));
    ASSERT_EQ(0, v23.at(3));

    Vector v24 = m.row(1);
    ASSERT_EQ(4, v24.length());
    ASSERT_EQ(1, v24.at(0));
    ASSERT_EQ(100, v24.at(1));
    ASSERT_EQ(200, v24.at(2));
    ASSERT_EQ(300, v24.at(3));

    Vector v25 = m.row(2);
    ASSERT_EQ(4, v25.length());
    ASSERT_EQ(0, v25.at(0));
    ASSERT_EQ(1, v25.at(1));
    ASSERT_EQ(11, v25.at(2));
    ASSERT_EQ(12, v25.at(3));

    Vector v26 = m.row(3);
    ASSERT_EQ(4, v26.length());
    ASSERT_EQ(3, v26.at(0));
    ASSERT_EQ(2, v26.at(1));
    ASSERT_EQ(21, v26.at(2));
    ASSERT_EQ(22, v26.at(3));

}

void MatrixTestSuite::prependEmpty()
{
    Matrix m(2, 3);
    m.set(0, 0, 11);
    m.set(0, 1, 12);
    m.set(0, 2, 13);
    m.set(1, 0, 21);
    m.set(1, 1, 22);
    m.set(1, 2, 23);

    Vector emptyRow(4);
    Vector emptyColumn(2);

    m.prependColumn(emptyColumn);
    ASSERT_EQ(2, m.rowCount());
    ASSERT_EQ(4, m.columnCount());
    // 0 11 12 13
    // 0 21 22 23
    ASSERT_EQ(0, m.get(0, 0));
    ASSERT_EQ(11, m.get(0, 1));
    ASSERT_EQ(12, m.get(0, 2));
    ASSERT_EQ(13, m.get(0, 3));
    ASSERT_EQ(0, m.get(1, 0));
    ASSERT_EQ(21, m.get(1, 1));
    ASSERT_EQ(22, m.get(1, 2));
    ASSERT_EQ(23, m.get(1, 3));

    Vector v1 = m.column(0);
    ASSERT_EQ(2, v1.length());
    ASSERT_EQ(0, v1.at(0));
    ASSERT_EQ(0, v1.at(1));

    Vector v2 = m.column(1);
    ASSERT_EQ(2, v2.length());
    ASSERT_EQ(11, v2.at(0));
    ASSERT_EQ(21, v2.at(1));

    Vector v3 = m.column(2);
    ASSERT_EQ(2, v3.length());
    ASSERT_EQ(12, v3.at(0));
    ASSERT_EQ(22, v3.at(1));

    Vector v4 = m.column(3);
    ASSERT_EQ(2, v4.length());
    ASSERT_EQ(13, v4.at(0));
    ASSERT_EQ(23, v4.at(1));

    Vector v5 = m.row(0);
    ASSERT_EQ(4, v5.length());
    ASSERT_EQ(0, v5.at(0));
    ASSERT_EQ(11, v5.at(1));
    ASSERT_EQ(12, v5.at(2));
    ASSERT_EQ(13, v5.at(3));

    Vector v6 = m.row(1);
    ASSERT_EQ(4, v6.length());
    ASSERT_EQ(0, v6.at(0));
    ASSERT_EQ(21, v6.at(1));
    ASSERT_EQ(22, v6.at(2));
    ASSERT_EQ(23, v6.at(3));

    m.prependRow(emptyRow);
    // 0  0  0  0
    // 0 11 12 13
    // 0 21 22 23
    ASSERT_EQ(0, m.get(0, 0));
    ASSERT_EQ(0, m.get(0, 1));
    ASSERT_EQ(0, m.get(0, 2));
    ASSERT_EQ(0, m.get(0, 3));
    ASSERT_EQ(0, m.get(1, 0));
    ASSERT_EQ(11, m.get(1, 1));
    ASSERT_EQ(12, m.get(1, 2));
    ASSERT_EQ(13, m.get(1, 3));
    ASSERT_EQ(0, m.get(2, 0));
    ASSERT_EQ(21, m.get(2, 1));
    ASSERT_EQ(22, m.get(2, 2));
    ASSERT_EQ(23, m.get(2, 3));

    Vector v7 = m.column(0);
    ASSERT_EQ(3, v7.length());
    ASSERT_EQ(0, v7.at(0));
    ASSERT_EQ(0, v7.at(1));
    ASSERT_EQ(0, v7.at(2));

    Vector v8 = m.column(1);
    ASSERT_EQ(3, v8.length());
    ASSERT_EQ(0, v8.at(0));
    ASSERT_EQ(11, v8.at(1));
    ASSERT_EQ(21, v8.at(2));

    Vector v9 = m.column(2);
    ASSERT_EQ(3, v9.length());
    ASSERT_EQ(0, v9.at(0));
    ASSERT_EQ(12, v9.at(1));
    ASSERT_EQ(22, v9.at(2));

    Vector v10 = m.column(3);

    ASSERT_EQ(3, v10.length());
    ASSERT_EQ(0, v10.at(0));
    ASSERT_EQ(13, v10.at(1));
    ASSERT_EQ(23, v10.at(2));

    Vector v11 = m.row(0);
    ASSERT_EQ(4, v11.length());
    ASSERT_EQ(0, v11.at(0));
    ASSERT_EQ(0, v11.at(1));
    ASSERT_EQ(0, v11.at(2));
    ASSERT_EQ(0, v11.at(3));

    Vector v12 = m.row(1);
    ASSERT_EQ(4, v12.length());
    ASSERT_EQ(0, v12.at(0));
    ASSERT_EQ(11, v12.at(1));
    ASSERT_EQ(12, v12.at(2));
    ASSERT_EQ(13, v12.at(3));

    Vector v13 = m.row(2);
    ASSERT_EQ(4, v13.length());
    ASSERT_EQ(0, v13.at(0));
    ASSERT_EQ(21, v13.at(1));
    ASSERT_EQ(22, v13.at(2));
    ASSERT_EQ(23, v13.at(3));

}

void MatrixTestSuite::prependEmpty2()
{
    Matrix m(2, 3);
    m.set(0, 0, 11);
    m.set(0, 1, 12);
    m.set(0, 2, 13);
    m.set(1, 0, 21);
    m.set(1, 1, 22);
    m.set(1, 2, 23);

    m.insertEmptyColumn(0);
    ASSERT_EQ(2, m.rowCount());
    ASSERT_EQ(4, m.columnCount());
    // 0 11 12 13
    // 0 21 22 23
    ASSERT_EQ(0, m.get(0, 0));
    ASSERT_EQ(11, m.get(0, 1));
    ASSERT_EQ(12, m.get(0, 2));
    ASSERT_EQ(13, m.get(0, 3));
    ASSERT_EQ(0, m.get(1, 0));
    ASSERT_EQ(21, m.get(1, 1));
    ASSERT_EQ(22, m.get(1, 2));
    ASSERT_EQ(23, m.get(1, 3));

    Vector v1 = m.column(0);
    ASSERT_EQ(2, v1.length());
    ASSERT_EQ(0, v1.at(0));
    ASSERT_EQ(0, v1.at(1));

    Vector v2 = m.column(1);
    ASSERT_EQ(2, v2.length());
    ASSERT_EQ(11, v2.at(0));
    ASSERT_EQ(21, v2.at(1));

    Vector v3 = m.column(2);
    ASSERT_EQ(2, v3.length());
    ASSERT_EQ(12, v3.at(0));
    ASSERT_EQ(22, v3.at(1));

    Vector v4 = m.column(3);
    ASSERT_EQ(2, v4.length());
    ASSERT_EQ(13, v4.at(0));
    ASSERT_EQ(23, v4.at(1));

    Vector v5 = m.row(0);
    ASSERT_EQ(4, v5.length());
    ASSERT_EQ(0, v5.at(0));
    ASSERT_EQ(11, v5.at(1));
    ASSERT_EQ(12, v5.at(2));
    ASSERT_EQ(13, v5.at(3));

    Vector v6 = m.row(1);
    ASSERT_EQ(4, v6.length());
    ASSERT_EQ(0, v6.at(0));
    ASSERT_EQ(21, v6.at(1));
    ASSERT_EQ(22, v6.at(2));
    ASSERT_EQ(23, v6.at(3));

    m.insertEmptyRow(0);
    // 0  0  0  0
    // 0 11 12 13
    // 0 21 22 23
    ASSERT_EQ(0, m.get(0, 0));
    ASSERT_EQ(0, m.get(0, 1));
    ASSERT_EQ(0, m.get(0, 2));
    ASSERT_EQ(0, m.get(0, 3));
    ASSERT_EQ(0, m.get(1, 0));
    ASSERT_EQ(11, m.get(1, 1));
    ASSERT_EQ(12, m.get(1, 2));
    ASSERT_EQ(13, m.get(1, 3));
    ASSERT_EQ(0, m.get(2, 0));
    ASSERT_EQ(21, m.get(2, 1));
    ASSERT_EQ(22, m.get(2, 2));
    ASSERT_EQ(23, m.get(2, 3));

    Vector v7 = m.column(0);
    ASSERT_EQ(3, v7.length());
    ASSERT_EQ(0, v7.at(0));
    ASSERT_EQ(0, v7.at(1));
    ASSERT_EQ(0, v7.at(2));

    Vector v8 = m.column(1);
    ASSERT_EQ(3, v8.length());
    ASSERT_EQ(0, v8.at(0));
    ASSERT_EQ(11, v8.at(1));
    ASSERT_EQ(21, v8.at(2));

    Vector v9 = m.column(2);
    ASSERT_EQ(3, v9.length());
    ASSERT_EQ(0, v9.at(0));
    ASSERT_EQ(12, v9.at(1));
    ASSERT_EQ(22, v9.at(2));

    Vector v10 = m.column(3);

    ASSERT_EQ(3, v10.length());
    ASSERT_EQ(0, v10.at(0));
    ASSERT_EQ(13, v10.at(1));
    ASSERT_EQ(23, v10.at(2));

    Vector v11 = m.row(0);
    ASSERT_EQ(4, v11.length());
    ASSERT_EQ(0, v11.at(0));
    ASSERT_EQ(0, v11.at(1));
    ASSERT_EQ(0, v11.at(2));
    ASSERT_EQ(0, v11.at(3));

    Vector v12 = m.row(1);
    ASSERT_EQ(4, v12.length());
    ASSERT_EQ(0, v12.at(0));
    ASSERT_EQ(11, v12.at(1));
    ASSERT_EQ(12, v12.at(2));
    ASSERT_EQ(13, v12.at(3));

    Vector v13 = m.row(2);
    ASSERT_EQ(4, v13.length());
    ASSERT_EQ(0, v13.at(0));
    ASSERT_EQ(21, v13.at(1));
    ASSERT_EQ(22, v13.at(2));
    ASSERT_EQ(23, v13.at(3));

}

void MatrixTestSuite::append()
{
    Matrix m(2, 2);
    m.set(0, 0, 11);
    m.set(0, 1, 12);
    m.set(1, 0, 21);
    m.set(1, 1, 22);

    Vector vi1(2);
    vi1.set(0, 1);
    vi1.set(1, 2);

    m.appendColumn(vi1);

    ASSERT_EQ(2, m.rowCount());
    ASSERT_EQ(3, m.columnCount());
    ASSERT_EQ(1, m.get(0, 2));
    ASSERT_EQ(2, m.get(1, 2));
    ASSERT_EQ(11, m.get(0, 0));
    ASSERT_EQ(12, m.get(0, 1));
    ASSERT_EQ(21, m.get(1, 0));
    ASSERT_EQ(22, m.get(1, 1));

    Vector v1 = m.column(2);
    ASSERT_EQ(2, v1.length());
    ASSERT_EQ(1, v1.at(0));
    ASSERT_EQ(2, v1.at(1));

    Vector v2 = m.column(0);
    ASSERT_EQ(2, v2.length());
    ASSERT_EQ(11, v2.at(0));
    ASSERT_EQ(21, v2.at(1));

    Vector v3 = m.column(1);
    ASSERT_EQ(2, v3.length());
    ASSERT_EQ(12, v3.at(0));
    ASSERT_EQ(22, v3.at(1));

    Vector v4 = m.row(0);
    ASSERT_EQ(3, v4.length());
    ASSERT_EQ(1, v4.at(2));
    ASSERT_EQ(11, v4.at(0));
    ASSERT_EQ(12, v4.at(1));

    Vector v5 = m.row(1);
    ASSERT_EQ(3, v5.length());
    ASSERT_EQ(2, v5.at(2));
    ASSERT_EQ(21, v5.at(0));
    ASSERT_EQ(22, v5.at(1));

    Vector vr1(3);
    vr1.set(0, 100);
    vr1.set(1, 200);
    vr1.set(2, 300);

    m.appendRow(vr1);

    ASSERT_EQ(3, m.rowCount());
    ASSERT_EQ(3, m.columnCount());
    ASSERT_EQ(100, m.get(2, 0));
    ASSERT_EQ(1, m.get(0, 2));
    ASSERT_EQ(2, m.get(1, 2));
    ASSERT_EQ(200, m.get(2, 1));
    ASSERT_EQ(11, m.get(0, 0));
    ASSERT_EQ(12, m.get(0, 1));
    ASSERT_EQ(21, m.get(1, 0));
    ASSERT_EQ(22, m.get(1, 1));
    ASSERT_EQ(300, m.get(2, 2));

    Vector v6 = m.column(0);
    ASSERT_EQ(3, v6.length());
    ASSERT_EQ(100, v6.at(2));
    ASSERT_EQ(11, v6.at(0));
    ASSERT_EQ(21, v6.at(1));

    Vector v7 = m.column(1);
    ASSERT_EQ(3, v7.length());
    ASSERT_EQ(200, v7.at(2));
    ASSERT_EQ(12, v7.at(0));
    ASSERT_EQ(22, v7.at(1));

    Vector v8 = m.column(2);
    ASSERT_EQ(3, v8.length());
    ASSERT_EQ(300, v8.at(2));
    ASSERT_EQ(1, v8.at(0));
    ASSERT_EQ(2, v8.at(1));

    Vector v9 = m.row(2);
    ASSERT_EQ(3, v9.length());
    ASSERT_EQ(100, v9.at(0));
    ASSERT_EQ(200, v9.at(1));
    ASSERT_EQ(300, v9.at(2));

    Vector v10 = m.row(0);
    ASSERT_EQ(3, v10.length());
    ASSERT_EQ(1, v10.at(2));
    ASSERT_EQ(11, v10.at(0));
    ASSERT_EQ(12, v10.at(1));

    Vector v11 = m.row(1);
    ASSERT_EQ(3, v11.length());
    ASSERT_EQ(2, v11.at(2));
    ASSERT_EQ(21, v11.at(0));
    ASSERT_EQ(22, v11.at(1));

    Vector vi2(3);
    vi2.set(0, 1);
    vi2.set(1, 0);
    vi2.set(2, 3);

    m.appendColumn(vi2);

    ASSERT_EQ(3, m.rowCount());
    ASSERT_EQ(4, m.columnCount());
    ASSERT_EQ(11, m.get(0, 0));
    ASSERT_EQ(12, m.get(0, 1));
    ASSERT_EQ(1, m.get(0, 2));
    ASSERT_EQ(11, m.get(0, 0));
    ASSERT_EQ(21, m.get(1, 0));
    ASSERT_EQ(22, m.get(1, 1));
    ASSERT_EQ(2, m.get(1, 2));
    ASSERT_EQ(0, m.get(1, 3));
    ASSERT_EQ(100, m.get(2, 0));
    ASSERT_EQ(200, m.get(2, 1));
    ASSERT_EQ(300, m.get(2, 2));
    ASSERT_EQ(3, m.get(2, 3));

    Vector v12 = m.column(0);
    ASSERT_EQ(3, v12.length());
    ASSERT_EQ(11, v12.at(0));
    ASSERT_EQ(21, v12.at(1));
    ASSERT_EQ(100, v12.at(2));

    Vector v13 = m.column(1);
    ASSERT_EQ(3, v13.length());
    ASSERT_EQ(12, v13.at(0));
    ASSERT_EQ(22, v13.at(1));
    ASSERT_EQ(200, v13.at(2));

    Vector v14 = m.column(2);
    ASSERT_EQ(3, v14.length());
    ASSERT_EQ(1, v14.at(0));
    ASSERT_EQ(2, v14.at(1));
    ASSERT_EQ(300, v14.at(2));

    Vector v15 = m.column(3);
    ASSERT_EQ(3, v15.length());
    ASSERT_EQ(1, v15.at(0));
    ASSERT_EQ(0, v15.at(1));
    ASSERT_EQ(3, v15.at(2));

    Vector v16 = m.row(2);
    ASSERT_EQ(4, v16.length());
    ASSERT_EQ(3, v16.at(3));
    ASSERT_EQ(100, v16.at(0));
    ASSERT_EQ(200, v16.at(1));
    ASSERT_EQ(300, v16.at(2));

    Vector v17 = m.row(0);
    ASSERT_EQ(4, v17.length());
    ASSERT_EQ(11, v17.at(0));
    ASSERT_EQ(12, v17.at(1));
    ASSERT_EQ(1, v17.at(2));
    ASSERT_EQ(1, v17.at(3));

    Vector v18 = m.row(1);
    ASSERT_EQ(4, v18.length());
    ASSERT_EQ(21, v18.at(0));
    ASSERT_EQ(22, v18.at(1));
    ASSERT_EQ(2, v18.at(2));
    ASSERT_EQ(0, v18.at(3));

    Vector vr2(4);
    vr2.set(0, 1);
    vr2.set(1, 0);
    vr2.set(2, 1);
    vr2.set(3, 0);
    m.appendRow(vr2);

    ASSERT_EQ(4, m.rowCount());
    ASSERT_EQ(4, m.columnCount());
    ASSERT_EQ(11, m.get(0, 0));
    ASSERT_EQ(12, m.get(0, 1));
    ASSERT_EQ(1, m.get(0, 2));
    ASSERT_EQ(11, m.get(0, 0));
    ASSERT_EQ(21, m.get(1, 0));
    ASSERT_EQ(22, m.get(1, 1));
    ASSERT_EQ(2, m.get(1, 2));
    ASSERT_EQ(0, m.get(1, 3));
    ASSERT_EQ(100, m.get(2, 0));
    ASSERT_EQ(200, m.get(2, 1));
    ASSERT_EQ(300, m.get(2, 2));
    ASSERT_EQ(3, m.get(2, 3));
    ASSERT_EQ(1, m.get(3, 0));
    ASSERT_EQ(0, m.get(3, 1));
    ASSERT_EQ(1, m.get(3, 2));
    ASSERT_EQ(0, m.get(3, 3));

    Vector v19 = m.column(0);
    ASSERT_EQ(4, v19.length());
    ASSERT_EQ(11, v19.at(0));
    ASSERT_EQ(21, v19.at(1));
    ASSERT_EQ(100, v19.at(2));
    ASSERT_EQ(1, v19.at(3));

    Vector v20 = m.column(1);
    ASSERT_EQ(4, v20.length());
    ASSERT_EQ(12, v20.at(0));
    ASSERT_EQ(22, v20.at(1));
    ASSERT_EQ(200, v20.at(2));
    ASSERT_EQ(0, v20.at(3));

    Vector v21 = m.column(2);
    ASSERT_EQ(4, v21.length());
    ASSERT_EQ(1, v21.at(0));
    ASSERT_EQ(2, v21.at(1));
    ASSERT_EQ(300, v21.at(2));
    ASSERT_EQ(1, v21.at(3));

    Vector v22 = m.column(3);
    ASSERT_EQ(4, v22.length());
    ASSERT_EQ(1, v22.at(0));
    ASSERT_EQ(0, v22.at(1));
    ASSERT_EQ(3, v22.at(2));
    ASSERT_EQ(0, v22.at(3));

    Vector v23 = m.row(2);
    ASSERT_EQ(4, v23.length());
    ASSERT_EQ(3, v23.at(3));
    ASSERT_EQ(100, v23.at(0));
    ASSERT_EQ(200, v23.at(1));
    ASSERT_EQ(300, v23.at(2));

    Vector v24 = m.row(0);
    ASSERT_EQ(4, v24.length());
    ASSERT_EQ(11, v24.at(0));
    ASSERT_EQ(12, v24.at(1));
    ASSERT_EQ(1, v24.at(2));
    ASSERT_EQ(1, v24.at(3));

    Vector v25 = m.row(1);
    ASSERT_EQ(4, v25.length());
    ASSERT_EQ(21, v25.at(0));
    ASSERT_EQ(22, v25.at(1));
    ASSERT_EQ(2, v25.at(2));
    ASSERT_EQ(0, v25.at(3));

    Vector v26 = m.row(3);
    ASSERT_EQ(4, v26.length());
    ASSERT_EQ(1, v26.at(0));
    ASSERT_EQ(0, v26.at(1));
    ASSERT_EQ(1, v26.at(2));
    ASSERT_EQ(0, v26.at(3));

}

void MatrixTestSuite::appendEmpty()
{
    Matrix m(2, 3);
    m.set(0, 0, 11);
    m.set(0, 1, 12);
    m.set(0, 2, 13);
    m.set(1, 0, 21);
    m.set(1, 1, 22);
    m.set(1, 2, 23);

    Vector emptyRow(4);
    Vector emptyColumn(2);

    m.appendColumn(emptyColumn);
    ASSERT_EQ(2, m.rowCount());
    ASSERT_EQ(4, m.columnCount());
    // 11 12 13 0
    // 21 22 23 0
    ASSERT_EQ(11, m.get(0, 0));
    ASSERT_EQ(12, m.get(0, 1));
    ASSERT_EQ(13, m.get(0, 2));
    ASSERT_EQ(0, m.get(0, 3));
    ASSERT_EQ(21, m.get(1, 0));
    ASSERT_EQ(22, m.get(1, 1));
    ASSERT_EQ(23, m.get(1, 2));
    ASSERT_EQ(0, m.get(1, 3));

    Vector v1 = m.column(0);
    ASSERT_EQ(2, v1.length());
    ASSERT_EQ(11, v1.at(0));
    ASSERT_EQ(21, v1.at(1));

    Vector v2 = m.column(1);
    ASSERT_EQ(2, v2.length());
    ASSERT_EQ(12, v2.at(0));
    ASSERT_EQ(22, v2.at(1));

    Vector v3 = m.column(2);
    ASSERT_EQ(2, v3.length());
    ASSERT_EQ(13, v3.at(0));
    ASSERT_EQ(23, v3.at(1));

    Vector v4 = m.column(3);
    ASSERT_EQ(2, v4.length());
    ASSERT_EQ(0, v4.at(0));
    ASSERT_EQ(0, v4.at(1));


    Vector v5 = m.row(0);
    ASSERT_EQ(4, v5.length());
    ASSERT_EQ(11, v5.at(0));
    ASSERT_EQ(12, v5.at(1));
    ASSERT_EQ(13, v5.at(2));
    ASSERT_EQ(0, v5.at(3));

    Vector v6 = m.row(1);
    ASSERT_EQ(4, v6.length());
    ASSERT_EQ(21, v6.at(0));
    ASSERT_EQ(22, v6.at(1));
    ASSERT_EQ(23, v6.at(2));
    ASSERT_EQ(0, v6.at(3));

    m.appendRow(emptyRow);
    // 11 12 13 0
    // 21 22 23 0
    // 0  0  0  0    
    ASSERT_EQ(11, m.get(0, 0));
    ASSERT_EQ(12, m.get(0, 1));
    ASSERT_EQ(13, m.get(0, 2));
    ASSERT_EQ(0, m.get(0, 3));
    ASSERT_EQ(21, m.get(1, 0));
    ASSERT_EQ(22, m.get(1, 1));
    ASSERT_EQ(23, m.get(1, 2));
    ASSERT_EQ(0, m.get(1, 3));
    ASSERT_EQ(0, m.get(2, 0));
    ASSERT_EQ(0, m.get(2, 1));
    ASSERT_EQ(0, m.get(2, 2));
    ASSERT_EQ(0, m.get(2, 3));


    Vector v7 = m.column(0);
    ASSERT_EQ(3, v7.length());
    ASSERT_EQ(11, v7.at(0));
    ASSERT_EQ(21, v7.at(1));
    ASSERT_EQ(0, v7.at(2));

    Vector v8 = m.column(1);
    ASSERT_EQ(3, v8.length());
    ASSERT_EQ(12, v8.at(0));
    ASSERT_EQ(22, v8.at(1));
    ASSERT_EQ(0, v8.at(2));

    Vector v9 = m.column(2);
    ASSERT_EQ(3, v9.length());
    ASSERT_EQ(13, v9.at(0));
    ASSERT_EQ(23, v9.at(1));
    ASSERT_EQ(0, v9.at(2));

    Vector v10 = m.column(3);

    ASSERT_EQ(3, v10.length());
    ASSERT_EQ(0, v10.at(0));
    ASSERT_EQ(0, v10.at(1));
    ASSERT_EQ(0, v10.at(2));

    Vector v11 = m.row(0);
    ASSERT_EQ(4, v11.length());
    ASSERT_EQ(11, v11.at(0));
    ASSERT_EQ(12, v11.at(1));
    ASSERT_EQ(13, v11.at(2));
    ASSERT_EQ(0, v11.at(3));

    Vector v12 = m.row(1);
    ASSERT_EQ(4, v12.length());
    ASSERT_EQ(21, v12.at(0));
    ASSERT_EQ(22, v12.at(1));
    ASSERT_EQ(23, v12.at(2));
    ASSERT_EQ(0, v12.at(3));

    Vector v13 = m.row(2);
    ASSERT_EQ(4, v13.length());
    ASSERT_EQ(0, v13.at(0));
    ASSERT_EQ(0, v13.at(1));
    ASSERT_EQ(0, v13.at(2));
    ASSERT_EQ(0, v13.at(3));

}

void MatrixTestSuite::appendEmpty2()
{
    Matrix m(2, 3);
    m.set(0, 0, 11);
    m.set(0, 1, 12);
    m.set(0, 2, 13);
    m.set(1, 0, 21);
    m.set(1, 1, 22);
    m.set(1, 2, 23);

    m.insertEmptyColumn(3);
    ASSERT_EQ(2, m.rowCount());
    ASSERT_EQ(4, m.columnCount());
    // 11 12 13 0
    // 21 22 23 0
    ASSERT_EQ(11, m.get(0, 0));
    ASSERT_EQ(12, m.get(0, 1));
    ASSERT_EQ(13, m.get(0, 2));
    ASSERT_EQ(0, m.get(0, 3));
    ASSERT_EQ(21, m.get(1, 0));
    ASSERT_EQ(22, m.get(1, 1));
    ASSERT_EQ(23, m.get(1, 2));
    ASSERT_EQ(0, m.get(1, 3));

    Vector v1 = m.column(0);
    ASSERT_EQ(2, v1.length());
    ASSERT_EQ(11, v1.at(0));
    ASSERT_EQ(21, v1.at(1));

    Vector v2 = m.column(1);
    ASSERT_EQ(2, v2.length());
    ASSERT_EQ(12, v2.at(0));
    ASSERT_EQ(22, v2.at(1));

    Vector v3 = m.column(2);
    ASSERT_EQ(2, v3.length());
    ASSERT_EQ(13, v3.at(0));
    ASSERT_EQ(23, v3.at(1));

    Vector v4 = m.column(3);
    ASSERT_EQ(2, v4.length());
    ASSERT_EQ(0, v4.at(0));
    ASSERT_EQ(0, v4.at(1));


    Vector v5 = m.row(0);
    ASSERT_EQ(4, v5.length());
    ASSERT_EQ(11, v5.at(0));
    ASSERT_EQ(12, v5.at(1));
    ASSERT_EQ(13, v5.at(2));
    ASSERT_EQ(0, v5.at(3));

    Vector v6 = m.row(1);
    ASSERT_EQ(4, v6.length());
    ASSERT_EQ(21, v6.at(0));
    ASSERT_EQ(22, v6.at(1));
    ASSERT_EQ(23, v6.at(2));
    ASSERT_EQ(0, v6.at(3));

    m.insertEmptyRow(2);
    // 11 12 13 0
    // 21 22 23 0
    // 0  0  0  0    
    ASSERT_EQ(11, m.get(0, 0));
    ASSERT_EQ(12, m.get(0, 1));
    ASSERT_EQ(13, m.get(0, 2));
    ASSERT_EQ(0, m.get(0, 3));
    ASSERT_EQ(21, m.get(1, 0));
    ASSERT_EQ(22, m.get(1, 1));
    ASSERT_EQ(23, m.get(1, 2));
    ASSERT_EQ(0, m.get(1, 3));
    ASSERT_EQ(0, m.get(2, 0));
    ASSERT_EQ(0, m.get(2, 1));
    ASSERT_EQ(0, m.get(2, 2));
    ASSERT_EQ(0, m.get(2, 3));


    Vector v7 = m.column(0);
    ASSERT_EQ(3, v7.length());
    ASSERT_EQ(11, v7.at(0));
    ASSERT_EQ(21, v7.at(1));
    ASSERT_EQ(0, v7.at(2));

    Vector v8 = m.column(1);
    ASSERT_EQ(3, v8.length());
    ASSERT_EQ(12, v8.at(0));
    ASSERT_EQ(22, v8.at(1));
    ASSERT_EQ(0, v8.at(2));

    Vector v9 = m.column(2);
    ASSERT_EQ(3, v9.length());
    ASSERT_EQ(13, v9.at(0));
    ASSERT_EQ(23, v9.at(1));
    ASSERT_EQ(0, v9.at(2));

    Vector v10 = m.column(3);

    ASSERT_EQ(3, v10.length());
    ASSERT_EQ(0, v10.at(0));
    ASSERT_EQ(0, v10.at(1));
    ASSERT_EQ(0, v10.at(2));

    Vector v11 = m.row(0);
    ASSERT_EQ(4, v11.length());
    ASSERT_EQ(11, v11.at(0));
    ASSERT_EQ(12, v11.at(1));
    ASSERT_EQ(13, v11.at(2));
    ASSERT_EQ(0, v11.at(3));

    Vector v12 = m.row(1);
    ASSERT_EQ(4, v12.length());
    ASSERT_EQ(21, v12.at(0));
    ASSERT_EQ(22, v12.at(1));
    ASSERT_EQ(23, v12.at(2));
    ASSERT_EQ(0, v12.at(3));

    Vector v13 = m.row(2);
    ASSERT_EQ(4, v13.length());
    ASSERT_EQ(0, v13.at(0));
    ASSERT_EQ(0, v13.at(1));
    ASSERT_EQ(0, v13.at(2));
    ASSERT_EQ(0, v13.at(3));

}

void MatrixTestSuite::insertColumn()
{
    Matrix m(2, 3);
    m.set(0, 0, 11);
    m.set(0, 1, 12);
    m.set(0, 2, 13);
    m.set(1, 0, 21);
    m.set(1, 1, 22);
    m.set(1, 2, 23);

    Vector c1(2);
    c1.set(0, 100);
    c1.set(1, 200);

    Vector c2(2);
    c2.set(0, 300);
    c2.set(1, 0);

    Vector c3(2);

    m.insertColumn(1, c1);
    // 11 100 12 13
    // 21 200 22 23

    ASSERT_EQ(2, m.rowCount());
    ASSERT_EQ(4, m.columnCount());
    ASSERT_EQ(11, m.get(0, 0));
    ASSERT_EQ(100, m.get(0, 1));
    ASSERT_EQ(12, m.get(0, 2));
    ASSERT_EQ(13, m.get(0, 3));
    ASSERT_EQ(21, m.get(1, 0));
    ASSERT_EQ(200, m.get(1, 1));
    ASSERT_EQ(22, m.get(1, 2));
    ASSERT_EQ(23, m.get(1, 3));

    Vector v1 = m.column(0);
    ASSERT_EQ(2, v1.length());
    ASSERT_EQ(11, v1.at(0));
    ASSERT_EQ(21, v1.at(1));

    Vector v2 = m.column(1);
    ASSERT_EQ(2, v2.length());
    ASSERT_EQ(100, v2.at(0));
    ASSERT_EQ(200, v2.at(1));

    Vector v3 = m.column(2);
    ASSERT_EQ(2, v3.length());
    ASSERT_EQ(12, v3.at(0));
    ASSERT_EQ(22, v3.at(1));

    Vector v4 = m.column(3);
    ASSERT_EQ(2, v4.length());
    ASSERT_EQ(13, v4.at(0));
    ASSERT_EQ(23, v4.at(1));

    Vector v5 = m.row(0);
    ASSERT_EQ(4, v5.length());
    ASSERT_EQ(11, v5.at(0));
    ASSERT_EQ(100, v5.at(1));
    ASSERT_EQ(12, v5.at(2));
    ASSERT_EQ(13, v5.at(3));

    Vector v6 = m.row(1);
    ASSERT_EQ(4, v6.length());
    ASSERT_EQ(21, v6.at(0));
    ASSERT_EQ(200, v6.at(1));
    ASSERT_EQ(22, v6.at(2));
    ASSERT_EQ(23, v6.at(3));

    m.insertColumn(3, c2);
    // 11 100 12 300 13
    // 21 200 22   0 23    

    ASSERT_EQ(2, m.rowCount());
    ASSERT_EQ(5, m.columnCount());
    ASSERT_EQ(11, m.get(0, 0));
    ASSERT_EQ(100, m.get(0, 1));
    ASSERT_EQ(12, m.get(0, 2));
    ASSERT_EQ(300, m.get(0, 3));
    ASSERT_EQ(13, m.get(0, 4));
    ASSERT_EQ(21, m.get(1, 0));
    ASSERT_EQ(200, m.get(1, 1));
    ASSERT_EQ(22, m.get(1, 2));
    ASSERT_EQ(0, m.get(1, 3));
    ASSERT_EQ(23, m.get(1, 4));

    Vector v7 = m.column(0);
    ASSERT_EQ(2, v7.length());
    ASSERT_EQ(11, v7.at(0));
    ASSERT_EQ(21, v7.at(1));

    Vector v8 = m.column(1);
    ASSERT_EQ(2, v8.length());
    ASSERT_EQ(100, v8.at(0));
    ASSERT_EQ(200, v8.at(1));

    Vector v9 = m.column(2);
    ASSERT_EQ(2, v9.length());
    ASSERT_EQ(12, v9.at(0));
    ASSERT_EQ(22, v9.at(1));

    Vector v10 = m.column(3);
    ASSERT_EQ(2, v10.length());
    ASSERT_EQ(300, v10.at(0));
    ASSERT_EQ(0, v10.at(1));

    Vector v11 = m.column(4);
    ASSERT_EQ(2, v11.length());
    ASSERT_EQ(13, v11.at(0));
    ASSERT_EQ(23, v11.at(1));

    Vector v12 = m.row(0);
    ASSERT_EQ(5, v12.length());
    ASSERT_EQ(11, v12.at(0));
    ASSERT_EQ(100, v12.at(1));
    ASSERT_EQ(12, v12.at(2));
    ASSERT_EQ(300, v12.at(3));
    ASSERT_EQ(13, v12.at(4));

    Vector v13 = m.row(1);
    ASSERT_EQ(5, v13.length());
    ASSERT_EQ(21, v13.at(0));
    ASSERT_EQ(200, v13.at(1));
    ASSERT_EQ(22, v13.at(2));
    ASSERT_EQ(0, v13.at(3));
    ASSERT_EQ(23, v13.at(4));

    m.insertColumn(2, c3);
    // 11 100 0 12 300 13
    // 21 200 0 22   0 23

    ASSERT_EQ(2, m.rowCount());
    ASSERT_EQ(6, m.columnCount());
    ASSERT_EQ(11, m.get(0, 0));
    ASSERT_EQ(100, m.get(0, 1));
    ASSERT_EQ(0, m.get(0, 2));
    ASSERT_EQ(12, m.get(0, 3));
    ASSERT_EQ(300, m.get(0, 4));
    ASSERT_EQ(13, m.get(0, 5));
    ASSERT_EQ(21, m.get(1, 0));
    ASSERT_EQ(200, m.get(1, 1));
    ASSERT_EQ(0, m.get(1, 2));
    ASSERT_EQ(22, m.get(1, 3));
    ASSERT_EQ(0, m.get(1, 4));
    ASSERT_EQ(23, m.get(1, 5));

    Vector v14 = m.column(0);
    ASSERT_EQ(2, v14.length());
    ASSERT_EQ(11, v14.at(0));
    ASSERT_EQ(21, v14.at(1));

    Vector v15 = m.column(1);
    ASSERT_EQ(2, v15.length());
    ASSERT_EQ(100, v15.at(0));
    ASSERT_EQ(200, v15.at(1));

    Vector v16 = m.column(2);
    ASSERT_EQ(2, v16.length());
    ASSERT_EQ(0, v16.at(0));
    ASSERT_EQ(0, v16.at(1));

    Vector v17 = m.column(3);
    ASSERT_EQ(2, v17.length());
    ASSERT_EQ(12, v17.at(0));
    ASSERT_EQ(22, v9.at(1));

    Vector v18 = m.column(4);
    ASSERT_EQ(2, v18.length());
    ASSERT_EQ(300, v18.at(0));
    ASSERT_EQ(0, v18.at(1));

    Vector v19 = m.column(5);
    ASSERT_EQ(2, v19.length());
    ASSERT_EQ(13, v19.at(0));
    ASSERT_EQ(23, v19.at(1));

    Vector v20 = m.row(0);
    ASSERT_EQ(6, v20.length());
    ASSERT_EQ(11, v20.at(0));
    ASSERT_EQ(100, v20.at(1));
    ASSERT_EQ(0, v20.at(2));
    ASSERT_EQ(12, v20.at(3));
    ASSERT_EQ(300, v20.at(4));
    ASSERT_EQ(13, v20.at(5));

    Vector v21 = m.row(1);
    ASSERT_EQ(6, v21.length());
    ASSERT_EQ(21, v21.at(0));
    ASSERT_EQ(200, v21.at(1));
    ASSERT_EQ(0, v21.at(2));
    ASSERT_EQ(22, v21.at(3));
    ASSERT_EQ(0, v21.at(4));
    ASSERT_EQ(23, v21.at(5));

}

void MatrixTestSuite::insertEmptyColumn()
{
    Matrix m(2, 3);
    m.set(0, 0, 11);
    m.set(0, 1, 12);
    m.set(0, 2, 13);
    m.set(1, 0, 21);
    m.set(1, 1, 22);
    m.set(1, 2, 23);

    m.insertEmptyColumn(1);
    ASSERT_EQ(2, m.rowCount());
    ASSERT_EQ(4, m.columnCount());
    // 11 0 12 13
    // 21 0 22 23
    ASSERT_EQ(11, m.get(0, 0));
    ASSERT_EQ(0, m.get(0, 1));
    ASSERT_EQ(12, m.get(0, 2));
    ASSERT_EQ(13, m.get(0, 3));
    ASSERT_EQ(21, m.get(1, 0));
    ASSERT_EQ(0, m.get(1, 1));
    ASSERT_EQ(22, m.get(1, 2));
    ASSERT_EQ(23, m.get(1, 3));

    Vector v1 = m.column(0);
    ASSERT_EQ(2, v1.length());
    ASSERT_EQ(11, v1.at(0));
    ASSERT_EQ(21, v1.at(1));

    Vector v2 = m.column(1);
    ASSERT_EQ(2, v2.length());
    ASSERT_EQ(0, v2.at(0));
    ASSERT_EQ(0, v2.at(1));

    Vector v3 = m.column(2);
    ASSERT_EQ(2, v3.length());
    ASSERT_EQ(12, v3.at(0));
    ASSERT_EQ(22, v3.at(1));

    Vector v4 = m.column(3);
    ASSERT_EQ(2, v4.length());
    ASSERT_EQ(13, v4.at(0));
    ASSERT_EQ(23, v4.at(1));

    Vector v5 = m.row(0);
    ASSERT_EQ(4, v5.length());
    ASSERT_EQ(11, v5.at(0));
    ASSERT_EQ(0, v5.at(1));
    ASSERT_EQ(12, v5.at(2));
    ASSERT_EQ(13, v5.at(3));

    Vector v6 = m.row(1);
    ASSERT_EQ(4, v6.length());
    ASSERT_EQ(21, v6.at(0));
    ASSERT_EQ(0, v6.at(1));
    ASSERT_EQ(22, v6.at(2));
    ASSERT_EQ(23, v6.at(3));
}

void MatrixTestSuite::insertRow()
{
    Matrix m(2, 3);
    m.set(0, 0, 11);
    m.set(0, 1, 12);
    m.set(0, 2, 13);
    m.set(1, 0, 21);
    m.set(1, 1, 22);
    m.set(1, 2, 23);

    Vector r1(3);
    r1.set(0, 100);
    r1.set(1, 200);
    r1.set(2, 300);

    Vector r2(3);
    r2.set(0, 400);
    r2.set(1, 0);
    r2.set(2, 500);

    Vector r3(3);

    m.insertRow(1, r1);
    //  11  12  13
    // 100 200 300
    //  21  22  23
    ASSERT_EQ(3, m.rowCount());
    ASSERT_EQ(3, m.columnCount());
    ASSERT_EQ(11, m.get(0, 0));
    ASSERT_EQ(12, m.get(0, 1));
    ASSERT_EQ(13, m.get(0, 2));
    ASSERT_EQ(100, m.get(1, 0));
    ASSERT_EQ(200, m.get(1, 1));
    ASSERT_EQ(300, m.get(1, 2));
    ASSERT_EQ(21, m.get(2, 0));
    ASSERT_EQ(22, m.get(2, 1));
    ASSERT_EQ(23, m.get(2, 2));

    Vector v1 = m.column(0);
    ASSERT_EQ(3, v1.length());
    ASSERT_EQ(11, v1.at(0));
    ASSERT_EQ(100, v1.at(1));
    ASSERT_EQ(21, v1.at(2));

    Vector v2 = m.column(1);
    ASSERT_EQ(3, v2.length());
    ASSERT_EQ(12, v2.at(0));
    ASSERT_EQ(200, v2.at(1));
    ASSERT_EQ(22, v2.at(2));

    Vector v3 = m.column(2);
    ASSERT_EQ(3, v3.length());
    ASSERT_EQ(13, v3.at(0));
    ASSERT_EQ(300, v3.at(1));
    ASSERT_EQ(23, v3.at(2));

    Vector v4 = m.row(0);
    ASSERT_EQ(3, v4.length());
    ASSERT_EQ(11, v4.at(0));
    ASSERT_EQ(12, v4.at(1));
    ASSERT_EQ(13, v4.at(2));

    Vector v5 = m.row(1);
    ASSERT_EQ(3, v5.length());
    ASSERT_EQ(100, v5.at(0));
    ASSERT_EQ(200, v5.at(1));
    ASSERT_EQ(300, v5.at(2));

    Vector v6 = m.row(2);
    ASSERT_EQ(3, v6.length());
    ASSERT_EQ(21, v6.at(0));
    ASSERT_EQ(22, v6.at(1));
    ASSERT_EQ(23, v6.at(2));

    m.insertRow(2, r2);
    //  11  12  13
    // 100 200 300
    // 400   0 500
    //  21  22  23
    ASSERT_EQ(4, m.rowCount());
    ASSERT_EQ(3, m.columnCount());
    ASSERT_EQ(11, m.get(0, 0));
    ASSERT_EQ(12, m.get(0, 1));
    ASSERT_EQ(13, m.get(0, 2));
    ASSERT_EQ(100, m.get(1, 0));
    ASSERT_EQ(200, m.get(1, 1));
    ASSERT_EQ(300, m.get(1, 2));
    ASSERT_EQ(400, m.get(2, 0));
    ASSERT_EQ(0, m.get(2, 1));
    ASSERT_EQ(500, m.get(2, 2));
    ASSERT_EQ(21, m.get(3, 0));
    ASSERT_EQ(22, m.get(3, 1));
    ASSERT_EQ(23, m.get(3, 2));

    Vector v7 = m.column(0);
    ASSERT_EQ(4, v7.length());
    ASSERT_EQ(11, v7.at(0));
    ASSERT_EQ(100, v7.at(1));
    ASSERT_EQ(400, v7.at(2));
    ASSERT_EQ(21, v7.at(3));

    Vector v8 = m.column(1);
    ASSERT_EQ(4, v8.length());
    ASSERT_EQ(12, v8.at(0));
    ASSERT_EQ(200, v8.at(1));
    ASSERT_EQ(0, v8.at(2));
    ASSERT_EQ(22, v8.at(3));

    Vector v9 = m.column(2);
    ASSERT_EQ(4, v9.length());
    ASSERT_EQ(13, v9.at(0));
    ASSERT_EQ(300, v9.at(1));
    ASSERT_EQ(500, v9.at(2));
    ASSERT_EQ(23, v9.at(3));

    Vector v10 = m.row(0);
    ASSERT_EQ(3, v10.length());
    ASSERT_EQ(11, v10.at(0));
    ASSERT_EQ(12, v10.at(1));
    ASSERT_EQ(13, v10.at(2));

    Vector v11 = m.row(1);
    ASSERT_EQ(3, v11.length());
    ASSERT_EQ(100, v11.at(0));
    ASSERT_EQ(200, v11.at(1));
    ASSERT_EQ(300, v11.at(2));

    Vector v12 = m.row(2);
    ASSERT_EQ(3, v12.length());
    ASSERT_EQ(400, v12.at(0));
    ASSERT_EQ(0, v12.at(1));
    ASSERT_EQ(500, v12.at(2));


    Vector v13 = m.row(3);
    ASSERT_EQ(3, v13.length());
    ASSERT_EQ(21, v13.at(0));
    ASSERT_EQ(22, v13.at(1));
    ASSERT_EQ(23, v13.at(2));

    m.insertRow(3, r3);
    //  11  12  13
    // 100 200 300
    // 400   0 500
    //   0   0   0
    //  21  22  23
    ASSERT_EQ(5, m.rowCount());
    ASSERT_EQ(3, m.columnCount());
    ASSERT_EQ(11, m.get(0, 0));
    ASSERT_EQ(12, m.get(0, 1));
    ASSERT_EQ(13, m.get(0, 2));
    ASSERT_EQ(100, m.get(1, 0));
    ASSERT_EQ(200, m.get(1, 1));
    ASSERT_EQ(300, m.get(1, 2));
    ASSERT_EQ(400, m.get(2, 0));
    ASSERT_EQ(0, m.get(2, 1));
    ASSERT_EQ(500, m.get(2, 2));
    ASSERT_EQ(0, m.get(3, 0));
    ASSERT_EQ(0, m.get(3, 1));
    ASSERT_EQ(0, m.get(3, 2));
    ASSERT_EQ(21, m.get(4, 0));
    ASSERT_EQ(22, m.get(4, 1));
    ASSERT_EQ(23, m.get(4, 2));

    Vector v14 = m.column(0);
    ASSERT_EQ(5, v14.length());
    ASSERT_EQ(11, v14.at(0));
    ASSERT_EQ(100, v14.at(1));
    ASSERT_EQ(400, v14.at(2));
    ASSERT_EQ(0, v14.at(3));
    ASSERT_EQ(21, v14.at(4));

    Vector v15 = m.column(1);
    ASSERT_EQ(5, v15.length());
    ASSERT_EQ(12, v15.at(0));
    ASSERT_EQ(200, v15.at(1));
    ASSERT_EQ(0, v15.at(2));
    ASSERT_EQ(0, v15.at(3));
    ASSERT_EQ(22, v15.at(4));

    Vector v16 = m.column(2);
    ASSERT_EQ(5, v16.length());
    ASSERT_EQ(13, v16.at(0));
    ASSERT_EQ(300, v16.at(1));
    ASSERT_EQ(500, v16.at(2));
    ASSERT_EQ(0, v16.at(3));
    ASSERT_EQ(23, v16.at(4));

    Vector v17 = m.row(0);
    ASSERT_EQ(3, v17.length());
    ASSERT_EQ(11, v17.at(0));
    ASSERT_EQ(12, v17.at(1));
    ASSERT_EQ(13, v17.at(2));

    Vector v18 = m.row(1);
    ASSERT_EQ(3, v18.length());
    ASSERT_EQ(100, v18.at(0));
    ASSERT_EQ(200, v18.at(1));
    ASSERT_EQ(300, v18.at(2));

    Vector v19 = m.row(2);
    ASSERT_EQ(3, v19.length());
    ASSERT_EQ(400, v19.at(0));
    ASSERT_EQ(0, v19.at(1));
    ASSERT_EQ(500, v19.at(2));

    Vector v20 = m.row(3);
    ASSERT_EQ(3, v20.length());
    ASSERT_EQ(0, v20.at(0));
    ASSERT_EQ(0, v20.at(1));
    ASSERT_EQ(0, v20.at(2));

    Vector v21 = m.row(4);
    ASSERT_EQ(3, v21.length());
    ASSERT_EQ(21, v21.at(0));
    ASSERT_EQ(22, v21.at(1));
    ASSERT_EQ(23, v21.at(2));

}

void MatrixTestSuite::insertEmptyRow()
{
    Matrix m(2, 3);
    m.set(0, 0, 11);
    m.set(0, 1, 12);
    m.set(0, 2, 13);
    m.set(1, 0, 21);
    m.set(1, 1, 22);
    m.set(1, 2, 23);

    m.insertEmptyRow(1);
    ASSERT_EQ(3, m.rowCount());
    ASSERT_EQ(3, m.columnCount());
    // 11 12 13
    //  0  0  0
    // 21 22 23
    ASSERT_EQ(11, m.get(0, 0));
    ASSERT_EQ(12, m.get(0, 1));
    ASSERT_EQ(13, m.get(0, 2));
    ASSERT_EQ(0, m.get(1, 0));
    ASSERT_EQ(0, m.get(1, 1));
    ASSERT_EQ(0, m.get(1, 2));
    ASSERT_EQ(21, m.get(2, 0));
    ASSERT_EQ(22, m.get(2, 1));
    ASSERT_EQ(23, m.get(2, 2));

    Vector v1 = m.column(0);
    ASSERT_EQ(3, v1.length());
    ASSERT_EQ(11, v1.at(0));
    ASSERT_EQ(0, v1.at(1));
    ASSERT_EQ(21, v1.at(2));

    Vector v2 = m.column(1);
    ASSERT_EQ(3, v2.length());
    ASSERT_EQ(12, v2.at(0));
    ASSERT_EQ(0, v2.at(1));
    ASSERT_EQ(22, v2.at(2));

    Vector v3 = m.column(2);
    ASSERT_EQ(3, v3.length());
    ASSERT_EQ(13, v3.at(0));
    ASSERT_EQ(0, v3.at(1));
    ASSERT_EQ(23, v3.at(2));

    Vector v4 = m.row(0);
    ASSERT_EQ(3, v4.length());
    ASSERT_EQ(11, v4.at(0));
    ASSERT_EQ(12, v4.at(1));
    ASSERT_EQ(13, v4.at(2));

    Vector v5 = m.row(1);
    ASSERT_EQ(3, v5.length());
    ASSERT_EQ(0, v5.at(0));
    ASSERT_EQ(0, v5.at(1));
    ASSERT_EQ(0, v5.at(2));

    Vector v6 = m.row(2);
    ASSERT_EQ(3, v6.length());
    ASSERT_EQ(21, v6.at(0));
    ASSERT_EQ(22, v6.at(1));
    ASSERT_EQ(23, v6.at(2));
}

void MatrixTestSuite::removeRow()
{
    Matrix m(4, 2);
    m.set(0, 0, 11);
    m.set(0, 1, 12);
    m.set(1, 0, 21);
    m.set(1, 1, 22);
    m.set(2, 0, 31);
    m.set(2, 1, 32);
    m.set(3, 0, 41);
    m.set(3, 1, 42);

    m.removeRow(1);

    ASSERT_EQ(3, m.rowCount());
    ASSERT_EQ(2, m.columnCount());
    // 11 12
    // 31 32
    // 41 42
    ASSERT_EQ(11, m.get(0, 0));
    ASSERT_EQ(12, m.get(0, 1));
    ASSERT_EQ(31, m.get(1, 0));
    ASSERT_EQ(32, m.get(1, 1));
    ASSERT_EQ(41, m.get(2, 0));
    ASSERT_EQ(42, m.get(2, 1));

    Vector v1 = m.column(0);
    ASSERT_EQ(3, v1.length());
    ASSERT_EQ(11, v1.at(0));
    ASSERT_EQ(31, v1.at(1));
    ASSERT_EQ(41, v1.at(2));

    Vector v2 = m.column(1);
    ASSERT_EQ(3, v2.length());
    ASSERT_EQ(12, v2.at(0));
    ASSERT_EQ(32, v2.at(1));
    ASSERT_EQ(42, v2.at(2));

    Vector v3 = m.row(0);
    ASSERT_EQ(2, v3.length());
    ASSERT_EQ(11, v3.at(0));
    ASSERT_EQ(12, v3.at(1));

    Vector v4 = m.row(1);
    ASSERT_EQ(2, v4.length());
    ASSERT_EQ(31, v4.at(0));
    ASSERT_EQ(32, v4.at(1));

    Vector v5 = m.row(2);
    ASSERT_EQ(2, v5.length());
    ASSERT_EQ(41, v5.at(0));
    ASSERT_EQ(42, v5.at(1));

    m.removeRow(0);

    ASSERT_EQ(2, m.rowCount());
    ASSERT_EQ(2, m.columnCount());
    // 31 32
    // 41 42
    ASSERT_EQ(31, m.get(0, 0));
    ASSERT_EQ(32, m.get(0, 1));
    ASSERT_EQ(41, m.get(1, 0));
    ASSERT_EQ(42, m.get(1, 1));

    Vector v6 = m.column(0);
    ASSERT_EQ(2, v6.length());
    ASSERT_EQ(31, v6.at(0));
    ASSERT_EQ(41, v6.at(1));

    Vector v7 = m.column(1);
    ASSERT_EQ(2, v7.length());
    ASSERT_EQ(32, v7.at(0));
    ASSERT_EQ(42, v7.at(1));

    Vector v8 = m.row(0);
    ASSERT_EQ(2, v8.length());
    ASSERT_EQ(31, v8.at(0));
    ASSERT_EQ(32, v8.at(1));

    Vector v9 = m.row(1);
    ASSERT_EQ(2, v9.length());
    ASSERT_EQ(41, v9.at(0));
    ASSERT_EQ(42, v9.at(1));

    m.removeRow(1);

    ASSERT_EQ(1, m.rowCount());
    ASSERT_EQ(2, m.columnCount());
    // 31 32
    ASSERT_EQ(31, m.get(0, 0));
    ASSERT_EQ(32, m.get(0, 1));

    Vector v10 = m.column(0);
    ASSERT_EQ(1, v10.length());
    ASSERT_EQ(31, v10.at(0));

    Vector v11 = m.column(1);
    ASSERT_EQ(1, v11.length());
    ASSERT_EQ(32, v11.at(0));

    Vector v12 = m.row(0);
    ASSERT_EQ(2, v12.length());
    ASSERT_EQ(31, v12.at(0));
    ASSERT_EQ(32, v12.at(1));

    m.removeRow(0);
    ASSERT_EQ(0, m.rowCount());
    ASSERT_EQ(0, m.columnCount());
}

void MatrixTestSuite::removeColumn()
{
    Matrix m(2, 4);
    m.set(0, 0, 11);
    m.set(0, 1, 12);
    m.set(0, 2, 13);
    m.set(0, 3, 14);
    m.set(1, 0, 21);
    m.set(1, 1, 22);
    m.set(1, 2, 23);
    m.set(1, 3, 24);

    m.removeColumn(1);
    // 11 13 14
    // 21 23 24
    ASSERT_EQ(2, m.rowCount());
    ASSERT_EQ(3, m.columnCount());
    ASSERT_EQ(11, m.get(0, 0));
    ASSERT_EQ(13, m.get(0, 1));
    ASSERT_EQ(14, m.get(0, 2));
    ASSERT_EQ(21, m.get(1, 0));
    ASSERT_EQ(23, m.get(1, 1));
    ASSERT_EQ(24, m.get(1, 2));

    Vector v1 = m.column(0);
    ASSERT_EQ(2, v1.length());
    ASSERT_EQ(11, v1.at(0));
    ASSERT_EQ(21, v1.at(1));

    Vector v2 = m.column(1);
    ASSERT_EQ(2, v2.length());
    ASSERT_EQ(13, v2.at(0));
    ASSERT_EQ(23, v2.at(1));

    Vector v3 = m.column(2);
    ASSERT_EQ(2, v3.length());
    ASSERT_EQ(14, v3.at(0));
    ASSERT_EQ(24, v3.at(1));

    Vector v4 = m.row(0);
    ASSERT_EQ(3, v4.length());
    ASSERT_EQ(11, v4.at(0));
    ASSERT_EQ(13, v4.at(1));
    ASSERT_EQ(14, v4.at(2));

    Vector v5 = m.row(1);
    ASSERT_EQ(3, v5.length());
    ASSERT_EQ(21, v5.at(0));
    ASSERT_EQ(23, v5.at(1));
    ASSERT_EQ(24, v5.at(2));

    m.removeColumn(0);
    // 13 14
    // 23 24
    ASSERT_EQ(2, m.rowCount());
    ASSERT_EQ(2, m.columnCount());
    ASSERT_EQ(13, m.get(0, 0));
    ASSERT_EQ(14, m.get(0, 1));
    ASSERT_EQ(23, m.get(1, 0));
    ASSERT_EQ(24, m.get(1, 1));

    Vector v6 = m.column(0);
    ASSERT_EQ(2, v6.length());
    ASSERT_EQ(13, v6.at(0));
    ASSERT_EQ(23, v6.at(1));

    Vector v7 = m.column(1);
    ASSERT_EQ(2, v7.length());
    ASSERT_EQ(14, v7.at(0));
    ASSERT_EQ(24, v7.at(1));

    Vector v8 = m.row(0);
    ASSERT_EQ(2, v8.length());
    ASSERT_EQ(13, v8.at(0));
    ASSERT_EQ(14, v8.at(1));

    Vector v9 = m.row(1);
    ASSERT_EQ(2, v9.length());
    ASSERT_EQ(23, v9.at(0));
    ASSERT_EQ(24, v9.at(1));

    m.removeColumn(1);
    // 13
    // 23
    ASSERT_EQ(2, m.rowCount());
    ASSERT_EQ(1, m.columnCount());
    ASSERT_EQ(13, m.get(0, 0));
    ASSERT_EQ(23, m.get(1, 0));

    Vector v10 = m.column(0);
    ASSERT_EQ(2, v10.length());
    ASSERT_EQ(13, v10.at(0));
    ASSERT_EQ(23, v10.at(1));

    Vector v11 = m.row(0);
    ASSERT_EQ(1, v11.length());
    ASSERT_EQ(13, v11.at(0));

    Vector v12 = m.row(1);
    ASSERT_EQ(1, v12.length());
    ASSERT_EQ(23, v12.at(0));

    m.removeColumn(0);
    ASSERT_EQ(0, m.rowCount());
    ASSERT_EQ(0, m.columnCount());

}

void MatrixTestSuite::nonZerosAndDensity()
{
    Matrix m(3, 4);

    ASSERT_EQ(0, m.nonZeros());
    ASSERT_EQ(0.0, m.density());

    m.set(0, 0, 0);
    m.set(0, 1, 0);
    m.set(0, 2, 3);
    m.set(0, 3, 0);
    m.set(1, 0, 5);
    m.set(1, 1, 6);
    m.set(1, 2, 0);
    m.set(1, 3, 8);
    m.set(2, 0, 9);
    m.set(2, 1, 0);
    m.set(2, 2, 11);
    m.set(2, 3, 0);

    ASSERT_EQ(6, m.nonZeros());
    ASSERT_EQ(0.5, m.density());

}

void MatrixTestSuite::newNonzero()
{
    Matrix m(3, 4);
    m.setNewNonzero(0, 0, 1);
    m.setNewNonzero(0, 1, 2);
    m.setNewNonzero(0, 2, 3);
    m.setNewNonzero(0, 3, 4);
    m.setNewNonzero(1, 0, 5);
    m.setNewNonzero(1, 1, 6);
    m.setNewNonzero(1, 2, 7);
    m.setNewNonzero(1, 3, 8);
    m.setNewNonzero(2, 0, 9);
    m.setNewNonzero(2, 1, 10);
    m.setNewNonzero(2, 2, 11);
    m.setNewNonzero(2, 3, 12);

    ASSERT_EQ(1, m.get(0, 0));
    ASSERT_EQ(2, m.get(0, 1));
    ASSERT_EQ(3, m.get(0, 2));
    ASSERT_EQ(4, m.get(0, 3));

    ASSERT_EQ(5, m.get(1, 0));
    ASSERT_EQ(6, m.get(1, 1));
    ASSERT_EQ(7, m.get(1, 2));
    ASSERT_EQ(8, m.get(1, 3));

    ASSERT_EQ(9, m.get(2, 0));
    ASSERT_EQ(10, m.get(2, 1));
    ASSERT_EQ(11, m.get(2, 2));
    ASSERT_EQ(12, m.get(2, 3));

    Vector v1 = m.column(0);

    ASSERT_EQ(3, v1.length());
    ASSERT_EQ(1, v1.at(0));
    ASSERT_EQ(5, v1.at(1));
    ASSERT_EQ(9, v1.at(2));

    Vector v2 = m.column(1);

    ASSERT_EQ(3, v2.length());
    ASSERT_EQ(2, v2.at(0));
    ASSERT_EQ(6, v2.at(1));
    ASSERT_EQ(10, v2.at(2));

    Vector v3 = m.column(2);

    ASSERT_EQ(3, v3.length());
    ASSERT_EQ(3, v3.at(0));
    ASSERT_EQ(7, v3.at(1));
    ASSERT_EQ(11, v3.at(2));

    Vector v4 = m.column(3);

    ASSERT_EQ(3, v4.length());
    ASSERT_EQ(4, v4.at(0));
    ASSERT_EQ(8, v4.at(1));
    ASSERT_EQ(12, v4.at(2));

    Vector v5 = m.row(0);

    ASSERT_EQ(4, v5.length());
    ASSERT_EQ(1, v5.at(0));
    ASSERT_EQ(2, v5.at(1));
    ASSERT_EQ(3, v5.at(2));
    ASSERT_EQ(4, v5.at(3));

    Vector v6 = m.row(1);

    ASSERT_EQ(4, v6.length());
    ASSERT_EQ(5, v6.at(0));
    ASSERT_EQ(6, v6.at(1));
    ASSERT_EQ(7, v6.at(2));
    ASSERT_EQ(8, v6.at(3));

    Vector v7 = m.row(2);

    ASSERT_EQ(4, v7.length());
    ASSERT_EQ(9, v7.at(0));
    ASSERT_EQ(10, v7.at(1));
    ASSERT_EQ(11, v7.at(2));
    ASSERT_EQ(12, v7.at(3));
}

void MatrixTestSuite::addScalarMatrix()
{
    Matrix m(2, 3);
    m.set(0, 0, 11);
    m.set(0, 1, 12);
    m.set(0, 2, 13);
    m.set(1, 0, 21);
    m.set(1, 1, 22);
    m.set(1, 2, 23);

    Matrix result1 = 0.0 + m;
    ASSERT_EQ(2, result1.rowCount());
    ASSERT_EQ(3, result1.columnCount());
    ASSERT_EQ(11, result1.get(0, 0));
    ASSERT_EQ(12, result1.get(0, 1));
    ASSERT_EQ(13, result1.get(0, 2));
    ASSERT_EQ(21, result1.get(1, 0));
    ASSERT_EQ(22, result1.get(1, 1));
    ASSERT_EQ(23, result1.get(1, 2));

    Vector v1 = result1.column(0);
    ASSERT_EQ(2, v1.length());
    ASSERT_EQ(11, v1.at(0));
    ASSERT_EQ(21, v1.at(1));

    Vector v2 = result1.column(1);
    ASSERT_EQ(2, v2.length());
    ASSERT_EQ(12, v2.at(0));
    ASSERT_EQ(22, v2.at(1));

    Vector v3 = result1.column(2);
    ASSERT_EQ(2, v3.length());
    ASSERT_EQ(13, v3.at(0));
    ASSERT_EQ(23, v3.at(1));

    Vector v4 = result1.row(0);
    ASSERT_EQ(3, v4.length());
    ASSERT_EQ(11, v4.at(0));
    ASSERT_EQ(12, v4.at(1));
    ASSERT_EQ(13, v4.at(2));

    Vector v5 = result1.row(1);
    ASSERT_EQ(3, v5.length());
    ASSERT_EQ(21, v5.at(0));
    ASSERT_EQ(22, v5.at(1));
    ASSERT_EQ(23, v5.at(2));

    Matrix result2 = 10 + m;
    ASSERT_EQ(2, result2.rowCount());
    ASSERT_EQ(3, result2.columnCount());
    ASSERT_EQ(21, result2.get(0, 0));
    ASSERT_EQ(22, result2.get(0, 1));
    ASSERT_EQ(23, result2.get(0, 2));
    ASSERT_EQ(31, result2.get(1, 0));
    ASSERT_EQ(32, result2.get(1, 1));
    ASSERT_EQ(33, result2.get(1, 2));

    Vector v6 = result2.column(0);
    ASSERT_EQ(2, v6.length());
    ASSERT_EQ(21, v6.at(0));
    ASSERT_EQ(31, v6.at(1));

    Vector v7 = result2.column(1);
    ASSERT_EQ(2, v7.length());
    ASSERT_EQ(22, v7.at(0));
    ASSERT_EQ(32, v7.at(1));

    Vector v8 = result2.column(2);
    ASSERT_EQ(2, v8.length());
    ASSERT_EQ(23, v8.at(0));
    ASSERT_EQ(33, v8.at(1));

    Vector v9 = result2.row(0);
    ASSERT_EQ(3, v9.length());
    ASSERT_EQ(21, v9.at(0));
    ASSERT_EQ(22, v9.at(1));
    ASSERT_EQ(23, v9.at(2));

    Vector v10 = result2.row(1);
    ASSERT_EQ(3, v10.length());
    ASSERT_EQ(31, v10.at(0));
    ASSERT_EQ(32, v10.at(1));
    ASSERT_EQ(33, v10.at(2));

}

void MatrixTestSuite::sortElements()
{
    Numerical::Double oldRatio = SPARSITY_RATIO;
    SPARSITY_RATIO = 0.5;
    Matrix m(10, 8);
    //     0  1  2  3  4  5  6  7
    //    -------------------------
    // 0 | 0  0  0  0  0  0  0  0 | 0
    // 1 | 1  1  0  0  1  0  0  0 | 3
    // 2 | 1  0  0  0  0  0  0  1 | 2
    // 3 | 0  1  1  0  0  0  1  0 | 3
    // 4 | 1  0  0  0  0  0  1  0 | 2
    // 5 | 1  0  0  0  1  0  0  0 | 2
    // 6 | 0  1  1  0  1  0  0  1 | 4
    // 7 | 1  0  0  0  0  0  0  0 | 1
    // 8 | 0  0  1  0  0  0  1  0 | 2
    // 9 | 1  0  0  0  0  0  0  0 | 1
    //    ------------------------
    //     6  3  3  0  3  0  3  2
    m.set(8, 2, 1);
    m.set(1, 0, 1);
    m.set(6, 2, 1);
    m.set(1, 4, 1);
    m.set(2, 0, 1);
    m.set(6, 7, 1);
    m.set(1, 1, 1);
    m.set(6, 4, 1);
    m.set(7, 0, 1);
    m.set(6, 1, 1);
    m.set(4, 0, 1);
    m.set(3, 1, 1);
    m.set(4, 6, 1);
    m.set(3, 6, 1);
    m.set(5, 4, 1);
    m.set(8, 6, 1);
    m.set(3, 2, 1);
    m.set(9, 0, 1);
    m.set(2, 7, 1);
    m.set(5, 0, 1);

    m.sortVectors();
    ASSERT_EQ(10, m.rowCount());
    ASSERT_EQ(8, m.columnCount());
    Vector::NonzeroIterator iter = m.column(0).beginNonzero();
    // 1, 2, 4, 5, 7, 9
    ASSERT_EQ(1, iter.getIndex());
    iter++;
    ASSERT_EQ(2, iter.getIndex());
    iter++;
    ASSERT_EQ(4, iter.getIndex());
    iter++;
    ASSERT_EQ(5, iter.getIndex());
    iter++;
    ASSERT_EQ(7, iter.getIndex());
    iter++;
    ASSERT_EQ(9, iter.getIndex());

    iter = m.column(1).beginNonzero();
    // 1, 3, 6
    ASSERT_EQ(1, iter.getIndex());
    iter++;
    ASSERT_EQ(3, iter.getIndex());
    iter++;
    ASSERT_EQ(6, iter.getIndex());

    iter = m.column(2).beginNonzero();
    // 3, 6, 8
    ASSERT_EQ(3, iter.getIndex());
    iter++;
    ASSERT_EQ(6, iter.getIndex());
    iter++;
    ASSERT_EQ(8, iter.getIndex());

    iter = m.column(3).beginNonzero();
    ASSERT_TRUE(iter == m.column(3).endNonzero());

    iter = m.column(4).beginNonzero();
    // 1, 5, 6
    ASSERT_EQ(1, iter.getIndex());
    iter++;
    ASSERT_EQ(5, iter.getIndex());
    iter++;
    ASSERT_EQ(6, iter.getIndex());

    iter = m.column(5).beginNonzero();
    ASSERT_TRUE(iter == m.column(5).endNonzero());

    iter = m.column(6).beginNonzero();
    // 3, 4, 8
    ASSERT_EQ(3, iter.getIndex());
    iter++;
    ASSERT_EQ(4, iter.getIndex());
    iter++;
    ASSERT_EQ(8, iter.getIndex());

    iter = m.column(7).beginNonzero();
    // 2, 6
    ASSERT_EQ(2, iter.getIndex());
    iter++;
    ASSERT_EQ(6, iter.getIndex());

    iter = m.row(0).beginNonzero();
    ASSERT_TRUE(iter == m.row(0).endNonzero());

    iter = m.row(1).beginNonzero();
    // 0, 1, 4
    ASSERT_EQ(0, iter.getIndex());
    iter++;
    ASSERT_EQ(1, iter.getIndex());
    iter++;
    ASSERT_EQ(4, iter.getIndex());

    iter = m.row(2).beginNonzero();
    // 0, 7
    ASSERT_EQ(0, iter.getIndex());
    iter++;
    ASSERT_EQ(7, iter.getIndex());

    iter = m.row(3).beginNonzero();
    // 1, 2, 6
    ASSERT_EQ(1, iter.getIndex());
    iter++;
    ASSERT_EQ(2, iter.getIndex());
    iter++;
    ASSERT_EQ(6, iter.getIndex());

    iter = m.row(4).beginNonzero();
    // 0, 6
    ASSERT_EQ(0, iter.getIndex());
    iter++;
    ASSERT_EQ(6, iter.getIndex());

    iter = m.row(5).beginNonzero();
    // 0, 4
    ASSERT_EQ(0, iter.getIndex());
    iter++;
    ASSERT_EQ(4, iter.getIndex());

    iter = m.row(6).beginNonzero();
    // 1, 2, 4, 7
    ASSERT_EQ(1, iter.getIndex());
    iter++;
    ASSERT_EQ(2, iter.getIndex());
    iter++;
    ASSERT_EQ(4, iter.getIndex());
    iter++;
    ASSERT_EQ(7, iter.getIndex());

    iter = m.row(7).beginNonzero();
    // 0
    ASSERT_EQ(0, iter.getIndex());

    iter = m.row(8).beginNonzero();
    // 2, 6
    ASSERT_EQ(2, iter.getIndex());
    iter++;
    ASSERT_EQ(6, iter.getIndex());

    iter = m.row(9).beginNonzero();
    // 0
    ASSERT_EQ(0, iter.getIndex());


    SPARSITY_RATIO = oldRatio;
}

void MatrixTestSuite::determinant()
{
    Matrix m(3, 3);
    m.set(0, 0, -2);
    m.set(0, 1, 2);
    m.set(0, 2, 3);
    m.set(1, 0, -1);
    m.set(1, 1, 1);
    m.set(1, 2, 3);
    m.set(2, 0, 2);
    m.set(2, 1, 0);
    m.set(2, 2, -1);

    ASSERT_EQ(6, m.determinant());

    Matrix m2(2, 3);
    ASSERT_EQ(0, m2.determinant());

    Matrix m3(1, 1);
    m3.set(0, 0, 4);
    ASSERT_EQ(4, m3.determinant());

    Matrix m4(3, 3);
    m3.set(0, 0, 3);
    ASSERT_EQ(0, m4.determinant());
}

void MatrixTestSuite::reInit()
{
    Matrix m(3, 3);
    m.set(0, 0, -2);
    m.set(0, 1, 2);
    m.set(0, 2, 3);
    m.set(1, 0, -1);
    m.set(1, 1, 1);
    m.set(1, 2, 3);
    m.set(2, 0, 2);
    m.set(2, 1, 0);
    m.set(2, 2, -1);

    m.reInit(2, 3);
    ASSERT_EQ(2, m.rowCount());
    ASSERT_EQ(3, m.columnCount());
    ASSERT_EQ(0, m.get(0, 0));
    ASSERT_EQ(0, m.get(0, 1));
    ASSERT_EQ(0, m.get(0, 2));
    ASSERT_EQ(0, m.get(1, 0));
    ASSERT_EQ(0, m.get(1, 1));
    ASSERT_EQ(0, m.get(1, 2));

    Vector v1 = m.row(0);
    ASSERT_EQ(3, v1.length());
    ASSERT_EQ(0, v1.at(0));
    ASSERT_EQ(0, v1.at(1));
    ASSERT_EQ(0, v1.at(2));

    Vector v2 = m.row(1);
    ASSERT_EQ(3, v2.length());
    ASSERT_EQ(0, v2.at(0));
    ASSERT_EQ(0, v2.at(1));
    ASSERT_EQ(0, v2.at(2));

    Vector v3 = m.column(0);
    ASSERT_EQ(2, v3.length());
    ASSERT_EQ(0, v3.at(0));
    ASSERT_EQ(0, v3.at(1));

    Vector v4 = m.column(1);
    ASSERT_EQ(2, v4.length());
    ASSERT_EQ(0, v4.at(0));
    ASSERT_EQ(0, v4.at(1));

    Vector v5 = m.column(2);
    ASSERT_EQ(2, v5.length());
    ASSERT_EQ(0, v5.at(0));
    ASSERT_EQ(0, v5.at(1));

    m.reInit(2, 3, false);
    ASSERT_EQ(2, m.rowCount());
    ASSERT_EQ(3, m.columnCount());

    Vector v6 = m.row(0);
    ASSERT_EQ(0, v6.getData());

    Vector v7 = m.row(1);
    ASSERT_EQ(0, v7.getData());

    Vector v8 = m.column(0);
    ASSERT_EQ(0, v8.getData());

    Vector v9 = m.column(1);
    ASSERT_EQ(0, v9.getData());

    Vector v10 = m.column(2);
    ASSERT_EQ(0, v10.getData());
}

void MatrixTestSuite::gaussian()
{
    Matrix m(3, 4);
    m.set(0, 0, 2);
    m.set(0, 1, 1);
    m.set(0, 2, -1);
    m.set(0, 3, 8);

    m.set(1, 0, -3);
    m.set(1, 1, -1);
    m.set(1, 2, 2);
    m.set(1, 3, -11);

    m.set(2, 0, -2);
    m.set(2, 1, 1);
    m.set(2, 2, 2);
    m.set(2, 3, -3);

    m.gaussianElimination(false);
    //2         1        -1         8
    //0       0.5       0.5         1
    //0         0        -1         1
    ASSERT_EQ(3, m.rowCount());
    ASSERT_EQ(4, m.columnCount());
    ASSERT_EQ(2, m.get(0, 0));
    ASSERT_EQ(1, m.get(0, 1));
    ASSERT_EQ(-1, m.get(0, 2));
    ASSERT_EQ(8, m.get(0, 3));

    ASSERT_EQ(0, m.get(1, 0));
    ASSERT_EQ(0.5, m.get(1, 1));
    ASSERT_EQ(0.5, m.get(1, 2));
    ASSERT_EQ(1, m.get(1, 3));

    ASSERT_EQ(0, m.get(2, 0));
    ASSERT_EQ(0, m.get(2, 1));
    ASSERT_EQ(-1, m.get(2, 2));
    ASSERT_EQ(1, m.get(2, 3));
    
    Vector v1 = m.row(0);
    ASSERT_EQ(4, v1.length());
    ASSERT_EQ(2, v1.at(0));
    ASSERT_EQ(1, v1.at(1));
    ASSERT_EQ(-1, v1.at(2));
    ASSERT_EQ(8, v1.at(3));

    Vector v2 = m.row(1);
    ASSERT_EQ(4, v2.length());
    ASSERT_EQ(0, v2.at(0));
    ASSERT_EQ(0.5, v2.at(1));
    ASSERT_EQ(0.5, v2.at(2));
    ASSERT_EQ(1, v2.at(3));
    
    Vector v3 = m.row(2);
    ASSERT_EQ(4, v3.length());
    ASSERT_EQ(0, v3.at(0));
    ASSERT_EQ(0, v3.at(1));
    ASSERT_EQ(-1, v3.at(2));
    ASSERT_EQ(1, v3.at(3));   
    
    //2         1        -1         8
    //0       0.5       0.5         1
    //0         0        -1         1        
   
    m.set(0, 0, 2);
    m.set(0, 1, 1);
    m.set(0, 2, -1);
    m.set(0, 3, 8);

    m.set(1, 0, -3);
    m.set(1, 1, -1);
    m.set(1, 2, 2);
    m.set(1, 3, -11);

    m.set(2, 0, -2);
    m.set(2, 1, 1);
    m.set(2, 2, 2);
    m.set(2, 3, -3);

    m.gaussianElimination(true);    
    ASSERT_EQ(3, m.rowCount());
    ASSERT_EQ(4, m.columnCount());
    ASSERT_EQ(1, m.get(0, 0));
    ASSERT_EQ(0, m.get(0, 1));
    ASSERT_EQ(0, m.get(0, 2));
    ASSERT_EQ(2, m.get(0, 3));

    ASSERT_EQ(0, m.get(1, 0));
    ASSERT_EQ(1, m.get(1, 1));
    ASSERT_EQ(0, m.get(1, 2));
    ASSERT_EQ(3, m.get(1, 3));

    ASSERT_EQ(0, m.get(2, 0));
    ASSERT_EQ(0, m.get(2, 1));
    ASSERT_EQ(1, m.get(2, 2));
    ASSERT_EQ(-1, m.get(2, 3));
    
    Vector v4 = m.row(0);
    ASSERT_EQ(4, v4.length());
    ASSERT_EQ(1, v4.at(0));
    ASSERT_EQ(0, v4.at(1));
    ASSERT_EQ(0, v4.at(2));
    ASSERT_EQ(2, v4.at(3));

    Vector v5 = m.row(1);
    ASSERT_EQ(4, v5.length());
    ASSERT_EQ(0, v5.at(0));
    ASSERT_EQ(1, v5.at(1));
    ASSERT_EQ(0, v5.at(2));
    ASSERT_EQ(3, v5.at(3));
    
    Vector v6 = m.row(2);
    ASSERT_EQ(4, v6.length());
    ASSERT_EQ(0, v6.at(0));
    ASSERT_EQ(0, v6.at(1));
    ASSERT_EQ(1, v6.at(2));
    ASSERT_EQ(-1, v6.at(3));   
}