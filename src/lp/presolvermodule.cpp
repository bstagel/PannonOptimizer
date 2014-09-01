#include <lp/presolvermodule.h>
#include <debug.h>
#include <utils/numerical.h>

PresolverModule::PresolverModule(Presolver * parent) :
    m_parent(parent),
    m_name("undefined"),
    m_removedConstraints(0),
    m_removedVariables(0)
{
}

void PresolverModule::printStatistics() {
    LPINFO("[Presolver] Module " << getName() << " stats: eliminated rows - " << getRemovedConstraintCount() << ", eliminated columns - " << getRemovedVariableCount());
}

void LinearAlgebraicModule::printStatistics() {
    LPINFO("[Presolver] Module " << getName() << " stats: eliminated rows - " << getRemovedConstraintCount() << ", eliminated columns - " << getRemovedVariableCount() << ", eliminated nonzeros - " << m_removedNzr);
}

PresolverModule::~PresolverModule() { }

SingletonRowsModule::SingletonRowsModule(Presolver * parent) :
    PresolverModule(parent)
{
    m_name = "Singleton rows module";
}

SingletonRowsModule::~SingletonRowsModule() { }

void SingletonRowsModule::executeMethod() {
    int eliminatedVariableCount;

    //if no variable was eliminated from the model, no empty or singleton row is created, otherwise we need to check the rows again
    do {
        eliminatedVariableCount = 0;

        Vector::Iterator currentRow = m_parent->getRowNonzeros()->begin();
        Vector::Iterator lastRow = m_parent->getRowNonzeros()->end();
        while(currentRow < lastRow) {
            //handle empty rows
            if(*currentRow == 0) {
                if((*m_parent->getConstraints())[currentRow.getIndex()].getLowerBound() > 0 || (*m_parent->getConstraints())[currentRow.getIndex()].getUpperBound() < 0) {
                    throw Presolver::PresolverException("The problem is primal infeasible.");
                    return;
                } else {
                    //remove constraint
                    m_removedConstraints++;
                    m_parent->getModel()->removeConstraint(currentRow.getIndex());
                    m_parent->getImpliedDualLower()->removeElement(currentRow.getIndex());
                    m_parent->getImpliedDualUpper()->removeElement(currentRow.getIndex());
                    m_parent->getRowNonzeros()->removeElement(currentRow.getIndex());
                    lastRow = m_parent->getRowNonzeros()->end();
                    continue;
                }
            }

            //handle singleton rows
            if(*currentRow == 1) {
                //get the singleton position with skipping the removed column indices
                Vector::NonzeroIterator it = m_parent->getConstraints()->at(currentRow.getIndex()).getVector()->beginNonzero();
                int varIdx = it.getIndex();
                Numerical::Double varLowerBound = /*m_parent->getImpliedLower()->at(varIdx);*/(*m_parent->getVariables())[varIdx].getLowerBound();
                Numerical::Double varUpperBound = /*m_parent->getImpliedUpper()->at(varIdx);*/(*m_parent->getVariables())[varIdx].getUpperBound();
                Numerical::Double rowLowerBound = (*m_parent->getConstraints())[currentRow.getIndex()].getLowerBound();
                Numerical::Double rowUpperBound = (*m_parent->getConstraints())[currentRow.getIndex()].getUpperBound();

                //report infeasibility if bounds conflict
                //example for the 4 cases here:
                // -4 <=  2*x <= 4;   3 <= x <= 8
                // -4 <=  2*x <= 4;  -7 <= x <= -2.5
                // -4 <= -2*x <= 4;   3 <= x <= 8
                // -4 <= -2*x <= 4;  -7 <= x <= -2.5
                if(((*it) > 0 && Numerical::stableAdd(varLowerBound * (*it), (-1) * rowUpperBound) > 0) ||
                   ((*it) > 0 && Numerical::stableAdd(varUpperBound * (*it), (-1) * rowLowerBound) < 0) ||
                   ((*it) < 0 && Numerical::stableAdd(varLowerBound * (*it), (-1) * rowLowerBound) < 0) ||
                   ((*it) < 0 && Numerical::stableAdd(varUpperBound * (*it), (-1) * rowUpperBound) > 0)) {

                    //return with infeasibility error
                    throw Presolver::PresolverException("The problem is primal infeasible.");
                    return;
                }

                //fix variable if bounds are equal
                //TODO: Numerical::Double hasznalata eseten nem fordul le ambiguous conversion miatt (DoubleHistory -> bool)
//                Numerical::Double fixVal;
                double fixVal;
                if(((*m_parent->getConstraints())[currentRow.getIndex()].getType() == Constraint::EQUALITY && (fixVal = 1)) ||
                   ((*it) > 0 && Numerical::stableAdd(varLowerBound * (*it), (-1) * rowUpperBound) == 0 && (fixVal = 1)) ||
                   ((*it) > 0 && Numerical::stableAdd(varUpperBound * (*it), (-1) * rowLowerBound) == 0 && (fixVal = 2)) ||
                   ((*it) < 0 && Numerical::stableAdd(varLowerBound * (*it), (-1) * rowLowerBound) == 0 && (fixVal = 2)) ||
                   ((*it) < 0 && Numerical::stableAdd(varUpperBound * (*it), (-1) * rowUpperBound) == 0 && (fixVal = 1))) {

                    if(fixVal == 1) {
                        fixVal = rowUpperBound / (*it);
                    } else {
                        fixVal = rowLowerBound / (*it);
                    }
                    //remove constraint, fix variable and break
                    m_removedConstraints++;
                    m_parent->getModel()->removeConstraint(currentRow.getIndex());
                    m_parent->getRowNonzeros()->removeElement(currentRow.getIndex());
                    m_parent->getImpliedDualLower()->removeElement(currentRow.getIndex());
                    m_parent->getImpliedDualUpper()->removeElement(currentRow.getIndex());
                    lastRow = m_parent->getRowNonzeros()->end();

                    m_parent->fixVariable(varIdx, fixVal);
                    eliminatedVariableCount++;
                    m_removedVariables++;
                    continue;
                } else {
                    //update variable bounds
                    if((*it) > 0) {
                        if(varLowerBound < rowLowerBound / (*it)) {
                            (*m_parent->getVariables())[varIdx].setLowerBound(rowLowerBound / (*it));
                            m_parent->getImpliedLower()->set(varIdx, rowLowerBound / (*it));
                        }
                        if(varUpperBound > rowUpperBound / (*it)) {
                            (*m_parent->getVariables())[varIdx].setUpperBound(rowUpperBound / (*it));
                            m_parent->getImpliedUpper()->set(varIdx, rowUpperBound / (*it));
                        }
                    } else {
                        if(varLowerBound < rowUpperBound / (*it)) {
                            (*m_parent->getVariables())[varIdx].setLowerBound(rowUpperBound / (*it));
                            m_parent->getImpliedLower()->set(varIdx, rowUpperBound / (*it));
                        }
                        if(varUpperBound > rowLowerBound / (*it)) {
                            (*m_parent->getVariables())[varIdx].setUpperBound(rowLowerBound / (*it));
                            m_parent->getImpliedUpper()->set(varIdx, rowLowerBound / (*it));
                        }
                    }

                    //remove constraint
                    m_removedConstraints++;
                    m_parent->getModel()->removeConstraint(currentRow.getIndex());
                    m_parent->getRowNonzeros()->removeElement(currentRow.getIndex());
                    m_parent->getImpliedDualLower()->removeElement(currentRow.getIndex());
                    m_parent->getImpliedDualUpper()->removeElement(currentRow.getIndex());
                    m_parent->getColumnNonzeros()->set(varIdx, m_parent->getColumnNonzeros()->at(varIdx));
                    lastRow = m_parent->getRowNonzeros()->end();
                    continue;
                }
            }
            currentRow++;
        }
    } while (eliminatedVariableCount);
}

