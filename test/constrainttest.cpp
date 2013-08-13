#include "constrainttest.h"

ConstraintTestSuite::ConstraintTestSuite(const char * name) : UnitTest(name)
{
    ADD_TEST(ConstraintTestSuite::init);
    ADD_TEST(ConstraintTestSuite::type);
    ADD_TEST(ConstraintTestSuite::createGreaterType);
    ADD_TEST(ConstraintTestSuite::createLessType);
    ADD_TEST(ConstraintTestSuite::createRangeType);
    ADD_TEST(ConstraintTestSuite::createEqualityType);
    ADD_TEST(ConstraintTestSuite::createNonBindingType);
    ADD_TEST(ConstraintTestSuite::setGetLowerBound);
    ADD_TEST(ConstraintTestSuite::setGetUpperBound);
    ADD_TEST(ConstraintTestSuite::setGetName);
    ADD_TEST(ConstraintTestSuite::setGetVector);
    ADD_TEST(ConstraintTestSuite::equalityOperator);
    ADD_TEST(ConstraintTestSuite::assign);
    ADD_TEST(ConstraintTestSuite::copy);
}

void ConstraintTestSuite::init()
{
    Constraint constraint;
    TEST_ASSERT(constraint.m_lowerBound == 0.0);
    TEST_ASSERT(constraint.m_upperBound == PInfinity);
    TEST_ASSERT(constraint.m_type == Constraint::GREATER_OR_EQUAL);
    TEST_ASSERT(constraint.m_name == "");
    TEST_ASSERT(constraint.m_vector == 0);
}

void ConstraintTestSuite::createGreaterType()
{
    const char * testName = "x4";
    const Numerical::Double testLowerBound = -3.0;
    const Numerical::Double testUpperBound = PInfinity;

    /**
     * Testing a named constraint
     */
    Constraint constraint1 = Constraint::createGreaterTypeConstraint(testName,
        testLowerBound);
    TEST_ASSERT(constraint1.m_type == constraint1.getType());
    TEST_ASSERT(constraint1.m_type == Constraint::GREATER_OR_EQUAL);

    TEST_ASSERT(constraint1.m_lowerBound == testLowerBound);
    TEST_ASSERT(constraint1.getLowerBound() == testLowerBound);

    TEST_ASSERT(constraint1.m_upperBound == testUpperBound);
    TEST_ASSERT(constraint1.getUpperBound() == testUpperBound);

    TEST_ASSERT(constraint1.m_vector == 0);
    TEST_ASSERT(constraint1.getVector() == 0);

    TEST_ASSERT(constraint1.m_name == testName);
    TEST_ASSERT(strcmp(constraint1.getName(), testName) == 0);

    /**
     * Testing an unnamed constraint
     */
    Constraint constraint2 = Constraint::createGreaterTypeConstraint(0,
        testLowerBound);
    TEST_ASSERT(constraint2.m_type == constraint2.getType());
    TEST_ASSERT(constraint2.m_type == Constraint::GREATER_OR_EQUAL);

    TEST_ASSERT(constraint2.m_lowerBound == testLowerBound);
    TEST_ASSERT(constraint2.getLowerBound() == testLowerBound);

    TEST_ASSERT(constraint2.m_upperBound == testUpperBound);
    TEST_ASSERT(constraint2.getUpperBound() == testUpperBound);

    TEST_ASSERT(constraint2.m_vector == 0);
    TEST_ASSERT(constraint2.getVector() == 0);

    TEST_ASSERT(constraint2.m_name == "");
    TEST_ASSERT(strcmp(constraint2.getName(), "<NO NAME>") == 0);

    const Numerical::Double testLowerBound2 = -PInfinity;

    /**
     * Testing a named constraint, without lower bound
     */

    Constraint constraint3 = Constraint::createGreaterTypeConstraint(testName,
        testLowerBound2);

    TEST_ASSERT(constraint3.m_type == constraint3.getType());
    TEST_ASSERT(constraint3.m_type == Constraint::NON_BINDING);

    TEST_ASSERT(constraint3.m_lowerBound == testLowerBound2);
    TEST_ASSERT(constraint3.getLowerBound() == testLowerBound2);

    TEST_ASSERT(constraint3.m_upperBound == testUpperBound);
    TEST_ASSERT(constraint3.getUpperBound() == testUpperBound);

    TEST_ASSERT(constraint3.m_vector == 0);
    TEST_ASSERT(constraint3.getVector() == 0);

    TEST_ASSERT(constraint3.m_name == testName);
    TEST_ASSERT(strcmp(constraint3.getName(), testName) == 0);

    /**
     * Testing an unnamed variable, without lower bound
     */
    Constraint constraint4 = Constraint::createGreaterTypeConstraint(0,
        testLowerBound2);
    TEST_ASSERT(constraint4.m_type == constraint4.getType());
    TEST_ASSERT(constraint4.m_type == Constraint::NON_BINDING);

    TEST_ASSERT(constraint4.m_lowerBound == testLowerBound2);
    TEST_ASSERT(constraint4.getLowerBound() == testLowerBound2);

    TEST_ASSERT(constraint4.m_upperBound == testUpperBound);
    TEST_ASSERT(constraint4.getUpperBound() == testUpperBound);

    TEST_ASSERT(constraint4.m_vector == 0);
    TEST_ASSERT(constraint4.getVector() == 0);

    TEST_ASSERT(constraint4.m_name == "");
    TEST_ASSERT(strcmp(constraint4.getName(), "<NO NAME>") == 0);


    bool invalidLowerBoundExceptionCheck = false;
    try {
        Numerical::Double testLowerBound3 = PInfinity;
        Constraint constraint5 = Constraint::createGreaterTypeConstraint(0,
            testLowerBound3);
    } catch (Constraint::InvalidLowerBoundException & exception) {
        invalidLowerBoundExceptionCheck = true;

    } catch (...) {

    }
    TEST_ASSERT(invalidLowerBoundExceptionCheck == true);

}

