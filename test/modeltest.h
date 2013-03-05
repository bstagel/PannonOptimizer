/* 
 * File:   modeltest.h
 * Author: smidla
 *
 * Created on 2013. march 5., 21:29
 */

#ifndef MODELTEST_H
#define	MODELTEST_H

#include "framework/tester.h"
#include "framework/unittest.h"
#include <iostream>
#include <lp/model.h>

class ModelTestSuite :  public UnitTest
{
public:

    ModelTestSuite(const char * name): UnitTest(name) {}
    
    void run();

private:

};

#endif	/* MODELTEST_H */

