#include <lp/presolver.h>
#include <debug.h>
#include <simplex/simplexparameterhandler.h>

Presolver::Presolver(Model *model):
    m_impliedFreeCount(0),
    m_feasibilityTolerance(SimplexParameterHandler::getInstance().getDoubleParameterValue("Tolerances.e_feasibility"))
{

    m_model = model;
    //m_timeLimit = 5000;
    m_iterationLimit = 70;
    m_impliedFreeCount = 0;
    m_mode = DEFAULT;
    m_modules = std::vector<PresolverModule*>();
    unsigned int rowCount = m_model->constraintCount();
    unsigned int columnCount = m_model->variableCount();
    m_rowNonzeros = new DenseVector(rowCount);
    m_columnNonzeros = new DenseVector(columnCount);
    m_variables = m_model->getVariables();
    m_constraints = m_model->getConstraints();

    m_impliedLower = new DenseVector(columnCount);
    m_impliedUpper = new DenseVector(columnCount);
    m_impliedDualLower = new DenseVector(rowCount);
    m_impliedDualUpper = new DenseVector(rowCount);
    m_extraDualLowerSum = new DenseVector(columnCount);
    m_extraDualUpperSum = new DenseVector(columnCount);
    m_substituteVectors = new std::vector<DenseVector*>();

    //Fill the nonzero container vectors
    unsigned int biggerCount = (rowCount > columnCount ? columnCount : rowCount);
    for(unsigned int i = 0;
        i < biggerCount;
        i++){

        m_rowNonzeros->set(i, (*m_constraints)[i].getVector()->nonZeros());
        m_columnNonzeros->set(i, (*m_variables)[i].getVector()->nonZeros());
        m_impliedLower->set(i, (*m_variables)[i].getLowerBound());
        m_impliedUpper->set(i, (*m_variables)[i].getUpperBound());
        if((*m_variables)[i].getType() == Variable::BOUNDED) {
            //Change extra bound sums
            //Bounded variables are treated without their upper bound, which are handled with extra constraints
            m_extraDualLowerSum->set(i, m_extraDualLowerSum->at(i) + 1);
        }
        //Set dual variable bounds
        if((*m_constraints)[i].getType() == Constraint::RANGE) {
            //Range constraints are trated without their lower bound, which are handled with extra constraints
            m_impliedDualLower->set(i, -Numerical::Infinity);
            SparseVector::NonzeroIterator it = m_model->getMatrix().row(i).beginNonzero();
            SparseVector::NonzeroIterator itEnd = m_model->getMatrix().row(i).endNonzero();
            for(int i = 0; it < itEnd; i++, ++it) {
                if(*it > 0) {
                    m_extraDualUpperSum->set(it.getIndex(), m_extraDualUpperSum->at(it.getIndex()) + 1);
                } else {
                    m_extraDualLowerSum->set(it.getIndex(), m_extraDualLowerSum->at(it.getIndex()) + 1);
                }
            }
        } else {
            if((*m_constraints)[i].getType() == Constraint::EQUALITY) {
                m_impliedDualLower->set(i, -Numerical::Infinity);
                m_impliedDualUpper->set(i, Numerical::Infinity);
            } else {
                if((*m_constraints)[i].getLowerBound() != -Numerical::Infinity) {
                    m_impliedDualUpper->set(i, Numerical::Infinity);
                }
                if((*m_constraints)[i].getUpperBound() != Numerical::Infinity) {
                    m_impliedDualLower->set(i, -Numerical::Infinity);
                }
            }
        }
    }
    if(rowCount >= columnCount) {
        for(unsigned int i = columnCount; i < rowCount; i++) {
            m_rowNonzeros->set(i, (*m_constraints)[i].getVector()->nonZeros());
            //Set dual variable bounds
            if((*m_constraints)[i].getType() == Constraint::RANGE) {
                //Range constraints are trated without their lower bound, which are handled with extra constraints
                m_impliedDualLower->set(i, -Numerical::Infinity);
                SparseVector::NonzeroIterator it = m_model->getMatrix().row(i).beginNonzero();
                SparseVector::NonzeroIterator itEnd = m_model->getMatrix().row(i).endNonzero();
                for(int i = 0; it < itEnd; i++, ++it) {
                    if(*it > 0) {
                        m_extraDualUpperSum->set(it.getIndex(), m_extraDualUpperSum->at(it.getIndex()) + 1);
                    } else {
                        m_extraDualLowerSum->set(it.getIndex(), m_extraDualLowerSum->at(it.getIndex()) + 1);
                    }
                }
            } else {
                if((*m_constraints)[i].getType() == Constraint::EQUALITY) {
                    m_impliedDualLower->set(i, -Numerical::Infinity);
                    m_impliedDualUpper->set(i, Numerical::Infinity);
                } else {
                    if((*m_constraints)[i].getLowerBound() != -Numerical::Infinity) {
                        m_impliedDualUpper->set(i, Numerical::Infinity);
                    }
                    if((*m_constraints)[i].getUpperBound() != Numerical::Infinity) {
                        m_impliedDualLower->set(i, -Numerical::Infinity);
                    }
                }
            }
        }
    } else {
        for(unsigned int i = rowCount; i < columnCount; i++) {
            m_columnNonzeros->set(i, (*m_variables)[i].getVector()->nonZeros());
            m_impliedLower->set(i, (*m_variables)[i].getLowerBound());
            m_impliedUpper->set(i, (*m_variables)[i].getUpperBound());
            if((*m_variables)[i].getType() == Variable::BOUNDED) {
                //Change extra bound sums
                //Bounded variables are treated without their upper bound, which are handled with extra constraints
                m_extraDualLowerSum->set(i, m_extraDualLowerSum->at(i) + 1);
            }
        }
    }

    m_timer = new Timer();
}

