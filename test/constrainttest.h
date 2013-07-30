/* 
 * File:   constrainttest.h
 * Author: smidla
 *
 * Created on 2013. february 10., 15:51
 */

#ifndef CONSTRAINTTEST_H
#define	CONSTRAINTTEST_H

#include "framework/tester.h"
#include "framework/unittest.h"
#include <lp/constraint.h>

class ConstraintTestSuite : public UnitTest
{
public:

    ConstraintTestSuite(const char * name);

private:
    void init();
    void type();
    void createGreaterType();
    void createLessType();
    void createRangeType();
    void createEqualityType();
    void createNonBindingType();
    void setGetLowerBound();
    void setGetUpperBound();
    void setGetName();
    void setGetVector();
    void equalityOperator();
    void assign();
    void copy();
};


#endif	/* CONSTRAINTTEST_H */