void ConstraintTestSuite::createLessType()
{
    const char * testName = "x6";
    const Numerical::Double testLowerBound = -PInfinity;
    const Numerical::Double testUpperBound = 102.0;

    /**
     * Testing a named constraint
     */
    Constraint constraint1 = Constraint::createLessTypeConstraint(testName,
        testUpperBound);
    TEST_ASSERT(constraint1.m_type == constraint1.getType());
    TEST_ASSERT(constraint1.m_type == Constraint::LESS_OR_EQUAL);

    TEST_ASSERT(constraint1.m_lowerBound == testLowerBound);
    TEST_ASSERT(constraint1.getLowerBound() == testLowerBound);

    TEST_ASSERT(constraint1.m_upperBound == testUpperBound);
    TEST_ASSERT(constraint1.getUpperBound() == testUpperBound);

    TEST_ASSERT(constraint1.m_vector == 0);
    TEST_ASSERT(constraint1.getVector() == 0);

    TEST_ASSERT(constraint1.m_name == testName);
    TEST_ASSERT(strcmp(constraint1.getName(), testName) == 0);

    /**
     * Testing an unnamed constraint
     */
    Constraint constraint2 = Constraint::createLessTypeConstraint(0,
        testUpperBound);
    TEST_ASSERT(constraint2.m_type == constraint2.getType());
    TEST_ASSERT(constraint2.m_type == Constraint::LESS_OR_EQUAL);

    TEST_ASSERT(constraint2.m_lowerBound == testLowerBound);
    TEST_ASSERT(constraint2.getLowerBound() == testLowerBound);

    TEST_ASSERT(constraint2.m_upperBound == testUpperBound);
    TEST_ASSERT(constraint2.getUpperBound() == testUpperBound);

    TEST_ASSERT(constraint2.m_vector == 0);
    TEST_ASSERT(constraint2.getVector() == 0);

    TEST_ASSERT(constraint2.m_name == "");
    TEST_ASSERT(strcmp(constraint2.getName(), "<NO NAME>") == 0);

    const Numerical::Double testUpperBound2 = PInfinity;
    /**
     * Testing a named variable, without upper bound
     */
    Constraint constraint3 = Constraint::createLessTypeConstraint(testName,
        testUpperBound2);
    TEST_ASSERT(constraint3.m_type == constraint3.getType());
    TEST_ASSERT(constraint3.m_type == Constraint::NON_BINDING);

    TEST_ASSERT(constraint3.m_lowerBound == testLowerBound);
    TEST_ASSERT(constraint3.getLowerBound() == testLowerBound);

    TEST_ASSERT(constraint3.m_upperBound == testUpperBound2);
    TEST_ASSERT(constraint3.getUpperBound() == testUpperBound2);

    TEST_ASSERT(constraint3.m_vector == 0);
    TEST_ASSERT(constraint3.getVector() == 0);

    TEST_ASSERT(constraint3.m_name == testName);
    TEST_ASSERT(strcmp(constraint3.getName(), testName) == 0);

    /**
     * Testing an unnamed variable, without upper bound
     */
    Constraint constraint4 = Constraint::createLessTypeConstraint(0,
        testUpperBound2);
    TEST_ASSERT(constraint4.m_type == constraint4.getType());
    TEST_ASSERT(constraint4.m_type == Constraint::NON_BINDING);

    TEST_ASSERT(constraint4.m_lowerBound == testLowerBound);
    TEST_ASSERT(constraint4.getLowerBound() == testLowerBound);

    TEST_ASSERT(constraint4.m_upperBound == testUpperBound2);
    TEST_ASSERT(constraint4.getUpperBound() == testUpperBound2);

    TEST_ASSERT(constraint4.m_vector == 0);
    TEST_ASSERT(constraint4.getVector() == 0);

    TEST_ASSERT(constraint4.m_name == "");
    TEST_ASSERT(strcmp(constraint4.getName(), "<NO NAME>") == 0);

    bool invalidUpperBoundExceptionCheck = false;
    try {
        Numerical::Double testUpperBound3 = -PInfinity;
        Constraint constraint5 = Constraint::createLessTypeConstraint(0,
            testUpperBound3);
    } catch (Constraint::InvalidUpperBoundException & exception) {
        invalidUpperBoundExceptionCheck = true;

    } catch (...) {

    }
    TEST_ASSERT(invalidUpperBoundExceptionCheck == true);
}

