#include "linalgparameterhandlertest.h"

LinalgParameterHandlerTestSuite::LinalgParameterHandlerTestSuite(const char * name) : UnitTest(name)
{
    ADD_TEST(LinalgParameterHandlerTestSuite::getInstance);
}


void LinalgParameterHandlerTestSuite::getInstance()
{
    TEST_ASSERT(LinalgParameterHandler::getInstance().m_values.count("e_absolute")>0);
    TEST_ASSERT(LinalgParameterHandler::getInstance().m_values.count("e_relative")>0);
    TEST_ASSERT(LinalgParameterHandler::getInstance().m_values.count("sparsity_ratio")>0);
    TEST_ASSERT(LinalgParameterHandler::getInstance().m_values.count("elbowroom")>0);
}
