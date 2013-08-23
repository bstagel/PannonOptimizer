/*
 * File:   vectortest.h
 * Author: borocz
 *
 * Created on 2013. július 11., 10:21
 */

#ifndef VECTORTEST_H
#define	VECTORTEST_H

#include <sstream>
#include "framework/tester.h"
#include "framework/unittest.h"
#include <linalg/vector.h>
#include <linalg/matrix.h>

class VectorTestSuite : public UnitTest
{
public:

    VectorTestSuite(const char * name);

private:

    class VectorTester : public UnitTest::DefaultTester
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

    bool sortedCheck(const Vector & vector, bool testedValue, bool whatever = false);
    bool integrityCheck(const Vector & vector);
    
    void demo1();
    void demo2();

    void init();
    void fillAndClear();
    void set();
    void change();
    void setNewNonzero();
    void insertElement();
    void append();
    void removeElement();
    void resize();
    void setSparsityRatio();
    void maxIndex();
    void absMaxElement();
    void scaleBy();
    void scaleElementBy();
    void scaleByLambdas();
    void addVector();
    void dotProduct();
    void euclidNorm();
    void elementaryFtran();
    void elementaryBtran();
    void plus();
    void minus();
    void asterisk();
    void asterisk2();
    void equal();
    void sortElements();
    void reInit();
    void heap();
    void prepare();
    void iterators();
};

#endif	/* VECTORTEST_H */

