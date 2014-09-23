#include <lp/presolver.h>
#include <debug.h>

Presolver::Presolver(Model *model) {

    m_model = model;
    //m_timeLimit = 5000;
    m_iterationLimit = 70;
    m_impliedFreeCount = 0;
    m_mode = DEFAULT;
    m_modules = std::vector<PresolverModule*>();
    unsigned int rowCount = m_model->constraintCount();
    unsigned int columnCount = m_model->variableCount();
    m_rowNonzeros = new Vector(rowCount);
    m_columnNonzeros = new Vector(columnCount);
    m_variables = m_model->getVariables();
    m_constraints = m_model->getConstraints();

    m_impliedLower = new Vector(columnCount);
    m_impliedLower->setSparsityRatio(0);
    m_impliedUpper = new Vector(columnCount);
    m_impliedUpper->setSparsityRatio(0);
    m_impliedDualLower = new Vector(rowCount);
    m_impliedDualLower->setSparsityRatio(0);
    m_impliedDualUpper = new Vector(rowCount);
    m_impliedDualUpper->setSparsityRatio(0);
    m_extraDualLowerSum = new Vector(columnCount);
    m_extraDualLowerSum->setSparsityRatio(0);
    m_extraDualUpperSum = new Vector(columnCount);
    m_extraDualUpperSum->setSparsityRatio(0);
    m_substituteVectors = new std::vector<Vector*>();

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
            Vector::NonzeroIterator it = m_model->getMatrix().row(i).beginNonzero();
            Vector::NonzeroIterator itEnd = m_model->getMatrix().row(i).endNonzero();
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
                Vector::NonzeroIterator it = m_model->getMatrix().row(i).beginNonzero();
                Vector::NonzeroIterator itEnd = m_model->getMatrix().row(i).endNonzero();
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
    //make sure the variable is not fixed out of bounds
    if(Numerical::stableAdd(m_model->getVariable(index).getLowerBound(), -1 * value) > 0 ||
       Numerical::stableAdd(m_model->getVariable(index).getUpperBound(), -1 * value) < 0) {
        throw PresolverException("Variable fixing value is out of bounds.");
        return;
    }

    Vector::NonzeroIterator it = m_model->getVariable(index).getVector()->beginNonzero();
    Vector::NonzeroIterator itEnd = m_model->getVariable(index).getVector()->endNonzero();
    for(;it != itEnd; ++it) {
        (*m_constraints)[it.getIndex()].setBounds(Numerical::stableAdd(m_model->getConstraint(it.getIndex()).getLowerBound(), -1 * (*it) * value),
                                                   Numerical::stableAdd(m_model->getConstraint(it.getIndex()).getUpperBound(), -1 * (*it) * value));
        m_rowNonzeros->set(it.getIndex(), m_rowNonzeros->at(it.getIndex()) - 1);

    }
    //substitute the variable
    Vector * substituteVector = new Vector(m_model->variableCount() + 3);
    substituteVector->set(m_model->variableCount(), value);
    substituteVector->set(m_model->variableCount() + 1, index);
    substituteVector->set(m_model->variableCount() + 2, PRIMAL_VARIABLE);
    m_substituteVectors->push_back(substituteVector);

    //update cost constant and remove fixed variable from the model
    m_model->setCostConstant(Numerical::stableAdd(m_model->getCostConstant(), -1 * m_model->getCostVector().at(index) * value));
    m_model->removeVariable(index);
    m_impliedLower->removeElement(index);
    m_impliedUpper->removeElement(index);
    m_extraDualLowerSum->removeElement(index);
    m_extraDualUpperSum->removeElement(index);
    m_columnNonzeros->removeElement(index);
}

void Presolver::removeConstraint(int index) {
    getModel()->removeConstraint(index);
    getImpliedDualLower()->removeElement(index);
    getImpliedDualUpper()->removeElement(index);
    getRowNonzeros()->removeElement(index);
}

void Presolver::presolve() {

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
//    for(int i = 0; i < m_variables->size(); i++) {
//        if(m_variables->at(i).getType() != Variable::FREE) {
//            LPERROR("setting " << i << " to " << m_impliedLower->at(i) << "-" << m_impliedUpper->at(i) << Numerical::stableAdd(m_impliedLower->at(i), -1 * m_impliedUpper->at(i)));
//            m_variables->at(i).setLowerBound(m_impliedLower->at(i));
//            m_variables->at(i).setUpperBound(m_impliedUpper->at(i));
//        }
//    }
    m_model->setSubstitueVectors(m_substituteVectors);
    m_timer->stop();
}