void ConstraintTestSuite::createRangeType()
{
    const char * testName = "x1";
    const Numerical::Double testLowerBound = -2.0;
    const Numerical::Double testUpperBound = 42.0;

    /**
     * Testing a named constraint
     */
    Constraint constraint1 = Constraint::createRangeTypeConstraint(testName,
        testLowerBound,
        testUpperBound);
    TEST_ASSERT(constraint1.m_type == constraint1.getType());
    TEST_ASSERT(constraint1.m_type == Constraint::RANGE);

    TEST_ASSERT(constraint1.m_lowerBound == testLowerBound);
    TEST_ASSERT(constraint1.getLowerBound() == testLowerBound);

    TEST_ASSERT(constraint1.m_upperBound == testUpperBound);
    TEST_ASSERT(constraint1.getUpperBound() == testUpperBound);

    TEST_ASSERT(constraint1.m_vector == 0);
    TEST_ASSERT(constraint1.getVector() == 0);

    TEST_ASSERT(constraint1.m_name == testName);
    TEST_ASSERT(strcmp(constraint1.getName(), testName) == 0);

    /**
     * Testing an unnamed constraint
     */
    Constraint constraint2 = Constraint::createRangeTypeConstraint(0,
        testLowerBound,
        testUpperBound);
    TEST_ASSERT(constraint2.m_type == constraint2.getType());
    TEST_ASSERT(constraint2.m_type == Constraint::RANGE);

    TEST_ASSERT(constraint2.m_lowerBound == testLowerBound);
    TEST_ASSERT(constraint2.getLowerBound() == testLowerBound);

    TEST_ASSERT(constraint2.m_upperBound == testUpperBound);
    TEST_ASSERT(constraint2.getUpperBound() == testUpperBound);

    TEST_ASSERT(constraint2.m_vector == 0);
    TEST_ASSERT(constraint2.getVector() == 0);

    TEST_ASSERT(constraint2.m_name == "");
    TEST_ASSERT(strcmp(constraint2.getName(), "<NO NAME>") == 0);
    
    /**
     * Testing a greater type constraint
     */

    const Numerical::Double testUpperBound2 = PInfinity;

    Constraint constraint3 = Constraint::createRangeTypeConstraint(testName,
        testLowerBound,
        testUpperBound2);
    TEST_ASSERT(constraint3.m_type == constraint3.getType());
    TEST_ASSERT(constraint3.m_type == Constraint::GREATER_OR_EQUAL);

    TEST_ASSERT(constraint3.m_lowerBound == testLowerBound);
    TEST_ASSERT(constraint3.getLowerBound() == testLowerBound);

    TEST_ASSERT(constraint3.m_upperBound == testUpperBound2);
    TEST_ASSERT(constraint3.getUpperBound() == testUpperBound2);

    TEST_ASSERT(constraint3.m_vector == 0);
    TEST_ASSERT(constraint3.getVector() == 0);

    TEST_ASSERT(constraint3.m_name == testName);
    TEST_ASSERT(strcmp(constraint3.getName(), testName) == 0);

    /**
     * Testing a less type constraint
     */

    const Numerical::Double testLowerBound2 = -PInfinity;

    Constraint constraint4 = Constraint::createRangeTypeConstraint(testName,
        testLowerBound2,
        testUpperBound);
    TEST_ASSERT(constraint4.m_type == constraint4.getType());
    TEST_ASSERT(constraint4.m_type == Constraint::LESS_OR_EQUAL);

    TEST_ASSERT(constraint4.m_lowerBound == testLowerBound2);
    TEST_ASSERT(constraint4.getLowerBound() == testLowerBound2);

    TEST_ASSERT(constraint4.m_upperBound == testUpperBound);
    TEST_ASSERT(constraint4.getUpperBound() == testUpperBound);

    TEST_ASSERT(constraint4.m_vector == 0);
    TEST_ASSERT(constraint4.getVector() == 0);

    TEST_ASSERT(constraint4.m_name == testName);
    TEST_ASSERT(strcmp(constraint4.getName(), testName) == 0);

    /**
     * Testing a non-binding type constraint
     */

    Constraint constraint5 = Constraint::createRangeTypeConstraint(testName,
        testLowerBound2,
        testUpperBound2);
    TEST_ASSERT(constraint5.m_type == constraint5.getType());
    TEST_ASSERT(constraint5.m_type == Constraint::NON_BINDING);

    TEST_ASSERT(constraint5.m_lowerBound == testLowerBound2);
    TEST_ASSERT(constraint5.getLowerBound() == testLowerBound2);

    TEST_ASSERT(constraint5.m_upperBound == testUpperBound2);
    TEST_ASSERT(constraint5.getUpperBound() == testUpperBound2);

    TEST_ASSERT(constraint5.m_vector == 0);
    TEST_ASSERT(constraint5.getVector() == 0);

    TEST_ASSERT(constraint5.m_name == testName);
    TEST_ASSERT(strcmp(constraint5.getName(), testName) == 0);

    /**
     * Testing an equality type constraint
     */
    Numerical::Double testValue = 10.0;
    
    Constraint constraint6 = Constraint::createRangeTypeConstraint(testName, 
        testValue, testValue);
    TEST_ASSERT(constraint6.m_type == constraint6.getType());
    TEST_ASSERT(constraint6.m_type == Constraint::EQUALITY);

    TEST_ASSERT(constraint6.m_lowerBound == testValue);
    TEST_ASSERT(constraint6.getLowerBound() == testValue);

    TEST_ASSERT(constraint6.m_upperBound == testValue);
    TEST_ASSERT(constraint6.getUpperBound() == testValue);

    TEST_ASSERT(constraint6.m_vector == 0);
    TEST_ASSERT(constraint6.getVector() == 0);

    TEST_ASSERT(constraint6.m_name == testName);
    TEST_ASSERT(strcmp(constraint6.getName(), testName) == 0);

    bool invalidUpperBoundExceptionCheck = false;
    try {
        Numerical::Double testUpperBound3 = -PInfinity;
        Constraint constraint7 = Constraint::createRangeTypeConstraint(testName,
            testLowerBound,
            testUpperBound3);
    } catch (Constraint::InvalidUpperBoundException & exception) {
        invalidUpperBoundExceptionCheck = true;

    } catch (...) {

    }
    TEST_ASSERT(invalidUpperBoundExceptionCheck == true);

    bool invalidLowerBoundExceptionCheck = false;
    try {
        Numerical::Double testLowerBound3 = PInfinity;
        Constraint constraint7 = Constraint::createRangeTypeConstraint(testName,
            testLowerBound3,
            testUpperBound);
    } catch (Constraint::InvalidLowerBoundException & exception) {
        invalidLowerBoundExceptionCheck = true;

    } catch (...) {

    }
    TEST_ASSERT(invalidLowerBoundExceptionCheck == true);

    bool invalidBoundsExceptionCheck = false;
    try {
        Numerical::Double testLowerBound4 = 10.0;
        Numerical::Double testUpperBound4 = 0.0;
        Constraint constraint8 = Constraint::createRangeTypeConstraint(testName,
            testLowerBound4,
            testUpperBound4);
    } catch (Constraint::InvalidBoundsException & exception) {
        invalidBoundsExceptionCheck = true;

    } catch (...) {

    }
    TEST_ASSERT(invalidBoundsExceptionCheck == true);    
}

