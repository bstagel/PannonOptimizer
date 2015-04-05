#include <lp/pascalmodelbuilder.h>


PascalModelBuilder::PascalModelBuilder(unsigned int size)
{
    setSize(size);
    m_objectiveType = MINIMIZE;
    m_objectiveConstant = 0;
}

void PascalModelBuilder::setSize(unsigned int size)
{
    m_size = size;
    m_costVector.resize(size, 0.0);
    m_variables.resize(size, Variable::createPlusTypeVariable("", 0.0));
    m_constraints.resize(size, Constraint());
    m_matrix.resize(size);
    unsigned int rowIndex;
    for (rowIndex = 0; rowIndex < size; rowIndex++) {
        m_matrix[rowIndex].resize(size);
    }
    unsigned int index;
    for (index = 0; index < size; index++) {
        m_matrix[index][0] = 1.0;
        m_matrix[0][index] = 1.0;
    }
    m_constraints[0] = Constraint::createEqualityTypeConstraint("", m_size);
    for (rowIndex = 1; rowIndex < size; rowIndex++) {
        unsigned int columnIndex;
        double rhs = 1;
        for (columnIndex = 1; columnIndex < size; columnIndex++) {
            m_matrix[rowIndex][columnIndex] =
                    m_matrix[rowIndex - 1][columnIndex] + m_matrix[rowIndex][columnIndex - 1];
            rhs += m_matrix[rowIndex][columnIndex];
        }
        m_constraints[rowIndex] = Constraint::createEqualityTypeConstraint("", rhs);
    }
}

void PascalModelBuilder::setName(const std::string &name)
{
    m_name = name;
}

void PascalModelBuilder::setObjectiveFunctionConstant(double value)
{
    m_objectiveConstant = value;
}

void PascalModelBuilder::setCostCoefficient(unsigned int index, double cost)
{
    m_costVector[index] = cost;
}

void PascalModelBuilder::setObjectiveType(OBJECTIVE_TYPE type)
{
    m_objectiveType = type;
}

unsigned int PascalModelBuilder::getColumnCount() const
{
    return m_size;
}

unsigned int PascalModelBuilder::getRowCount() const
{
    return m_size;
}

const Variable &PascalModelBuilder::getVariable(unsigned int index) const
{
    return m_variables[index];
}

const Constraint &PascalModelBuilder::getConstraint(unsigned int index) const
{
    return m_constraints[index];
}

void PascalModelBuilder::buildRow(unsigned int index, SparseVector *rowVector, std::vector<unsigned int> *nonzeros) const
{
    unsigned int columnIndex;
    if (nonzeros != nullptr) {
        nonzeros->clear();
    }
    rowVector->prepareForData(m_size, m_size);
    for (columnIndex = 0; columnIndex < m_size; columnIndex++) {
        if (nonzeros != nullptr) {
            nonzeros->push_back(columnIndex);
        }
        rowVector->newNonZero(m_matrix[index][columnIndex] , columnIndex);
    }
}

void PascalModelBuilder::buildColumn(unsigned int index, SparseVector *columnVector, std::vector<unsigned int> *nonzeros) const
{
    unsigned int rowIndex;
    if (nonzeros != nullptr) {
        nonzeros->clear();
    }
    columnVector->prepareForData(m_size, m_size);
    for (rowIndex = 0; rowIndex < m_size; rowIndex++) {
        if (nonzeros != nullptr) {
            nonzeros->push_back(rowIndex);
        }
        columnVector->newNonZero(m_matrix[rowIndex][index], rowIndex);
    }
}

void PascalModelBuilder::buildCostVector(DenseVector *costVector) const
{
    costVector->resize(m_size);
    unsigned int index;
    for (index = 0; index < m_size; index++) {
        costVector->set(index, m_costVector[index]);
    }
}

double PascalModelBuilder::getObjectiveConstant() const
{
    return m_objectiveConstant;
}

std::string PascalModelBuilder::getName() const
{
    return m_name;
}

bool PascalModelBuilder::hasRowwiseRepresentation() const
{
    return true;
}

bool PascalModelBuilder::hasColumnwiseRepresentation() const
{
    return true;
}

OBJECTIVE_TYPE PascalModelBuilder::getObjectiveType() const
{
    return m_objectiveType;
}

