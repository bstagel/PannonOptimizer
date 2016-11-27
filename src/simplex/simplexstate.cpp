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
 * @file simplexstate.cpp This file contains the implementation of the SimplexState class.
 */

#include <simplex/simplexstate.h>

#define PLACEHOLDER_VAL 81291822

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

SimplexState::SimplexState(const std::vector<int> &basisHead, const IndexList<const Numerical::Double *> &variableStates,
                           const DenseVector &basicVariableValues, SimplexState *parent) : SimplexState()
{
    init(basisHead, variableStates, basicVariableValues, parent);
}

SimplexState::SimplexState(const SimplexState &other) : SimplexState()
{
    copy(other);
}

SimplexState::~SimplexState()
{
    clear();
}

SimplexState& SimplexState::operator=(const SimplexState &other)
{
    clear();
    copy(other);
    return *this;
}

void SimplexState::setSimplexState(const std::vector<int> &basisHead, const IndexList<const Numerical::Double *> &variableStates,
                                   const DenseVector &basicVariableValues, SimplexState *parent)
{
    clear();
    init(basisHead, variableStates, basicVariableValues, parent);
}

void SimplexState::setParent(SimplexState *parent)
{
    if(m_type == SIMPLEXSTATE_NULL || parent == m_parent) return;
    if(parent->getType() == SIMPLEXSTATE_NULL) return;
    const std::vector<int> basisHead = getBasisHead();
    const IndexList<const Numerical::Double*> variableStates = getVariableStates();
    const DenseVector basicVariableValues = getBasicVariableValues();
    clear();
    init(basisHead, variableStates, basicVariableValues, parent);
}

void SimplexState::registerChild(SimplexState *child)
{
    if(m_type == SIMPLEXSTATE_NULL) return;
    if(!child || child == this) return;
    if(!m_children) m_children = new std::list<SimplexState*>();
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
    for(; it != itEnd; ++it) if(*it == child) { m_children->erase(it); return; }
}

std::vector<int> SimplexState::getBasisHead() const
{
    if(m_type == SIMPLEXSTATE_NULL) return std::vector<int>();
    if(m_type == SIMPLEXSTATE_FULL) return *m_basisHead;
    if(m_type == SIMPLEXSTATE_DIFF && m_parent) {
        std::vector<int> basisHead = m_parent->getBasisHead();
        auto it = m_basisHeadDiff->beginNonzero();
        auto itEnd = m_basisHeadDiff->endNonzero();
        for(; it != itEnd; ++it) basisHead[it.getIndex()] = *it == PLACEHOLDER_VAL ? 0 : *it;
        return basisHead;
    }
    return std::vector<int>();
}

IndexList<const Numerical::Double *> SimplexState::getVariableStates() const
{
    if(m_type == SIMPLEXSTATE_NULL) return IndexList<const Numerical::Double*>();
    if(m_type == SIMPLEXSTATE_FULL) return *m_variableStates;
    if(m_type == SIMPLEXSTATE_DIFF && m_parent) {
        IndexList<const Numerical::Double*> variableStates = m_parent->getVariableStates();
        auto it = m_variableStatesDiff->beginNonzero();
        auto itEnd = m_variableStatesDiff->endNonzero();
        for(; it != itEnd; ++it) variableStates.move(it.getIndex(), *it == PLACEHOLDER_VAL ? 0 : *it);
        return variableStates;
    }
    return IndexList<const Numerical::Double*>();
}

DenseVector SimplexState::getBasicVariableValues() const
{
    if(m_type == SIMPLEXSTATE_NULL) return DenseVector();
    if(m_type == SIMPLEXSTATE_FULL) return *m_basicVariableValues;
    if(m_type == SIMPLEXSTATE_DIFF && m_parent) {
        DenseVector basicVariableValues = m_parent->getBasicVariableValues();
        auto it = m_basicVariableValuesDiff->beginNonzero();
        auto itEnd = m_basicVariableValuesDiff->endNonzero();
        for(; it != itEnd; ++it) basicVariableValues.set(it.getIndex(), *it == PLACEHOLDER_VAL ? 0 : *it);
        return basicVariableValues;
    }
    return DenseVector();
}

