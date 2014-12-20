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
    const Numerical::Double & feasibilityTolerance = m_parent->getFeasibilityTolerance();
    for(unsigned i = 0; i < m_parent->getModel()->constraintCount();i++) {
        m_parent->getRowNonzeros()->set(i, (*m_parent->getConstraints())[i].getVector()->nonZeros());
    }
    int rowCount = m_parent->getModel()->constraintCount();

    std::vector<int> removedRows(rowCount, 0);
    int removedCount = 0;

    //If no variable was eliminated from the model, no empty or singleton row is created,
    //otherwise we need to check the rows again
    do {
        eliminatedVariableCount = 0;

        DenseVector::Iterator currentRow = m_parent->getRowNonzeros()->begin();
        DenseVector::Iterator lastRow = m_parent->getRowNonzeros()->end();
        int index = 0;

        //Check every row of the model for empty or singleton rows
        while(currentRow < lastRow) {

            //handle empty rows
            if(*currentRow == 0 && removedRows[index] == 0) {
                const Constraint& curConstraint = (*m_parent->getConstraints())[index];

                //Empty rows are either infeasible or can be freely removed
                if(curConstraint.getLowerBound() > 0 || curConstraint.getUpperBound() < 0) {
                    throw Presolver::PresolverException("The problem is primal infeasible.");
                    return;
                } else {
                    //remove constraint
                    m_removedConstraints++;
                    removedCount++;
                    removedRows[index] = 1;
//                    m_parent->removeConstraint(index);
//                    lastRow = m_parent->getRowNonzeros()->end();
                    continue;
                }
            }

            //handle singleton rows
            if(*currentRow == 1 && removedRows[index] == 0) {
                const Constraint& curConstraint = (*m_parent->getConstraints())[index];

                //get the singleton position with skipping the removed column indices
                SparseVector::NonzeroIterator it = curConstraint.getVector()->beginNonzero();
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
                if(((*it) > 0 && (varLowerBound * (*it) - rowUpperBound) > feasibilityTolerance) ||
                   ((*it) > 0 && (varUpperBound * (*it) - rowLowerBound) < -feasibilityTolerance) ||
                   ((*it) < 0 && (varLowerBound * (*it) - rowLowerBound) < -feasibilityTolerance) ||
                   ((*it) < 0 && (varUpperBound * (*it) - rowUpperBound) > feasibilityTolerance)) {

                    //return with infeasibility error
                    throw Presolver::PresolverException("The problem is primal infeasible.");
                    return;
                }

                //fix variable if bounds are equal
                Numerical::Double fixVal;
                int upper;
                if((curConstraint.getType() == Constraint::EQUALITY && (upper = 1)) ||
                   ((*it) > 0 && (varLowerBound * (*it) - rowUpperBound) >= 0 && (upper = 1)) ||
                   ((*it) > 0 && (varUpperBound * (*it) - rowLowerBound) <= 0 && (upper = 2)) ||
                   ((*it) < 0 && (varLowerBound * (*it) - rowLowerBound) <= 0 && (upper = 2)) ||
                   ((*it) < 0 && (varUpperBound * (*it) - rowUpperBound) >= 0 && (upper = 1))) {

                    if(upper == 1) {
                        fixVal = rowUpperBound / (*it);
                    } else {
                        fixVal = rowLowerBound / (*it);
                    }
                    //remove constraint, fix variable and break
                    m_removedConstraints++;
                    removedCount++;
                    removedRows[index] = 1;
//                    m_parent->removeConstraint(index);
//                    lastRow = m_parent->getRowNonzeros()->end();

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
                    removedCount++;
                    removedRows[index] = 1;
//                    m_parent->removeConstraint(index);
                    m_parent->getColumnNonzeros()->set(varIdx, m_parent->getColumnNonzeros()->at(varIdx) - 1);
//                    lastRow = m_parent->getRowNonzeros()->end();
                    continue;
                }
            }
            ++currentRow;
            index++;
        }
    } while (eliminatedVariableCount);

    m_parent->removeConstraints(removedRows, removedCount);

}

SingletonColumnsModule::SingletonColumnsModule(Presolver *parent) :
    PresolverModule(parent)
{
    m_name = "Singleton columns module";
}

SingletonColumnsModule::~SingletonColumnsModule() { }

