#include "variabletest.h"
#include <cstring>
#include <globals.h>

VariableTestSuite::VariableTestSuite(const char* name) : UnitTest(name)
{
    ADD_TEST(VariableTestSuite::init);
    ADD_TEST(VariableTestSuite::type);
    ADD_TEST(VariableTestSuite::createVariable);
    ADD_TEST(VariableTestSuite::createPlusType);
    ADD_TEST(VariableTestSuite::createMinusType);
    ADD_TEST(VariableTestSuite::createBoundedType);
    ADD_TEST(VariableTestSuite::createFixedType);
    ADD_TEST(VariableTestSuite::createFreeType);
    ADD_TEST(VariableTestSuite::setGetLowerBound);
    ADD_TEST(VariableTestSuite::setGetUpperBound);
    ADD_TEST(VariableTestSuite::setGetName);
    ADD_TEST(VariableTestSuite::setGetVector);
    ADD_TEST(VariableTestSuite::equalityOperator);
    ADD_TEST(VariableTestSuite::assign);
    ADD_TEST(VariableTestSuite::copy);
}

void VariableTestSuite::init()
{
    Variable variable;
    TEST_ASSERT(variable.m_lowerBound == 0.0);
    TEST_ASSERT(variable.m_upperBound == PInfinity);
    TEST_ASSERT(variable.m_type == Variable::PLUS);
    TEST_ASSERT(variable.m_name == "");
    TEST_ASSERT(variable.m_vector == 0);
}

