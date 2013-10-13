#include <simplex/primalratiotest.h>



PrimalRatiotest::PrimalRatiotest(const SimplexModel &model, const IndexList<>& variableFeasibilities, const IndexList<const Numerical::Double *>& variableStates):
    m_model(model),m_variableFeasibilities(variableFeasibilities),m_variableStates(variableStates)
{

}