void ConstraintTestSuite::createEqualityType()
{
    const char * testName = "x1";
    const Numerical::Double testLowerBound = 12.0;
    const Numerical::Double testUpperBound = 12.0;

    /**
     * Testing a named constraint
     */
    Constraint constraint1 = Constraint::createEqualityTypeConstraint(testName,
        testLowerBound);
    TEST_ASSERT(constraint1.m_type == constraint1.getType());
    TEST_ASSERT(constraint1.m_type == Constraint::EQUALITY);

    TEST_ASSERT(constraint1.m_lowerBound == testLowerBound);
    TEST_ASSERT(constraint1.getLowerBound() == testLowerBound);

    TEST_ASSERT(constraint1.m_upperBound == testUpperBound);
    TEST_ASSERT(constraint1.getUpperBound() == testUpperBound);

    TEST_ASSERT(constraint1.m_vector == 0);
    TEST_ASSERT(constraint1.getVector() == 0);

    TEST_ASSERT(constraint1.m_name == testName);
    TEST_ASSERT(strcmp(constraint1.getName(), testName) == 0);

    /**
     * Testing an unnamed constraint
     */
    Constraint constraint2 = Constraint::createEqualityTypeConstraint(0,
        testLowerBound);
    TEST_ASSERT(constraint2.m_type == constraint2.getType());
    TEST_ASSERT(constraint2.m_type == Constraint::EQUALITY);

    TEST_ASSERT(constraint2.m_lowerBound == testLowerBound);
    TEST_ASSERT(constraint2.getLowerBound() == testLowerBound);

    TEST_ASSERT(constraint2.m_upperBound == testUpperBound);
    TEST_ASSERT(constraint2.getUpperBound() == testUpperBound);

    TEST_ASSERT(constraint2.m_vector == 0);
    TEST_ASSERT(constraint2.getVector() == 0);

    TEST_ASSERT(constraint2.m_name == "");
    TEST_ASSERT(strcmp(constraint2.getName(), "<NO NAME>") == 0);
}