void SingletonColumnsModule::executeMethod() {
    int eliminatedConstraintCount;

    for(unsigned i = 0; i < m_parent->getModel()->variableCount();i++) {
         m_parent->getColumnNonzeros()->set(i, (*m_parent->getVariables())[i].getVector()->nonZeros());
    }

    //If no constraint was eliminated from the model, no empty or singleton row is created,
    //otherwise we need to check the rows again
    do {
        eliminatedConstraintCount = 0;

        DenseVector::Iterator currentColumn = m_parent->getColumnNonzeros()->begin();
        DenseVector::Iterator lastColumn = m_parent->getColumnNonzeros()->end();
        int index = 0;

        //Check every column of the model for fixed variables, empty or singleton columns
        while(currentColumn < lastColumn) {

            const Variable& curVariable = (*m_parent->getVariables())[index];
            const Numerical::Double& varLowerBound = curVariable.getLowerBound();
            const Numerical::Double& varUpperBound = curVariable.getUpperBound();
            //handle fixed variables
            if(varLowerBound - varUpperBound >= 0) {
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
                SparseVector::NonzeroIterator it = curVariable.getVector()->beginNonzero();
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
                        if(costCoeff == 0) {
                            if(rowLowerBound == -Numerical::Infinity) {
                                if(rowUpperBound == Numerical::Infinity) {
                                    fixBound = 0;
                                } else {
                                    fixBound = rowUpperBound;
                                }
                            } else {
                                fixBound = rowLowerBound;
                            }
                        } else {
                            if(costCoeff > 0) {
                                fixBound = rowLowerBound;
                            } else {
                                fixBound = rowUpperBound;
                            }
                        }

                        //detect unboundedness
                        if(fixBound == Numerical::Infinity || fixBound == -Numerical::Infinity) {
                            throw Presolver::PresolverException("The problem is primal unbounded.");
                            return;
                        }
                    } else {

                        //get the constraint bound to fix the variable
                        if(costCoeff == 0) {
                            if(rowLowerBound == -Numerical::Infinity) {
                                if(rowUpperBound == Numerical::Infinity) {
                                    fixBound = 0;
                                } else {
                                    fixBound = rowUpperBound;
                                }
                            } else {
                                fixBound = rowLowerBound;
                            }
                        } else {
                            if(costCoeff > 0) {
                                fixBound = rowUpperBound;
                            } else {
                                fixBound = rowLowerBound;
                            }
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
                    DenseVector * substituteVector = new DenseVector(varCount + 3);
                    SparseVector::NonzeroIterator constraintIt = curConstraint.getVector()->beginNonzero();
                    SparseVector::NonzeroIterator constraintItEnd = curConstraint.getVector()->endNonzero();
                    for(; constraintIt < constraintItEnd; ++constraintIt) {
                        int varIndex = constraintIt.getIndex();
                        substituteVector->set(varIndex, (-1) * (*constraintIt) / (*it));
                        m_parent->getColumnNonzeros()->set(varIndex, m_parent->getColumnNonzeros()->at(varIndex) - 1);
                        m_parent->getModel()->addToCostCoefficient(varIndex, -1 * ((*constraintIt) * costCoeff / (*it)));
                    }
                    m_parent->getModel()->setCostConstant(m_parent->getModel()->getCostConstant() - fixBound * costCoeff / *it);
                    substituteVector->set(index, 0);
                    substituteVector->set(varCount, fixBound / (*it));
                    substituteVector->set(varCount + 1, index);
                    substituteVector->set(varCount + 2, Presolver::FIXED_VARIABLE);
                    m_parent->getSubstituteVectors()->push_back(substituteVector);

                    m_removedConstraints++;
                    eliminatedConstraintCount++;

                    std::vector<int> removeIndices(m_parent->getModel()->constraintCount(), 0);
                    removeIndices[constraintIdx] = 1;
                    m_parent->removeConstraints(removeIndices, 1);

                    m_removedVariables++;
                    m_parent->getModel()->removeVariable(index);
                    m_parent->getColumnNonzeros()->remove(index);
                    m_parent->getImpliedLower()->remove(index);
                    m_parent->getImpliedUpper()->remove(index);
                    m_parent->getExtraDualLowerSum()->remove(index);
                    m_parent->getExtraDualUpperSum()->remove(index);
                    lastColumn = m_parent->getColumnNonzeros()->end();
                    continue;
                }
            }
            ++currentColumn;
            index++;
        }
    } while (eliminatedConstraintCount);
}

ImpliedBoundsModule::ImpliedBoundsModule(Presolver *parent)
{
    m_removedConstraints = 0;
    m_removedVariables = 0;
    m_parent = parent;
    m_name = "Implied bounds module";
}

ImpliedBoundsModule::~ImpliedBoundsModule() { }


