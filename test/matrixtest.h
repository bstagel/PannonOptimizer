/* 
 * File:   matrixtest.h
 * Author: smidla
 *
 * Created on 2013. február 11., 12:30
 */

#ifndef MATRIXTEST_H
#define	MATRIXTEST_H

#include <sstream>
#include "framework/tester.h"
#include "framework/unittest.h"
#include <linalg/matrix.h>

class MatrixTestSuite : public UnitTest
{
public:

    MatrixTestSuite(const char * name);

private:

    class MatrixTester : public UnitTest::DefaultTester
    {
    public:

        void test()
        {
            const unsigned int elbowRooms[] = {0, 1, 2, 3, 4, 5, 10, 20, 100};
            const unsigned int count = sizeof (elbowRooms) / sizeof (unsigned int);
            double originalSparsityRatio = SPARSITY_RATIO;
            unsigned int originalElbowRoom = ELBOWROOM;
            unsigned int ratioIndex;
            unsigned int elbowRoomIndex;
            const unsigned int N = 20;
            for (elbowRoomIndex = 0; elbowRoomIndex < count; elbowRoomIndex++) {
                ELBOWROOM = elbowRooms[elbowRoomIndex];
                for (ratioIndex = 0; ratioIndex <= N; ratioIndex++) {
                    SPARSITY_RATIO = (double) ratioIndex / N;
                    std::ostringstream str;
                    str << "SPARSITY RATIO: " << SPARSITY_RATIO <<
                        " ELBOWROOM: " << ELBOWROOM;
                    setExtraInfo(str.str());
                    runTest();
                    setExtraInfo("");
                }
            }
            ELBOWROOM = originalElbowRoom;
            SPARSITY_RATIO = originalSparsityRatio;
        }
    };
    void equal();
    void copy();
    void assign();
    void transposed();
    void transpose();
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
    void invert();
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
    void gaussian();

};

#endif	/* MATRIXTEST_H */

