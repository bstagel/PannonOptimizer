/* 
 * File:   constrainttest.h
 * Author: smidla
 *
 * Created on 2013. february 10., 15:51
 */

#ifndef CONSTRAINTTEST_H
#define	CONSTRAINTTEST_H

#include <cpptest.h>
#include <lp/constraint.h>

class ConstraintTestSuite : public Test::Suite
{
public:

    ConstraintTestSuite()
    {
        TEST_ADD(ConstraintTestSuite::init)
        TEST_ADD(ConstraintTestSuite::type)
        TEST_ADD(ConstraintTestSuite::createGreaterType)
        TEST_ADD(ConstraintTestSuite::createLessType)
        TEST_ADD(ConstraintTestSuite::createRangeType)
        TEST_ADD(ConstraintTestSuite::createEqualityType)
        TEST_ADD(ConstraintTestSuite::createNonBindingType)
        TEST_ADD(ConstraintTestSuite::setGetLowerBound)
        TEST_ADD(ConstraintTestSuite::setGetUpperBound)
        TEST_ADD(ConstraintTestSuite::setGetName)
        TEST_ADD(ConstraintTestSuite::setGetVector)
    }

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
    // TODO: kell egy teszt az == operatornak
};


#endif	/* CONSTRAINTTEST_H */