void VariableTestSuite::createVariable()
{
    const char * testName = "x1";
    const Numerical::Double testLowerBound = -2.0;
    const Numerical::Double testUpperBound = 42.0;
    const Numerical::Double testValue = 12.0;

    /**
     * Testing a named variable
     */
    Variable variable1 = Variable::createVariable(testName,
        testLowerBound,
        testUpperBound);
    TEST_ASSERT(variable1.m_type == variable1.getType());
    TEST_ASSERT(variable1.m_type == Variable::BOUNDED);

    TEST_ASSERT(variable1.m_lowerBound == testLowerBound);
    TEST_ASSERT(variable1.getLowerBound() == testLowerBound);

    TEST_ASSERT(variable1.m_upperBound == testUpperBound);
    TEST_ASSERT(variable1.getUpperBound() == testUpperBound);


    TEST_ASSERT(variable1.m_vector == 0);
    TEST_ASSERT(variable1.getVector() == 0);

    TEST_ASSERT(variable1.m_name == testName);
    TEST_ASSERT(strcmp(variable1.getName(), testName) == 0);

    /**
     * Testing an unnamed variable
     */
    Variable variable2 = Variable::createVariable(0,
        testLowerBound,
        testUpperBound);
    TEST_ASSERT(variable2.m_type == variable2.getType());
    TEST_ASSERT(variable2.m_type == Variable::BOUNDED);

    TEST_ASSERT(variable2.m_lowerBound == testLowerBound);
    TEST_ASSERT(variable2.getLowerBound() == testLowerBound);

    TEST_ASSERT(variable2.m_upperBound == testUpperBound);
    TEST_ASSERT(variable2.getUpperBound() == testUpperBound);

    TEST_ASSERT(variable2.m_vector == 0);
    TEST_ASSERT(variable2.getVector() == 0);

    TEST_ASSERT(variable2.m_name == "");
    TEST_ASSERT(strcmp(variable2.getName(), "<NO NAME>") == 0);

    /**
     * Testing a plus type variable
     */

    const Numerical::Double testUpperBound2 = PInfinity;

    Variable variable3 = Variable::createVariable(testName,
        testLowerBound,
        testUpperBound2);
    TEST_ASSERT(variable3.m_type == variable3.getType());
    TEST_ASSERT(variable3.m_type == Variable::PLUS);

    TEST_ASSERT(variable3.m_lowerBound == testLowerBound);
    TEST_ASSERT(variable3.getLowerBound() == testLowerBound);

    TEST_ASSERT(variable3.m_upperBound == testUpperBound2);
    TEST_ASSERT(variable3.getUpperBound() == testUpperBound2);

    TEST_ASSERT(variable3.m_vector == 0);
    TEST_ASSERT(variable3.getVector() == 0);

    TEST_ASSERT(variable3.m_name == testName);
    TEST_ASSERT(strcmp(variable3.getName(), testName) == 0);

    /**
     * Testing a minus type variable
     */

    const Numerical::Double testLowerBound2 = -PInfinity;

    Variable variable4 = Variable::createVariable(testName,
        testLowerBound2,
        testUpperBound);
    TEST_ASSERT(variable4.m_type == variable4.getType());
    TEST_ASSERT(variable4.m_type == Variable::MINUS);

    TEST_ASSERT(variable4.m_lowerBound == testLowerBound2);
    TEST_ASSERT(variable4.getLowerBound() == testLowerBound2);

    TEST_ASSERT(variable4.m_upperBound == testUpperBound);
    TEST_ASSERT(variable4.getUpperBound() == testUpperBound);

    TEST_ASSERT(variable4.m_vector == 0);
    TEST_ASSERT(variable4.getVector() == 0);

    TEST_ASSERT(variable4.m_name == testName);
    TEST_ASSERT(strcmp(variable4.getName(), testName) == 0);

    /**
     * Testing a free type variable
     */

    Variable variable5 = Variable::createVariable(testName,
        testLowerBound2,
        testUpperBound2);
    TEST_ASSERT(variable5.m_type == variable5.getType());
    TEST_ASSERT(variable5.m_type == Variable::FREE);

    TEST_ASSERT(variable5.m_lowerBound == testLowerBound2);
    TEST_ASSERT(variable5.getLowerBound() == testLowerBound2);

    TEST_ASSERT(variable5.m_upperBound == testUpperBound2);
    TEST_ASSERT(variable5.getUpperBound() == testUpperBound2);

    TEST_ASSERT(variable5.m_vector == 0);
    TEST_ASSERT(variable5.getVector() == 0);

    TEST_ASSERT(variable5.m_name == testName);
    TEST_ASSERT(strcmp(variable5.getName(), testName) == 0);

    /**
     * Testing a fixed type variable
     */

    Variable variable6 = Variable::createVariable(testName,
        testValue,
        testValue);
    TEST_ASSERT(variable6.m_type == variable6.getType());
    TEST_ASSERT(variable6.m_type == Variable::FIXED);

    TEST_ASSERT(variable6.m_lowerBound == testValue);
    TEST_ASSERT(variable6.getLowerBound() == testValue);

    TEST_ASSERT(variable6.m_upperBound == testValue);
    TEST_ASSERT(variable6.getUpperBound() == testValue);

    TEST_ASSERT(variable6.m_vector == 0);
    TEST_ASSERT(variable6.getVector() == 0);

    TEST_ASSERT(variable6.m_name == testName);
    TEST_ASSERT(strcmp(variable6.getName(), testName) == 0);

    bool invalidUpperBoundExceptionCheck = false;
    try {
        Numerical::Double testUpperBound3 = -PInfinity;
        Variable variable7 = Variable::createVariable(testName,
            testLowerBound,
            testUpperBound3);
    } catch (Variable::InvalidUpperBoundException & exception) {
        invalidUpperBoundExceptionCheck = true;

    } catch (...) {

    }
    TEST_ASSERT(invalidUpperBoundExceptionCheck == true);

    bool invalidLowerBoundExceptionCheck = false;
    try {
        Numerical::Double testLowerBound3 = PInfinity;
        Variable variable7 = Variable::createVariable(testName,
            testLowerBound3,
            testUpperBound);
    } catch (Variable::InvalidLowerBoundException & exception) {
        invalidLowerBoundExceptionCheck = true;

    } catch (...) {

    }
    TEST_ASSERT(invalidLowerBoundExceptionCheck == true);

    bool invalidBoundsExceptionCheck = false;
    try {
        Numerical::Double testLowerBound4 = 10.0;
        Numerical::Double testUpperBound4 = 0.0;
        Variable variable8 = Variable::createVariable(testName,
            testLowerBound4,
            testUpperBound4);
    } catch (Variable::InvalidBoundsException & exception) {
        invalidBoundsExceptionCheck = true;

    } catch (...) {

    }
    TEST_ASSERT(invalidBoundsExceptionCheck == true);    
}

