#include <simplexparameterhandlertest.h>

SimplexParameterHandlerTestSuite::SimplexParameterHandlerTestSuite(const char * name) : UnitTest(name)
{
    ADD_TEST(SimplexParameterHandlerTestSuite::getInstance);
}

void SimplexParameterHandlerTestSuite::getInstance()
{
    TEST_ASSERT(SimplexParameterHandler::getInstance().m_values.count("e_pivot")>0);
    TEST_ASSERT(SimplexParameterHandler::getInstance().m_values.count("e_feasibility")>0);
    TEST_ASSERT(SimplexParameterHandler::getInstance().m_values.count("e_optimality")>0);
    TEST_ASSERT(SimplexParameterHandler::getInstance().m_values.count("presolve")>0);
    TEST_ASSERT(SimplexParameterHandler::getInstance().m_values.count("scaling")>0);
    TEST_ASSERT(SimplexParameterHandler::getInstance().m_values.count("starting_basis")>0);
    TEST_ASSERT(SimplexParameterHandler::getInstance().m_values.count("factorization_type")>0);
    TEST_ASSERT(SimplexParameterHandler::getInstance().m_values.count("triangular_method")>0);
    TEST_ASSERT(SimplexParameterHandler::getInstance().m_values.count("nontriangular_method")>0);
    TEST_ASSERT(SimplexParameterHandler::getInstance().m_values.count("triangular_pivot_rule")>0);
    TEST_ASSERT(SimplexParameterHandler::getInstance().m_values.count("nontriangular_pivot_rule")>0);
    TEST_ASSERT(SimplexParameterHandler::getInstance().m_values.count("pivot_threshold")>0);
    TEST_ASSERT(SimplexParameterHandler::getInstance().m_values.count("reinversion_frequency")>0);
    TEST_ASSERT(SimplexParameterHandler::getInstance().m_values.count("pricing_type")>0);
    TEST_ASSERT(SimplexParameterHandler::getInstance().m_values.count("debug_level")>0);
    TEST_ASSERT(SimplexParameterHandler::getInstance().m_values.count("iteration_limit")>0);
    TEST_ASSERT(SimplexParameterHandler::getInstance().m_values.count("time_limit")>0);
}