void SimplexState::print(bool parent) const
{
    LPINFO("SimplexState object (" << this << ")");
    if(m_type == SIMPLEXSTATE_NULL) {
        LPINFO("Uninitialized / not valid simplex state object.");
        return;
    } else if(m_type == SIMPLEXSTATE_FULL) {
        LPINFO("Full simplex state.")
        LPINFO("Basis head: ");
        for(auto it = m_basisHead->begin(); it!=m_basisHead->end(); ++it) LPINFO((*it) << "; ");
        LPINFO("Variable states: ");
        IndexList<const Numerical::Double*>::Iterator it, itEnd;
        m_variableStates->getIterators(&it, &itEnd, 0, 5);
        for(; it != itEnd; ++it) LPINFO(it.getData() << ": " << it.getPartitionIndex());
        LPINFO("Basic variable values: " << (*m_basicVariableValues));
    } else if(m_type == SIMPLEXSTATE_DIFF) {
        LPINFO("Simplex state difference.");
        if (parent) {
            LPINFO("Parent is " << m_parent << ".");
            LPINFO("Parent values:");
            LPINFO("Basis head: ");
            auto head = m_parent->getBasisHead();
            for(auto it = head.begin(); it!=head.end(); ++it) LPINFO((*it) << "; ");
            LPINFO("Variable states: ");
            auto ilist = m_parent->getVariableStates();
            IndexList<const Numerical::Double*>::Iterator it, itEnd;
            ilist.getIterators(&it, &itEnd, 0, 5);
            for(; it != itEnd; ++it) LPINFO(it.getData() << ": " << it.getPartitionIndex());
            LPINFO("Basic variable values: " << m_parent->getBasicVariableValues());
        }
        LPINFO("Difference vectors:");
        LPINFO("Basis head: ");
        for(auto it = m_basisHeadDiff->beginNonzero(); it!=m_basisHeadDiff->endNonzero(); ++it) LPINFO(it.getIndex() << ": " << (*it == PLACEHOLDER_VAL ? 0 : *it) << "; ");
        LPINFO("Variable states: ");
        for(auto it = m_variableStatesDiff->beginNonzero(); it!=m_variableStatesDiff->endNonzero(); ++it) LPINFO(it.getIndex() << ": " << (*it == PLACEHOLDER_VAL ? 0 : *it) << "; ");
        LPINFO("Basic variable values: ");
        for(auto it = m_basicVariableValuesDiff->beginNonzero(); it!=m_basicVariableValuesDiff->endNonzero(); ++it) LPINFO(it.getIndex() << ": " << (*it == PLACEHOLDER_VAL ? 0 : *it) << "; ");
    }
}

