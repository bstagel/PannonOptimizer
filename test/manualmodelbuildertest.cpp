#include "manualmodelbuildertest.h"

void ManualModelBuilderTestSuite::init()
{
    ManualModelBuilder builder;
    TEST_ASSERT(builder.m_objectiveConstant == 0.0);
    TEST_ASSERT(builder.getObjectiveConstant() == 0.0);

    TEST_ASSERT(builder.m_name == "");
    TEST_ASSERT(builder.getName() == "");

    TEST_ASSERT(builder.m_columns.size() == 0);
    TEST_ASSERT(builder.m_nonZerosInColumns.size() == 0);
    TEST_ASSERT(builder.m_rows.size() == 0);
    TEST_ASSERT(builder.m_nonZerosInRows.size() == 0);
    TEST_ASSERT(builder.m_constraints.size() == 0);
    TEST_ASSERT(builder.m_variables.size() == 0);
    TEST_ASSERT(builder.m_costVector.size() == 0);
}

void ManualModelBuilderTestSuite::setGetName()
{
    const std::string testName = "Demo Model";
    ManualModelBuilder builder;
    builder.setName(testName);
    TEST_ASSERT(builder.m_name == testName);
    TEST_ASSERT(builder.getName() == testName);
}

void ManualModelBuilderTestSuite::setGetObjectiveFunctionConstant()
{
    const Numerical::Double testConstant = 12.4;
    ManualModelBuilder builder;
    builder.setObjectiveFunctionConstant(testConstant);
    TEST_ASSERT(builder.m_objectiveConstant == testConstant);
    TEST_ASSERT(builder.getObjectiveConstant() == testConstant);
    TEST_ASSERT(builder.m_variables.size() == 0);
    TEST_ASSERT(builder.m_constraints.size() == 0);
    TEST_ASSERT(builder.m_costVector.size() == 0);
    TEST_ASSERT(builder.m_columns.size() == 0);
    TEST_ASSERT(builder.m_nonZerosInColumns.size() == 0);
    TEST_ASSERT(builder.m_rows.size() == 0);
    TEST_ASSERT(builder.m_nonZerosInRows.size() == 1);
}

void ManualModelBuilderTestSuite::addVariable1()
{
    ManualModelBuilder builder;
    Variable testVariable1 = Variable::createMinusTypeVariable("x1", 0.0, 10.0);
    const Numerical::Double testValues1[] = {1.0, -2.3, 0.13};
    const unsigned int testIndices1[] = {7, 3, 10};
    const unsigned int testNonZeros1 = 3;
    const Numerical::Double testCostCoefficient1 = 4.2;

    builder.addVariable(testVariable1, testCostCoefficient1,
        testNonZeros1, testValues1, testIndices1);

    TEST_ASSERT(builder.m_variables.size() == 1);
    TEST_ASSERT(builder.m_columns.size() == 1);
    TEST_ASSERT(builder.m_costVector.size() == 1);
    TEST_ASSERT(builder.m_constraints.size() == 10);
    
    std::list<ManualModelBuilder::IndexValuePair>::const_iterator iter =
        builder.m_columns[0].begin();
    std::list<ManualModelBuilder::IndexValuePair>::const_iterator iterEnd =
        builder.m_columns[0].end();

    for (; iter != iterEnd; iter++) {
        
    }
}