//=================================================================================================
/*!
//  This file is part of the Pannon Optimizer library.
//  This library is free software; you can redistribute it and/or modify it under the
//  terms of the GNU Lesser General Public License as published by the Free Software
//  Foundation; either version 3.0, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
//  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//  See the GNU General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License; see the file
//  COPYING. If not, see http://www.gnu.org/licenses/.
*/
//=================================================================================================

/**
 * @file simplexstate.cpp
 */
#include <simplex/simplexstate.h>

SimplexState* SimplexState::sm_lastSimplexState = nullptr;

SimplexState::SimplexState() :
    m_type(SIMPLEXSTATE_NULL),
    m_basisHead(nullptr),
    m_variableStates(nullptr),
    m_basicVariableValues(nullptr),
    m_parent(nullptr),
    m_basisHeadDiff(nullptr),
    m_variableStatesDiff(nullptr),
    m_basicVariableValuesDiff(nullptr),
    m_children(nullptr)
{
}

SimplexState::SimplexState(std::vector<int> *basisHead, IndexList<const Numerical::Double *> *variableStates,
                           DenseVector *basicVariableValues, SimplexState *parent) : SimplexState()
{
    SimplexState* parentCandidate = parent;
    if(!parentCandidate || parentCandidate->getType() != SIMPLEXSTATE_NULL || parentCandidate->getBasisHead().size() != basisHead->size() ||
       parentCandidate->getVariableStates().getIndexCount() != variableStates->getIndexCount() ||
       parentCandidate->getBasicVariableValues().length() != basicVariableValues->length())
    {
       parentCandidate = sm_lastSimplexState;
       if(!parentCandidate || parentCandidate->getType() != SIMPLEXSTATE_NULL || parentCandidate->getBasisHead().size() != basisHead->size() ||
          parentCandidate->getVariableStates().getIndexCount() != variableStates->getIndexCount() ||
          parentCandidate->getBasicVariableValues().length() != basicVariableValues->length())
       {
           parentCandidate = nullptr;
       }
    }
    m_parent = parentCandidate;
    if(!m_parent){

        // The whole state should be recorded, this state has no parent state.
        m_type = SIMPLEXSTATE_FULL;
        m_basisHead = new std::vector<int>(*basisHead);
        m_variableStates = new IndexList<const Numerical::Double*>(*variableStates);
        m_basicVariableValues = new DenseVector(*basicVariableValues);
        sm_lastSimplexState = this;
        m_children = new std::list<SimplexState*>();
    } else {

        // The state difference should be recorded from the parent state.
        m_type = SIMPLEXSTATE_DIFF;
        m_basisHeadDiff = new SparseVector(basisHead->size());
        m_variableStatesDiff = new SparseVector(variableStates->getIndexCount());
        m_basicVariableValuesDiff = new SparseVector(basicVariableValues->length());

        std::vector<int> parentBasisHead = m_parent->getBasisHead();
        IndexList<const Numerical::Double *> parentVariableStates = m_parent->getVariableStates();
        DenseVector parentBasicVariableValues = m_parent->getBasicVariableValues();
        // Record changes in the basis head.
        {
            auto it = basisHead->begin(); auto itEnd = basisHead->end();
            auto itParent = parentBasisHead.begin();
            int index;
            for(index = 0; it != itEnd; ++it, ++itParent, index++) {
                if(*it != *itParent) m_basisHeadDiff->set(index, *it);
            }
        }

        // Record changes in the variable states.
        {
            IndexList<const Numerical::Double*>::Iterator it, itEnd, itParent, itParentEnd;
            variableStates->getIterators(&it, &itEnd, 0, variableStates->getPartitionCount());
            parentVariableStates.getIterators(&itParent, &itParentEnd, 0, parentVariableStates.getPartitionCount());
            int index;
            for(index = 0; it != itEnd; ++it, ++itParent, index++) {
                if(it.getPartitionIndex() != itParent.getPartitionIndex()) m_variableStatesDiff->set(index, it.getPartitionIndex());
            }
        }

        // Record changes in the basic variable values.
        {
            auto it = basicVariableValues->begin(); auto itEnd = basicVariableValues->end();
            auto itParent = parentBasicVariableValues.begin();
            int index;
            for(index = 0; it != itEnd; ++it, ++itParent, index++) {
                if(*it != *itParent) m_basicVariableValuesDiff->set(index, *it);
            }
        }

        m_parent->registerChild(this);
    }
}

