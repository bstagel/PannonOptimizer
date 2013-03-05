#include "modeltest.h"
#include <lp/manualmodelbuilder.h>

void ModelTestSuite::run()
{
    init();
    build();
}

void ModelTestSuite::init()
{
    Model model;

    TEST_ASSERT(model.getName() == "");
    TEST_ASSERT(model.m_name == "");
    TEST_ASSERT(model.m_costConstant == 0.0);
    TEST_ASSERT(model.getCostConstant() == 0.0);
    TEST_ASSERT(model.m_constraints.size() == 0);
    TEST_ASSERT(model.constraintCount() == 0);
    TEST_ASSERT(model.m_variables.size() == 0);
    TEST_ASSERT(model.variableCount() == 0);
    TEST_ASSERT(model.m_costVector.length() == 0);
    TEST_ASSERT(model.m_matrix.rowCount() == 0);
    TEST_ASSERT(model.m_matrix.columnCount() == 0);
}

void ModelTestSuite::build()
{
    /***************************************************
     * Testing an empty model building
     **************************************************/

    Model model1;
    ManualModelBuilder builder1;
    model1.build(builder1);

    TEST_ASSERT(model1.getName() == "");
    TEST_ASSERT(model1.m_name == "");
    TEST_ASSERT(model1.m_costConstant == 0.0);
    TEST_ASSERT(model1.getCostConstant() == 0.0);
    TEST_ASSERT(model1.m_constraints.size() == 0);
    TEST_ASSERT(model1.constraintCount() == 0);
    TEST_ASSERT(model1.m_variables.size() == 0);
    TEST_ASSERT(model1.variableCount() == 0);
    TEST_ASSERT(model1.m_costVector.length() == 0);
    TEST_ASSERT(model1.m_matrix.rowCount() == 0);
    TEST_ASSERT(model1.m_matrix.columnCount() == 0);

    /**
     * TODO:
     * Testing other cases
     */

}