void VariableTestSuite::createPlusType()
{
    const char * testName = "x4";
    const Numerical::Double testLowerBound = -3.0;
    const Numerical::Double testUpperBound = PInfinity;
    /**
     * Testing a named variable
     */

    Variable variable1 = Variable::createPlusTypeVariable(testName,
        testLowerBound);    

    TEST_ASSERT(variable1.m_type == variable1.getType());
    TEST_ASSERT(variable1.m_type == Variable::PLUS);

    TEST_ASSERT(variable1.m_lowerBound == testLowerBound);
    TEST_ASSERT(variable1.getLowerBound() == testLowerBound);

    TEST_ASSERT(variable1.m_upperBound == testUpperBound);
    TEST_ASSERT(variable1.getUpperBound() == testUpperBound);

    TEST_ASSERT(variable1.m_vector == 0);
    TEST_ASSERT(variable1.getVector() == 0);

    TEST_ASSERT(variable1.m_name == testName);
    TEST_ASSERT(strcmp(variable1.getName(), testName) == 0);

    /**
     * Testing an unnamed variable
     */
    Variable variable2 = Variable::createPlusTypeVariable(0,
        testLowerBound);
    TEST_ASSERT(variable2.m_type == variable2.getType());
    TEST_ASSERT(variable2.m_type == Variable::PLUS);

    TEST_ASSERT(variable2.m_lowerBound == testLowerBound);
    TEST_ASSERT(variable2.getLowerBound() == testLowerBound);

    TEST_ASSERT(variable2.m_upperBound == testUpperBound);
    TEST_ASSERT(variable2.getUpperBound() == testUpperBound);

    TEST_ASSERT(variable2.m_vector == 0);
    TEST_ASSERT(variable2.getVector() == 0);

    TEST_ASSERT(variable2.m_name == "");
    TEST_ASSERT(strcmp(variable2.getName(), "<NO NAME>") == 0);


    const Numerical::Double testLowerBound2 = -PInfinity;

    /**
     * Testing a named variable, without lower bound
     */

    Variable variable3 = Variable::createPlusTypeVariable(testName,
        testLowerBound2);

    TEST_ASSERT(variable3.m_type == variable3.getType());
    TEST_ASSERT(variable3.m_type == Variable::FREE);

    TEST_ASSERT(variable3.m_lowerBound == testLowerBound2);
    TEST_ASSERT(variable3.getLowerBound() == testLowerBound2);

    TEST_ASSERT(variable3.m_upperBound == testUpperBound);
    TEST_ASSERT(variable3.getUpperBound() == testUpperBound);

    TEST_ASSERT(variable3.m_vector == 0);
    TEST_ASSERT(variable3.getVector() == 0);

    TEST_ASSERT(variable3.m_name == testName);
    TEST_ASSERT(strcmp(variable3.getName(), testName) == 0);

    /**
     * Testing an unnamed variable, without lower bound
     */
    Variable variable4 = Variable::createPlusTypeVariable(0,
        testLowerBound2);
    TEST_ASSERT(variable4.m_type == variable4.getType());
    TEST_ASSERT(variable4.m_type == Variable::FREE);

    TEST_ASSERT(variable4.m_lowerBound == testLowerBound2);
    TEST_ASSERT(variable4.getLowerBound() == testLowerBound2);

    TEST_ASSERT(variable4.m_upperBound == testUpperBound);
    TEST_ASSERT(variable4.getUpperBound() == testUpperBound);

    TEST_ASSERT(variable4.m_vector == 0);
    TEST_ASSERT(variable4.getVector() == 0);

    TEST_ASSERT(variable4.m_name == "");
    TEST_ASSERT(strcmp(variable4.getName(), "<NO NAME>") == 0);

    bool invalidLowerBoundExceptionCheck = false;
    try {
        Numerical::Double testLowerBound3 = PInfinity;
        Variable variable5 = Variable::createPlusTypeVariable(0,
            testLowerBound3);
        __UNUSED(variable5);
    } catch (Variable::InvalidLowerBoundException & exception) {
        invalidLowerBoundExceptionCheck = true;

    } catch (...) {

    }
    TEST_ASSERT(invalidLowerBoundExceptionCheck == true);
}