Presolver::~Presolver() {
    /* Itt jártam. Anikó 2015. majdnem május*/
    clearModules();
}

void Presolver::printStatistics() {
    int eR = 0;
    int eC = 0;
    LPINFO("[Presolver] Finished sucessfully!");
    for(unsigned int i = 0; i < m_modules.size(); i++) {
        eR += m_modules[i]->getRemovedConstraintCount();
        eC += m_modules[i]->getRemovedVariableCount();
        m_modules[i]->printStatistics();
    }
    m_makeSparserModule->printStatistics();
    LPINFO("[Presolver] Total eliminated rows: " << eR);
    LPINFO("[Presolver] Total eliminated columns: " << eC << " (+" << m_impliedFreeCount << " implied free variables)");
    LPINFO("[Presolver] Presolve time: " << m_timer->getTotalElapsed() / 1000000);
}

void Presolver::fixVariable(int index, Numerical::Double value) {

    SparseVector::NonzeroIterator it = m_model->getVariable(index).getVector()->beginNonzero();
    SparseVector::NonzeroIterator itEnd = m_model->getVariable(index).getVector()->endNonzero();
    for(;it != itEnd; ++it) {
        int constraintIdx = it.getIndex();
        (*m_constraints)[constraintIdx].setBounds(Numerical::stableAdd(m_model->getConstraint(constraintIdx).getLowerBound(), -1 * (*it) * value),
                                                   Numerical::stableAdd(m_model->getConstraint(constraintIdx).getUpperBound(), -1 * (*it) * value));
        m_rowNonzeros->set(constraintIdx, m_rowNonzeros->at(constraintIdx) - 1);
    }
    //substitute the variable
    DenseVector * substituteVector = new DenseVector(m_model->variableCount() + 3);
    substituteVector->set(m_model->variableCount(), value);
    substituteVector->set(m_model->variableCount() + 1, index);
    substituteVector->set(m_model->variableCount() + 2, FIXED_VARIABLE);
    m_substituteVectors->push_back(substituteVector);

    //update cost constant and remove fixed variable from the model
    m_model->setCostConstant(Numerical::stableAdd(m_model->getCostConstant(), -1 * m_model->getCostVector().at(index) * value));
    m_model->removeVariable(index);
    m_impliedLower->remove(index);
    m_impliedUpper->remove(index);
    m_extraDualLowerSum->remove(index);
    m_extraDualUpperSum->remove(index);
    m_columnNonzeros->remove(index);
}

