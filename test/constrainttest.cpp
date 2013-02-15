#include "constrainttest.h"

void ConstraintTestSuite::init()
{
    Constraint constraint;
    TEST_ASSERT(constraint.m_lowerBound == 0.0);
    TEST_ASSERT(constraint.m_upperBound == infinity);
    TEST_ASSERT(constraint.m_type == Constraint::GREATER_OR_EQUAL);
    TEST_ASSERT(constraint.m_name == "");
    TEST_ASSERT(constraint.m_vector == 0);
}

void ConstraintTestSuite::createGreaterType()
{
    const char * testName = "x4";
    const Numerical::Double testLowerBound = -3.0;
    const Numerical::Double testUpperBound = infinity;
    
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
}

void ConstraintTestSuite::createLessType()
{
    const char * testName = "x6";
    const Numerical::Double testLowerBound = -infinity;
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
    const Numerical::Double testLowerBound = -infinity;
    const Numerical::Double testUpperBound = infinity;

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

    constraint.setLowerBound(-infinity);
    TEST_ASSERT(constraint.m_lowerBound == -infinity);
    TEST_ASSERT(constraint.getLowerBound() == -infinity);
}

void ConstraintTestSuite::setGetUpperBound()
{
    Constraint constraint;
    constraint.setUpperBound(-10);
    TEST_ASSERT(constraint.m_upperBound == -10.0);
    TEST_ASSERT(constraint.getUpperBound() == -10.0);

    constraint.setUpperBound(10);
    TEST_ASSERT(constraint.m_upperBound == 10.0);
    TEST_ASSERT(constraint.getUpperBound() == 10.0);

    constraint.setUpperBound(0);
    TEST_ASSERT(constraint.m_upperBound == 0);
    TEST_ASSERT(constraint.getUpperBound() == 0);

    constraint.setUpperBound(infinity);
    TEST_ASSERT(constraint.m_upperBound == infinity);
    TEST_ASSERT(constraint.getUpperBound() == infinity);
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
    constraint1.setLowerBound(-infinity);
    constraint1.setUpperBound(infinity);
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
    constraint5.setUpperBound(infinity);
    TEST_ASSERT(constraint5.m_type == constraint5.getType());
    TEST_ASSERT(constraint5.m_type == Constraint::GREATER_OR_EQUAL);
    
    Constraint constraint6;
    constraint6.setLowerBound(0.0);
    constraint6.setUpperBound(infinity);
    TEST_ASSERT(constraint6.m_type == constraint6.getType());
    TEST_ASSERT(constraint6.m_type == Constraint::GREATER_OR_EQUAL);

    Constraint constraint7;
    constraint7.setLowerBound(10.0);
    constraint7.setUpperBound(infinity);
    TEST_ASSERT(constraint7.m_type == constraint7.getType());
    TEST_ASSERT(constraint7.m_type == Constraint::GREATER_OR_EQUAL);
    
    Constraint constraint8;
    constraint8.setLowerBound(-infinity);
    constraint8.setUpperBound(-10.0);
    TEST_ASSERT(constraint8.m_type == constraint8.getType());
    TEST_ASSERT(constraint8.m_type == Constraint::LESS_OR_EQUAL);
    
    Constraint constraint9;
    constraint9.setLowerBound(-infinity);
    constraint9.setUpperBound(0.0);
    TEST_ASSERT(constraint9.m_type == constraint9.getType());
    TEST_ASSERT(constraint9.m_type == Constraint::LESS_OR_EQUAL);

    Constraint constraint10;
    constraint10.setLowerBound(-infinity);
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