void VariableTestSuite::createMinusType()
{
    const char * testName = "x6";
    const Numerical::Double testLowerBound = -PInfinity;
    const Numerical::Double testUpperBound = 102.0;
    /**
     * Testing a named variable
     */
    Variable variable1 = Variable::createMinusTypeVariable(testName,
        testUpperBound);
    TEST_ASSERT(variable1.m_type == variable1.getType());
    TEST_ASSERT(variable1.m_type == Variable::MINUS);

    TEST_ASSERT(variable1.m_lowerBound == testLowerBound);
    TEST_ASSERT(variable1.getLowerBound() == testLowerBound);

    TEST_ASSERT(variable1.m_upperBound == testUpperBound);
    TEST_ASSERT(variable1.getUpperBound() == testUpperBound);

    TEST_ASSERT(variable1.m_vector == 0);
    TEST_ASSERT(variable1.getVector() == 0);

    TEST_ASSERT(variable1.m_name == testName);
    TEST_ASSERT(strcmp(variable1.getName(), testName) == 0);

    /**
     * Testing an unnamed variable
     */
    Variable variable2 = Variable::createMinusTypeVariable(0,
        testUpperBound);
    TEST_ASSERT(variable2.m_type == variable2.getType());
    TEST_ASSERT(variable2.m_type == Variable::MINUS);

    TEST_ASSERT(variable2.m_lowerBound == testLowerBound);
    TEST_ASSERT(variable2.getLowerBound() == testLowerBound);

    TEST_ASSERT(variable2.m_upperBound == testUpperBound);
    TEST_ASSERT(variable2.getUpperBound() == testUpperBound);

    TEST_ASSERT(variable2.m_vector == 0);
    TEST_ASSERT(variable2.getVector() == 0);

    TEST_ASSERT(variable2.m_name == "");
    TEST_ASSERT(strcmp(variable2.getName(), "<NO NAME>") == 0);

    const Numerical::Double testUpperBound2 = PInfinity;
    /**
     * Testing a named variable, without upper bound
     */
    Variable variable3 = Variable::createMinusTypeVariable(testName,
        testUpperBound2);
    TEST_ASSERT(variable3.m_type == variable3.getType());
    TEST_ASSERT(variable3.m_type == Variable::FREE);

    TEST_ASSERT(variable3.m_lowerBound == testLowerBound);
    TEST_ASSERT(variable3.getLowerBound() == testLowerBound);

    TEST_ASSERT(variable3.m_upperBound == testUpperBound2);
    TEST_ASSERT(variable3.getUpperBound() == testUpperBound2);

    TEST_ASSERT(variable3.m_vector == 0);
    TEST_ASSERT(variable3.getVector() == 0);

    TEST_ASSERT(variable3.m_name == testName);
    TEST_ASSERT(strcmp(variable3.getName(), testName) == 0);

    /**
     * Testing an unnamed variable, without upper bound
     */
    Variable variable4 = Variable::createMinusTypeVariable(0,
        testUpperBound2);
    TEST_ASSERT(variable4.m_type == variable4.getType());
    TEST_ASSERT(variable4.m_type == Variable::FREE);

    TEST_ASSERT(variable4.m_lowerBound == testLowerBound);
    TEST_ASSERT(variable4.getLowerBound() == testLowerBound);

    TEST_ASSERT(variable4.m_upperBound == testUpperBound2);
    TEST_ASSERT(variable4.getUpperBound() == testUpperBound2);

    TEST_ASSERT(variable4.m_vector == 0);
    TEST_ASSERT(variable4.getVector() == 0);

    TEST_ASSERT(variable4.m_name == "");
    TEST_ASSERT(strcmp(variable4.getName(), "<NO NAME>") == 0);

    bool invalidUpperBoundExceptionCheck = false;
    try {
        Numerical::Double testUpperBound3 = -PInfinity;
        Variable variable5 = Variable::createMinusTypeVariable(0,
            testUpperBound3);
        __UNUSED(variable5);
    } catch (Variable::InvalidUpperBoundException & exception) {
        invalidUpperBoundExceptionCheck = true;

    } catch (...) {

    }
    TEST_ASSERT(invalidUpperBoundExceptionCheck == true);
}

