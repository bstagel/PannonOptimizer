#include <lp/presolvermodule.h>
#include <debug.h>
#include <utils/numerical.h>

PresolverModule::PresolverModule() { }

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

void MakeSparserModule::printStatistics() {
    LPINFO("[Presolver] Module " << getName() << " stats: eliminated nonzeros - " << m_removedNzr);
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

    //If no variable was eliminated from the model, no empty or singleton row is created,
    //otherwise we need to check the rows again
    do {
        eliminatedVariableCount = 0;

        Vector::Iterator currentRow = m_parent->getRowNonzeros()->begin();
        Vector::Iterator lastRow = m_parent->getRowNonzeros()->end();

        //Check every row of the model for empty or singleton rows
        while(currentRow < lastRow) {

            //handle empty rows
            if(*currentRow == 0) {
                int index = currentRow.getIndex();
                const Constraint& curConstraint = (*m_parent->getConstraints())[index];

                //Empty rows are either infeasible or can be freely removed
                if(curConstraint.getLowerBound() > 0 || curConstraint.getUpperBound() < 0) {
                    throw Presolver::PresolverException("The problem is primal infeasible.");
                    return;
                } else {
                    //remove constraint
                    m_removedConstraints++;
                    m_parent->removeConstraint(index);
                    lastRow = m_parent->getRowNonzeros()->end();
                    continue;
                }
            }

            //handle singleton rows
            if(*currentRow == 1) {
                int index = currentRow.getIndex();
                const Constraint& curConstraint = (*m_parent->getConstraints())[index];

                //get the singleton position with skipping the removed column indices
                Vector::NonzeroIterator it = curConstraint.getVector()->beginNonzero();
                int varIdx = it.getIndex();

                //Reference is not constant to be able to adjust bounds
                Variable& curVariable = (*m_parent->getVariables())[varIdx];

                //!Use the implied bounds instead of original ones (numerical errors eg. AGG)
                const Numerical::Double& varLowerBound = /*m_parent->getImpliedLower()->at(varIdx);*/curVariable.getLowerBound();
                const Numerical::Double& varUpperBound = /*m_parent->getImpliedUpper()->at(varIdx);*/curVariable.getUpperBound();
                const Numerical::Double& rowLowerBound = curConstraint.getLowerBound();
                const Numerical::Double& rowUpperBound = curConstraint.getUpperBound();

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
                Numerical::Double fixVal;
                int upper;
                if((curConstraint.getType() == Constraint::EQUALITY && (upper = 1)) ||
                   ((*it) > 0 && Numerical::stableAdd(varLowerBound * (*it), (-1) * rowUpperBound) == 0 && (upper = 1)) ||
                   ((*it) > 0 && Numerical::stableAdd(varUpperBound * (*it), (-1) * rowLowerBound) == 0 && (upper = 2)) ||
                   ((*it) < 0 && Numerical::stableAdd(varLowerBound * (*it), (-1) * rowLowerBound) == 0 && (upper = 2)) ||
                   ((*it) < 0 && Numerical::stableAdd(varUpperBound * (*it), (-1) * rowUpperBound) == 0 && (upper = 1))) {

                    if(upper == 1) {
                        fixVal = rowUpperBound / (*it);
                    } else {
                        fixVal = rowLowerBound / (*it);
                    }
                    //remove constraint, fix variable and break
                    m_removedConstraints++;
                    m_parent->removeConstraint(index);
                    lastRow = m_parent->getRowNonzeros()->end();

                    m_parent->fixVariable(varIdx, fixVal);
                    eliminatedVariableCount++;
                    m_removedVariables++;
                    continue;
                } else {
                    //update variable bounds
                    if((*it) > 0) {
                        if(varLowerBound < rowLowerBound / (*it)) {
                            curVariable.setLowerBound(rowLowerBound / (*it));
                            m_parent->getImpliedLower()->set(varIdx, rowLowerBound / (*it));
                        }
                        if(varUpperBound > rowUpperBound / (*it)) {
                            curVariable.setUpperBound(rowUpperBound / (*it));
                            m_parent->getImpliedUpper()->set(varIdx, rowUpperBound / (*it));
                        }
                    } else {
                        if(varLowerBound < rowUpperBound / (*it)) {
                            curVariable.setLowerBound(rowUpperBound / (*it));
                            m_parent->getImpliedLower()->set(varIdx, rowUpperBound / (*it));
                        }
                        if(varUpperBound > rowLowerBound / (*it)) {
                            curVariable.setUpperBound(rowLowerBound / (*it));
                            m_parent->getImpliedUpper()->set(varIdx, rowLowerBound / (*it));
                        }
                    }

                    //remove constraint
                    m_removedConstraints++;
                    m_parent->removeConstraint(index);
                    m_parent->getColumnNonzeros()->set(varIdx, m_parent->getColumnNonzeros()->at(varIdx) - 1);
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

    //If no constraint was eliminated from the model, no empty or singleton row is created,
    //otherwise we need to check the rows again
    do {
        eliminatedConstraintCount = 0;

        Vector::Iterator currentColumn = m_parent->getColumnNonzeros()->begin();
        Vector::Iterator lastColumn = m_parent->getColumnNonzeros()->end();

        //Check every column of the model for fixed variables, empty or singleton columns
        while(currentColumn < lastColumn) {

            int index = currentColumn.getIndex();
            const Variable& curVariable = (*m_parent->getVariables())[index];
            const Numerical::Double& varLowerBound = curVariable.getLowerBound();
            const Numerical::Double& varUpperBound = curVariable.getUpperBound();

            //handle fixed variables
            if(Numerical::stableAdd(varLowerBound, -1 * varUpperBound) == 0) {

                m_parent->fixVariable(index, varLowerBound);
                lastColumn = m_parent->getColumnNonzeros()->end();
                m_removedVariables++;
                continue;
            }

            //handle empty columns
            if(*currentColumn == 0) {
                Numerical::Double costCoeff = m_parent->getCostCoefficient(index);
                if(m_parent->getModel()->getObjectiveType() == MINIMIZE) {

                    if((varLowerBound == -Numerical::Infinity && costCoeff > 0) ||
                       (varUpperBound == Numerical::Infinity && costCoeff < 0)) {
                        throw Presolver::PresolverException("The problem is primal unbounded.");
                        return;
                    } else {
                        if(costCoeff > 0) {
                            m_parent->fixVariable(index, varLowerBound);
                        } else {
                            m_parent->fixVariable(index, varUpperBound);
                        }
                        lastColumn = m_parent->getColumnNonzeros()->end();
                        m_removedVariables++;
                        continue;
                    }

                } else {

                    if((varLowerBound == -Numerical::Infinity && costCoeff < 0) ||
                       (varUpperBound == Numerical::Infinity && costCoeff > 0)) {
                        throw Presolver::PresolverException("The problem is primal unbounded.");
                        return;
                    } else {
                        if(costCoeff < 0) {
                            m_parent->fixVariable(index, varLowerBound);
                        } else {
                            m_parent->fixVariable(index, varUpperBound);
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
                Vector::NonzeroIterator it = curVariable.getVector()->beginNonzero();
                int constraintIdx = it.getIndex();
                const Constraint& curConstraint = (*m_parent->getConstraints())[constraintIdx];
                const Numerical::Double& rowLowerBound = curConstraint.getLowerBound();
                const Numerical::Double& rowUpperBound = curConstraint.getUpperBound();

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
                if((varLowerBound == -Numerical::Infinity && varUpperBound == Numerical::Infinity/* && (fixSign = 0 || true)*/)) {// ||
                   //!((*it) > 0 && varUpperBound == Numerical::Infinity && m_parent->getRows().getAttachedData(constraintIdx).getUpperBound() == Numerical::Infinity && (fixSign = 1 || true)) ||
                   //!((*it) > 0 && varLowerBound == -Numerical::Infinity && m_parent->getRows().getAttachedData(constraintIdx).getLowerBound() == -Numerical::Infinity && (fixSign = -1 || true)) ||
                   //!((*it) < 0 && varLowerBound == -Numerical::Infinity && m_parent->getRows().getAttachedData(constraintIdx).getUpperBound() == Numerical::Infinity && (fixSign = -1 || true)) ||
                   //!((*it) < 0 && varUpperBound == Numerical::Infinity && m_parent->getRows().getAttachedData(constraintIdx).getLowerBound() == -Numerical::Infinity && (fixSign = 1 || true))) {
                    Numerical::Double costCoeff = m_parent->getCostCoefficient(index);
                    Numerical::Double fixBound;
                    if(*it > 0) {

                        //get the constraint bound to fix the variable
                        if(costCoeff > 0) {
                            fixBound = rowLowerBound;
                        } else {
                            fixBound = rowUpperBound;
                        }

                        //detect unboundedness
                        if(fixBound == Numerical::Infinity || fixBound == -Numerical::Infinity) {
                            throw Presolver::PresolverException("The problem is primal unbounded.");
                            return;
                        }
                    } else {

                        //get the constraint bound to fix the variable
                        if(costCoeff > 0) {
                            fixBound = rowUpperBound;
                        } else {
                            fixBound = rowLowerBound;
                        }

                        //detect unboundedness
                        if(fixBound == Numerical::Infinity || fixBound == -Numerical::Infinity) {
                            throw Presolver::PresolverException("The problem is primal unbounded.");
                            return;
                        }
                    }

                    //substitute the variable with the other variables present in the constraint,
                    //change the cost vector, and remove the constraint and the variable from the model
                    int varCount = m_parent->getModel()->variableCount();
                    Vector * substituteVector = new Vector(varCount + 3);
                    Vector::NonzeroIterator constraintIt = curConstraint.getVector()->beginNonzero();
                    Vector::NonzeroIterator constraintItEnd = curConstraint.getVector()->endNonzero();
                    for(; constraintIt < constraintItEnd; constraintIt++) {
                        int varIndex = constraintIt.getIndex();
                        substituteVector->set(varIndex, (-1) * (*constraintIt) / (*it));
                        m_parent->getColumnNonzeros()->set(varIndex, m_parent->getColumnNonzeros()->at(varIndex) - 1);
                        m_parent->getModel()->addToCostCoefficient(varIndex, -1 * ((*constraintIt) * costCoeff / (*it)));
                    }
                    m_parent->getModel()->setCostConstant(Numerical::stableAdd(m_parent->getModel()->getCostConstant(), -1 * fixBound * costCoeff / (*it)));
                    substituteVector->set(index, 0);
                    substituteVector->set(varCount, fixBound / (*it));
                    substituteVector->set(varCount + 1, index);
                    substituteVector->set(varCount + 2, Presolver::PRIMAL_VARIABLE);
                    m_parent->getSubstituteVectors()->push_back(substituteVector);

                    m_removedConstraints++;
                    eliminatedConstraintCount++;
                    m_parent->removeConstraint(constraintIdx);

                    m_removedVariables++;
                    m_parent->getModel()->removeVariable(index);
                    m_parent->getColumnNonzeros()->removeElement(index);
                    m_parent->getImpliedLower()->removeElement(index);
                    m_parent->getImpliedUpper()->removeElement(index);
                    m_parent->getExtraDualLowerSum()->removeElement(index);
                    m_parent->getExtraDualUpperSum()->removeElement(index);
                    lastColumn = m_parent->getColumnNonzeros()->end();
                    continue;
                }
            }
            currentColumn++;
        }
    } while (eliminatedConstraintCount);
}

ImpliedBoundsModule::ImpliedBoundsModule(Presolver *parent)
{
    m_removedConstraints = 0;
    m_removedVariables = 0;
    m_parent = parent;
    m_name = "Implied bounds module";
    m_impliedLower = m_parent->getImpliedLower();
    m_impliedUpper = m_parent->getImpliedUpper();
}

ImpliedBoundsModule::~ImpliedBoundsModule() { }

void ImpliedBoundsModule::executeMethod() {

    //Initialize the stacks according to the current state of the model
    int rowCount = m_parent->getModel()->constraintCount();
    m_constraintsToCheck = new Vector(rowCount);
    m_constraintsToCheck->setSparsityRatio(0);
    m_constraintStack = new Vector(rowCount);
    m_constraintStack->setSparsityRatio(0);
    m_variablesToFix = new Vector(m_parent->getModel()->variableCount());
    m_variablesToFix->setSparsityRatio(0);
    for(int i = 0; i < rowCount; i++) {
        m_constraintsToCheck->set(i, 1);
    }
    Vector::NonzeroIterator begin = m_constraintsToCheck->beginNonzero();
    Vector::NonzeroIterator end = m_constraintsToCheck->endNonzero();
    Numerical::Double impliedUB, impliedLB;

    //This loop iterates through the constraints of the model. After the first loop through every constraint of the model
    //only the constraints with possibly changed implied bounds are checked.
    int stackSwapCounter = 0;
    while(true) {

        //Swap the stacks containing the constraints to check and control the loop
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

        int index = begin.getIndex();
        const Constraint& curConstraint = (*m_parent->getConstraints())[index];
        const Numerical::Double& rowLowerBound = curConstraint.getLowerBound();
        const Numerical::Double& rowUpperBound = curConstraint.getUpperBound();

        impliedLB = 0;
        impliedUB = 0;
        Vector::NonzeroIterator it = curConstraint.getVector()->beginNonzero();
        Vector::NonzeroIterator itEnd = curConstraint.getVector()->endNonzero();
        Vector::NonzeroIterator itV = it;
        Vector::NonzeroIterator itVEnd = itEnd;

        //Calculate implied constraint bounds
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

        //Check if bounds conflict
        if(Numerical::stableAdd(impliedLB, -1 * rowUpperBound) > 0 ||
           Numerical::stableAdd(impliedUB, -1 * rowLowerBound) < 0) {
            throw Presolver::PresolverException("The problem is primal infeasible.");
            return;
        }

        //Check if constraint is redundant
        if(impliedLB > rowLowerBound &&
           impliedUB < rowUpperBound) {
            m_parent->getModel()->removeConstraint(index);
            m_parent->getRowNonzeros()->removeElement(index);
            m_parent->getImpliedDualLower()->removeElement(index);
            m_parent->getImpliedDualUpper()->removeElement(index);
            m_constraintsToCheck->removeElement(index);
            m_constraintStack->removeElement(index);
            end = m_constraintsToCheck->endNonzero();
            if(*begin != 1) { begin++; }
            m_removedConstraints++;
            continue;
        }

        it = curConstraint.getVector()->beginNonzero();

        //Fix all participating variables to their bounds if constaint is forcing.
        //Constraint can be removed after.
        if(impliedLB == rowUpperBound) {
            for(; it < itEnd; it++) {
                m_variablesToFix->set(it.getIndex(), 1);
            }
            itV = m_variablesToFix->beginNonzero();
            itVEnd = m_variablesToFix->endNonzero();
            if(*itV != 1) { itV++; }
            while(itV < itVEnd) {
                int varIdx = itV.getIndex();
                const Variable& curVariable = (*m_parent->getVariables())[varIdx];
                it = curVariable.getVector()->beginNonzero();
                itEnd = curVariable.getVector()->endNonzero();
                for(; it < itEnd; it++) {
                    m_constraintStack->set(it.getIndex(), 1);
                }

                if(curConstraint.getVector()->at(varIdx) > 0) {
                    m_parent->fixVariable(varIdx, m_impliedLower->at(varIdx));
                } else {
                    m_parent->fixVariable(varIdx, m_impliedUpper->at(varIdx));
                }
                m_variablesToFix->removeElement(varIdx);
                itVEnd = m_variablesToFix->endNonzero();
                m_removedVariables++;
                if(*itV != 1) { itV++; }
            }
            m_parent->removeConstraint(index);
            m_constraintsToCheck->removeElement(index);
            m_constraintStack->removeElement(index);
            m_removedConstraints++;
            end = m_constraintsToCheck->endNonzero();
            if(*begin != 1) { begin++; }
            continue;
        }
        if(impliedUB == rowLowerBound) {
            for(; it < itEnd; it++) {
                m_variablesToFix->set(it.getIndex(), 1);
            }
            itV = m_variablesToFix->beginNonzero();
            itVEnd = m_variablesToFix->endNonzero();
            if(*itV != 1) { itV++; }
            while(itV < itVEnd) {
                int varIdx = itV.getIndex();
                const Variable& curVariable = (*m_parent->getVariables())[varIdx];
                it = curVariable.getVector()->beginNonzero();
                itEnd = curVariable.getVector()->endNonzero();
                for(; it < itEnd; it++) {
                    m_constraintStack->set(it.getIndex(), 1);
                }

                if(m_parent->getModel()->getMatrix().get(index, varIdx) > 0) {
                    m_parent->fixVariable(varIdx, m_impliedUpper->at(varIdx));
                } else {
                    m_parent->fixVariable(varIdx, m_impliedLower->at(varIdx));
                }
                m_variablesToFix->removeElement(varIdx);
                itVEnd = m_variablesToFix->endNonzero();
                m_removedVariables++;
                if(*itV != 1) { itV++; }
            }
            m_parent->removeConstraint(index);
            m_constraintsToCheck->removeElement(index);
            m_constraintStack->removeElement(index);
            m_removedConstraints++;
            end = m_constraintsToCheck->endNonzero();
            if(*begin != 1) { begin++; }
            continue;
        }

//        //Use the tightest computed bounds for constraints
//        if(impliedLB > curConstraint.getLowerBound()) {
//            curConstraint.setLowerBound(impliedLB);
//        }

//        if(impliedUB < curConstraint.getUpperBound()) {
//            curConstraint.setUpperBound(impliedUB);
//        }

        //If constraint is neither redundant nor forcing, calculate the implied bounds of the participating
        //variables and compare them to the original bounds.
        //!Make every comparsion based on implied bounds. Current implementetion is numerically unstable.
        if(impliedLB != -Numerical::Infinity && rowUpperBound != Numerical::Infinity) {
            it = curConstraint.getVector()->beginNonzero();
            itEnd = curConstraint.getVector()->endNonzero();
            for(; it < itEnd; it++) {
                int varIdx = it.getIndex();
                const Variable& curVariable = (*m_parent->getVariables())[varIdx];
                if(*it > 0) {
                    if(Numerical::stableAdd(Numerical::stableAdd((rowUpperBound - impliedLB) / *it, m_impliedLower->at(varIdx)), -1 * m_impliedUpper->at(varIdx)) < 0) {
                        m_impliedUpper->set(varIdx, Numerical::stableAdd((rowUpperBound - impliedLB) / *it, m_impliedLower->at(varIdx)));
                        itV = curVariable.getVector()->beginNonzero();
                        itVEnd = curVariable.getVector()->endNonzero();
                        for(; itV < itVEnd; itV++) {
                            m_constraintStack->set(itV.getIndex(), 1);
                        }
                    }

                } else {
                    if(Numerical::stableAdd(m_impliedLower->at(varIdx), -1 * Numerical::stableAdd((rowUpperBound - impliedLB) / *it,  m_impliedLower->at(varIdx))) < 0) {
                       m_impliedLower->set(varIdx, Numerical::stableAdd((rowUpperBound - impliedLB) / *it,  m_impliedUpper->at(varIdx)));
                       itV = curVariable.getVector()->beginNonzero();
                       itVEnd = curVariable.getVector()->endNonzero();
                       for(; itV < itVEnd; itV++) {
                           m_constraintStack->set(itV.getIndex(), 1);
                       }
                    }
                }
            }
        }

        if(impliedUB != -Numerical::Infinity && rowLowerBound != Numerical::Infinity) {
            it = curConstraint.getVector()->beginNonzero();
            itEnd = curConstraint.getVector()->endNonzero();
            for(; it < itEnd; it++) {
                int varIdx = it.getIndex();
                const Variable& curVariable = (*m_parent->getVariables())[varIdx];
                if(*it > 0) {
                    if(Numerical::stableAdd(m_impliedLower->at(varIdx), -1 * Numerical::stableAdd((curConstraint.getLowerBound() - impliedUB) / *it, m_impliedLower->at(varIdx))) < 0) {
                       m_impliedLower->set(varIdx, Numerical::stableAdd((curConstraint.getLowerBound() - impliedUB) / *it, m_impliedUpper->at(varIdx)));
                       itV = curVariable.getVector()->beginNonzero();
                       itVEnd = curVariable.getVector()->endNonzero();
                       for(; itV < itVEnd; itV++) {
                           m_constraintStack->set(itV.getIndex(), 1);
                       }
                    }

                } else {
                    if(Numerical::stableAdd(Numerical::stableAdd((rowLowerBound - impliedUB) / *it, m_impliedLower->at(varIdx)), -1 * m_impliedUpper->at(varIdx)) < 0) {
                       m_impliedUpper->set(varIdx, Numerical::stableAdd((rowLowerBound - impliedUB) / *it, m_impliedLower->at(varIdx)));
                       itV = curVariable.getVector()->beginNonzero();
                       itVEnd = curVariable.getVector()->endNonzero();
                       for(; itV < itVEnd; itV++) {
                           m_constraintStack->set(itV.getIndex(), 1);
                       }
                    }
                }
            }
        }


        m_constraintsToCheck->set(index, 0);
        begin++;
    }
}

DualBoundsModule::DualBoundsModule(Presolver *parent) :
    PresolverModule(parent)
{
    m_name = "Dual bounds module";
    m_impliedDualLower = m_parent->getImpliedDualLower();
    m_impliedDualUpper = m_parent->getImpliedDualUpper();
    m_extraDualLowerSum = m_parent->getExtraDualLowerSum();
    m_extraDualUpperSum = m_parent->getExtraDualUpperSum();
}

DualBoundsModule::~DualBoundsModule() { }

void DualBoundsModule::executeMethod() {

    int columnCount = m_parent->getModel()->variableCount();
    m_variablesToCheck = new Vector(columnCount);
    m_variablesToCheck->setSparsityRatio(0);
    for(int i = 0; i < columnCount; i++) {
        m_variablesToCheck->set(i, 1);
    }
    m_variableStack = new Vector(columnCount);
    m_variableStack->setSparsityRatio(0);
    Vector::NonzeroIterator begin = m_variablesToCheck->beginNonzero();
    Vector::NonzeroIterator end = m_variablesToCheck->endNonzero();
    Numerical::Double impliedUB, impliedLB;

    //This loop iterates through the variables of the model. After the first loop through every variable of the model
    //only the variables with possibly changed implied bounds are checked.
    int stackSwapCounter = 0;
    while(true) {

        //Swap the stacks containing the variables to check and control the loop
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
        int index = begin.getIndex();
        const Variable& curVariable = (*m_parent->getVariables())[index];
        const Numerical::Double& varLowerBound = curVariable.getLowerBound();
        const Numerical::Double& varUpperBound = curVariable.getUpperBound();

        Vector::NonzeroIterator it = curVariable.getVector()->beginNonzero();
        Vector::NonzeroIterator itEnd = curVariable.getVector()->endNonzero();
        Vector::NonzeroIterator itC = it;
        Vector::NonzeroIterator itCEnd = itEnd;

        //Calculating the implied dual constraint bounds
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
        if(m_extraDualLowerSum->at(index) != 0) {
            impliedLB = -Numerical::Infinity;
        }
        if(m_extraDualUpperSum->at(index) != 0) {
            impliedUB = Numerical::Infinity;
        }

        const Vector& costVector = m_parent->getModel()->getCostVector();

        //Check if variable can be specially treated
        if(varUpperBound == Numerical::Infinity/* || curVariable.getType() == Variable::BOUNDED*/) {

            //Check if implied bounds conflict
            if(costVector.at(index) < impliedLB) {
                throw Presolver::PresolverException("The problem is dual infeasible.");
                return;
            }

            //Check if variable is dominated
            if(costVector.at(index) > impliedUB) {
                if(varLowerBound == -Numerical::Infinity) {
                    throw Presolver::PresolverException("The problem is dual unbounded.");
                    return;
                } else {
                    m_parent->fixVariable(index, varLowerBound);
                    m_variablesToCheck->removeElement(index);
                    m_variableStack->removeElement(index);
                    end = m_variablesToCheck->endNonzero();
                    if(*begin != 1) { begin++; }
                    m_removedVariables++;
                    continue;
                }
            }

            //If variable is neither conflicted nor dominated, the bounds of dual variables corresponding to the rows
            //this variable participates in can be tightened.
            if(impliedLB < costVector.at(index) && impliedUB > costVector.at(index) && impliedLB != -Numerical::Infinity) {
                it = curVariable.getVector()->beginNonzero();
                itEnd = curVariable.getVector()->endNonzero();
                for(; it < itEnd; it++) {
                    int conIdx = it.getIndex();
                    const Constraint& curConstraint = (*m_parent->getConstraints())[conIdx];
                    if(*it > 0) {
                        if(Numerical::stableAdd(Numerical::stableAdd((costVector.at(index) - impliedLB) / *it, m_impliedDualLower->at(conIdx)), -1 * m_impliedDualUpper->at(conIdx)) < 0) {
                            m_impliedDualUpper->set(conIdx, Numerical::stableAdd((costVector.at(index) - impliedLB) / *it, m_impliedDualLower->at(conIdx)));
                               itC = curConstraint.getVector()->beginNonzero();
                               itCEnd = curConstraint.getVector()->endNonzero();
                               for(; itC < itCEnd; itC++) {
                                   m_variableStack->set(itC.getIndex(), 1);
                               }
                        }
                    } else {
                        if(Numerical::stableAdd(m_impliedDualLower->at(conIdx), -1 * Numerical::stableAdd((costVector.at(index) - impliedLB) / *it,  m_impliedDualUpper->at(conIdx))) < 0) {
                           m_impliedDualLower->set(conIdx, Numerical::stableAdd((costVector.at(index) - impliedLB) / *it,  m_impliedDualUpper->at(conIdx)));
                           itC = curConstraint.getVector()->beginNonzero();
                           itCEnd = curConstraint.getVector()->endNonzero();
                           for(; itC < itCEnd; itC++) {
                               m_variableStack->set(itC.getIndex(), 1);
                           }
                        }
                    }
                }
            }
        }

        //Check if variable can be specially treated
        if(varLowerBound == -Numerical::Infinity) {

            //Check if implied bounds conflict
            if(costVector.at(index) > impliedUB) {
                throw Presolver::PresolverException("The problem is dual infeasible.");
                return;
            }

            //Check if variable is dominated
            if(costVector.at(index) < impliedLB) {
                if(varLowerBound == -Numerical::Infinity) {
                    throw Presolver::PresolverException("The problem is dual unbounded.");
                    return;
                } else {
                    m_parent->fixVariable(index, varLowerBound);
                    m_variablesToCheck->removeElement(index);
                    m_variableStack->removeElement(index);
                    end = m_variablesToCheck->endNonzero();
                    if(*begin != 1) { begin++; }
                    m_removedVariables++;
                    continue;
                }
            }

            //If variable is neither conflicted nor dominated, the bounds of dual variables corresponding to the rows
            //this variable participates in can be tightened.
            if(impliedLB < costVector.at(index) && impliedUB > costVector.at(index) && impliedUB != -Numerical::Infinity) {
                it = curVariable.getVector()->beginNonzero();
                itEnd = curVariable.getVector()->endNonzero();
                for(; it < itEnd; it++) {
                    int conIdx = it.getIndex();
                    const Constraint& curConstraint = (*m_parent->getConstraints())[conIdx];
                    if(*it > 0) {
                        if(Numerical::stableAdd(m_impliedDualLower->at(conIdx), -1 * Numerical::stableAdd((costVector.at(index) - impliedUB) / *it,  m_impliedDualUpper->at(conIdx))) < 0) {
                            m_impliedDualLower->set(conIdx, Numerical::stableAdd((costVector.at(index) - impliedUB) / *it, m_impliedDualUpper->at(conIdx)));
                               itC = curConstraint.getVector()->beginNonzero();
                               itCEnd = curConstraint.getVector()->endNonzero();
                               for(; itC < itCEnd; itC++) {
                                   m_variableStack->set(itC.getIndex(), 1);
                               }
                        }
                    } else {
                        if(Numerical::stableAdd(Numerical::stableAdd((costVector.at(index) - impliedUB) / *it, m_impliedDualLower->at(conIdx)), -1 * m_impliedDualUpper->at(conIdx)) < 0) {
                           m_impliedDualUpper->set(conIdx, Numerical::stableAdd((costVector.at(index) - impliedUB) / *it,  m_impliedDualLower->at(conIdx)));
                           itC = curConstraint.getVector()->beginNonzero();
                           itCEnd = curConstraint.getVector()->endNonzero();
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
}

LinearAlgebraicModule::~LinearAlgebraicModule() { }

void LinearAlgebraicModule::executeMethod() {

    int rowCount = m_parent->getModel()->constraintCount();
    int columnCount = m_parent->getModel()->variableCount();

    //Index list to distuinguish rows with the same hashes
    IndexList<> hashedRows(rowCount, rowCount);

    Vector usedPartitions(rowCount);
    Vector indices(rowCount);

    for(int i = 0; i < rowCount; i++) {
        int vhash = 0;
        const Vector* curRowVector = (*m_parent->getConstraints())[i].getVector();

        //Calculating hash
        Vector::NonzeroIterator begin = curRowVector->beginNonzero();
        Vector::NonzeroIterator end = curRowVector->endNonzero();
        while(begin != end) {
            vhash += begin.getIndex()+1;
            begin++;
        }
        int partition = vhash % rowCount;
        usedPartitions.set(partition, usedPartitions.at(partition) + 1);
        hashedRows.insert(partition, i);
    }

    //Eliminating duplicate rows
    Vector::NonzeroIterator begin = usedPartitions.beginNonzero();
    Vector::NonzeroIterator end = usedPartitions.endNonzero();

    while(begin < end) {

        if(*begin < 2) { begin++; continue; }

        IndexList<>::PartitionIterator it, itFirst, itEnd, itZero;
        hashedRows.getIterators(&itFirst, &itEnd, begin.getIndex());

        while(itFirst != itEnd) {
            it = itFirst;
            it++;

            while(it != itEnd) {

                int origIdx = itFirst.getData();
                int duplIdx = it.getData();
                Constraint& original = (*m_parent->getConstraints())[origIdx];
                const Constraint& duplicate = (*m_parent->getConstraints())[duplIdx];

                Vector::NonzeroIterator origIterator = original.getVector()->beginNonzero();
                Vector::NonzeroIterator duplIterator = duplicate.getVector()->beginNonzero();

                Numerical::Double lambda = *origIterator / *duplIterator;
                Vector tryVector = *(duplicate.getVector()) * lambda - *(original.getVector());

                if(tryVector.nonZeros() == 0) {

                    //With negative lambda, duplicate bounds are swapped
                    if(lambda > 0) {
                        //Checking feasibility
                        if(duplicate.getLowerBound() * lambda > original.getUpperBound()
                           || duplicate.getUpperBound() * lambda < original.getLowerBound())
                        {
                            throw Presolver::PresolverException("The problem is primal infeasible.");
                            return;
                        }

                        //Applying the tighter bounds
                        if(duplicate.getLowerBound() * lambda > original.getLowerBound()) {
                            original.setLowerBound(duplicate.getLowerBound() * lambda);
                        }

                        if(duplicate.getUpperBound() * lambda < original.getUpperBound()) {
                            original.setUpperBound(duplicate.getUpperBound() * lambda);
                        }

                    } else {

                        //Checking feasibility
                        if(duplicate.getUpperBound() * lambda > original.getUpperBound()
                           || duplicate.getLowerBound() * lambda < original.getLowerBound())
                        {
                            throw Presolver::PresolverException("The problem is primal infeasible.");
                            return;
                        }

                        //Applying the tighter bounds
                        if(duplicate.getUpperBound() * lambda > original.getLowerBound()) {
                            original.setLowerBound(duplicate.getUpperBound() * lambda);
                        }

                        if(duplicate.getLowerBound() * lambda < original.getUpperBound()) {
                            original.setUpperBound(duplicate.getLowerBound() * lambda);
                        }
                    }
                    it++;
                    hashedRows.remove(duplIdx);
                    indices.set(duplIdx, 1);
                    hashedRows.getIterators(&itZero, &itEnd, begin.getIndex());
                } else {
                    it++;
                }
            }
            itFirst++;
        }
        begin++;
    }

    //The elimination of constraints needs to start at the row with the highest index

    begin = indices.beginNonzero();
    end = indices.endNonzero();

    while(begin < end) {

        int removeIndex = begin.getIndex();

        m_removedConstraints++;
        m_parent->getModel()->removeConstraint(removeIndex);
        m_parent->getRowNonzeros()->removeElement(removeIndex);
        m_parent->getImpliedDualLower()->removeElement(removeIndex);
        m_parent->getImpliedDualUpper()->removeElement(removeIndex);

        indices.removeElement(removeIndex);
        end = indices.endNonzero();
        if(*begin != 1) { begin++; }
    }

    rowCount = m_parent->getModel()->constraintCount();


    //Index list to distuinguish rows with the same hashes
    IndexList<> hashedColumns(columnCount, columnCount);

    usedPartitions.reInit(columnCount);
    indices.reInit(columnCount);

    for(int i = 0; i < columnCount; i++) {
        int vhash = 0;
        const Vector* curColumnVector = (*m_parent->getVariables())[i].getVector();

        //Calculating hash
        Vector::NonzeroIterator begin = curColumnVector->beginNonzero();
        Vector::NonzeroIterator end = curColumnVector->endNonzero();
        while(begin != end) {
            vhash += begin.getIndex()+1;
            begin++;
        }
        int partition = vhash % columnCount;
        usedPartitions.set(partition, usedPartitions.at(partition) + 1);
        hashedColumns.insert(partition, i);
    }

    //Eliminating duplicate columns
    begin = usedPartitions.beginNonzero();
    end = usedPartitions.endNonzero();

    while(begin < end) {

        if(*begin < 2) { begin++; continue; }

        IndexList<>::PartitionIterator it, itFirst, itEnd, itZero;
        hashedColumns.getIterators(&itFirst, &itEnd, begin.getIndex());

        while(itFirst != itEnd) {
            it = itFirst;
            it++;

            while(it != itEnd) {

                int origIdx = itFirst.getData();
                int duplIdx = it.getData();
                Variable& original = (*m_parent->getVariables())[origIdx];
                const Variable& duplicate = (*m_parent->getVariables())[duplIdx];

                Vector::NonzeroIterator origIterator = original.getVector()->beginNonzero();
                Vector::NonzeroIterator duplIterator = duplicate.getVector()->beginNonzero();

                Numerical::Double lambda = *origIterator / *duplIterator;
                Vector tryVector = *(duplicate.getVector()) * lambda - *(original.getVector());

                if(tryVector.nonZeros() == 0 && m_parent->getModel()->getCostVector().at(duplIdx) * lambda == m_parent->getModel()->getCostVector().at(origIdx)) {

                    //With negative lambda, duplicate bounds are swapped
                    if(lambda > 0) {

                        //Checking feasibility
                        if(duplicate.getLowerBound() * lambda + original.getLowerBound()
                           > duplicate.getUpperBound() * lambda + original.getUpperBound())
                        {
                            throw Presolver::PresolverException("The problem is primal infeasible.");
                            return;
                        }

                        //Creating bounds
                        original.setLowerBound(duplicate.getLowerBound() * lambda + original.getLowerBound());
                        original.setUpperBound(duplicate.getUpperBound() * lambda + original.getUpperBound());
                        m_parent->getImpliedLower()->set(origIdx, duplicate.getLowerBound() * lambda + original.getLowerBound());
                        m_parent->getImpliedUpper()->set(origIdx, duplicate.getUpperBound() * lambda + original.getUpperBound());

                    } else {

                        //Checking feasibility
                        if(duplicate.getUpperBound() * lambda + original.getLowerBound()
                           > duplicate.getLowerBound() * lambda + original.getUpperBound())
                        {
                            throw Presolver::PresolverException("The problem is primal infeasible.");
                            return;
                        }

                        //Creating bounds
                        original.setLowerBound(duplicate.getUpperBound() * lambda + original.getLowerBound());
                        original.setUpperBound(duplicate.getLowerBound() * lambda + original.getUpperBound());
                        m_parent->getImpliedLower()->set(origIdx, duplicate.getUpperBound() * lambda + original.getLowerBound());
                        m_parent->getImpliedUpper()->set(origIdx, duplicate.getLowerBound() * lambda + original.getUpperBound());

                    }

                    it++;
                    hashedColumns.remove(duplIdx);
                    indices.set(duplIdx, 1);
                    hashedColumns.getIterators(&itZero, &itEnd, begin.getIndex());
                } else {
                    it++;
                }
            }
            itFirst++;
        }
        begin++;
    }

    //The elimination of variables needs to start at the row with the highest index
    //A special type of substitute vector is created to be able to regain correct values for the original variables

    begin = indices.beginNonzero();
    end = indices.endNonzero();

    while(begin < end) {

        int removeIndex = begin.getIndex();

        Vector * substituteVector = new Vector(columnCount + 3);
        substituteVector->set(columnCount + 1, removeIndex);
        substituteVector->set(columnCount + 2, Presolver::DUPLICATE_VARIABLE);
        m_parent->getSubstituteVectors()->push_back(substituteVector);

        m_removedVariables++;
        m_parent->getModel()->removeVariable(removeIndex);
        m_parent->getColumnNonzeros()->removeElement(removeIndex);
        m_parent->getImpliedLower()->removeElement(removeIndex);
        m_parent->getImpliedUpper()->removeElement(removeIndex);
        m_parent->getExtraDualLowerSum()->removeElement(removeIndex);
        m_parent->getExtraDualUpperSum()->removeElement(removeIndex);

        indices.removeElement(removeIndex);
        end = indices.endNonzero();
        if(*begin != 1) { begin++; }
    }
}

MakeSparserModule::MakeSparserModule(Presolver *parent) :
    PresolverModule(parent)
{
    m_name = "Make sparser module";
    m_removedNzr = 0;
}

MakeSparserModule::~MakeSparserModule() { }

void MakeSparserModule::executeMethod() {

    int rowCount = m_parent->getModel()->constraintCount();

    //Index list to sort the rows by nonzero count
    IndexList<> sortedRows(rowCount, rowCount);
    Vector usedPartitions(rowCount);

    //Set up the sorted list
    for(int i = 0; i < rowCount; i++) {
        int nonzeros = m_parent->getRowNonzeros()->at(i);
        if(m_parent->getConstraints()->at(i).getType() == Constraint::EQUALITY)
        {
            sortedRows.insert(nonzeros, i);
            usedPartitions.set(nonzeros, usedPartitions.at(nonzeros) + 1);
        }
    }

    IndexList<>::PartitionIterator it, itEnd;

    Vector::NonzeroIterator begin = usedPartitions.beginNonzero();
    Vector::NonzeroIterator end = usedPartitions.endNonzero();
    while(begin.getIndex() < 1) begin++;
    while(begin < end) {
        sortedRows.getIterators(&it, &itEnd, begin.getIndex());
        while(it != itEnd) {
            int index = it.getData();
            const Constraint& pivotRow = m_parent->getConstraints()->at(index);
            Vector::NonzeroIterator pivotIt = pivotRow.getVector()->beginNonzero();
            Vector::NonzeroIterator pivotItEnd = pivotRow.getVector()->endNonzero();
            int columnIdx = pivotIt.getIndex();
            int pivotNonzeros = pivotRow.getVector()->nonZeros();
            Vector::NonzeroIterator beginColumn = m_parent->getVariables()->at(columnIdx).getVector()->beginNonzero();
            Vector::NonzeroIterator endColumn = m_parent->getVariables()->at(columnIdx).getVector()->endNonzero();
            while(beginColumn < endColumn) {
                int secondIndex = beginColumn.getIndex();
                Constraint& curRow = m_parent->getConstraints()->at(secondIndex);
                int nonzeros = curRow.getVector()->nonZeros();
                if(secondIndex != index &&
                   nonzeros >= pivotNonzeros)
                {
                    while(pivotIt < pivotItEnd) {
                        if(curRow.getVector()->at(pivotIt.getIndex()) == 0) {
                            break;
                        }
                        pivotIt++;
                    }
                    if(pivotIt == pivotItEnd) {
                        pivotIt = pivotRow.getVector()->beginNonzero();
                        Numerical::Double lambda = *beginColumn / *pivotIt;
                        m_parent->getModel()->addToConstraint(secondIndex, index, -lambda);
                        curRow.setBounds(curRow.getLowerBound() - lambda * pivotRow.getLowerBound(), curRow.getUpperBound() - lambda * pivotRow.getUpperBound());
                        if(nonzeros - (int)curRow.getVector()->nonZeros() < 0) LPERROR("Make sparser module error: vector substraction created extra " << -(nonzeros - (int)curRow.getVector()->nonZeros()) << " nonzeros");
                        m_removedNzr += nonzeros - (int)curRow.getVector()->nonZeros();
                    } else {
                        pivotIt = pivotRow.getVector()->beginNonzero();
                    }
                }
                beginColumn++;
            }
            it++;
        }
        begin++;
    }
}