SingletonColumnsModule::SingletonColumnsModule(Presolver *parent) :
    PresolverModule(parent)
{
    m_name = "Singleton columns module";
}

SingletonColumnsModule::~SingletonColumnsModule() { }

void SingletonColumnsModule::executeMethod() {
    int eliminatedConstraintCount;

    do {
        eliminatedConstraintCount = 0;

        Vector::Iterator currentColumn = m_parent->getColumnNonzeros()->begin();
        Vector::Iterator lastColumn = m_parent->getColumnNonzeros()->end();
        while(currentColumn < lastColumn) {

            //handle fixed variables
            if(Numerical::stableAdd((*m_parent->getVariables())[currentColumn.getIndex()].getLowerBound(), -1 * (*m_parent->getVariables())[currentColumn.getIndex()].getUpperBound()) == 0) {

                m_parent->fixVariable(currentColumn.getIndex(), (*m_parent->getVariables())[currentColumn.getIndex()].getLowerBound());
                lastColumn = m_parent->getColumnNonzeros()->end();
                m_removedVariables++;
                continue;
            }

            //handle empty columns
            if(*currentColumn == 0) {
                if(m_parent->getModel()->getObjectiveType() == MINIMIZE) {
                    if(((*m_parent->getVariables())[currentColumn.getIndex()].getLowerBound() == -Numerical::Infinity && m_parent->getModel()->getCostVector().at(currentColumn.getIndex()) > 0) ||
                       ((*m_parent->getVariables())[currentColumn.getIndex()].getUpperBound() == Numerical::Infinity && m_parent->getModel()->getCostVector().at(currentColumn.getIndex()) < 0)) {
                        throw Presolver::PresolverException("The problem is primal unbounded.");
                        return;
                    } else {
                        if(m_parent->getModel()->getCostVector().at(currentColumn.getIndex()) > 0) {
                            m_parent->fixVariable(currentColumn.getIndex(), (*m_parent->getVariables())[currentColumn.getIndex()].getLowerBound());
                        } else {
                            m_parent->fixVariable(currentColumn.getIndex(), (*m_parent->getVariables())[currentColumn.getIndex()].getUpperBound());
                        }
                        lastColumn = m_parent->getColumnNonzeros()->end();
                        m_removedVariables++;
                        continue;
                    }
                } else {
                    if(((*m_parent->getVariables())[currentColumn.getIndex()].getLowerBound() == -Numerical::Infinity && m_parent->getModel()->getCostVector().at(currentColumn.getIndex()) < 0) ||
                       ((*m_parent->getVariables())[currentColumn.getIndex()].getUpperBound() == Numerical::Infinity && m_parent->getModel()->getCostVector().at(currentColumn.getIndex()) > 0)) {
                        throw Presolver::PresolverException("The problem is primal unbounded.");
                        return;
                    } else {
                        if(m_parent->getModel()->getCostVector().at(currentColumn.getIndex()) < 0) {
                            m_parent->fixVariable(currentColumn.getIndex(), (*m_parent->getVariables())[currentColumn.getIndex()].getLowerBound());
                        } else {
                            m_parent->fixVariable(currentColumn.getIndex(), (*m_parent->getVariables())[currentColumn.getIndex()].getUpperBound());
                        }
                        lastColumn = m_parent->getColumnNonzeros()->end();
                        m_removedVariables++;
                        continue;
                    }
                }
            }

            //handle singleton columns
            if(*currentColumn == 1) {
                //get the singleton position with skipping the removed row indices
                Vector::NonzeroIterator it = (*m_parent->getVariables())[currentColumn.getIndex()].getVector()->beginNonzero();
                int constraintIdx = it.getIndex();

                //this module only treats free variables, implied free variables are checked in the implied
                //variable bounds module
                //!int fixSign;

                //example for the 5 cases here:
                // -4  <=  2*x + 3*y + 4*z <=  4;           y free
                // -4  <=  2*x + 3*y + 4*z < inf;    -5  <= y < inf
                // -inf <  2*x + 3*y + 4*z <=  4;    -inf < y <= 5
                // -4  <=  2*x - 3*y + 4*z < inf;    -inf < y <= 5
                // -inf <  2*x - 3*y + 4*z <=  4;    -5  <= y < inf
                //!currently only the first case is checked, the other cases are not presented by the literature
                if((m_parent->getImpliedLower()->at(currentColumn.getIndex()) == -Numerical::Infinity && m_parent->getImpliedUpper()->at(currentColumn.getIndex()) == Numerical::Infinity/* && (fixSign = 0 || true)*/)) {// ||
                   //!((*it) > 0 && currentColumn.getAttached()->getUpperBound() == Numerical::Infinity && m_parent->getRows().getAttachedData(constraintIdx).getUpperBound() == Numerical::Infinity && (fixSign = 1 || true)) ||
                   //!((*it) > 0 && currentColumn.getAttached()->getLowerBound() == -Numerical::Infinity && m_parent->getRows().getAttachedData(constraintIdx).getLowerBound() == -Numerical::Infinity && (fixSign = -1 || true)) ||
                   //!((*it) < 0 && currentColumn.getAttached()->getLowerBound() == -Numerical::Infinity && m_parent->getRows().getAttachedData(constraintIdx).getUpperBound() == Numerical::Infinity && (fixSign = -1 || true)) ||
                   //!((*it) < 0 && currentColumn.getAttached()->getUpperBound() == Numerical::Infinity && m_parent->getRows().getAttachedData(constraintIdx).getLowerBound() == -Numerical::Infinity && (fixSign = 1 || true))) {
                    Numerical::Double costCoeff = m_parent->getModel()->getCostVector().at(currentColumn.getIndex());
                    Numerical::Double fixBound;
                    if(*it > 0) {

                        //get the constraint bound to fix the variable
                        if(costCoeff > 0) {
                            fixBound = (*m_parent->getConstraints())[constraintIdx].getLowerBound();
                        } else {
                            fixBound = (*m_parent->getConstraints())[constraintIdx].getUpperBound();
                        }

                        //detect unboundedness
                        if(fixBound == Numerical::Infinity || fixBound == -Numerical::Infinity) {
                            throw Presolver::PresolverException("The problem is primal unbounded.");
                            return;
                        }
                    } else {

                        //get the constraint bound to fix the variable
                        if(costCoeff > 0) {
                            fixBound = (*m_parent->getConstraints())[constraintIdx].getUpperBound();
                        } else {
                            fixBound = (*m_parent->getConstraints())[constraintIdx].getLowerBound();
                        }

                        //detect unboundedness
                        if(fixBound == Numerical::Infinity || fixBound == -Numerical::Infinity) {
                            throw Presolver::PresolverException("The problem is primal unbounded.");
                            return;
                        }
                    }

                    //substitute the variable with the other variables present in the constraint,
                    //change the cost vector, and remove the constraint and the variable from the model
                    Vector * substituteVector = new Vector(m_parent->getModel()->variableCount() + 3);
                    Vector::NonzeroIterator ConstraintIt = (*m_parent->getConstraints())[constraintIdx].getVector()->beginNonzero();
                    Vector::NonzeroIterator ConstraintItEnd = (*m_parent->getConstraints())[constraintIdx].getVector()->endNonzero();
                    for(; ConstraintIt < ConstraintItEnd; ConstraintIt++) {
                        substituteVector->set(ConstraintIt.getIndex(), (-1) * (*ConstraintIt) / (*it));
                        m_parent->getColumnNonzeros()->set(ConstraintIt.getIndex(), m_parent->getColumnNonzeros()->at(ConstraintIt.getIndex()) - 1);
                        m_parent->getModel()->addToCostCoefficient(ConstraintIt.getIndex(), -1 * ((*ConstraintIt) * costCoeff / (*it)));
                    }
                    m_parent->getModel()->setCostConstant(Numerical::stableAdd(m_parent->getModel()->getCostConstant(), -1 * fixBound * costCoeff / (*it)));
                    substituteVector->set(currentColumn.getIndex(), 0);
                    substituteVector->set(m_parent->getModel()->variableCount(), fixBound / (*it));
                    substituteVector->set(m_parent->getModel()->variableCount() + 1, currentColumn.getIndex());
                    substituteVector->set(m_parent->getModel()->variableCount() + 2, Presolver::PRIMAL_VARIABLE);
                    m_parent->getSubstituteVectors()->push_back(substituteVector);

                    m_removedConstraints++;
                    eliminatedConstraintCount++;
                    m_parent->getModel()->removeConstraint(constraintIdx);
                    m_parent->getRowNonzeros()->removeElement(constraintIdx);
                    m_parent->getImpliedDualLower()->removeElement(constraintIdx);
                    m_parent->getImpliedDualUpper()->removeElement(constraintIdx);

                    m_removedVariables++;
                    m_parent->getModel()->removeVariable(currentColumn.getIndex());
                    m_parent->getColumnNonzeros()->removeElement(currentColumn.getIndex());
                    m_parent->getImpliedLower()->removeElement(currentColumn.getIndex());
                    m_parent->getImpliedUpper()->removeElement(currentColumn.getIndex());
                    m_parent->getExtraDualLowerSum()->removeElement(currentColumn.getIndex());
                    m_parent->getExtraDualUpperSum()->removeElement(currentColumn.getIndex());
                    lastColumn = m_parent->getColumnNonzeros()->end();
                    continue;
                }
            }
            currentColumn++;
        }
    } while (eliminatedConstraintCount);
}

