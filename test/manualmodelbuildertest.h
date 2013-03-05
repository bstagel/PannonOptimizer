/* 
 * File:   manualmodelbuildertest.h
 * Author: smidla
 *
 * Created on 2013. february 14., 14:42
 */

#ifndef MANUALMODELBUILDERTEST_H
#define	MANUALMODELBUILDERTEST_H

#include "framework/tester.h"
#include "framework/unittest.h"
#include <lp/manualmodelbuilder.h>

class ManualModelBuilderTestSuite : public UnitTest
{
public:

    ManualModelBuilderTestSuite(const char * name) : UnitTest(name)
    {
    }
    
    void run();

private:
    void init();
    void setGetName();
    void setGetObjectiveFunctionConstant();
    void addVariable1();
    void addVariable2();
    void addVariable3();
    void setGetConstraint();
    void addConstraint1();
    void addConstraint2();
    void addConstraint3();
    void setGetVariable();
    void setCostCoefficient();
    void addVariableAndConstraint();
    void buildRow();
    void buildColumn();
    void buildCostVector();

    void printVectors(const std::vector< std::list<ManualModelBuilder::IndexValuePair> > & vector) const;
};

#endif	/* MANUALMODELBUILDERTEST_H */

