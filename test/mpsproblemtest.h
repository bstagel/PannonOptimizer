/* 
 * File:   mpsproblemtest.h
 * Author: smidla
 *
 * Created on 2013. augusztus 13., 14:01
 */

#ifndef MPSPROBLEMTEST_H
#define	MPSPROBLEMTEST_H

#include "framework/tester.h"
#include "framework/unittest.h"
#include <lp/mpsproblem.h>
#include <lp/model.h>

class MpsProblemTestSuite : public UnitTest
{
public:
    MpsProblemTestSuite(const char * name);
private:

    void init();
};

#endif	/* MPSPROBLEMTEST_H */