ImpliedBoundsModule::ImpliedBoundsModule(Presolver *parent) :
    PresolverModule(parent)
{
    m_name = "Implied bounds module";
}

ImpliedBoundsModule::~ImpliedBoundsModule() { }

void ImpliedBoundsModule::executeMethod() {

    m_constraintsToCheck = new Vector(m_parent->getModel()->constraintCount());
    m_constraintsToCheck->setSparsityRatio(0);
    for(unsigned int i = 0; i < m_parent->getModel()->constraintCount(); i++) { m_constraintsToCheck->set(i, 1); }
    m_constraintStack = new Vector(m_parent->getModel()->constraintCount());
    m_constraintStack->setSparsityRatio(0);
    m_variablesToFix = new Vector(m_parent->getModel()->variableCount());
    m_variablesToFix->setSparsityRatio(0);
    m_impliedLower = m_parent->getImpliedLower();
    m_impliedUpper = m_parent->getImpliedUpper();
    Vector::NonzeroIterator begin = m_constraintsToCheck->beginNonzero();
    Vector::NonzeroIterator end = m_constraintsToCheck->endNonzero();
    Numerical::Double impliedUB, impliedLB;

    int stackSwapCounter = 0;
    while(true) {
        if (begin >= end) {
            if(m_constraintStack->nonZeros() != 0 && stackSwapCounter < 10) {
                Vector * temp = m_constraintsToCheck;
                m_constraintsToCheck = m_constraintStack;
                m_constraintStack = temp;
                begin = m_constraintsToCheck->beginNonzero();
                end = m_constraintsToCheck->endNonzero();
                stackSwapCounter++;
            } else {
                break;
            }
        }
        impliedLB = 0;
        impliedUB = 0;
        Vector::NonzeroIterator it = m_parent->getModel()->getMatrix().row(begin.getIndex()).beginNonzero();
        Vector::NonzeroIterator itEnd = m_parent->getModel()->getMatrix().row(begin.getIndex()).endNonzero();
        Vector::NonzeroIterator itV = it;
        Vector::NonzeroIterator itVEnd = itEnd;

        for(; it < itEnd; it++) {
            int curVar = it.getIndex();
            if(*it > 0) {
                impliedLB = Numerical::stableAdd(impliedLB, *it * m_impliedLower->at(curVar));
                impliedUB = Numerical::stableAdd(impliedUB, *it * m_impliedUpper->at(curVar));
            } else {
                impliedLB = Numerical::stableAdd(impliedLB, *it * m_impliedUpper->at(curVar));
                impliedUB = Numerical::stableAdd(impliedUB, *it * m_impliedLower->at(curVar));
            }
        }

        Constraint& currentConstraint = (*m_parent->getConstraints())[begin.getIndex()];

        if(Numerical::stableAdd(impliedLB, -1 * currentConstraint.getUpperBound()) > 0 ||
           Numerical::stableAdd(impliedUB, -1 * currentConstraint.getLowerBound()) < 0) {
            throw Presolver::PresolverException("The problem is primal infeasible.");
            return;
        }

        if(impliedLB > currentConstraint.getLowerBound() &&
           impliedUB < currentConstraint.getUpperBound()) {
            m_parent->getModel()->removeConstraint(begin.getIndex());
            m_parent->getRowNonzeros()->removeElement(begin.getIndex());
            m_parent->getImpliedDualLower()->removeElement(begin.getIndex());
            m_parent->getImpliedDualUpper()->removeElement(begin.getIndex());
            m_constraintsToCheck->removeElement(begin.getIndex());
            m_constraintStack->removeElement(begin.getIndex());
            end = m_constraintsToCheck->endNonzero();
            if(*begin != 1) { begin++; }
            m_removedConstraints++;
            continue;
        }

        it = m_parent->getModel()->getMatrix().row(begin.getIndex()).beginNonzero();

        if(impliedLB == currentConstraint.getUpperBound()) {
            int varIdx;
            for(; it < itEnd; it++) {
                varIdx = it.getIndex();
                m_variablesToFix->set(varIdx, 1);
            }
            itV = m_variablesToFix->beginNonzero();
            itVEnd = m_variablesToFix->endNonzero();
            if(*itV != 1) { itV++; }
            while(itV < itVEnd) {
                it = m_parent->getModel()->getMatrix().column(itV.getIndex()).beginNonzero();
                itEnd = m_parent->getModel()->getMatrix().column(itV.getIndex()).endNonzero();
                for(; it < itEnd; it++) {
                    m_constraintStack->set(it.getIndex(), 1);
                }

                if(m_parent->getModel()->getMatrix().get(begin.getIndex(), itV.getIndex()) > 0) {
                    m_parent->fixVariable(itV.getIndex(), m_impliedLower->at(itV.getIndex()));
                } else {
                    m_parent->fixVariable(itV.getIndex(), m_impliedUpper->at(itV.getIndex()));
                }
                m_variablesToFix->removeElement(itV.getIndex());
                itVEnd = m_variablesToFix->endNonzero();
                m_removedVariables++;
                if(*itV != 1) { itV++; }
            }
            m_parent->getModel()->removeConstraint(begin.getIndex());
            m_parent->getImpliedDualLower()->removeElement(begin.getIndex());
            m_parent->getImpliedDualUpper()->removeElement(begin.getIndex());
            m_parent->getRowNonzeros()->removeElement(begin.getIndex());
            m_constraintsToCheck->removeElement(begin.getIndex());
            m_constraintStack->removeElement(begin.getIndex());
            m_removedConstraints++;
            end = m_constraintsToCheck->endNonzero();
            if(*begin != 1) { begin++; }
            continue;
        }
        if(impliedUB == currentConstraint.getLowerBound()) {
            int varIdx;
            for(; it < itEnd; it++) {
                varIdx = it.getIndex();
                m_variablesToFix->set(varIdx, 1);
            }
            itV = m_variablesToFix->beginNonzero();
            itVEnd = m_variablesToFix->endNonzero();
            if(*itV != 1) { itV++; }
            while(itV < itVEnd) {
                it = m_parent->getModel()->getMatrix().column(itV.getIndex()).beginNonzero();
                itEnd = m_parent->getModel()->getMatrix().column(itV.getIndex()).endNonzero();
                for(; it < itEnd; it++) {
                    m_constraintStack->set(it.getIndex(), 1);
                }

                if(m_parent->getModel()->getMatrix().get(begin.getIndex(), itV.getIndex()) > 0) {
                    m_parent->fixVariable(itV.getIndex(), m_impliedUpper->at(itV.getIndex()));
                } else {
                    m_parent->fixVariable(itV.getIndex(), m_impliedLower->at(itV.getIndex()));
                }
                m_variablesToFix->removeElement(itV.getIndex());
                itVEnd = m_variablesToFix->endNonzero();
                m_removedVariables++;
                if(*itV != 1) { itV++; }
            }
            m_parent->getModel()->removeConstraint(begin.getIndex());
            m_parent->getImpliedDualLower()->removeElement(begin.getIndex());
            m_parent->getImpliedDualUpper()->removeElement(begin.getIndex());
            m_parent->getRowNonzeros()->removeElement(begin.getIndex());
            m_constraintsToCheck->removeElement(begin.getIndex());
            m_constraintStack->removeElement(begin.getIndex());
            m_removedConstraints++;
            end = m_constraintsToCheck->endNonzero();
            if(*begin != 1) { begin++; }
            continue;
        }

//        if(impliedLB > currentConstraint.getLowerBound()) {
//            currentConstraint.setLowerBound(impliedLB);
//        }

//        if(impliedUB < currentConstraint.getUpperBound()) {
//            currentConstraint.setUpperBound(impliedUB);
//        }

        if(impliedLB != -Numerical::Infinity && currentConstraint.getUpperBound() != Numerical::Infinity) {
            it = m_parent->getModel()->getMatrix().row(begin.getIndex()).beginNonzero();
            itEnd = m_parent->getModel()->getMatrix().row(begin.getIndex()).endNonzero();
            for(; it < itEnd; it++) {
                if(*it > 0) {
                    if(Numerical::stableAdd(Numerical::stableAdd((currentConstraint.getUpperBound() - impliedLB) / *it, m_impliedLower->at(it.getIndex())), -1 * m_impliedUpper->at(it.getIndex())) < 0) {
                        m_impliedUpper->set(it.getIndex(), Numerical::stableAdd((currentConstraint.getUpperBound() - impliedLB) / *it, m_impliedLower->at(it.getIndex())));
                           itV = m_parent->getModel()->getMatrix().column(it.getIndex()).beginNonzero();
                           itVEnd = m_parent->getModel()->getMatrix().column(it.getIndex()).endNonzero();
                           for(; itV < itVEnd; itV++) {
                               m_constraintStack->set(itV.getIndex(), 1);
                           }
                    }

                } else {
                    if(Numerical::stableAdd(m_impliedLower->at(it.getIndex()), -1 * Numerical::stableAdd((currentConstraint.getUpperBound() - impliedLB) / *it,  m_impliedLower->at(it.getIndex()))) < 0) {
                       m_impliedLower->set(it.getIndex(), Numerical::stableAdd((currentConstraint.getUpperBound() - impliedLB) / *it,  m_impliedUpper->at(it.getIndex())));
                       itV = m_parent->getModel()->getMatrix().column(it.getIndex()).beginNonzero();
                       itVEnd = m_parent->getModel()->getMatrix().column(it.getIndex()).endNonzero();
                       for(; itV < itVEnd; itV++) {
                           m_constraintStack->set(itV.getIndex(), 1);
                       }
                    }
                }
            }
        }

        if(impliedUB != -Numerical::Infinity && currentConstraint.getLowerBound() != Numerical::Infinity) {
            it = m_parent->getModel()->getMatrix().row(begin.getIndex()).beginNonzero();
            itEnd = m_parent->getModel()->getMatrix().row(begin.getIndex()).endNonzero();
            for(; it < itEnd; it++) {
                if(*it > 0) {
                    if(Numerical::stableAdd(m_impliedLower->at(it.getIndex()), -1 * Numerical::stableAdd((currentConstraint.getLowerBound() - impliedUB) / *it, m_impliedLower->at(it.getIndex()))) < 0) {
                       m_impliedLower->set(it.getIndex(), Numerical::stableAdd((currentConstraint.getLowerBound() - impliedUB) / *it, m_impliedUpper->at(it.getIndex())));
                       itV = m_parent->getModel()->getMatrix().column(it.getIndex()).beginNonzero();
                       itVEnd = m_parent->getModel()->getMatrix().column(it.getIndex()).endNonzero();
                       for(; itV < itVEnd; itV++) {
                           m_constraintStack->set(itV.getIndex(), 1);
                       }
                    }

                } else {
                    if(Numerical::stableAdd(Numerical::stableAdd((currentConstraint.getLowerBound() - impliedUB) / *it, m_impliedLower->at(it.getIndex())), -1 * m_impliedUpper->at(it.getIndex())) < 0) {
                       m_impliedUpper->set(it.getIndex(), Numerical::stableAdd((currentConstraint.getLowerBound() - impliedUB) / *it, m_impliedLower->at(it.getIndex())));
                       itV = m_parent->getModel()->getMatrix().column(it.getIndex()).beginNonzero();
                       itVEnd = m_parent->getModel()->getMatrix().column(it.getIndex()).endNonzero();
                       for(; itV < itVEnd; itV++) {
                           m_constraintStack->set(itV.getIndex(), 1);
                       }
                    }
                }
            }
        }


        m_constraintsToCheck->set(begin.getIndex(), 0);
        begin++;
    }
}