void Presolver::fixVariables(const std::vector<double> &fixValues, int removeCount, SUBSTITUTED_VARIABLE_FLAG fixMode) {
    if(removeCount < 0) {
        removeCount = 0;
        for(auto it = fixValues.begin(), itEnd = fixValues.end(); it != itEnd; ++it) {
            if(*it != 0) removeCount++;
        }
    }
    int newVariableCount = m_model->variableCount() - removeCount;
    int removedCount = 0;
    DenseVector * newImpliedLower = new DenseVector(newVariableCount);
    DenseVector * newImpliedUpper = new DenseVector(newVariableCount);
    DenseVector * newColumnNzr = new DenseVector(newVariableCount);
    DenseVector * newExtraDLSum = new DenseVector(newVariableCount);
    DenseVector * newExtraDUSum = new DenseVector(newVariableCount);
    for(auto itStart = fixValues.begin(), it = fixValues.begin(), itEnd = fixValues.end(); it != itEnd; ++it) {
        int index = it - itStart - removedCount;
        if(*it != 0) {
            double fixVal = *it==Numerical::Infinity?0:*it;
            SparseVector::NonzeroIterator itV = m_model->getVariable(index).getVector()->beginNonzero();
            SparseVector::NonzeroIterator itVEnd = m_model->getVariable(index).getVector()->endNonzero();
            for(;itV != itVEnd; ++itV) {
                int vIndex = itV.getIndex();
                (*m_constraints)[vIndex].setBounds(Numerical::stableAdd(m_model->getConstraint(vIndex).getLowerBound(), -1 * (*itV) * fixVal),
                                                           Numerical::stableAdd(m_model->getConstraint(vIndex).getUpperBound(), -1 * (*itV) * fixVal));
                m_rowNonzeros->set(vIndex, m_rowNonzeros->at(vIndex) - 1);
            }
            if(fixMode == FIXED_VARIABLE){
                DenseVector * substituteVector = new DenseVector(m_model->variableCount() + 3);
                substituteVector->set(m_model->variableCount(), fixVal);
                substituteVector->set(m_model->variableCount() + 1, index);
                substituteVector->set(m_model->variableCount() + 2, FIXED_VARIABLE);
                m_substituteVectors->push_back(substituteVector);
            }
            m_model->setCostConstant(Numerical::stableAdd(m_model->getCostConstant(), -1 * m_model->getCostVector().at(index) * fixVal));
            m_model->removeVariable(index);
            removedCount++;
        } else {
            newColumnNzr->set(index, m_columnNonzeros->at(index + removedCount));
            newImpliedLower->set(index, m_impliedLower->at(index + removedCount));
            newImpliedUpper->set(index, m_impliedUpper->at(index + removedCount));
            newExtraDLSum->set(index, m_extraDualLowerSum->at(index + removedCount));
            newExtraDUSum->set(index, m_extraDualUpperSum->at(index + removedCount));
        }
    }
    m_columnNonzeros = newColumnNzr;
    m_impliedLower = newImpliedLower;
    m_impliedUpper = newImpliedUpper;
    m_extraDualLowerSum = newExtraDLSum;
    m_extraDualUpperSum = newExtraDUSum;
}

void Presolver::removeConstraint(int index) {
    getModel()->removeConstraint(index);
    getImpliedDualLower()->remove(index);
    getImpliedDualUpper()->remove(index);
    getRowNonzeros()->remove(index);
}