void SimplexState::init(const std::vector<int> &basisHead, const IndexList<const Numerical::Double *> &variableStates,
                        const DenseVector &basicVariableValues, SimplexState *parent)
{
    SimplexState* parentCandidate = parent;
    if(!parentCandidate || parentCandidate->getType() == SIMPLEXSTATE_NULL || parentCandidate->getBasisHead().size() != basisHead.size() ||
       parentCandidate->getVariableStates().getIndexCount() != variableStates.getIndexCount() ||
       parentCandidate->getBasicVariableValues().length() != basicVariableValues.length())
    {
       parentCandidate = sm_lastSimplexState;
       if(!parentCandidate || parentCandidate->getType() == SIMPLEXSTATE_NULL || parentCandidate->getBasisHead().size() != basisHead.size() ||
          parentCandidate->getVariableStates().getIndexCount() != variableStates.getIndexCount() ||
          parentCandidate->getBasicVariableValues().length() != basicVariableValues.length())
       {
           parentCandidate = nullptr;
       }
    }
    m_parent = parentCandidate;
    if(!m_parent){

        // The whole state should be recorded, this state has no parent state.
        m_type = SIMPLEXSTATE_FULL;
        m_basisHead = new std::vector<int>(basisHead);
        m_variableStates = new IndexList<const Numerical::Double*>(variableStates);
        m_basicVariableValues = new DenseVector(basicVariableValues);
        sm_lastSimplexState = this;
        m_children = new std::list<SimplexState*>();
    } else {

        // The state difference should be recorded from the parent state.
        m_type = SIMPLEXSTATE_DIFF;
        m_basisHeadDiff = new SparseVector(basisHead.size());
        m_variableStatesDiff = new SparseVector(variableStates.getIndexCount());
        m_basicVariableValuesDiff = new SparseVector(basicVariableValues.length());

        std::vector<int> parentBasisHead = m_parent->getBasisHead();
        IndexList<const Numerical::Double *> parentVariableStates = m_parent->getVariableStates();
        DenseVector parentBasicVariableValues = m_parent->getBasicVariableValues();
        // Record changes in the basis head.
        {
            auto it = basisHead.begin(); auto itEnd = basisHead.end();
            auto itParent = parentBasisHead.begin();
            int index;
            for(index = 0; it != itEnd; ++it, ++itParent, index++) {
                if(*it != *itParent) m_basisHeadDiff->set(index, *it == 0 ? PLACEHOLDER_VAL : *it);
            }
        }

        // Record changes in the variable states.
        {
            IndexList<const Numerical::Double*>::Iterator it, itEnd;
            variableStates.getIterators(&it, &itEnd, 0, variableStates.getPartitionCount());
            for(;it != itEnd; ++it) {
                if(it.getPartitionIndex() != parentVariableStates.where(it.getData())) m_variableStatesDiff->set(it.getData(), it.getPartitionIndex() == 0 ? PLACEHOLDER_VAL : it.getPartitionIndex());
            }
        }

        // Record changes in the basic variable values.
        {
            auto it = basicVariableValues.begin(); auto itEnd = basicVariableValues.end();
            auto itParent = parentBasicVariableValues.begin();
            int index;
            for(index = 0; it != itEnd; ++it, ++itParent, index++) {
                if(*it != *itParent) m_basicVariableValuesDiff->set(index, *it == 0 ? PLACEHOLDER_VAL : *it);
            }
        }

        m_parent->registerChild(this);
    }
}

void SimplexState::copy(const SimplexState &other)
{
    m_type = other.m_type;
    if(other.m_basisHead) m_basisHead = new std::vector<int>(*other.m_basisHead);
    if(other.m_variableStates) m_variableStates = new IndexList<const Numerical::Double*>(*other.m_variableStates);
    if(other.m_basicVariableValues) m_basicVariableValues = new DenseVector(*other.m_basicVariableValues);
    if(other.m_parent) {
        m_parent = other.m_parent;
        m_parent->registerChild(this);
    }
    if(other.m_basisHeadDiff) m_basisHeadDiff = new SparseVector(*other.m_basisHeadDiff);
    if(other.m_variableStatesDiff) m_variableStatesDiff = new SparseVector(*other.m_variableStatesDiff);
    if(other.m_basicVariableValuesDiff) m_basicVariableValuesDiff = new SparseVector(*other.m_basicVariableValuesDiff);
}

void SimplexState::clear()
{
    if(!m_children->empty()) {
        auto it = m_children->begin();
        auto itEnd = m_children->end();
        SimplexState* newParent = m_parent ? m_parent : *it;
        if(!m_parent) {
            ++it;
            newParent->setParent(nullptr);
        }
        for(; it != itEnd; ++it) (*it)->setParent(newParent);
        if(sm_lastSimplexState == this && newParent->getType() == SIMPLEXSTATE_FULL) sm_lastSimplexState = newParent;
    }
    if(m_parent) m_parent->removeChild(this);
    if(m_basisHead) { delete m_basisHead; m_basisHead = nullptr; }
    if(m_variableStates) { delete m_variableStates; m_variableStates = nullptr; }
    if(m_basicVariableValues) { delete m_basicVariableValues; m_basicVariableValues = nullptr; }
    if(m_basisHeadDiff) { delete m_basisHeadDiff; m_basisHeadDiff = nullptr; }
    if(m_variableStatesDiff) { delete m_variableStatesDiff; m_variableStatesDiff = nullptr; }
    if(m_basicVariableValuesDiff) { delete m_basicVariableValuesDiff; m_basicVariableValuesDiff = nullptr; }
    if(m_children) { delete m_children; m_children = nullptr; }
    if(sm_lastSimplexState == this) sm_lastSimplexState = nullptr;
}