DualBoundsModule::DualBoundsModule(Presolver *parent) :
    PresolverModule(parent)
{
    m_name = "Dual bounds module";
}

DualBoundsModule::~DualBoundsModule() { }

void DualBoundsModule::executeMethod() {

    m_variablesToCheck = new Vector(m_parent->getModel()->variableCount());
    m_variablesToCheck->setSparsityRatio(0);
    for(unsigned int i = 0; i < m_parent->getModel()->variableCount(); i++) { m_variablesToCheck->set(i, 1); }
    m_variableStack = new Vector(m_parent->getModel()->variableCount());
    m_variableStack->setSparsityRatio(0);
    m_impliedDualLower = m_parent->getImpliedDualLower();
    m_impliedDualUpper = m_parent->getImpliedDualUpper();
    m_extraDualLowerSum = m_parent->getExtraDualLowerSum();
    m_extraDualUpperSum = m_parent->getExtraDualUpperSum();
    Vector::NonzeroIterator begin = m_variablesToCheck->beginNonzero();
    Vector::NonzeroIterator end = m_variablesToCheck->endNonzero();
    Numerical::Double impliedUB, impliedLB;
//    int c = 0;
    int stackSwapCounter = 0;

    while(true) {

        if (begin >= end) {
            if(m_variableStack->nonZeros() != 0 && stackSwapCounter < 10) {
                Vector * temp = m_variablesToCheck;
                m_variablesToCheck = m_variableStack;
                m_variableStack = temp;
                begin = m_variablesToCheck->beginNonzero();
                end = m_variablesToCheck->endNonzero();
                stackSwapCounter++;
            } else {
                break;
            }
        }
        impliedLB = 0;
        impliedUB = 0;

        Vector::NonzeroIterator it = m_parent->getModel()->getMatrix().column(begin.getIndex()).beginNonzero();
        Vector::NonzeroIterator itEnd = m_parent->getModel()->getMatrix().column(begin.getIndex()).endNonzero();
        Vector::NonzeroIterator itC = it;
        Vector::NonzeroIterator itCEnd = itEnd;

        for(; it < itEnd; it++) {
            int curRow = it.getIndex();
            if(*it > 0) {
                impliedLB = Numerical::stableAdd(impliedLB, *it * m_impliedDualLower->at(curRow));
                impliedUB = Numerical::stableAdd(impliedUB, *it * m_impliedDualUpper->at(curRow));
            } else {
                impliedLB = Numerical::stableAdd(impliedLB, *it * m_impliedDualUpper->at(curRow));
                impliedUB = Numerical::stableAdd(impliedUB, *it * m_impliedDualLower->at(curRow));
            }
        }
        if(m_extraDualLowerSum->at(begin.getIndex()) != 0) {
            impliedLB = -Numerical::Infinity;
        }
        if(m_extraDualUpperSum->at(begin.getIndex()) != 0) {
            impliedUB = Numerical::Infinity;
        }

        Variable& currentVariable = (*m_parent->getVariables())[begin.getIndex()];
        //Check if variable is dominated

        const Vector& costVector = m_parent->getModel()->getCostVector();

        if(currentVariable.getUpperBound() == Numerical::Infinity/* || currentVariable.getType() == Variable::BOUNDED*/) {

            if(costVector.at(begin.getIndex()) < impliedLB) {
                throw Presolver::PresolverException("The problem is dual infeasible.");
                return;
            }

            if(costVector.at(begin.getIndex()) > impliedUB) {
                if(currentVariable.getLowerBound() == -Numerical::Infinity) {
                    throw Presolver::PresolverException("The problem is dual unbounded.");
                    return;
                } else {
                    m_parent->fixVariable(begin.getIndex(), currentVariable.getLowerBound());
                    m_variablesToCheck->removeElement(begin.getIndex());
                    m_variableStack->removeElement(begin.getIndex());
                    end = m_variablesToCheck->endNonzero();
                    if(*begin != 1) { begin++; }
                    m_removedVariables++;
                    continue;
                }
            }

            if(impliedLB < costVector.at(begin.getIndex()) && impliedUB > costVector.at(begin.getIndex()) && impliedLB != -Numerical::Infinity) {
                it = m_parent->getModel()->getMatrix().column(begin.getIndex()).beginNonzero();
                itEnd = m_parent->getModel()->getMatrix().column(begin.getIndex()).endNonzero();
                for(; it < itEnd; it++) {
                    if(*it > 0) {
                        if(Numerical::stableAdd(Numerical::stableAdd((costVector.at(begin.getIndex()) - impliedLB) / *it, m_impliedDualLower->at(it.getIndex())), -1 * m_impliedDualUpper->at(it.getIndex())) < 0) {
                            m_impliedDualUpper->set(it.getIndex(), Numerical::stableAdd((costVector.at(begin.getIndex()) - impliedLB) / *it, m_impliedDualLower->at(it.getIndex())));
                               itC = m_parent->getModel()->getMatrix().row(it.getIndex()).beginNonzero();
                               itCEnd = m_parent->getModel()->getMatrix().row(it.getIndex()).endNonzero();
                               for(; itC < itCEnd; itC++) {
                                   m_variableStack->set(itC.getIndex(), 1);
                               }
                        }
                    } else {
                        if(Numerical::stableAdd(m_impliedDualLower->at(it.getIndex()), -1 * Numerical::stableAdd((costVector.at(begin.getIndex()) - impliedLB) / *it,  m_impliedDualUpper->at(it.getIndex()))) < 0) {
                           m_impliedDualLower->set(it.getIndex(), Numerical::stableAdd((costVector.at(begin.getIndex()) - impliedLB) / *it,  m_impliedDualUpper->at(it.getIndex())));
                           itC = m_parent->getModel()->getMatrix().row(it.getIndex()).beginNonzero();
                           itCEnd = m_parent->getModel()->getMatrix().row(it.getIndex()).endNonzero();
                           for(; itC < itCEnd; itC++) {
                               m_variableStack->set(itC.getIndex(), 1);
                           }
                        }
                    }
                }
            }
        }

        if(currentVariable.getLowerBound() == -Numerical::Infinity) {

            if(costVector.at(begin.getIndex()) > impliedUB) {
                throw Presolver::PresolverException("The problem is dual infeasible.");
                return;
            }

            if(costVector.at(begin.getIndex()) < impliedLB) {
                if(currentVariable.getLowerBound() == -Numerical::Infinity) {
                    throw Presolver::PresolverException("The problem is dual unbounded.");
                    return;
                } else {
                    m_parent->fixVariable(begin.getIndex(), currentVariable.getLowerBound());
                    m_variablesToCheck->removeElement(begin.getIndex());
                    m_variableStack->removeElement(begin.getIndex());
                    end = m_variablesToCheck->endNonzero();
                    if(*begin != 1) { begin++; }
                    m_removedVariables++;
                    continue;
                }
            }

            if(impliedLB < costVector.at(begin.getIndex()) && impliedUB > costVector.at(begin.getIndex()) && impliedUB != -Numerical::Infinity) {
                it = m_parent->getModel()->getMatrix().column(begin.getIndex()).beginNonzero();
                itEnd = m_parent->getModel()->getMatrix().column(begin.getIndex()).endNonzero();
                for(; it < itEnd; it++) {
                    if(*it > 0) {
                        if(Numerical::stableAdd(m_impliedDualLower->at(it.getIndex()), -1 * Numerical::stableAdd((costVector.at(begin.getIndex()) - impliedUB) / *it,  m_impliedDualUpper->at(it.getIndex()))) < 0) {
                            m_impliedDualLower->set(it.getIndex(), Numerical::stableAdd((costVector.at(begin.getIndex()) - impliedUB) / *it, m_impliedDualUpper->at(it.getIndex())));
                               itC = m_parent->getModel()->getMatrix().row(it.getIndex()).beginNonzero();
                               itCEnd = m_parent->getModel()->getMatrix().row(it.getIndex()).endNonzero();
                               for(; itC < itCEnd; itC++) {
                                   m_variableStack->set(itC.getIndex(), 1);
                               }
                        }
                    } else {
                        if(Numerical::stableAdd(Numerical::stableAdd((costVector.at(begin.getIndex()) - impliedUB) / *it, m_impliedDualLower->at(it.getIndex())), -1 * m_impliedDualUpper->at(it.getIndex())) < 0) {
                           m_impliedDualUpper->set(it.getIndex(), Numerical::stableAdd((costVector.at(begin.getIndex()) - impliedUB) / *it,  m_impliedDualLower->at(it.getIndex())));
                           itC = m_parent->getModel()->getMatrix().row(it.getIndex()).beginNonzero();
                           itCEnd = m_parent->getModel()->getMatrix().row(it.getIndex()).endNonzero();
                           for(; itC < itCEnd; itC++) {
                               m_variableStack->set(itC.getIndex(), 1);
                           }
                        }
                    }
                }
            }
        }

        m_variablesToCheck->set(begin.getIndex(), 0);
        begin++;
    }
}