void VariableTestSuite::createBoundedType()
{
    const char * testName = "x1";
    const Numerical::Double testValue = 12.0;
    const Numerical::Double testLowerBound = -2.0;
    const Numerical::Double testUpperBound = 42.0;

    /**
     * Testing a named variable
     */
    Variable variable1 = Variable::createBoundedTypeVariable(testName,
        testLowerBound,
        testUpperBound);
    TEST_ASSERT(variable1.m_type == variable1.getType());
    TEST_ASSERT(variable1.m_type == Variable::BOUNDED);

    TEST_ASSERT(variable1.m_lowerBound == testLowerBound);
    TEST_ASSERT(variable1.getLowerBound() == testLowerBound);

    TEST_ASSERT(variable1.m_upperBound == testUpperBound);
    TEST_ASSERT(variable1.getUpperBound() == testUpperBound);

    TEST_ASSERT(variable1.m_vector == 0);
    TEST_ASSERT(variable1.getVector() == 0);

    TEST_ASSERT(variable1.m_name == testName);
    TEST_ASSERT(strcmp(variable1.getName(), testName) == 0);

    /**
     * Testing an unnamed variable
     */
    Variable variable2 = Variable::createBoundedTypeVariable(0,
        testLowerBound,
        testUpperBound);
    TEST_ASSERT(variable2.m_type == variable2.getType());
    TEST_ASSERT(variable2.m_type == Variable::BOUNDED);

    TEST_ASSERT(variable2.m_lowerBound == testLowerBound);
    TEST_ASSERT(variable2.getLowerBound() == testLowerBound);

    TEST_ASSERT(variable2.m_upperBound == testUpperBound);
    TEST_ASSERT(variable2.getUpperBound() == testUpperBound);

    TEST_ASSERT(variable2.m_vector == 0);
    TEST_ASSERT(variable2.getVector() == 0);

    TEST_ASSERT(variable2.m_name == "");
    TEST_ASSERT(strcmp(variable2.getName(), "<NO NAME>") == 0);

    /**
     * Testing a plus type variable
     */

    const Numerical::Double testUpperBound2 = PInfinity;

    Variable variable3 = Variable::createBoundedTypeVariable(testName,
        testLowerBound,
        testUpperBound2);
    TEST_ASSERT(variable3.m_type == variable3.getType());
    TEST_ASSERT(variable3.m_type == Variable::PLUS);

    TEST_ASSERT(variable3.m_lowerBound == testLowerBound);
    TEST_ASSERT(variable3.getLowerBound() == testLowerBound);

    TEST_ASSERT(variable3.m_upperBound == testUpperBound2);
    TEST_ASSERT(variable3.getUpperBound() == testUpperBound2);

    TEST_ASSERT(variable3.m_vector == 0);
    TEST_ASSERT(variable3.getVector() == 0);

    TEST_ASSERT(variable3.m_name == testName);
    TEST_ASSERT(strcmp(variable3.getName(), testName) == 0);

    /**
     * Testing a minus type variable
     */

    const Numerical::Double testLowerBound2 = -PInfinity;

    Variable variable4 = Variable::createBoundedTypeVariable(testName,
        testLowerBound2,
        testUpperBound);
    TEST_ASSERT(variable4.m_type == variable4.getType());
    TEST_ASSERT(variable4.m_type == Variable::MINUS);

    TEST_ASSERT(variable4.m_lowerBound == testLowerBound2);
    TEST_ASSERT(variable4.getLowerBound() == testLowerBound2);

    TEST_ASSERT(variable4.m_upperBound == testUpperBound);
    TEST_ASSERT(variable4.getUpperBound() == testUpperBound);

    TEST_ASSERT(variable4.m_vector == 0);
    TEST_ASSERT(variable4.getVector() == 0);

    TEST_ASSERT(variable4.m_name == testName);
    TEST_ASSERT(strcmp(variable4.getName(), testName) == 0);

    /**
     * Testing a free type variable
     */

    Variable variable5 = Variable::createBoundedTypeVariable(testName,
        testLowerBound2,
        testUpperBound2);
    TEST_ASSERT(variable5.m_type == variable5.getType());
    TEST_ASSERT(variable5.m_type == Variable::FREE);

    TEST_ASSERT(variable5.m_lowerBound == testLowerBound2);
    TEST_ASSERT(variable5.getLowerBound() == testLowerBound2);

    TEST_ASSERT(variable5.m_upperBound == testUpperBound2);
    TEST_ASSERT(variable5.getUpperBound() == testUpperBound2);

    TEST_ASSERT(variable5.m_vector == 0);
    TEST_ASSERT(variable5.getVector() == 0);

    TEST_ASSERT(variable5.m_name == testName);
    TEST_ASSERT(strcmp(variable5.getName(), testName) == 0);

    /**
     * Testing a fixed type variable
     */

    Variable variable6 = Variable::createBoundedTypeVariable(testName,
        testValue,
        testValue);
    TEST_ASSERT(variable6.m_type == variable6.getType());
    TEST_ASSERT(variable6.m_type == Variable::FIXED);

    TEST_ASSERT(variable6.m_lowerBound == testValue);
    TEST_ASSERT(variable6.getLowerBound() == testValue);

    TEST_ASSERT(variable6.m_upperBound == testValue);
    TEST_ASSERT(variable6.getUpperBound() == testValue);

    TEST_ASSERT(variable6.m_vector == 0);
    TEST_ASSERT(variable6.getVector() == 0);

    TEST_ASSERT(variable6.m_name == testName);
    TEST_ASSERT(strcmp(variable6.getName(), testName) == 0);

    bool invalidUpperBoundExceptionCheck = false;
    try {
        Numerical::Double testUpperBound3 = -PInfinity;
        Variable variable7 = Variable::createBoundedTypeVariable(testName,
            testLowerBound,
            testUpperBound3);
    } catch (Variable::InvalidUpperBoundException & exception) {
        invalidUpperBoundExceptionCheck = true;

    } catch (...) {

    }
    TEST_ASSERT(invalidUpperBoundExceptionCheck == true);

    bool invalidLowerBoundExceptionCheck = false;
    try {
        Numerical::Double testLowerBound3 = PInfinity;
        Variable variable7 = Variable::createBoundedTypeVariable(testName,
            testLowerBound3,
            testUpperBound);
    } catch (Variable::InvalidLowerBoundException & exception) {
        invalidLowerBoundExceptionCheck = true;

    } catch (...) {

    }
    TEST_ASSERT(invalidLowerBoundExceptionCheck == true);

    bool invalidBoundsExceptionCheck = false;
    try {
        Numerical::Double testLowerBound4 = 10.0;
        Numerical::Double testUpperBound4 = 0.0;
        Variable variable8 = Variable::createBoundedTypeVariable(testName,
            testLowerBound4,
            testUpperBound4);
    } catch (Variable::InvalidBoundsException & exception) {
        invalidBoundsExceptionCheck = true;

    } catch (...) {

    }
    TEST_ASSERT(invalidBoundsExceptionCheck == true);
}

