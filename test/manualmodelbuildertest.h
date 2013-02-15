/* 
 * File:   manualmodelbuildertest.h
 * Author: smidla
 *
 * Created on 2013. february 14., 14:42
 */

#ifndef MANUALMODELBUILDERTEST_H
#define	MANUALMODELBUILDERTEST_H

#include <cpptest.h>
#include <lp/manualmodelbuilder.h>

class ManualModelBuilderTestSuite : public Test::Suite
{
public:

    ManualModelBuilderTestSuite()
    {
        TEST_ADD(ManualModelBuilderTestSuite::init)
        TEST_ADD(ManualModelBuilderTestSuite::setGetName)
        TEST_ADD(ManualModelBuilderTestSuite::setGetObjectiveFunctionConstant)
        TEST_ADD(ManualModelBuilderTestSuite::addVariable1)
    }
private:
    void init();
    void setGetName();
    void setGetObjectiveFunctionConstant();
    void addVariable1();
};

#endif	/* MANUALMODELBUILDERTEST_H */

