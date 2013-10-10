/**
 * @file sbf_logical.cpp
 */

#include <simplex/startingbasisfinder/sbf_logical.h>

#include <simplex/simplex.h>

SbfLogical::SbfLogical(const SimplexModel& model,
                       std::vector<int>* basisHead,
                       IndexList<const Numerical::Double*>* variableStates,
                       Vector* basicVaraibleValues,
                       LOGICAL_BASIS_STRATEGY strategy):
    SbfSuper(model, basisHead, variableStates, basicVaraibleValues),
    m_strategy(strategy)
{

}

SbfLogical::~SbfLogical()
{

}

void SbfLogical::run()
{
    register unsigned int i;
    register unsigned int j;

    unsigned int rowCount = m_model.getRowCount();
    unsigned int columnCount = m_model.getColumnCount();

    m_basisHead->clear();

    /* Basic variables: set state to BASIC */
    for (i=0, j=columnCount; i < rowCount; i++, j++) {
        m_basisHead->push_back(j);
        m_variableStates->insert(Simplex::BASIC, j, &INVALID);
    }

    /* Nonbasic variables: set state to NONBASIC_AT_UB/LB depending on the strategy used */
    switch (m_strategy) {

    case LOWER_LOGICAL: {
        for (i=0; i<columnCount; i++) {
            adjustVariableByType(i, Simplex::NONBASIC_AT_LB);
        }
        break;
    }

    case UPPER_LOGICAL: {
        for (i=0; i<columnCount; i++) {
            adjustVariableByType(i, Simplex::NONBASIC_AT_UB);
        }
        break;
    }

    case MIXED_LOGICAL: {
        const Vector & costs = m_model.getCostVector();
        switch (m_model.getObjectiveType()) {
        case MINIMIZE:
            for (i=0; i<columnCount; i++) {
                if (costs.at(i) < 0) {
                    adjustVariableByType(i, Simplex::NONBASIC_AT_LB);
                } else {
                    adjustVariableByType(i, Simplex::NONBASIC_AT_UB);
                }
            }
            break;

        case MAXIMIZE:
            for (i=0; i<columnCount; i++) {
                if (costs.at(i) > 0) {
                    adjustVariableByType(i, Simplex::NONBASIC_AT_UB);
                } else {
                    adjustVariableByType(i, Simplex::NONBASIC_AT_LB);
                }
            }
            break;
        default:
            LPERROR("Unknown objective type");
        }
    }

    default: {
        /* */
        DEVWARNING(D::SBF_LOGICAL, "Unhandled logical basis finder algorithm selected.");
        break;
    }

    }
}