LinearAlgebraicModule::LinearAlgebraicModule(Presolver *parent) :
    PresolverModule(parent)
{
    m_name = "Linear algebraic module";
    m_removedNzr = 0;
}

LinearAlgebraicModule::~LinearAlgebraicModule() { }

void LinearAlgebraicModule::executeMethod() {

    //Index list to follow which rows are to be removed without changing the indices in the ordered list
    IndexList<> validRows(m_parent->getModel()->constraintCount(), 2);

    //Setting up the list of row vector hashes linked with indices to be sorted
    vector<pair<int, long long int>> sortedRows(m_parent->getModel()->constraintCount());

    for(unsigned int i = 0; i < m_parent->getModel()->constraintCount(); i++) {
        int vhash = 0;
        validRows.insert(0, i);

        //Calculating hash
        Vector::NonzeroIterator begin = (*m_parent->getConstraints())[i].getVector()->beginNonzero();
        Vector::NonzeroIterator end = (*m_parent->getConstraints())[i].getVector()->endNonzero();
        while(begin != end) {
            vhash += begin.getIndex()+1;
            begin++;
        }
        sortedRows[i] = pair<int, long long int>(i, vhash);
    }

    //Sorting the vector pointers primarily by nonzero count, secondarily by hash
    //!TODO use efficient sorting
    for(unsigned int a = 0; a < m_parent->getModel()->constraintCount() - 1; a++) {
        int smallest = a;
        for(unsigned int b = a + 1; b < m_parent->getModel()->constraintCount(); b++) {
            if(m_parent->getRowNonzeros()->at(sortedRows[b].first) < m_parent->getRowNonzeros()->at(sortedRows[smallest].first)
              || (m_parent->getRowNonzeros()->at(sortedRows[b].first) == m_parent->getRowNonzeros()->at(sortedRows[smallest].first) && sortedRows[b].second < sortedRows[smallest].second))
            {
                smallest = b;
            }
        }
        pair<int, long long int> temp;
        temp = sortedRows[a];
        sortedRows[a] = sortedRows[smallest];
        sortedRows[smallest] = temp;
    }

    //Eliminating duplicate rows

    for(unsigned int i = 0; i < m_parent->getModel()->constraintCount() - 1; i++) {
        if(sortedRows[i].second == sortedRows[i+1].second) {

            int origIdx = sortedRows[i+1].first;
            int duplIdx = sortedRows[i].first;

            Vector::NonzeroIterator origIterator = (*m_parent->getConstraints())[origIdx].getVector()->beginNonzero();
            Vector::NonzeroIterator duplIterator = (*m_parent->getConstraints())[duplIdx].getVector()->beginNonzero();

            Numerical::Double lambda = *origIterator / *duplIterator;
            Vector tryVector = *((*m_parent->getConstraints())[duplIdx].getVector()) * lambda - *((*m_parent->getConstraints())[origIdx].getVector());

            if(tryVector.nonZeros() == 0) {

                //With negative lambda, duplicate bounds are swapped
                if(lambda > 0) {
                    //Checking feasibility
                    if((*m_parent->getConstraints())[duplIdx].getLowerBound() * lambda > (*m_parent->getConstraints())[origIdx].getUpperBound()
                       || (*m_parent->getConstraints())[duplIdx].getUpperBound() * lambda < (*m_parent->getConstraints())[origIdx].getLowerBound())
                    {
                        throw Presolver::PresolverException("The problem is primal infeasible.");
                        return;
                    }

                    //Applying the tighter bounds
                    if((*m_parent->getConstraints())[duplIdx].getLowerBound() * lambda > (*m_parent->getConstraints())[origIdx].getLowerBound()) {
                        (*m_parent->getConstraints())[origIdx].setLowerBound((*m_parent->getConstraints())[duplIdx].getLowerBound() * lambda);
                    }

                    if((*m_parent->getConstraints())[duplIdx].getUpperBound() * lambda < (*m_parent->getConstraints())[origIdx].getUpperBound()) {
                        (*m_parent->getConstraints())[origIdx].setUpperBound((*m_parent->getConstraints())[duplIdx].getUpperBound() * lambda);
                    }

                } else {

                    //Checking feasibility
                    if((*m_parent->getConstraints())[duplIdx].getUpperBound() * lambda > (*m_parent->getConstraints())[origIdx].getUpperBound()
                       || (*m_parent->getConstraints())[duplIdx].getLowerBound() * lambda < (*m_parent->getConstraints())[origIdx].getLowerBound())
                    {
                        throw Presolver::PresolverException("The problem is primal infeasible.");
                        return;
                    }

                    //Applying the tighter bounds
                    if((*m_parent->getConstraints())[duplIdx].getUpperBound() * lambda > (*m_parent->getConstraints())[origIdx].getLowerBound()) {
                        (*m_parent->getConstraints())[origIdx].setLowerBound((*m_parent->getConstraints())[duplIdx].getUpperBound() * lambda);
                    }

                    if((*m_parent->getConstraints())[duplIdx].getLowerBound() * lambda < (*m_parent->getConstraints())[origIdx].getUpperBound()) {
                        (*m_parent->getConstraints())[origIdx].setUpperBound((*m_parent->getConstraints())[duplIdx].getLowerBound() * lambda);
                    }
                }

                validRows.move(i, 1);
            }
        }
    }

    //Setting up a vector with the indices of constraints to be removed
    //The elimination of constraints needs to start at the row with the highest index

    IndexList<>::Iterator it, itEnd;
    validRows.getIterators(&it, &itEnd, 1, 1);
    Vector cIndices(m_parent->getModel()->constraintCount());

    while(it != itEnd) {
        cIndices.set(sortedRows[it.getData()].first, 1);
        it++;
    }

    Vector::NonzeroIterator begin = cIndices.beginNonzero();
    Vector::NonzeroIterator end = cIndices.endNonzero();

    while(begin < end) {

        m_removedConstraints++;
        m_parent->getModel()->removeConstraint(begin.getIndex());
        m_parent->getRowNonzeros()->removeElement(begin.getIndex());
        m_parent->getImpliedDualLower()->removeElement(begin.getIndex());
        m_parent->getImpliedDualUpper()->removeElement(begin.getIndex());

        cIndices.removeElement(begin.getIndex());
        end = cIndices.endNonzero();
        if(*begin != 1) { begin++; }
    }

//    //Making the matrix sparser

//    validRows.getIterators(&it, &itEnd, 0, 1);

//    while(it != itEnd) {
//        unsigned int pivotIndex = sortedRows[it.getData()].first;
//        Vector::NonzeroIterator beginFirst = (*m_parent->getConstraints())[pivotIndex].getVector()->beginNonzero();
//        begin = (*m_parent->getVariables())[beginFirst.getIndex()].getVector()->beginNonzero();
//        end = (*m_parent->getVariables())[beginFirst.getIndex()].getVector()->endNonzero();
//        while(begin < end) {
//            if(begin.getIndex() != pivotIndex && m_parent->getRowNonzeros()->at(pivotIndex) < m_parent->getRowNonzeros()->at(begin.getIndex())) {
//                Vector::NonzeroIterator beginSecond = (*m_parent->getConstraints())[begin.getIndex()].getVector()->beginNonzero();
//                Numerical::Double lambda = *beginSecond / *beginFirst;
//                int i;
//                for(i = 1; i < 4; i++) {
//                    if(i < m_parent->getRowNonzeros()->at(pivotIndex)) {
//                        beginFirst++;
//                        beginSecond++;
//                    }
//                    if(beginFirst.getIndex() != beginSecond.getIndex()) {
//                        break;
//                    }
//                }
//                if(i == 4) {
//                    int orignum = m_parent->getRowNonzeros()->at(begin.getIndex());
//                    if(lambda > 0) {
//                        LPERROR("ADDING " << pivotIndex << " * " << lambda << " TO " << begin.getIndex());
//                        LPERROR(*(*m_parent->getConstraints()).at(pivotIndex).getVector());
//                        LPERROR(*(*m_parent->getConstraints()).at(begin.getIndex()).getVector());
//                        m_parent->getModel()->addToConstraint(begin.getIndex(), pivotIndex, -lambda);
//                        (*m_parent->getConstraints()).at(begin.getIndex()).setBounds(((*m_parent->getConstraints()).at(begin.getIndex()).getLowerBound() - ((*m_parent->getConstraints()).at(pivotIndex).getLowerBound() * lambda)),
//                                                                                     ((*m_parent->getConstraints()).at(begin.getIndex()).getUpperBound() - ((*m_parent->getConstraints()).at(pivotIndex).getUpperBound() * lambda)));
//                        m_parent->getRowNonzeros()->set(begin.getIndex(), (*m_parent->getConstraints())[begin.getIndex()].getVector()->nonZeros());
//                        if(orignum <= m_parent->getRowNonzeros()->at(begin.getIndex())) {
//                            LPERROR("WTF");
//                        }
//                    }
//                }
//            }
//            begin++;
//        }
//        it++;
//    }

    //Index list to follow which columns are to be removed without changing the indices in the ordered list
    //For every element to be removed the lambda value is attached
    IndexList<Numerical::Double> validColumns(m_parent->getModel()->variableCount(), 2);

    //Setting up the list of column vector hashes linked with indices to be sorted
    vector<pair<int, long long int>> sortedColumns(m_parent->getModel()->variableCount());

    for(unsigned int i = 0; i < m_parent->getModel()->variableCount(); i++) {
        int vhash = 0;
        validColumns.insert(0, i);

        //Calculating hash
        Vector::NonzeroIterator begin = (*m_parent->getVariables())[i].getVector()->beginNonzero();
        Vector::NonzeroIterator end = (*m_parent->getVariables())[i].getVector()->endNonzero();
        while(begin != end) {
            vhash += begin.getIndex()+1;
            begin++;
        }
        sortedColumns[i] = pair<int, long long int>(i, vhash);
    }

    //Sorting the vector pointers primarily by nonzero count, secondarily by hash
    //!TODO use efficient sorting
    for(unsigned int a = 0; a < m_parent->getModel()->variableCount() - 1; a++) {
        int smallest = a;
        for(unsigned int b = a + 1; b < m_parent->getModel()->variableCount(); b++) {
            if(m_parent->getColumnNonzeros()->at(sortedColumns[b].first) < m_parent->getColumnNonzeros()->at(sortedColumns[smallest].first)
              || (m_parent->getColumnNonzeros()->at(sortedColumns[b].first) == m_parent->getColumnNonzeros()->at(sortedColumns[smallest].first) && sortedColumns[b].second < sortedColumns[smallest].second))
            {
                smallest = b;
            }
        }
        pair<int, long long int> temp;
        temp = sortedColumns[a];
        sortedColumns[a] = sortedColumns[smallest];
        sortedColumns[smallest] = temp;
    }

    //Eliminating duplicate columns

    for(unsigned int i = 0; i < m_parent->getModel()->variableCount() - 1; i++) {
        if(sortedColumns[i].second == sortedColumns[i+1].second) {

            int origIdx = sortedColumns[i+1].first;
            int duplIdx = sortedColumns[i].first;

            Vector::NonzeroIterator origIterator = (*m_parent->getVariables())[origIdx].getVector()->beginNonzero();
            Vector::NonzeroIterator duplIterator = (*m_parent->getVariables())[duplIdx].getVector()->beginNonzero();

            Numerical::Double lambda = *origIterator / *duplIterator;
            Vector tryVector = *((*m_parent->getVariables())[duplIdx].getVector()) * lambda - *((*m_parent->getVariables())[origIdx].getVector());

            if(tryVector.nonZeros() == 0) {

                if(m_parent->getModel()->getCostVector().at(duplIdx) * lambda == m_parent->getModel()->getCostVector().at(origIdx)) {

                    //With negative lambda, duplicate bounds are swapped
                    if(lambda > 0) {

                        //Checking feasibility
                        if((*m_parent->getVariables())[duplIdx].getLowerBound() * lambda + (*m_parent->getVariables())[origIdx].getLowerBound()
                           > (*m_parent->getVariables())[duplIdx].getUpperBound() * lambda + (*m_parent->getVariables())[origIdx].getUpperBound())
                        {
                            throw Presolver::PresolverException("The problem is primal infeasible.");
                            return;
                        }

                        //Creating bounds
                        (*m_parent->getVariables())[origIdx].setLowerBound((*m_parent->getVariables())[duplIdx].getLowerBound() * lambda + (*m_parent->getVariables())[origIdx].getLowerBound());
                        (*m_parent->getVariables())[origIdx].setUpperBound((*m_parent->getVariables())[duplIdx].getUpperBound() * lambda + (*m_parent->getVariables())[origIdx].getUpperBound());
                        m_parent->getImpliedLower()->set(origIdx, (*m_parent->getVariables())[duplIdx].getLowerBound() * lambda + (*m_parent->getVariables())[origIdx].getLowerBound());
                        m_parent->getImpliedUpper()->set(origIdx, (*m_parent->getVariables())[duplIdx].getUpperBound() * lambda + (*m_parent->getVariables())[origIdx].getUpperBound());

                    } else {

                        //Checking feasibility
                        if((*m_parent->getVariables())[duplIdx].getUpperBound() * lambda + (*m_parent->getVariables())[origIdx].getLowerBound()
                           > (*m_parent->getVariables())[duplIdx].getLowerBound() * lambda + (*m_parent->getVariables())[origIdx].getUpperBound())
                        {
                            throw Presolver::PresolverException("The problem is primal infeasible.");
                            return;
                        }

                        //Creating bounds
                        (*m_parent->getVariables())[origIdx].setLowerBound((*m_parent->getVariables())[duplIdx].getUpperBound() * lambda + (*m_parent->getVariables())[origIdx].getLowerBound());
                        (*m_parent->getVariables())[origIdx].setUpperBound((*m_parent->getVariables())[duplIdx].getLowerBound() * lambda + (*m_parent->getVariables())[origIdx].getUpperBound());
                        m_parent->getImpliedLower()->set(origIdx, (*m_parent->getVariables())[duplIdx].getUpperBound() * lambda + (*m_parent->getVariables())[origIdx].getLowerBound());
                        m_parent->getImpliedUpper()->set(origIdx, (*m_parent->getVariables())[duplIdx].getLowerBound() * lambda + (*m_parent->getVariables())[origIdx].getUpperBound());

                    }

                    validColumns.move(i, 1);
                    validColumns.setAttachedData(i, lambda);
                }
            }
        }
    }

    //Setting up a vector with the indices of variables to be removed
    //The elimination of variables needs to start at the row with the highest index
    //A special type of substitute vector is created to be able to regain correct values for the original variables

    IndexList<Numerical::Double>::Iterator itV, itEndV;
    validColumns.getIterators(&itV, &itEndV, 1, 1);
    Vector vIndices(m_parent->getModel()->variableCount());

    while(itV != itEndV) {
        vIndices.set(sortedColumns[itV.getData()].first, 1);
        itV++;
    }

    begin = vIndices.beginNonzero();
    end = vIndices.endNonzero();

    while(begin < end) {

        Vector * substituteVector = new Vector(m_parent->getModel()->variableCount() + 3);
        substituteVector->set(m_parent->getModel()->variableCount() + 1, begin.getIndex());
        substituteVector->set(m_parent->getModel()->variableCount() + 2, Presolver::DUPLICATE_VARIABLE);
        m_parent->getSubstituteVectors()->push_back(substituteVector);

        m_removedVariables++;
        m_parent->getModel()->removeVariable(begin.getIndex());
        m_parent->getColumnNonzeros()->removeElement(begin.getIndex());
        m_parent->getImpliedLower()->removeElement(begin.getIndex());
        m_parent->getImpliedUpper()->removeElement(begin.getIndex());
        m_parent->getExtraDualLowerSum()->removeElement(begin.getIndex());
        m_parent->getExtraDualUpperSum()->removeElement(begin.getIndex());

        vIndices.removeElement(begin.getIndex());
        end = vIndices.endNonzero();
        if(*begin != 1) { begin++; }
    }

}
