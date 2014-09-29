/**
 * @file sbf_logical.cpp
 */

#include <simplex/startingbasisfinder/sbf_logical.h>

#include <simplex/simplex.h>

SbfLogical::SbfLogical(const SimplexModel& model,
                       std::vector<int>* basisHead,
                       IndexList<const Numerical::Double*>* variableStates,
                       StartingBasisFinder::STARTING_NONBASIC_STATES nonbasicStates):
    SbfSuper(model, basisHead, variableStates, nonbasicStates)
{

}

SbfLogical::~SbfLogical()
{

}

void SbfLogical::run()
{
    unsigned int i;
    unsigned int j;

    unsigned int rowCount = m_model.getRowCount();
    unsigned int columnCount = m_model.getColumnCount();

    m_basisHead->clear();

    /* Basic variables: set state to BASIC */
    for (i=0, j=columnCount; i < rowCount; i++, j++) {
        m_basisHead->push_back(j);
        m_variableStates->insert(Simplex::BASIC, j, &ZERO);
    }

    /* Nonbasic variables: set state to NONBASIC_AT_UB/LB depending on the strategy used */
    switch (m_nonbasicStates) {

    case StartingBasisFinder::LOWER: {
        for (i=0; i<columnCount; i++) {
            adjustVariableByType(i, Simplex::NONBASIC_AT_LB);
        }
        break;
    }

    case StartingBasisFinder::UPPER: {
        for (i=0; i<columnCount; i++) {
            adjustVariableByType(i, Simplex::NONBASIC_AT_UB);
        }
        break;
    }

    case StartingBasisFinder::MIXED: {
        const Vector & costs = m_model.getCostVector();
        //Always minimize
        for (i=0; i<columnCount; i++) {
            if (costs.at(i) < 0) {
                adjustVariableByType(i, Simplex::NONBASIC_AT_UB);
            } else {
                adjustVariableByType(i, Simplex::NONBASIC_AT_LB);
            }
        }
        break;
    }

    default: {
        throw ParameterException("Invalid starting nonbasic state given!");
        break;
    }

    }
}