//TODO: Az constraintek ellenorzesi modszerenek ujrairasa gyorsabbra
void ImpliedBoundsModule::executeMethod() {
    const Numerical::Double & feasibilityTolerance = m_parent->getFeasibilityTolerance();
    m_impliedLower = m_parent->getImpliedLower();
    m_impliedUpper = m_parent->getImpliedUpper();

    //Initializing
    int rowCount = m_parent->getModel()->constraintCount();
    std::vector<int> removedRows(rowCount, 0);
    int removedCount = 0;

    m_constraintsToCheck = new DenseVector(rowCount);
    m_constraintStack = new DenseVector(rowCount);
    for(int i = 0; i < rowCount; i++) {
        m_constraintsToCheck->set(i, 1);
    }
    DenseVector::NonzeroIterator begin = m_constraintsToCheck->beginNonzero();
    DenseVector::NonzeroIterator end = m_constraintsToCheck->endNonzero();
    Numerical::Double impliedUB, impliedLB;

    //This loop iterates through the constraints of the model. After the first loop through every constraint of the model
    //only the constraints with possibly changed implied bounds are checked.
    int stackSwapCounter = 0;
    while(true) {
        //Swap the stacks containing the constraints to check and control the loop
        if (begin >= end) {
            if(m_constraintStack->nonZeros() != 0 && stackSwapCounter < 10) {
                DenseVector * temp = m_constraintsToCheck;
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

        Numerical::Summarizer impliedLBSummarizer;
        Numerical::Summarizer impliedUBSummarizer;
        SparseVector::NonzeroIterator it = curConstraint.getVector()->beginNonzero();
        SparseVector::NonzeroIterator itEnd = curConstraint.getVector()->endNonzero();
        SparseVector::NonzeroIterator itV = it;
        SparseVector::NonzeroIterator itVEnd = itEnd;

        Numerical::Number number;

        DenseVector* lowers[2];
        DenseVector* uppers[2];
        lowers[0] = m_impliedLower;
        lowers[1] = m_impliedUpper;
        uppers[0] = m_impliedUpper;
        uppers[1] = m_impliedLower;

        //Calculate implied constraint bounds
        for(; it < itEnd; ++it) {
            int curVar = it.getIndex();
#if DOUBLE_TYPE == DOUBLE_CLASSIC
            number.m_num = *it;
            impliedLBSummarizer.add( *it * lowers[number.m_bits >> 63]->at(curVar));
            impliedUBSummarizer.add( *it * uppers[number.m_bits >> 63]->at(curVar));
#else
            int sign = *it > 0.0 ? 1 : 0;
            impliedLBSummarizer.add( *it * lowers[sign]->at(curVar));
            impliedUBSummarizer.add( *it * uppers[sign]->at(curVar));
#endif
        }

        impliedLB = impliedLBSummarizer.getResult();
        impliedUB = impliedUBSummarizer.getResult();

        //Check if bounds conflict
        if(impliedLB - rowUpperBound > feasibilityTolerance ||
           impliedUB - rowLowerBound < -feasibilityTolerance) {
            throw Presolver::PresolverException("The problem is primal infeasible.");
            return;
        }

        //Check if constraint is redundant
        if(impliedLB >= rowLowerBound &&
           impliedUB <= rowUpperBound ) {
            bool finiteLower = rowLowerBound > -Numerical::Infinity;
            bool finiteUpper = rowUpperBound < Numerical::Infinity;
            it = curConstraint.getVector()->beginNonzero();
            if(finiteLower && finiteUpper){
                while(it < itEnd) {
                    int varIdx = it.getIndex();
                    (*m_parent->getVariables())[varIdx].setLowerBound(m_impliedLower->at(varIdx));
                    (*m_parent->getVariables())[varIdx].setUpperBound(m_impliedUpper->at(varIdx));
                    ++it;
                }
            } else if(finiteLower){
                while(it < itEnd) {
                    int varIdx = it.getIndex();
                    if(*it > 0) {
                        (*m_parent->getVariables())[varIdx].setLowerBound(m_impliedLower->at(varIdx));
                    } else {
                        (*m_parent->getVariables())[varIdx].setUpperBound(m_impliedUpper->at(varIdx));
                    }
                    ++it;
                }
            } else if(finiteUpper){
                while(it < itEnd) {
                    int varIdx = it.getIndex();
                    if(*it < 0) {
                        (*m_parent->getVariables())[varIdx].setLowerBound(m_impliedLower->at(varIdx));
                    } else {
                        (*m_parent->getVariables())[varIdx].setUpperBound(m_impliedUpper->at(varIdx));
                    }
                    ++it;
                }
            }
            removedRows[index] = 1;
            for(auto it = curConstraint.getVector()->beginNonzero(), itEnd = curConstraint.getVector()->endNonzero(); it != itEnd; ++it) {
                m_parent->getColumnNonzeros()->set(it.getIndex(), m_parent->getColumnNonzeros()->at(it.getIndex()) - 1);
            }
            m_constraintStack->set(index, 0);
            removedCount++;
            ++begin;
            m_constraintsToCheck->set(index, 0);
//            m_parent->removeConstraint(index);
//            m_constraintsToCheck->removeElement(index);
//            m_constraintStack->removeElement(index);
            end = m_constraintsToCheck->endNonzero();
            if(*begin != 1) { ++begin; }
            m_removedConstraints++;
            continue;
        }


        //Fix all participating variables to their bounds if constaint is forcing.
        //Constraint can be removed after.
        if(impliedLB >= rowUpperBound) {
            it = curConstraint.getVector()->beginNonzero();
            while(it < itEnd) {
                int varIdx = it.getIndex();
                const Variable& curVariable = (*m_parent->getVariables())[varIdx];
                itV = curVariable.getVector()->beginNonzero();
                itVEnd = curVariable.getVector()->endNonzero();
                for(; itV < itVEnd; ++itV) {
                    if(removedRows[itV.getIndex()] == 0)
                    {
                        m_constraintStack->set(itV.getIndex(), 1);
                    }
                }
                if(*it > 0) {
                    m_parent->fixVariable(varIdx, m_impliedLower->at(varIdx));
                } else {
                    m_parent->fixVariable(varIdx, m_impliedUpper->at(varIdx));
                }
                itEnd = curConstraint.getVector()->endNonzero();
                m_removedVariables++;
                if(*it == 0.0) { ++it; }
            }
            removedRows[index] = 1;
            m_constraintStack->set(index, 0);
            removedCount++;
            ++begin;
            m_constraintsToCheck->set(index, 0);
//            m_parent->removeConstraint(index);
//            m_constraintsToCheck->removeElement(index);
//            m_constraintStack->removeElement(index);
            m_removedConstraints++;
            end = m_constraintsToCheck->endNonzero();
            if(*begin != 1) { ++begin; }
            continue;
        }
        if(impliedUB <= rowLowerBound) {
            it = curConstraint.getVector()->beginNonzero();
            while(it < itEnd) {
                int varIdx = it.getIndex();
                const Variable& curVariable = (*m_parent->getVariables())[varIdx];
                itV = curVariable.getVector()->beginNonzero();
                itVEnd = curVariable.getVector()->endNonzero();
                for(; itV < itVEnd; ++itV) {
                    if(removedRows[itV.getIndex()] == 0)
                    {
                        m_constraintStack->set(itV.getIndex(), 1);
                    }
                }
                if(*it > 0) {
                    m_parent->fixVariable(varIdx, m_impliedUpper->at(varIdx));
                } else {
                    m_parent->fixVariable(varIdx, m_impliedLower->at(varIdx));
                }
                itEnd = curConstraint.getVector()->endNonzero();
                m_removedVariables++;
                if(*it == 0.0) { ++it; }
            }
            removedRows[index] = 1;
            m_constraintStack->set(index, 0);
            removedCount++;
            ++begin;
            m_constraintsToCheck->set(index, 0);
//            m_parent->removeConstraint(index);
//            m_constraintsToCheck->removeElement(index);
//            m_constraintStack->removeElement(index);
            m_removedConstraints++;
            end = m_constraintsToCheck->endNonzero();
            if(*begin != 1) { ++begin; }
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
        //Make every comparsion based on implied bounds.
        if(impliedLB > -Numerical::Infinity && rowUpperBound < Numerical::Infinity) {
            it = curConstraint.getVector()->beginNonzero();
            itEnd = curConstraint.getVector()->endNonzero();
            for(; it < itEnd; ++it) {
                int varIdx = it.getIndex();
                const Variable& curVariable = (*m_parent->getVariables())[varIdx];
                if(*it > 0) {
                    Numerical::Double newUpperBound = Numerical::stableAdd((rowUpperBound - impliedLB) / *it, m_impliedLower->at(varIdx));
                    if( newUpperBound < m_impliedUpper->at(varIdx)) {
                        m_impliedUpper->set(varIdx, newUpperBound);
                        itV = curVariable.getVector()->beginNonzero();
                        itVEnd = curVariable.getVector()->endNonzero();
                        for(; itV < itVEnd; ++itV) {
                            if(removedRows[itV.getIndex()] == 0)
                            {
                                m_constraintStack->set(itV.getIndex(), 1);
                            }
                        }
                    }

                } else {
                    Numerical::Double newLowerBound = Numerical::stableAdd((rowUpperBound - impliedLB) / *it,  m_impliedUpper->at(varIdx));
                    if(newLowerBound > m_impliedLower->at(varIdx)) {
                       m_impliedLower->set(varIdx, newLowerBound);
                       itV = curVariable.getVector()->beginNonzero();
                       itVEnd = curVariable.getVector()->endNonzero();
                       for(; itV < itVEnd; ++itV) {
                           if(removedRows[itV.getIndex()] == 0)
                           {
                               m_constraintStack->set(itV.getIndex(), 1);
                           }
                       }
                    }
                }
            }
        }

        if(impliedUB < Numerical::Infinity && rowLowerBound > -Numerical::Infinity) {
            it = curConstraint.getVector()->beginNonzero();
            itEnd = curConstraint.getVector()->endNonzero();
            for(; it < itEnd; ++it) {
                int varIdx = it.getIndex();
                const Variable& curVariable = (*m_parent->getVariables())[varIdx];
                if(*it > 0) {
                    Numerical::Double newLowerBound = Numerical::stableAdd((rowLowerBound - impliedUB) / *it, m_impliedUpper->at(varIdx));
                    if(newLowerBound > m_impliedLower->at(varIdx)) {
                       m_impliedLower->set(varIdx, newLowerBound);
                       itV = curVariable.getVector()->beginNonzero();
                       itVEnd = curVariable.getVector()->endNonzero();
                       for(; itV < itVEnd; ++itV) {
                           if(removedRows[itV.getIndex()] == 0)
                           {
                               m_constraintStack->set(itV.getIndex(), 1);
                           }
                       }
                    }
                } else {
                    Numerical::Double newUpperBound = Numerical::stableAdd((rowLowerBound - impliedUB) / *it, m_impliedLower->at(varIdx));
                    if(newUpperBound < m_impliedUpper->at(varIdx)) {
                       m_impliedUpper->set(varIdx, newUpperBound);
                       itV = curVariable.getVector()->beginNonzero();
                       itVEnd = curVariable.getVector()->endNonzero();
                       for(; itV < itVEnd; ++itV) {
                           if(removedRows[itV.getIndex()] == 0)
                           {
                               m_constraintStack->set(itV.getIndex(), 1);
                           }
                       }
                    }
                }
            }
        }


        m_constraintsToCheck->set(index, 0);
        ++begin;
    }
    m_parent->removeConstraints(removedRows, removedCount);

}

DualBoundsModule::DualBoundsModule(Presolver *parent) :
    PresolverModule(parent)
{
    m_name = "Dual bounds module";
}

DualBoundsModule::~DualBoundsModule() { }

void DualBoundsModule::executeMethod() {
    m_impliedDualLower = m_parent->getImpliedDualLower();
    m_impliedDualUpper = m_parent->getImpliedDualUpper();
    m_extraDualLowerSum = m_parent->getExtraDualLowerSum();
    m_extraDualUpperSum = m_parent->getExtraDualUpperSum();
    int columnCount = m_parent->getModel()->variableCount();
    m_variablesToCheck = new DenseVector(columnCount);
    for(int i = 0; i < columnCount; i++) {
        m_variablesToCheck->set(i, 1);
    }
    m_variableStack = new DenseVector(columnCount);
    DenseVector::NonzeroIterator begin = m_variablesToCheck->beginNonzero();
    DenseVector::NonzeroIterator end = m_variablesToCheck->endNonzero();
    Numerical::Double impliedUB, impliedLB;

    //This loop iterates through the variables of the model. After the first loop through every variable of the model
    //only the variables with possibly changed implied bounds are checked.
    int stackSwapCounter = 0;
    while(true) {

        //Swap the stacks containing the variables to check and control the loop
        if (begin >= end) {
            if(m_variableStack->nonZeros() != 0 && stackSwapCounter < 10) {
                DenseVector * temp = m_variablesToCheck;
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

        SparseVector::NonzeroIterator it = curVariable.getVector()->beginNonzero();
        SparseVector::NonzeroIterator itEnd = curVariable.getVector()->endNonzero();
        SparseVector::NonzeroIterator itC = it;
        SparseVector::NonzeroIterator itCEnd = itEnd;
        //Calculating the implied dual constraint bounds
        for(; it < itEnd; ++it) {
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

        const DenseVector& costVector = m_parent->getModel()->getCostVector();

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
                    m_variablesToCheck->remove(index);
                    m_variableStack->remove(index);
                    end = m_variablesToCheck->endNonzero();
                    if(*begin != 1) { ++begin; }
                    m_removedVariables++;
                    continue;
                }
            }

            //If variable is neither conflicted nor dominated, the bounds of dual variables corresponding to the rows
            //this variable participates in can be tightened.
            if(impliedLB < costVector.at(index) && impliedUB > costVector.at(index) && impliedLB != -Numerical::Infinity) {
                it = curVariable.getVector()->beginNonzero();
                itEnd = curVariable.getVector()->endNonzero();
                for(; it < itEnd; ++it) {
                    int conIdx = it.getIndex();
                    const Constraint& curConstraint = (*m_parent->getConstraints())[conIdx];
                    if(*it > 0) {
                        if(Numerical::stableAdd(Numerical::stableAdd((costVector.at(index) - impliedLB) / *it, m_impliedDualLower->at(conIdx)), -1 * m_impliedDualUpper->at(conIdx)) < 0) {
                            m_impliedDualUpper->set(conIdx, Numerical::stableAdd((costVector.at(index) - impliedLB) / *it, m_impliedDualLower->at(conIdx)));
                               itC = curConstraint.getVector()->beginNonzero();
                               itCEnd = curConstraint.getVector()->endNonzero();
                               for(; itC < itCEnd; ++itC) {
                                   m_variableStack->set(itC.getIndex(), 1);
                               }
                        }
                    } else {
                        if(Numerical::stableAdd(m_impliedDualLower->at(conIdx), -1 * Numerical::stableAdd((costVector.at(index) - impliedLB) / *it,  m_impliedDualUpper->at(conIdx))) < 0) {
                           m_impliedDualLower->set(conIdx, Numerical::stableAdd((costVector.at(index) - impliedLB) / *it,  m_impliedDualUpper->at(conIdx)));
                           itC = curConstraint.getVector()->beginNonzero();
                           itCEnd = curConstraint.getVector()->endNonzero();
                           for(; itC < itCEnd; ++itC) {
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
                    m_variablesToCheck->remove(index);
                    m_variableStack->remove(index);
                    end = m_variablesToCheck->endNonzero();
                    if(*begin != 1) { ++begin; }
                    m_removedVariables++;
                    continue;
                }
            }

            //If variable is neither conflicted nor dominated, the bounds of dual variables corresponding to the rows
            //this variable participates in can be tightened.
            if(impliedLB < costVector.at(index) && impliedUB > costVector.at(index) && impliedUB != -Numerical::Infinity) {
                it = curVariable.getVector()->beginNonzero();
                itEnd = curVariable.getVector()->endNonzero();
                for(; it < itEnd; ++it) {
                    int conIdx = it.getIndex();
                    const Constraint& curConstraint = (*m_parent->getConstraints())[conIdx];
                    if(*it > 0) {
                        if(Numerical::stableAdd(m_impliedDualLower->at(conIdx), -1 * Numerical::stableAdd((costVector.at(index) - impliedUB) / *it,  m_impliedDualUpper->at(conIdx))) < 0) {
                            m_impliedDualLower->set(conIdx, Numerical::stableAdd((costVector.at(index) - impliedUB) / *it, m_impliedDualUpper->at(conIdx)));
                               itC = curConstraint.getVector()->beginNonzero();
                               itCEnd = curConstraint.getVector()->endNonzero();
                               for(; itC < itCEnd; ++itC) {
                                   m_variableStack->set(itC.getIndex(), 1);
                               }
                        }
                    } else {
                        if(Numerical::stableAdd(Numerical::stableAdd((costVector.at(index) - impliedUB) / *it, m_impliedDualLower->at(conIdx)), -1 * m_impliedDualUpper->at(conIdx)) < 0) {
                           m_impliedDualUpper->set(conIdx, Numerical::stableAdd((costVector.at(index) - impliedUB) / *it,  m_impliedDualLower->at(conIdx)));
                           itC = curConstraint.getVector()->beginNonzero();
                           itCEnd = curConstraint.getVector()->endNonzero();
                           for(; itC < itCEnd; ++itC) {
                               m_variableStack->set(itC.getIndex(), 1);
                           }
                        }
                    }
                }
            }
        }

        m_variablesToCheck->set(index, 0);
        ++begin;
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
    int removeCount = 0;

    //Index list to distuinguish rows with the same hashes
    IndexList<> hashedRows(rowCount, rowCount);

    std::vector<int> usedPartitions(rowCount,0);
    std::vector<int> indices(rowCount,0);
    for(int i = 0; i < rowCount; i++) {
        unsigned int vhash = 0;
        const SparseVector* curRowVector = (*m_parent->getConstraints())[i].getVector();

        if(curRowVector->nonZeros() < 1) continue;

        //Calculating hash
        SparseVector::NonzeroIterator begin = curRowVector->beginNonzero();
        SparseVector::NonzeroIterator end = curRowVector->endNonzero();
        while(begin != end) {
            vhash += begin.getIndex()+1;
            ++begin;
        }
        int partition = vhash % rowCount;
        ++(usedPartitions[partition]);
        hashedRows.insert(partition, i);
    }
    //Eliminating duplicate rows

    for(auto begin = usedPartitions.begin(),
        current = usedPartitions.begin(),
        end = usedPartitions.end() ;current != end; ++current) {
        if(*current < 2) {
            continue;
        }

        IndexList<>::PartitionIterator itSecond, itFirst, itEnd;
        hashedRows.getIterators(&itFirst, &itEnd, current-begin);

        while(itFirst != itEnd) {
            itSecond = itFirst;
            ++itSecond;

            while(itSecond != itEnd) {

                int origIdx = itFirst.getData();
                int duplIdx = itSecond.getData();
                Constraint& original = (*m_parent->getConstraints())[origIdx];
                const Constraint& duplicate = (*m_parent->getConstraints())[duplIdx];

                Numerical::Double lambda = *original.getVector()->beginNonzero() / *duplicate.getVector()->beginNonzero();
                SparseVector tryVector(*(original.getVector()));
                tryVector.addVector(-lambda, *(duplicate.getVector()));

                if(tryVector.nonZeros() == 0) {
                    Numerical::Double duplicateLower;
                    Numerical::Double duplicateUpper;

                    //With negative lambda, duplicate bounds are swapped
                    if(lambda > 0) {
                        duplicateLower = duplicate.getLowerBound() * lambda;
                        duplicateUpper = duplicate.getUpperBound() * lambda;
                    } else {
                        duplicateUpper = duplicate.getLowerBound() * lambda;
                        duplicateLower = duplicate.getUpperBound() * lambda;
                    }

                    //Checking feasibility
                    if(duplicateLower > original.getUpperBound()
                       || duplicateUpper < original.getLowerBound())
                    {
                        throw Presolver::PresolverException("The problem is primal infeasible.");
                        return;
                    }

                    //Applying the tighter bounds
                    if(duplicateLower > original.getLowerBound()) {
                        original.setLowerBound(duplicateLower);
                    }

                    if(duplicateUpper < original.getUpperBound()) {
                        original.setUpperBound(duplicateUpper);
                    }

                    ++itSecond;
                    indices[duplIdx] = 1;
                    removeCount++;
                    for(auto it = duplicate.getVector()->beginNonzero(), itEnd = duplicate.getVector()->endNonzero(); it != itEnd; ++it) {
                        m_parent->getColumnNonzeros()->set(it.getIndex(), m_parent->getColumnNonzeros()->at(it.getIndex()) - 1);
                    }
                    hashedRows.remove(duplIdx);
                } else {
                    ++itSecond;
                }
            }
            ++itFirst;
        }
    }

    //The elimination of constraints needs to start at the row with the highest index
    //Reverse iterators are used to resolve index conflicts

    m_parent->removeConstraints(indices, removeCount);
    m_removedConstraints += removeCount;

//    for(int i=indices.size()-1 ; i>0; --i) {
//        if(indices[i] == 0){
//            continue;
//        }
//        m_removedConstraints++;
//        m_parent->removeConstraint(i);
//    }

    rowCount = m_parent->getModel()->constraintCount();
    removeCount = 0;

    //Index list to distuinguish columns with the same hashes
    IndexList<> hashedColumns(columnCount, columnCount);

    usedPartitions.resize(columnCount,0);
//    indices.resize(columnCount);

//    for(int i = 0; i < columnCount; i++) {
//        indices[i] = 0;
//    }
    std::vector<Numerical::Double> vIndices(columnCount, 0.0);

    for(int i = 0; i < columnCount; i++) {
        int vhash = 0;
        const SparseVector* curColumnVector = (*m_parent->getVariables())[i].getVector();
        if(curColumnVector->nonZeros() < 1) continue;

        //Calculating hash
        SparseVector::NonzeroIterator begin = curColumnVector->beginNonzero();
        SparseVector::NonzeroIterator end = curColumnVector->endNonzero();
        while(begin != end) {
            vhash += begin.getIndex()+1;
            ++begin;
        }
        int partition = vhash % columnCount;
        ++(usedPartitions[partition]);
        hashedColumns.insert(partition, i);
    }

    //Eliminating duplicate columns

    for(auto begin = usedPartitions.begin(),
        current = usedPartitions.begin(),
        end = usedPartitions.end() ;current != end; ++current) {
        if(*current < 2) {
            continue;
        }

        IndexList<>::PartitionIterator itSecond, itFirst, itEnd;
        hashedColumns.getIterators(&itFirst, &itEnd, current-begin);

        while(itFirst != itEnd) {
            itSecond = itFirst;
            ++itSecond;

            while(itSecond != itEnd) {

                int origIdx = itFirst.getData();
                int duplIdx = itSecond.getData();
                Variable& original = (*m_parent->getVariables())[origIdx];
                const Variable& duplicate = (*m_parent->getVariables())[duplIdx];

                Numerical::Double lambda = *original.getVector()->beginNonzero() / *duplicate.getVector()->beginNonzero();
                SparseVector tryVector(*(original.getVector()));
                tryVector.addVector(-lambda, *(duplicate.getVector()));

                if(tryVector.nonZeros() == 0 && m_parent->getModel()->getCostVector().at(duplIdx) * lambda == m_parent->getModel()->getCostVector().at(origIdx)) {

                    Numerical::Double duplicateLower;
                    Numerical::Double duplicateUpper;

                    //With negative lambda, duplicate bounds are swapped
                    if(lambda > 0) {
                        duplicateLower = duplicate.getLowerBound() * (1/lambda);
                        duplicateUpper = duplicate.getUpperBound() * (1/lambda);
                    } else {
                        duplicateUpper = duplicate.getLowerBound() * (1/lambda);
                        duplicateLower = duplicate.getUpperBound() * (1/lambda);
                    }

                    //Checking feasibility
                    if(duplicateLower + original.getLowerBound()
                       > duplicateUpper + original.getUpperBound())
                    {
                        throw Presolver::PresolverException("The problem is primal infeasible.");
                        return;
                    }

                    //Creating bounds
                    original.setBounds(duplicateLower + original.getLowerBound(),
                                       duplicateUpper + original.getUpperBound());
                    m_parent->getImpliedLower()->set(origIdx, duplicateLower + original.getLowerBound());
                    m_parent->getImpliedUpper()->set(origIdx, duplicateUpper + original.getUpperBound());

                    ++itSecond;
                    vIndices[duplIdx] = Numerical::Infinity;
                    removeCount++;
                    for(auto it = duplicate.getVector()->beginNonzero(), itEnd = duplicate.getVector()->endNonzero(); it != itEnd; ++it) {
                        m_parent->getRowNonzeros()->set(it.getIndex(), m_parent->getRowNonzeros()->at(it.getIndex()) - 1);
                    }
                    hashedColumns.remove(duplIdx);
                } else {
                    ++itSecond;
                }
            }
            ++itFirst;
        }
    }

    //The elimination of variables needs to start at the row with the highest index
    //A special type of substitute vector is created to be able to regain correct values for the original variables

    m_parent->fixVariables(vIndices, removeCount);

//    for(int i=vIndices.size()-1 ; i>0; --i) {
//        if(vIndices[i] == 0.0){
//            continue;
//        }
//        Vector * substituteVector = new Vector(columnCount + 3);
////        substituteVector->set(columnCount - 1, m_parent->getModel()->getCostVector().at(i) / m_parent->getModel()->getCostVector().at((*begin) - 1));
////        substituteVector->set(columnCount, *begin);
//        substituteVector->set(columnCount + 1, i);
//        substituteVector->set(columnCount + 2, Presolver::DUPLICATE_VARIABLE);
//        m_parent->getSubstituteVectors()->push_back(substituteVector);
//        m_removedVariables++;
//        m_parent->getModel()->removeVariable(i);
//        m_parent->getColumnNonzeros()->removeElement(i);
//        m_parent->getImpliedLower()->removeElement(i);
//        m_parent->getImpliedUpper()->removeElement(i);
//        m_parent->getExtraDualLowerSum()->removeElement(i);
//        m_parent->getExtraDualUpperSum()->removeElement(i);
//    }

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
    int columnCount = m_parent->getModel()->variableCount();

    //Index list to sort the rows by nonzero count
    IndexList<> sortedRows(rowCount, columnCount+1);
    SparseVector usedPartitions(rowCount);

    //Set up the sorted list
    for(int i = 0; i < rowCount; i++) {
        int nonzeros = m_parent->getConstraints()->at(i).getVector()->nonZeros();
        if(m_parent->getConstraints()->at(i).getType() == Constraint::EQUALITY)
        {
            sortedRows.insert(nonzeros, i);
            usedPartitions.set(nonzeros, usedPartitions.at(nonzeros) + 1);
        }
    }
    IndexList<>::PartitionIterator it, itEnd;

    SparseVector::NonzeroIterator begin = usedPartitions.beginNonzero();
    SparseVector::NonzeroIterator end = usedPartitions.endNonzero();
    if(begin.getIndex() == 0) ++begin;
    while(begin < end) {
        sortedRows.getIterators(&it, &itEnd, begin.getIndex());
        while(it != itEnd) {
            int index = it.getData();
            const Constraint& pivotRow = m_parent->getConstraints()->at(index);
            SparseVector::NonzeroIterator pivotIt = pivotRow.getVector()->beginNonzero();
            SparseVector::NonzeroIterator pivotItEnd = pivotRow.getVector()->endNonzero();
            int columnIdx = pivotIt.getIndex();
            int pivotNonzeros = pivotRow.getVector()->nonZeros();
            if(pivotNonzeros == 0) {
                ++it;
                continue;
            }
            SparseVector::NonzeroIterator beginColumn = m_parent->getVariables()->at(columnIdx).getVector()->beginNonzero();
            SparseVector::NonzeroIterator endColumn = m_parent->getVariables()->at(columnIdx).getVector()->endNonzero();
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
                        ++pivotIt;
                    }
                    if(pivotIt == pivotItEnd) {
                        pivotIt = pivotRow.getVector()->beginNonzero();
                        Numerical::Double lambda = *beginColumn / *pivotIt;

                        m_parent->getModel()->addToConstraint(secondIndex, index, -lambda);

                        Numerical::Double newLowerBound;
                        Numerical::Double newUpperBound;
                        if(lambda > 0) {
                            newLowerBound = curRow.getLowerBound() - lambda * pivotRow.getLowerBound();
                            newUpperBound = curRow.getUpperBound() - lambda * pivotRow.getUpperBound();
                        } else {
                            newLowerBound = curRow.getLowerBound() - lambda * pivotRow.getUpperBound();
                            newUpperBound = curRow.getUpperBound() - lambda * pivotRow.getLowerBound();
                        }

                        curRow.setBounds(newLowerBound, newUpperBound);
                        if(nonzeros - (int)curRow.getVector()->nonZeros() < 0) LPERROR("Make sparser module error: vector substraction created extra " << (nonzeros - (int)curRow.getVector()->nonZeros()) << " nonzeros");
                        int curNzrDiff = nonzeros - (int)curRow.getVector()->nonZeros();
                        m_parent->getRowNonzeros()->set(secondIndex, m_parent->getRowNonzeros()->at(secondIndex) - curNzrDiff);
                        m_removedNzr += curNzrDiff;
                    } else {
                        pivotIt = pivotRow.getVector()->beginNonzero();
                    }
                }
                ++beginColumn;
            }
            ++it;
        }
        ++begin;
    }
}
