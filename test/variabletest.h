#ifndef _VARIABLE_TEST_H_
#define _VARIABLE_TEST_H_

#include <cpptest.h>
#include <iostream>
#include <lp/variable.h>

class VariableTestSuite : public Test::Suite
{
public:

    VariableTestSuite()
    {
        TEST_ADD(VariableTestSuite::init)
        TEST_ADD(VariableTestSuite::type)
        TEST_ADD(VariableTestSuite::createPlusType)
        TEST_ADD(VariableTestSuite::createMinusType)
        TEST_ADD(VariableTestSuite::createBoundedType)
        TEST_ADD(VariableTestSuite::createFixedType)
        TEST_ADD(VariableTestSuite::createFreeType)
        TEST_ADD(VariableTestSuite::setGetLowerBound)
        TEST_ADD(VariableTestSuite::setGetUpperBound)
        TEST_ADD(VariableTestSuite::setGetValue)
        TEST_ADD(VariableTestSuite::setGetName)
        TEST_ADD(VariableTestSuite::setGetVector)
    }

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