void ConstraintTestSuite::createNonBindingType()
{
    const char * testName = "x1";
    const Numerical::Double testLowerBound = -PInfinity;
    const Numerical::Double testUpperBound = PInfinity;

    /**
     * Testing a named constraint
     */
    Constraint constraint1 = Constraint::createNonBindingTypeConstraint(testName);
    TEST_ASSERT(constraint1.m_type == constraint1.getType());
    TEST_ASSERT(constraint1.m_type == Constraint::NON_BINDING);

    TEST_ASSERT(constraint1.m_lowerBound == testLowerBound);
    TEST_ASSERT(constraint1.getLowerBound() == testLowerBound);

    TEST_ASSERT(constraint1.m_upperBound == testUpperBound);
    TEST_ASSERT(constraint1.getUpperBound() == testUpperBound);

    TEST_ASSERT(constraint1.m_vector == 0);
    TEST_ASSERT(constraint1.getVector() == 0);

    TEST_ASSERT(constraint1.m_name == testName);
    TEST_ASSERT(strcmp(constraint1.getName(), testName) == 0);

    /**
     * Testing an unnamed constraint
     */
    Constraint constraint2 = Constraint::createNonBindingTypeConstraint(0);
    TEST_ASSERT(constraint2.m_type == constraint2.getType());
    TEST_ASSERT(constraint2.m_type == Constraint::NON_BINDING);

    TEST_ASSERT(constraint2.m_lowerBound == testLowerBound);
    TEST_ASSERT(constraint2.getLowerBound() == testLowerBound);

    TEST_ASSERT(constraint2.m_upperBound == testUpperBound);
    TEST_ASSERT(constraint2.getUpperBound() == testUpperBound);

    TEST_ASSERT(constraint2.m_vector == 0);
    TEST_ASSERT(constraint2.getVector() == 0);

    TEST_ASSERT(constraint2.m_name == "");
    TEST_ASSERT(strcmp(constraint2.getName(), "<NO NAME>") == 0);
}