void VariableTestSuite::createFixedType()
{
    const char * testName = "x1";
    const Numerical::Double testValue = 12.0;
    const Numerical::Double testLowerBound = 12.0;
    const Numerical::Double testUpperBound = 12.0;

    /**
     * Testing a named variable
     */
    Variable variable1 = Variable::createFixedTypeVariable(testName,
        testValue);
    TEST_ASSERT(variable1.m_type == variable1.getType());
    TEST_ASSERT(variable1.m_type == Variable::FIXED);

    TEST_ASSERT(variable1.m_lowerBound == testLowerBound);
    TEST_ASSERT(variable1.getLowerBound() == testLowerBound);

    TEST_ASSERT(variable1.m_upperBound == testUpperBound);
    TEST_ASSERT(variable1.getUpperBound() == testUpperBound);

    TEST_ASSERT(variable1.m_vector == 0);
    TEST_ASSERT(variable1.getVector() == 0);

    TEST_ASSERT(variable1.m_name == testName);
    TEST_ASSERT(strcmp(variable1.getName(), testName) == 0);

    /**
     * Testing an unnamed variable
     */
    Variable variable2 = Variable::createFixedTypeVariable(0,
        testValue);
    TEST_ASSERT(variable2.m_type == variable2.getType());
    TEST_ASSERT(variable2.m_type == Variable::FIXED);

    TEST_ASSERT(variable2.m_lowerBound == testLowerBound);
    TEST_ASSERT(variable2.getLowerBound() == testLowerBound);

    TEST_ASSERT(variable2.m_upperBound == testUpperBound);
    TEST_ASSERT(variable2.getUpperBound() == testUpperBound);

    TEST_ASSERT(variable2.m_vector == 0);
    TEST_ASSERT(variable2.getVector() == 0);

    TEST_ASSERT(variable2.m_name == "");
    TEST_ASSERT(strcmp(variable2.getName(), "<NO NAME>") == 0);
}

void VariableTestSuite::createFreeType()
{
    const char * testName = "x1";
    const Numerical::Double testLowerBound = -PInfinity;
    const Numerical::Double testUpperBound = PInfinity;

    /**
     * Testing a named variable
     */
    Variable variable1 = Variable::createFreeTypeVariable(testName);
    TEST_ASSERT(variable1.m_type == variable1.getType());
    TEST_ASSERT(variable1.m_type == Variable::FREE);

    TEST_ASSERT(variable1.m_lowerBound == testLowerBound);
    TEST_ASSERT(variable1.getLowerBound() == testLowerBound);

    TEST_ASSERT(variable1.m_upperBound == testUpperBound);
    TEST_ASSERT(variable1.getUpperBound() == testUpperBound);

    TEST_ASSERT(variable1.m_vector == 0);
    TEST_ASSERT(variable1.getVector() == 0);

    TEST_ASSERT(variable1.m_name == testName);
    TEST_ASSERT(strcmp(variable1.getName(), testName) == 0);

    /**
     * Testing an unnamed variable
     */
    Variable variable2 = Variable::createFreeTypeVariable(0);
    TEST_ASSERT(variable2.m_type == variable2.getType());
    TEST_ASSERT(variable2.m_type == Variable::FREE);

    TEST_ASSERT(variable2.m_lowerBound == testLowerBound);
    TEST_ASSERT(variable2.getLowerBound() == testLowerBound);

    TEST_ASSERT(variable2.m_upperBound == testUpperBound);
    TEST_ASSERT(variable2.getUpperBound() == testUpperBound);

    TEST_ASSERT(variable2.m_vector == 0);
    TEST_ASSERT(variable2.getVector() == 0);

    TEST_ASSERT(variable2.m_name == "");
    TEST_ASSERT(strcmp(variable2.getName(), "<NO NAME>") == 0);
}

void VariableTestSuite::setGetLowerBound()
{
    Variable variable;
    variable.setLowerBound(-10);

    TEST_ASSERT(variable.m_lowerBound == -10.0);
    TEST_ASSERT(variable.getLowerBound() == -10.0);

    variable.setLowerBound(10);
    TEST_ASSERT(variable.m_lowerBound == 10.0);
    TEST_ASSERT(variable.getLowerBound() == 10.0);

    variable.setLowerBound(0);
    TEST_ASSERT(variable.m_lowerBound == 0);
    TEST_ASSERT(variable.getLowerBound() == 0);

    variable.setLowerBound(-PInfinity);
    TEST_ASSERT(variable.m_lowerBound == -PInfinity);
    TEST_ASSERT(variable.getLowerBound() == -PInfinity);

    bool invalidLowerBoundException = false;
    try {
        variable.setLowerBound(PInfinity);
    } catch (Variable::InvalidLowerBoundException & exception) {
        invalidLowerBoundException = true;
    } catch (...) {

    }
    TEST_ASSERT(invalidLowerBoundException == true);

    bool invalidBoundsException = false;
    try {
        variable.setLowerBound(-PInfinity);
        variable.setUpperBound(10.0);
        variable.setLowerBound(20.0);
    } catch (Variable::InvalidBoundsException & exception) {
        invalidBoundsException = true;
    } catch (...) {

    }
    TEST_ASSERT(invalidBoundsException == true);
}

