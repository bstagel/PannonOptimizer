/**
 * @file simplex.cpp
 */

#include "simplex/simplex.h"

Simplex::Simplex():
    m_simplexModel(NULL),
    m_variableStates(0,0),
    m_variableFeasibilities(0,0),
    m_reducedCostFeasibilities(0,0)
{

}

Simplex::~Simplex()
{

}

//void Simplex::getVariablesByState(VARIABLE_STATE state, IndexList::Iterator *begin, IndexList::Iterator *end) const
//{

//}

//void Simplex::getVariablesByFeasibility(VARIABLE_STATE state, IndexList::Iterator *begin, IndexList::Iterator *end) const
//{

//}

//void Simplex::getReducedCostsByFeasibility(VARIABLE_STATE state, IndexList::Iterator *begin, IndexList::Iterator *end) const
//{

//}

void Simplex::setModel(const Model &model)
{
    if(m_simplexModel){
        delete m_simplexModel;
    }
    unregisterMethodWithModel(this, model);
    m_simplexModel = new SimplexModel(model);
    registerMethodWithModel(this, model);
}

void Simplex::constraintAdded()
{

}

void Simplex::variableAdded()
{

}