void ConstraintTestSuite::setGetLowerBound()
{
    Constraint constraint;
    constraint.setLowerBound(-10);
    TEST_ASSERT(constraint.m_lowerBound == -10.0);
    TEST_ASSERT(constraint.getLowerBound() == -10.0);

    constraint.setLowerBound(10);
    TEST_ASSERT(constraint.m_lowerBound == 10.0);
    TEST_ASSERT(constraint.getLowerBound() == 10.0);

    constraint.setLowerBound(0);
    TEST_ASSERT(constraint.m_lowerBound == 0);
    TEST_ASSERT(constraint.getLowerBound() == 0);

    constraint.setLowerBound(-PInfinity);
    TEST_ASSERT(constraint.m_lowerBound == -PInfinity);
    TEST_ASSERT(constraint.getLowerBound() == -PInfinity);
    
    bool invalidLowerBoundException = false;
    try {
        constraint.setLowerBound(PInfinity);
    } catch (Constraint::InvalidLowerBoundException & exception) {
        invalidLowerBoundException = true;
    } catch (...) {

    }
    TEST_ASSERT(invalidLowerBoundException == true);

    bool invalidBoundsException = false;
    try {
        constraint.setLowerBound(-PInfinity);
        constraint.setUpperBound(10.0);
        constraint.setLowerBound(20.0);
    } catch (Constraint::InvalidBoundsException & exception) {
        invalidBoundsException = true;
    } catch (...) {

    }
    TEST_ASSERT(invalidBoundsException == true);    
}

void ConstraintTestSuite::setGetUpperBound()
{
    Constraint constraint;
    constraint.setLowerBound(-20);
    constraint.setUpperBound(-10);
    TEST_ASSERT(constraint.m_upperBound == -10.0);
    TEST_ASSERT(constraint.getUpperBound() == -10.0);

    constraint.setUpperBound(10);
    TEST_ASSERT(constraint.m_upperBound == 10.0);
    TEST_ASSERT(constraint.getUpperBound() == 10.0);

    constraint.setUpperBound(0);
    TEST_ASSERT(constraint.m_upperBound == 0);
    TEST_ASSERT(constraint.getUpperBound() == 0);

    constraint.setUpperBound(PInfinity);
    TEST_ASSERT(constraint.m_upperBound == PInfinity);
    TEST_ASSERT(constraint.getUpperBound() == PInfinity);
    
    bool invalidUpperBoundException = false;
    try {
        constraint.setUpperBound(-PInfinity);
    } catch (Constraint::InvalidUpperBoundException & exception) {
        invalidUpperBoundException = true;
    } catch (...) {

    }
    TEST_ASSERT(invalidUpperBoundException == true);

    bool invalidBoundsException = false;
    try {
        constraint.setUpperBound(PInfinity);
        constraint.setLowerBound(20.0);
        constraint.setUpperBound(10.0);
    } catch (Constraint::InvalidBoundsException & exception) {
        invalidBoundsException = true;
    } catch (...) {

    }
    TEST_ASSERT(invalidBoundsException == true);    
}

