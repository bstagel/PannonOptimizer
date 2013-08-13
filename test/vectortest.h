/*
 * File:   vectortest.h
 * Author: borocz
 *
 * Created on 2013. július 11., 10:21
 */

#ifndef VECTORTEST_H
#define	VECTORTEST_H

#include "framework/tester.h"
#include "framework/unittest.h"
#include <linalg/vector.h>
#include <linalg/matrix.h>

class VectorTestSuite : public UnitTest
{
public:

    VectorTestSuite(const char * name);

private:
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

};

#endif	/* VECTORTEST_H */