void VariableTestSuite::setGetUpperBound()
{
    Variable variable;
    variable.setLowerBound(-20.0);
    variable.setUpperBound(-10);
    TEST_ASSERT(variable.m_upperBound == -10.0);
    TEST_ASSERT(variable.getUpperBound() == -10.0);

    variable.setUpperBound(10);
    TEST_ASSERT(variable.m_upperBound == 10.0);
    TEST_ASSERT(variable.getUpperBound() == 10.0);

    variable.setUpperBound(0);
    TEST_ASSERT(variable.m_upperBound == 0);
    TEST_ASSERT(variable.getUpperBound() == 0);

    variable.setUpperBound(PInfinity);
    TEST_ASSERT(variable.m_upperBound == PInfinity);
    TEST_ASSERT(variable.getUpperBound() == PInfinity);

    bool invalidUpperBoundException = false;
    try {
        variable.setUpperBound(-PInfinity);
    } catch (Variable::InvalidUpperBoundException & exception) {
        invalidUpperBoundException = true;
    } catch (...) {

    }
    TEST_ASSERT(invalidUpperBoundException == true);

    bool invalidBoundsException = false;
    try {
        variable.setUpperBound(PInfinity);
        variable.setLowerBound(20.0);
        variable.setUpperBound(10.0);
    } catch (Variable::InvalidBoundsException & exception) {
        invalidBoundsException = true;
    } catch (...) {

    }
    TEST_ASSERT(invalidBoundsException == true);
}

void VariableTestSuite::setGetName()
{
    Variable variable;
    variable.setName("");
    TEST_ASSERT(variable.m_name == "");
    TEST_ASSERT(strcmp(variable.getName(), "<NO NAME>") == 0);

    variable.setName("x1");
    TEST_ASSERT(variable.m_name == "x1");
    TEST_ASSERT(strcmp(variable.getName(), "x1") == 0);
}

void VariableTestSuite::setGetVector()
{
    Variable variable;
    const Vector vector1, vector2;

    variable.setVector(vector1);
    TEST_ASSERT(variable.m_vector == variable.getVector());
    TEST_ASSERT(variable.m_vector == &vector1);

    variable.setVector(vector2);
    TEST_ASSERT(variable.m_vector == variable.getVector());
    TEST_ASSERT(variable.m_vector == &vector2);
}

void VariableTestSuite::type()
{
    Variable variable1;
    variable1.setLowerBound(-PInfinity);
    variable1.setUpperBound(PInfinity);
    TEST_ASSERT(variable1.m_type == variable1.getType());
    TEST_ASSERT(variable1.m_type == Variable::FREE);

    Variable variable2;
    variable2.setLowerBound(0.0);
    variable2.setUpperBound(0.0);
    TEST_ASSERT(variable2.m_type == variable2.getType());
    TEST_ASSERT(variable2.m_type == Variable::FIXED);

    Variable variable3;
    variable3.setLowerBound(-10.0);
    variable3.setUpperBound(-10.0);
    TEST_ASSERT(variable3.m_type == variable3.getType());
    TEST_ASSERT(variable3.m_type == Variable::FIXED);

    Variable variable4;
    variable4.setLowerBound(10.0);
    variable4.setUpperBound(10.0);
    TEST_ASSERT(variable4.m_type == variable4.getType());
    TEST_ASSERT(variable4.m_type == Variable::FIXED);

    Variable variable5;
    variable5.setLowerBound(-10.0);
    variable5.setUpperBound(PInfinity);
    TEST_ASSERT(variable5.m_type == variable5.getType());
    TEST_ASSERT(variable5.m_type == Variable::PLUS);

    Variable variable6;
    variable6.setLowerBound(0.0);
    variable6.setUpperBound(PInfinity);
    TEST_ASSERT(variable6.m_type == variable6.getType());
    TEST_ASSERT(variable6.m_type == Variable::PLUS);

    Variable variable7;
    variable7.setLowerBound(10.0);
    variable7.setUpperBound(PInfinity);
    TEST_ASSERT(variable7.m_type == variable7.getType());
    TEST_ASSERT(variable7.m_type == Variable::PLUS);

    Variable variable8;
    variable8.setLowerBound(-PInfinity);
    variable8.setUpperBound(-10.0);
    TEST_ASSERT(variable8.m_type == variable8.getType());
    TEST_ASSERT(variable8.m_type == Variable::MINUS);

    Variable variable9;
    variable9.setLowerBound(-PInfinity);
    variable9.setUpperBound(0.0);
    TEST_ASSERT(variable9.m_type == variable9.getType());
    TEST_ASSERT(variable9.m_type == Variable::MINUS);

    Variable variable10;
    variable10.setLowerBound(-PInfinity);
    variable10.setUpperBound(10.0);
    TEST_ASSERT(variable10.m_type == variable10.getType());
    TEST_ASSERT(variable10.m_type == Variable::MINUS);

    Variable variable11;
    variable11.setLowerBound(-20.0);
    variable11.setUpperBound(-10.0);
    TEST_ASSERT(variable11.m_type == variable11.getType());
    TEST_ASSERT(variable11.m_type == Variable::BOUNDED);

    Variable variable12;
    variable12.setLowerBound(-20.0);
    variable12.setUpperBound(0.0);
    TEST_ASSERT(variable12.m_type == variable12.getType());
    TEST_ASSERT(variable12.m_type == Variable::BOUNDED);

    Variable variable13;
    variable13.setLowerBound(-20.0);
    variable13.setUpperBound(10.0);
    TEST_ASSERT(variable13.m_type == variable13.getType());
    TEST_ASSERT(variable13.m_type == Variable::BOUNDED);

    Variable variable14;
    variable14.setLowerBound(0.0);
    variable14.setUpperBound(10.0);
    TEST_ASSERT(variable14.m_type == variable14.getType());
    TEST_ASSERT(variable14.m_type == Variable::BOUNDED);

    Variable variable15;
    variable15.setLowerBound(10.0);
    variable15.setUpperBound(20.0);
    TEST_ASSERT(variable15.m_type == variable15.getType());
    TEST_ASSERT(variable15.m_type == Variable::BOUNDED);
}