void ConstraintTestSuite::setGetName()
{
    Constraint constraint;
    constraint.setName("");
    TEST_ASSERT(constraint.m_name == "");
    TEST_ASSERT(strcmp(constraint.getName(), "<NO NAME>") == 0);

    constraint.setName("x1");
    TEST_ASSERT(constraint.m_name == "x1");
    TEST_ASSERT(strcmp(constraint.getName(), "x1") == 0);
}

void ConstraintTestSuite::setGetVector()
{
    Constraint constraint;
    const Vector vector1, vector2;

    constraint.setVector(vector1);
    TEST_ASSERT(constraint.m_vector == constraint.getVector());
    TEST_ASSERT(constraint.m_vector == &vector1);

    constraint.setVector(vector2);
    TEST_ASSERT(constraint.m_vector == constraint.getVector());
    TEST_ASSERT(constraint.m_vector == &vector2);
}

void ConstraintTestSuite::type()
{
    Constraint constraint1;
    constraint1.setLowerBound(-PInfinity);
    constraint1.setUpperBound(PInfinity);
    TEST_ASSERT(constraint1.m_type == constraint1.getType());
    TEST_ASSERT(constraint1.m_type == Constraint::NON_BINDING);

    Constraint constraint2;
    constraint2.setLowerBound(0.0);
    constraint2.setUpperBound(0.0);
    TEST_ASSERT(constraint2.m_type == constraint2.getType());
    TEST_ASSERT(constraint2.m_type == Constraint::EQUALITY);

    Constraint constraint3;
    constraint3.setLowerBound(-10.0);
    constraint3.setUpperBound(-10.0);
    TEST_ASSERT(constraint3.m_type == constraint3.getType());
    TEST_ASSERT(constraint3.m_type == Constraint::EQUALITY);

    Constraint constraint4;
    constraint4.setLowerBound(10.0);
    constraint4.setUpperBound(10.0);
    TEST_ASSERT(constraint4.m_type == constraint4.getType());
    TEST_ASSERT(constraint4.m_type == Constraint::EQUALITY);

    Constraint constraint5;
    constraint5.setLowerBound(-10.0);
    constraint5.setUpperBound(PInfinity);
    TEST_ASSERT(constraint5.m_type == constraint5.getType());
    TEST_ASSERT(constraint5.m_type == Constraint::GREATER_OR_EQUAL);

    Constraint constraint6;
    constraint6.setLowerBound(0.0);
    constraint6.setUpperBound(PInfinity);
    TEST_ASSERT(constraint6.m_type == constraint6.getType());
    TEST_ASSERT(constraint6.m_type == Constraint::GREATER_OR_EQUAL);

    Constraint constraint7;
    constraint7.setLowerBound(10.0);
    constraint7.setUpperBound(PInfinity);
    TEST_ASSERT(constraint7.m_type == constraint7.getType());
    TEST_ASSERT(constraint7.m_type == Constraint::GREATER_OR_EQUAL);

    Constraint constraint8;
    constraint8.setLowerBound(-PInfinity);
    constraint8.setUpperBound(-10.0);
    TEST_ASSERT(constraint8.m_type == constraint8.getType());
    TEST_ASSERT(constraint8.m_type == Constraint::LESS_OR_EQUAL);

    Constraint constraint9;
    constraint9.setLowerBound(-PInfinity);
    constraint9.setUpperBound(0.0);
    TEST_ASSERT(constraint9.m_type == constraint9.getType());
    TEST_ASSERT(constraint9.m_type == Constraint::LESS_OR_EQUAL);

    Constraint constraint10;
    constraint10.setLowerBound(-PInfinity);
    constraint10.setUpperBound(10.0);
    TEST_ASSERT(constraint10.m_type == constraint10.getType());
    TEST_ASSERT(constraint10.m_type == Constraint::LESS_OR_EQUAL);

    Constraint constraint11;
    constraint11.setLowerBound(-20.0);
    constraint11.setUpperBound(-10.0);
    TEST_ASSERT(constraint11.m_type == constraint11.getType());
    TEST_ASSERT(constraint11.m_type == Constraint::RANGE);

    Constraint constraint12;
    constraint12.setLowerBound(-20.0);
    constraint12.setUpperBound(0.0);
    TEST_ASSERT(constraint12.m_type == constraint12.getType());
    TEST_ASSERT(constraint12.m_type == Constraint::RANGE);

    Constraint constraint13;
    constraint13.setLowerBound(-20.0);
    constraint13.setUpperBound(10.0);
    TEST_ASSERT(constraint13.m_type == constraint13.getType());
    TEST_ASSERT(constraint13.m_type == Constraint::RANGE);

    Constraint constraint14;
    constraint14.setLowerBound(0.0);
    constraint14.setUpperBound(10.0);
    TEST_ASSERT(constraint14.m_type == constraint14.getType());
    TEST_ASSERT(constraint14.m_type == Constraint::RANGE);

    Constraint constraint15;
    constraint15.setLowerBound(10.0);
    constraint15.setUpperBound(20.0);
    TEST_ASSERT(constraint15.m_type == constraint15.getType());
    TEST_ASSERT(constraint15.m_type == Constraint::RANGE);
}

