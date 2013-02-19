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
        TEST_ADD(ManualModelBuilderTestSuite::addVariable2)
        TEST_ADD(ManualModelBuilderTestSuite::addVariable3)
        TEST_ADD(ManualModelBuilderTestSuite::setGetConstraint)
    }
private:
    void init();
    void setGetName();
    void setGetObjectiveFunctionConstant();
    void addVariable1();
    void addVariable2();
    void addVariable3();
    void setGetConstraint();
    
    void printVectors(const std::vector< std::list<ManualModelBuilder::IndexValuePair> > & vector) const;
};

#endif	/* MANUALMODELBUILDERTEST_H */