void VariableTestSuite::equalityOperator()
{
    Variable variable1, variable2;
    TEST_ASSERT(variable1 == variable2);

    variable1.setName("y1");
    TEST_ASSERT(!(variable1 == variable2));
    variable1.setName("");
    TEST_ASSERT(variable1 == variable2);

    variable1.setLowerBound(-1.0);
    TEST_ASSERT(!(variable1 == variable2));
    variable1.setLowerBound(0.0);
    TEST_ASSERT(variable1 == variable2);

    variable1.setUpperBound(1.0);
    TEST_ASSERT(!(variable1 == variable2));
    variable1.setUpperBound(PInfinity);
    TEST_ASSERT(variable1 == variable2);

    Vector vector;
    variable1.setVector(vector);
    TEST_ASSERT(!(variable1 == variable2));
}

void VariableTestSuite::assign()
{
    Variable variable1 = Variable::createFreeTypeVariable("x1");
    Variable variable2 = Variable::createFixedTypeVariable("x4", 6);
    variable1 = variable2;

    const Numerical::Double outputLowerBound1 = 6.0;
    const Numerical::Double outputUpperBound1 = 6.0;
    const Variable::TYPE outputType1 = Variable::FIXED;
    const std::string outputName1 = "x4";
    Vector * const outputVector1 = 0;

    TEST_ASSERT(variable1.m_lowerBound == outputLowerBound1);
    TEST_ASSERT(variable1.m_upperBound == outputUpperBound1);
    TEST_ASSERT(variable1.m_type == outputType1);
    TEST_ASSERT(variable1.m_name == outputName1);
    TEST_ASSERT(variable1.m_vector == outputVector1);
    
    variable2 = variable2;

    const Numerical::Double outputLowerBound2 = 6.0;
    const Numerical::Double outputUpperBound2 = 6.0;
    const Variable::TYPE outputType2 = Variable::FIXED;
    const std::string outputName2 = "x4";
    Vector * const outputVector2 = 0;

    TEST_ASSERT(variable2.m_lowerBound == outputLowerBound2);
    TEST_ASSERT(variable2.m_upperBound == outputUpperBound2);
    TEST_ASSERT(variable2.m_type == outputType2);
    TEST_ASSERT(variable2.m_name == outputName2);
    TEST_ASSERT(variable2.m_vector == outputVector2);
}

void VariableTestSuite::copy()
{
    Variable variable1 = Variable::createFixedTypeVariable("x4", 6);
    Variable variable2 = variable1;

    const Numerical::Double outputLowerBound1 = 6.0;
    const Numerical::Double outputUpperBound1 = 6.0;
    const Variable::TYPE outputType1 = Variable::FIXED;
    const std::string outputName1 = "x4";
    Vector * const outputVector1 = 0;

    TEST_ASSERT(variable2.m_lowerBound == outputLowerBound1);
    TEST_ASSERT(variable2.m_upperBound == outputUpperBound1);
    TEST_ASSERT(variable2.m_type == outputType1);
    TEST_ASSERT(variable2.m_name == outputName1);
    TEST_ASSERT(variable2.m_vector == outputVector1);
}