void ConstraintTestSuite::equalityOperator()
{
    Constraint constraint1, constraint2;
    TEST_ASSERT(constraint1 == constraint2);

    constraint1.setName("r1");
    TEST_ASSERT(!(constraint1 == constraint2));
    constraint1.setName("");
    TEST_ASSERT(constraint1 == constraint2);

    constraint1.setLowerBound(-1.0);
    TEST_ASSERT(!(constraint1 == constraint2));
    constraint1.setLowerBound(0.0);
    TEST_ASSERT(constraint1 == constraint2);

    constraint1.setUpperBound(1.0);
    TEST_ASSERT(!(constraint1 == constraint2));
    constraint1.setUpperBound(PInfinity);
    TEST_ASSERT(constraint1 == constraint2);

    Vector vector;
    constraint1.setVector(vector);
    TEST_ASSERT(!(constraint1 == constraint2));
}

void ConstraintTestSuite::assign()
{
    Constraint constraint1 = Constraint::createNonBindingTypeConstraint("r1");
    Constraint constraint2 = Constraint::createEqualityTypeConstraint("r4", 6);
    constraint1 = constraint2;

    const Numerical::Double outputLowerBound1 = 6.0;
    const Numerical::Double outputUpperBound1 = 6.0;
    const Constraint::TYPE outputType1 = Constraint::EQUALITY;
    const std::string outputName1 = "r4";
    Vector * const outputVector1 = 0;

    TEST_ASSERT(constraint1.m_lowerBound == outputLowerBound1);
    TEST_ASSERT(constraint1.m_upperBound == outputUpperBound1);
    TEST_ASSERT(constraint1.m_type == outputType1);
    TEST_ASSERT(constraint1.m_name == outputName1);
    TEST_ASSERT(constraint1.m_vector == outputVector1);

    constraint2 = constraint2;

    const Numerical::Double outputLowerBound2 = 6.0;
    const Numerical::Double outputUpperBound2 = 6.0;
    const Constraint::TYPE outputType2 = Constraint::EQUALITY;
    const std::string outputName2 = "r4";
    Vector * const outputVector2 = 0;

    TEST_ASSERT(constraint2.m_lowerBound == outputLowerBound2);
    TEST_ASSERT(constraint2.m_upperBound == outputUpperBound2);
    TEST_ASSERT(constraint2.m_type == outputType2);
    TEST_ASSERT(constraint2.m_name == outputName2);
    TEST_ASSERT(constraint2.m_vector == outputVector2);
}

void ConstraintTestSuite::copy()
{
    Constraint constraint1 = Constraint::createEqualityTypeConstraint("r4", 6);
    Constraint constraint2 = constraint1;

    const Numerical::Double outputLowerBound1 = 6.0;
    const Numerical::Double outputUpperBound1 = 6.0;
    const Constraint::TYPE outputType1 = Constraint::EQUALITY;
    const std::string outputName1 = "r4";
    Vector * const outputVector1 = 0;

    TEST_ASSERT(constraint2.m_lowerBound == outputLowerBound1);
    TEST_ASSERT(constraint2.m_upperBound == outputUpperBound1);
    TEST_ASSERT(constraint2.m_type == outputType1);
    TEST_ASSERT(constraint2.m_name == outputName1);
    TEST_ASSERT(constraint2.m_vector == outputVector1);
}
