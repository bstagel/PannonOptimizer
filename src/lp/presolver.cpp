#include <lp/presolver.h>
#include <debug.h>

Presolver::PresolverException::~PresolverException() {

}

Presolver::Presolver(Model *model) {

    m_model = model;
    m_timeLimit = 5000;
    m_iterationLimit = 10;
    m_mode = DEFAULT;
    m_modules = std::vector<PresolverModule*>();
    unsigned int rowCount = m_model->constraintCount();
    unsigned int columnCount = m_model->variableCount();
    m_rowNonzeros = new Vector(rowCount);
    m_columnNonzeros = new Vector(columnCount);
    m_variables = m_model->getVariables();
    m_constraints = m_model->getConstraints();

    //Fill the nonzero container vectors
    unsigned int biggerCount = (rowCount > columnCount ? columnCount : rowCount);
    for(unsigned int i = 0;
        i < biggerCount;
        i++){

        m_rowNonzeros->set(i, m_model->getConstraint(i).getVector()->nonZeros());
        m_columnNonzeros->set(i, m_model->getVariable(i).getVector()->nonZeros());

    }
    if(rowCount >= columnCount) {
        for(unsigned int i = columnCount; i < rowCount; i++) {
            m_rowNonzeros->set(i, m_model->getConstraint(i).getVector()->nonZeros());
        }
    } else {
        for(unsigned int i = rowCount; i < columnCount; i++) {
            m_columnNonzeros->set(i, m_model->getVariable(i).getVector()->nonZeros());
        }
    }
}

Presolver::~Presolver() {
    clearModules();
}

void Presolver::printStatistics() {
    int eR = 0;
    int eC = 0;
    for(unsigned int i = 0; i < m_modules.size(); i++) {
        eR += m_modules.at(i)->getRemovedConstraintCount();
        eC += m_modules.at(i)->getRemovedVariableCount();
    }
    LPINFO("Presolver finished sucessfully!");
    LPINFO("Presolver eliminated rows: " << eR);
    LPINFO("Presolver eliminated columns: " << eC);
}

void Presolver::fixVariable(int index, Numerical::Double value) {
    //make sure the variable is not fixed out of bounds
    if(m_model->getVariable(index).getLowerBound() > value || m_model->getVariable(index).getUpperBound() < value) {
        throw PresolverException("Variable fixing value is out of bounds.");
        return;
    }

    Vector::NonzeroIterator it = m_model->getVariable(index).getVector()->beginNonzero();
    Vector::NonzeroIterator itEnd = m_model->getVariable(index).getVector()->endNonzero();
    for(;it != itEnd; it++) {
        m_constraints->at(it.getIndex()).setBounds(m_model->getConstraint(it.getIndex()).getLowerBound() - (*it) * value,
                                                   m_model->getConstraint(it.getIndex()).getUpperBound() - (*it) * value);

        m_rowNonzeros->set(it.getIndex(), m_rowNonzeros->at(it.getIndex()) - 1);
    }
    //substitute the variable
    Vector * substituteVector = new Vector(m_model->variableCount() + 2);
    substituteVector->set(m_model->variableCount(), value);
    substituteVector->set(m_model->variableCount() + 1, index);
    m_substituteVectors.push_back(substituteVector);

    //update cost constant and remove fixed variable from the model
    m_model->setCostConstant(m_model->getCostConstant() - m_model->getCostVector().at(index) * value);
    m_model->removeVariable(index);
    m_columnNonzeros->removeElement(index);
}

void Presolver::presolve() {

    if(m_mode == NO_PRESOLVE)
        clearModules();
    if(m_mode == DEFAULT) {
        clearModules();
        addModule( new SingletonRowsModule(this));
        addModule( new SingletonColumnsModule(this));
    }
    if(m_mode == SINGLE_MODULE){
        while(m_modules.size() > 1)
             m_modules.pop_back();
    }
    int elim = 0;
    int i = 0;
    int last = -1;
    while(elim > last && i < (int)m_iterationLimit) {
        last = elim;
        elim = 0;
        for(unsigned int i = 0; i < m_modules.size(); i++) {
            m_modules.at(i)->executeMethod();
            elim += m_modules.at(i)->getRemovedConstraintCount();
            elim += m_modules.at(i)->getRemovedVariableCount();
        }
    }
}
