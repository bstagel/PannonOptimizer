#ifndef _VARIABLE_TEST_H_
#define _VARIABLE_TEST_H_

#include <framework/tester.h>
#include <framework/unittest.h>
#include <iostream>
#include <lp/variable.h>

class VariableTestSuite :  public UnitTest
{
public:

    VariableTestSuite(const char * name);

private:
    void init();
    void createVariable();    
    void createPlusType();
    void createMinusType();
    void createBoundedType();
    void createFixedType();
    void createFreeType();
    void setGetLowerBound();
    void setGetUpperBound();
    void setGetName();
    void setGetVector();
    void type();
    void equalityOperator();
    void assign();
    void copy();
};


#endif 
