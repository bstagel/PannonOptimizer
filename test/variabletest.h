#ifndef _VARIABLE_TEST_H_
#define _VARIABLE_TEST_H_

#include "framework/tester.h"
#include "framework/unittest.h"
#include <iostream>
#include <lp/variable.h>

class VariableTestSuite :  public UnitTest
{
public:

    VariableTestSuite(const char * name): UnitTest(name) {}
    
    void run();

private:
    void init();
    void createPlusType();
    void createMinusType();
    void createBoundedType();
    void createFixedType();
    void createFreeType();
    void setGetLowerBound();
    void setGetUpperBound();
    void setGetValue();
    void setGetName();
    void setGetVector();
    void type();
    //TODO: == operatorra tesztet irni!
};


#endif 