void Presolver::removeConstraints(const std::vector<int>& removeIndices, int removeCount) {
    if(removeCount < 0) {
        removeCount = 0;
        for(auto it = removeIndices.begin(), itEnd = removeIndices.end(); it != itEnd; ++it) {
            if(*it != 0) removeCount++;
        }
    }
    if(removeCount == 0) return;
    int newConstraintCount = m_model->constraintCount() - removeCount;
    int removedCount = 0;
    DenseVector * newRowNzr = new DenseVector(newConstraintCount);
    DenseVector * newImpDualLower = new DenseVector(newConstraintCount);
    DenseVector * newImpDualUpper = new DenseVector(newConstraintCount);
    for(auto itStart = removeIndices.begin(), it = removeIndices.begin(), itEnd = removeIndices.end(); it != itEnd; ++it) {
        int index = it - itStart - removedCount;
        if(*it != 0) {
            m_model->removeConstraint(index);
            removedCount++;
        } else {
            newRowNzr->set(index, m_rowNonzeros->at(index + removedCount));
            newImpDualLower->set(index, m_impliedDualLower->at(index + removedCount));
            newImpDualUpper->set(index, m_impliedDualUpper->at(index + removedCount));
        }
    }
    m_rowNonzeros = newRowNzr;
    m_impliedDualLower = newImpDualLower;
    m_impliedDualUpper = newImpDualUpper;
}

void Presolver::presolve() {
int nzr = m_model->getMatrix().nonZeros();
LPINFO("nzr count: " << nzr);
    if(m_mode == NO_PRESOLVE)
        clearModules();
    if(m_mode == DEFAULT) {
        clearModules();
        addModule( new DualBoundsModule(this));
        addModule( new ImpliedBoundsModule(this));
        addModule( new SingletonRowsModule(this));
        addModule( new SingletonColumnsModule(this));
        addModule( new LinearAlgebraicModule(this));
    }

    int elim = 0;
    int itc = 0;
    int last = -1;
    m_timer->start();

    while(elim > last && itc < (int)m_iterationLimit) {
        last = elim;
        elim = 0;
        for(unsigned int i = 0; i < m_modules.size(); i++) {
            m_modules[i]->executeMethod();
            elim += m_modules[i]->getRemovedConstraintCount();
            elim += m_modules[i]->getRemovedVariableCount();
        }
        itc++;
    }
    m_makeSparserModule = new MakeSparserModule(this);
    m_makeSparserModule->executeMethod();
    if(m_makeSparserModule->getRemovedNonzerosCount()) {
        for(unsigned int i = 0; i < m_modules.size(); i++) {
            std::string name = m_modules[i]->getName();
            if(name == "Singleton rows module" || name == "Singleton columns module") {
                m_modules[i]->executeMethod();
            }
        }
    }

//    for(int i = 1; i < m_variables->size(); i++) {
//        if((m_variables->at(i).getType() != Variable::FREE) && ((m_variables->at(i).getLowerBound() < m_impliedLower->at(i) && m_variables->at(i).getUpperBound() > m_impliedUpper->at(i)) ||
//           (m_variables->at(i).getLowerBound() < m_impliedLower->at(i) && m_variables->at(i).getType() == Variable::PLUS) ||
//           (m_variables->at(i).getUpperBound() > m_impliedUpper->at(i) && m_variables->at(i).getType() == Variable::MINUS))) {
//            LPWARNING("IMPLIED FREE " << i << " " << m_variables->at(i).getLowerBound() << "-" << m_variables->at(i).getUpperBound() << " TO " << m_impliedLower->at(i) << "-" << m_impliedUpper->at(i));
//            m_variables->at(i).setLowerBound(-Numerical::Infinity);
//            m_variables->at(i).setUpperBound(Numerical::Infinity);
//            m_impliedFreeCount++;
//        }
//    }
//    for(unsigned i = 0; i < m_variables->size(); i++) {
//        if(m_variables->at(i).getType() != Variable::FREE) {
//            LPERROR("setting " << i << " to " << m_impliedLower->at(i) << "-" << m_impliedUpper->at(i) << Numerical::stableAdd(m_impliedLower->at(i), -1 * m_impliedUpper->at(i)));
//            m_variables->at(i).setLowerBound(m_impliedLower->at(i));
//            m_variables->at(i).setUpperBound(m_impliedUpper->at(i));
//        }
//    }
    m_model->setSubstitueVectors(m_substituteVectors);
    m_timer->stop();
    LPINFO("removed nzr: " << nzr - m_model->getMatrix().nonZeros());
}
