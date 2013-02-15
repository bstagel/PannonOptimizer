/* 
 * File:   matrixtest.h
 * Author: smidla
 *
 * Created on 2013. február 11., 12:30
 */

#ifndef MATRIXTEST_H
#define	MATRIXTEST_H

#include <cpptest.h>
#include <linalg/matrix.h>

class MatrixTestSuite : public Test::Suite
{
public:

    MatrixTestSuite()
    {
        TEST_ADD(MatrixTestSuite::equal)
        TEST_ADD(MatrixTestSuite::copy)
        TEST_ADD(MatrixTestSuite::assign)
        TEST_ADD(MatrixTestSuite::transposed)
        TEST_ADD(MatrixTestSuite::add)
        TEST_ADD(MatrixTestSuite::addToThis)
        TEST_ADD(MatrixTestSuite::subtract)
        TEST_ADD(MatrixTestSuite::subtractFromThis)
        TEST_ADD(MatrixTestSuite::multipleByVector)
        TEST_ADD(MatrixTestSuite::multipleByDouble)
        TEST_ADD(MatrixTestSuite::multipleByMatrix_1)
        TEST_ADD(MatrixTestSuite::multipleByMatrix_2)
        TEST_ADD(MatrixTestSuite::multipleByDiagonalMatrix)
        TEST_ADD(MatrixTestSuite::inverse)
        TEST_ADD(MatrixTestSuite::diagonalMatrix)
        TEST_ADD(MatrixTestSuite::diagonalMatrix_Inverse)
        TEST_ADD(MatrixTestSuite::diagonalMatrix_Invert)
        TEST_ADD(MatrixTestSuite::rowVector)
        TEST_ADD(MatrixTestSuite::columnVector)
        TEST_ADD(MatrixTestSuite::comprehensive_1)
        TEST_ADD(MatrixTestSuite::constructor)
        TEST_ADD(MatrixTestSuite::setGet)
        TEST_ADD(MatrixTestSuite::columnWise)
        TEST_ADD(MatrixTestSuite::rowWise)
        TEST_ADD(MatrixTestSuite::clear)
        TEST_ADD(MatrixTestSuite::resize)
        TEST_ADD(MatrixTestSuite::scale)
        TEST_ADD(MatrixTestSuite::scaleRow)
        TEST_ADD(MatrixTestSuite::scaleColumn)
        TEST_ADD(MatrixTestSuite::prepend)
        TEST_ADD(MatrixTestSuite::prependEmpty)
        TEST_ADD(MatrixTestSuite::prependEmpty2)
        TEST_ADD(MatrixTestSuite::append)
        TEST_ADD(MatrixTestSuite::appendEmpty)
        TEST_ADD(MatrixTestSuite::appendEmpty2)
        TEST_ADD(MatrixTestSuite::insertColumn)
        TEST_ADD(MatrixTestSuite::insertEmptyColumn)
        TEST_ADD(MatrixTestSuite::insertRow)
        TEST_ADD(MatrixTestSuite::insertEmptyRow)
        TEST_ADD(MatrixTestSuite::removeRow)
        TEST_ADD(MatrixTestSuite::removeColumn)
        TEST_ADD(MatrixTestSuite::nonZerosAndDensity)
        TEST_ADD(MatrixTestSuite::newNonzero)
        TEST_ADD(MatrixTestSuite::addScalarMatrix)
        TEST_ADD(MatrixTestSuite::sortElements)
        TEST_ADD(MatrixTestSuite::determinant)
        TEST_ADD(MatrixTestSuite::reInit)
    }

private:
    void equal();
    void copy();
    void assign();
    void transposed();
    void add();
    void addToThis();
    void subtract();
    void subtractFromThis();
    void multipleByVector();
    void multipleByDouble();
    void multipleByMatrix_1();
    void multipleByMatrix_2();
    void multipleByDiagonalMatrix();
    void inverse();
    void diagonalMatrix();
    void diagonalMatrix_Inverse();
    void diagonalMatrix_Invert();
    void rowVector();
    void columnVector();
    void comprehensive_1();
    void constructor();
    void setGet();
    void columnWise();
    void rowWise();
    void clear();
    void resize();
    void scale();
    void scaleRow();
    void scaleColumn();
    void prepend();
    void prependEmpty();
    void prependEmpty2();
    void append();
    void appendEmpty();
    void appendEmpty2();
    void insertColumn();
    void insertEmptyColumn();
    void insertRow();
    void insertEmptyRow();
    void removeRow();
    void removeColumn();
    void nonZerosAndDensity();
    void newNonzero();
    void addScalarMatrix();
    void sortElements();
    void determinant();
    void reInit();


};

#endif	/* MATRIXTEST_H */

