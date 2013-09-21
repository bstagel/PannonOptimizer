/*
 * sbf_logical.cpp
 *
 *  Created on: Apr 1, 2012
 *      Author: Virag Varga <varga@dcs.uni-pannon.hu>
 */

#include <simplex/startingbasisfinder/sbf_logical.h>

#include <simplex/simplex.h>

SbfLogical::SbfLogical(const SimplexModel& model, std::vector<int>& basisHead, IndexList& variableStates, LogicalStrategy strategy)
 : SbfSuper(model, basisHead, variableStates),
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

    unsigned int basisSize = m_model.getRowCount();

    m_basisHead.clear();

    /* Basic variables: set state to BASIC */
    for (i=0, j=basisSize; i < basisSize; i++, j++) {
        m_basisHead.push_back(j);
        //TODO: X_B pointere
        m_variableStates.insert(Simplex::BASIC,j);
    }

    /* Nonbasic variables: set state to NONBASIC_AT_UB/LB depending on the strategy used */
    switch (m_strategy) {

    case LOWER_LOGICAL: {
        for (i=0; i<basisSize; i++) {
            //TODO: pointer
            m_variableStates.insert(Simplex::NONBASIC_AT_LB, i);
        }
        break;
    }

    case UPPER_LOGICAL: {
        for (i=0; i<basisSize; i++) {
            //TODO: pointer
            m_variableStates.insert(Simplex::NONBASIC_AT_UB, i);
        }
        break;
    }

    case MIXED_LOGICAL: {
        const Vector & costs = m_model.getCostVector();
        switch (m_model.getObjectiveType()) {

        case MINIMIZE:
            for (i=0; i<basisSize; i++) {
                if (costs.at(i) < 0) {
                    m_variableStates.insert(Simplex::NONBASIC_AT_LB, i);
                } else {
                    m_variableStates.insert(Simplex::NONBASIC_AT_UB, i);
                }
            }
            break;

        case MAXIMIZE:
            for (i=0; i<basisSize; i++) {
                if (costs.at(i) > 0) {
                    m_variableStates.insert(Simplex::NONBASIC_AT_UB, i);
                } else {
                    m_variableStates.insert(Simplex::NONBASIC_AT_LB, i);
                }
            }
            break;
        }
        break;
    }

    default: {
        /* */
        DEVWARNING(D::SBF_LOGICAL, "Unhandled logical basis finder algorithm selected.");
        break;
    }

    }
}
