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
                if(m_parent->getConstraints()->at(currentRow.getIndex()).getLowerBound() > 0 || m_parent->getConstraints()->at(currentRow.getIndex()).getUpperBound() < 0) {
                    throw Presolver::PresolverException("The problem is primal infeasible.");
                    return;
                } else {
                    //remove constraint
                    m_removedConstraints++;
                    m_parent->getModel()->removeConstraint(currentRow.getIndex());
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
                Numerical::Double varLowerBound = m_parent->getVariables()->at(varIdx).getLowerBound();
                Numerical::Double varUpperBound = m_parent->getVariables()->at(varIdx).getUpperBound();
                Numerical::Double rowLowerBound = m_parent->getConstraints()->at(currentRow.getIndex()).getLowerBound();
                Numerical::Double rowUpperBound = m_parent->getConstraints()->at(currentRow.getIndex()).getUpperBound();

                //report infeasibility if bounds conflict
                //example for the 4 cases here:
                // -4 <=  2*x <= 4;   3 <= x <= 8
                // -4 <=  2*x <= 4;  -7 <= x <= -2.5
                // -4 <= -2*x <= 4;   3 <= x <= 8
                // -4 <= -2*x <= 4;  -7 <= x <= -2.5
                if(((*it) > 0 && varLowerBound * (*it) > rowUpperBound) ||
                   ((*it) > 0 && varUpperBound * (*it) < rowLowerBound) ||
                   ((*it) < 0 && varLowerBound * (*it) < rowLowerBound) ||
                   ((*it) < 0 && varUpperBound * (*it) > rowUpperBound)) {

                    //return with infeasibility error
                    throw Presolver::PresolverException("The problem is primal infeasible.");
                    return;
                }

                //fix variable if bounds are equal
                //TODO: Numerical::Double hasznalata eseten nem fordul le ambiguous conversion miatt (DoubleHistory -> bool)
//                Numerical::Double fixVal;
                double fixVal;
                if((m_parent->getConstraints()->at(currentRow.getIndex()).getType() == Constraint::EQUALITY && (fixVal = 1)) ||
                   ((*it) > 0 && varLowerBound * (*it) == rowUpperBound && (fixVal = 1)) ||
                   ((*it) > 0 && varUpperBound * (*it) == rowLowerBound && (fixVal = 2)) ||
                   ((*it) < 0 && varLowerBound * (*it) == rowLowerBound && (fixVal = 2)) ||
                   ((*it) < 0 && varUpperBound * (*it) == rowUpperBound && (fixVal = 1))) {

                    if(fixVal == 1) {
                        fixVal = rowUpperBound / (*it);
                    } else {
                        fixVal = rowLowerBound / (*it);
                    }
                    //remove constraint, fix variable and break
                    m_removedConstraints++;
                    m_parent->getModel()->removeConstraint(currentRow.getIndex());
                    m_parent->getRowNonzeros()->removeElement(currentRow.getIndex());
                    lastRow = m_parent->getRowNonzeros()->end();

                    m_parent->fixVariable(varIdx, fixVal);
                    eliminatedVariableCount++;
                    m_removedVariables++;
                    continue;
                } else {
                    //update variable bounds
                    if((*it) > 0) {
                        if(varLowerBound < rowLowerBound / (*it)) {
                            m_parent->getVariables()->at(varIdx).setLowerBound(rowLowerBound / (*it));
                        }
                        if(varUpperBound > rowUpperBound / (*it)) {
                            m_parent->getVariables()->at(varIdx).setUpperBound(rowUpperBound / (*it));
                        }
                    } else {
                        if(varLowerBound < rowUpperBound / (*it)) {
                            m_parent->getVariables()->at(varIdx).setLowerBound(rowUpperBound / (*it));
                        }
                        if(varUpperBound > rowLowerBound / (*it)) {
                            m_parent->getVariables()->at(varIdx).setUpperBound(rowLowerBound / (*it));
                        }
                    }

                    //remove constraint
                    m_removedConstraints++;
                    m_parent->getModel()->removeConstraint(currentRow.getIndex());
                    m_parent->getRowNonzeros()->removeElement(currentRow.getIndex());
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
            if(m_parent->getVariables()->at(currentColumn.getIndex()).getLowerBound() == m_parent->getVariables()->at(currentColumn.getIndex()).getUpperBound()) {

                m_parent->fixVariable(currentColumn.getIndex(), m_parent->getVariables()->at(currentColumn.getIndex()).getLowerBound());
                lastColumn = m_parent->getColumnNonzeros()->end();
                m_removedVariables++;
                continue;
            }

            //handle empty columns
            if(*currentColumn == 0) {
                if(m_parent->getModel()->getObjectiveType() == MINIMIZE) {
                    if((m_parent->getModel()->getVariables()->at(currentColumn.getIndex()).getLowerBound() == -Numerical::Infinity && m_parent->getModel()->getCostVector().at(currentColumn.getIndex()) > 0) ||
                       (m_parent->getModel()->getVariables()->at(currentColumn.getIndex()).getUpperBound() == Numerical::Infinity && m_parent->getModel()->getCostVector().at(currentColumn.getIndex()) < 0)) {
                        throw Presolver::PresolverException("The problem is primal unbounded.");
                        return;
                    } else {
                        if(m_parent->getModel()->getCostVector().at(currentColumn.getIndex()) > 0) {
                            m_parent->fixVariable(currentColumn.getIndex(), m_parent->getVariables()->at(currentColumn.getIndex()).getLowerBound());
                        } else {
                            m_parent->fixVariable(currentColumn.getIndex(), m_parent->getVariables()->at(currentColumn.getIndex()).getUpperBound());
                        }
                        lastColumn = m_parent->getColumnNonzeros()->end();
                        m_removedVariables++;
                        continue;
                    }
                } else {
                    if((m_parent->getModel()->getVariables()->at(currentColumn.getIndex()).getLowerBound() == -Numerical::Infinity && m_parent->getModel()->getCostVector().at(currentColumn.getIndex()) < 0) ||
                       (m_parent->getModel()->getVariables()->at(currentColumn.getIndex()).getUpperBound() == Numerical::Infinity && m_parent->getModel()->getCostVector().at(currentColumn.getIndex()) > 0)) {
                        throw Presolver::PresolverException("The problem is primal unbounded.");
                        return;
                    } else {
                        if(m_parent->getModel()->getCostVector().at(currentColumn.getIndex()) < 0) {
                            m_parent->fixVariable(currentColumn.getIndex(), m_parent->getVariables()->at(currentColumn.getIndex()).getLowerBound());
                        } else {
                            m_parent->fixVariable(currentColumn.getIndex(), m_parent->getVariables()->at(currentColumn.getIndex()).getUpperBound());
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
                Vector::NonzeroIterator it = m_parent->getVariables()->at(currentColumn.getIndex()).getVector()->beginNonzero();
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
                if((m_parent->getVariables()->at(currentColumn.getIndex()).getLowerBound() == -Numerical::Infinity && m_parent->getVariables()->at(currentColumn.getIndex()).getUpperBound() == Numerical::Infinity/* && (fixSign = 0 || true)*/)) {// ||
                   //!((*it) > 0 && currentColumn.getAttached()->getUpperBound() == Numerical::Infinity && m_parent->getRows().getAttachedData(constraintIdx).getUpperBound() == Numerical::Infinity && (fixSign = 1 || true)) ||
                   //!((*it) > 0 && currentColumn.getAttached()->getLowerBound() == -Numerical::Infinity && m_parent->getRows().getAttachedData(constraintIdx).getLowerBound() == -Numerical::Infinity && (fixSign = -1 || true)) ||
                   //!((*it) < 0 && currentColumn.getAttached()->getLowerBound() == -Numerical::Infinity && m_parent->getRows().getAttachedData(constraintIdx).getUpperBound() == Numerical::Infinity && (fixSign = -1 || true)) ||
                   //!((*it) < 0 && currentColumn.getAttached()->getUpperBound() == Numerical::Infinity && m_parent->getRows().getAttachedData(constraintIdx).getLowerBound() == -Numerical::Infinity && (fixSign = 1 || true))) {
                    Numerical::Double fixBound;
                    if(m_parent->getModel()->getObjectiveType() == MINIMIZE) {

                        //get the constraint bound to fix the variable
                        if(m_parent->getModel()->getCostVector().at(currentColumn.getIndex()) > 0) {
                            fixBound = m_parent->getModel()->getConstraints()->at(constraintIdx).getLowerBound();
                        } else {
                            fixBound = m_parent->getModel()->getConstraints()->at(constraintIdx).getUpperBound();
                        }

                        //detect unboundedness
                        if(fixBound == Numerical::Infinity || fixBound == -Numerical::Infinity) {
                            throw Presolver::PresolverException("The problem is primal unbounded.");
                            return;
                        }
                    } else {

                        //get the constraint bound to fix the variable
                        if(m_parent->getModel()->getCostVector().at(currentColumn.getIndex()) > 0) {
                            fixBound = m_parent->getModel()->getConstraints()->at(constraintIdx).getUpperBound();
                        } else {
                            fixBound = m_parent->getModel()->getConstraints()->at(constraintIdx).getLowerBound();
                        }

                        //detect unboundedness
                        if(fixBound == Numerical::Infinity || fixBound == -Numerical::Infinity) {
                            throw Presolver::PresolverException("The problem is primal unbounded.");
                            return;
                        }
                    }

                    //substitute the variable with the other variables present in the constraint,
                    //change the cost vector, and remove the constraint and the variable from the model
                    Vector * substituteVector = new Vector(m_parent->getModel()->variableCount() + 2);
                    Vector::NonzeroIterator ConstraintIt = m_parent->getModel()->getConstraints()->at(constraintIdx).getVector()->beginNonzero();
                    Vector::NonzeroIterator ConstraintItEnd = m_parent->getModel()->getConstraints()->at(constraintIdx).getVector()->endNonzero();
                    for(; ConstraintIt < ConstraintItEnd; ConstraintIt++) {
                        substituteVector->set(ConstraintIt.getIndex(), (-1) * (*ConstraintIt) / (*it));
                        m_parent->getColumnNonzeros()->set(ConstraintIt.getIndex(), m_parent->getColumnNonzeros()->at(ConstraintIt.getIndex()) - 1);
                        m_parent->getModel()->addToCostCoefficient(ConstraintIt.getIndex(), -1 * ((*ConstraintIt) * m_parent->getModel()->getCostVector().at(currentColumn.getIndex()) / (*it)));
                    }
                    m_parent->getModel()->setCostConstant(m_parent->getModel()->getCostConstant() + fixBound * m_parent->getModel()->getCostVector().at(currentColumn.getIndex()) / (*it));
                    substituteVector->set(currentColumn.getIndex(), 0);
                    substituteVector->set(m_parent->getModel()->variableCount(), fixBound / (*it));
                    substituteVector->set(m_parent->getModel()->variableCount() + 1, currentColumn.getIndex());
                    m_parent->getSubstituteVectors()->push_back(substituteVector);

                    m_removedConstraints++;
                    eliminatedConstraintCount++;
                    m_parent->getModel()->removeConstraint(constraintIdx);
                    m_parent->getRowNonzeros()->removeElement(constraintIdx);

                    m_removedVariables++;
                    m_parent->getModel()->removeVariable(currentColumn.getIndex());
                    m_parent->getColumnNonzeros()->removeElement(currentColumn.getIndex());
                    lastColumn = m_parent->getColumnNonzeros()->end();
                    continue;
                }
            }
            currentColumn++;
        }
    } while (eliminatedConstraintCount);
}