SimplexState::~SimplexState()
{
    if(!m_children->empty()) {
        auto it = m_children->begin();
        auto itEnd = m_children->end();
        SimplexState* newParent = m_parent ? m_parent : *it;
        if(!m_parent) {
            ++it;
            newParent->setParent(m_parent);
        }
        for(; it != itEnd; ++it) (*it)->setParent(newParent);
        if(sm_lastSimplexState == this && newParent->getType() == SIMPLEXSTATE_FULL) sm_lastSimplexState = newParent;
    }
    if(m_parent) m_parent->removeChild(this);
    if(m_basisHead) delete m_basisHead;
    if(m_variableStates) delete m_variableStates;
    if(m_basicVariableValues) delete m_basicVariableValues;
    if(m_basisHeadDiff) delete m_basisHeadDiff;
    if(m_variableStatesDiff) delete m_variableStatesDiff;
    if(m_basicVariableValuesDiff) delete m_basicVariableValuesDiff;
    if(m_children) delete m_children;
    if(sm_lastSimplexState == this) sm_lastSimplexState = nullptr;
}

void SimplexState::setSimplexState(std::vector<int> *basisHead, IndexList<const Numerical::Double *> *variableStates, DenseVector *basicVariableValues, SimplexState *parent)
{
    __UNUSED(basisHead);
    __UNUSED(variableStates);
    __UNUSED(basicVariableValues);
    __UNUSED(parent);
}

void SimplexState::setParent(SimplexState *parent)
{
    __UNUSED(parent);
}

void SimplexState::registerChild(SimplexState *child)
{
    if(m_type == SIMPLEXSTATE_NULL) return;
    if(!child || child == this) return;
    auto it = m_children->begin();
    auto itEnd = m_children->end();
    for(; it != itEnd; ++it) if(*it == child) return;
    m_children->push_back(child);
}

void SimplexState::removeChild(SimplexState *child)
{
    if(m_type == SIMPLEXSTATE_NULL) return;
    if(!child || child == this) return;
    auto it = m_children->begin();
    auto itEnd = m_children->end();
    for(; it != itEnd; ++it) if(*it == child) m_children->erase(it);
}

std::vector<int> SimplexState::getBasisHead() const
{
    if(m_type == SIMPLEXSTATE_NULL) return std::vector<int>();
    if(m_type == SIMPLEXSTATE_FULL) {
        return *m_basisHead;
    } else if(m_type == SIMPLEXSTATE_DIFF && m_parent) {
        std::vector<int> basisHead = m_parent->getBasisHead();
        auto it = m_basisHeadDiff->beginNonzero();
        auto itEnd = m_basisHeadDiff->endNonzero();
        for(; it != itEnd; ++it) basisHead[it.getIndex()] = *it;
        return basisHead;
    }
    return std::vector<int>();
}

IndexList<const Numerical::Double *> SimplexState::getVariableStates() const
{
    if(m_type == SIMPLEXSTATE_NULL) return IndexList<const Numerical::Double*>();
    if(m_type == SIMPLEXSTATE_FULL) {
        return *m_variableStates;
    } else if(m_type == SIMPLEXSTATE_DIFF && m_parent) {
        IndexList<const Numerical::Double*> variableStates = m_parent->getVariableStates();
        auto it = m_variableStatesDiff->beginNonzero();
        auto itEnd = m_variableStatesDiff->endNonzero();
        for(; it != itEnd; ++it) variableStates.move(it.getIndex(), *it);
        return variableStates;
    }
    return IndexList<const Numerical::Double*>();
}

DenseVector SimplexState::getBasicVariableValues() const
{
    if(m_type == SIMPLEXSTATE_NULL) return DenseVector();
    if(m_type == SIMPLEXSTATE_FULL) {
        return *m_basicVariableValues;
    } else if(m_type == SIMPLEXSTATE_DIFF && m_parent) {
        DenseVector basicVariableValues = m_parent->getBasicVariableValues();
        auto it = m_basicVariableValuesDiff->beginNonzero();
        auto itEnd = m_basicVariableValuesDiff->endNonzero();
        for(; it != itEnd; ++it) basicVariableValues.set(it.getIndex(), *it);
        return basicVariableValues;
    }
    return DenseVector();
}
