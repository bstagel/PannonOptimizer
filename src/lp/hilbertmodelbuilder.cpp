#include <lp/hilbertmodelbuilder.h>


HilbertModelBuilder::HilbertModelBuilder(unsigned int size)
{
    setSize(size);
    m_objectiveType = MINIMIZE;
    m_objectiveConstant = 0;
}

void HilbertModelBuilder::setSize(unsigned int size)
{
    m_size = size;
    m_tempData.resize(size, 0.0);
    m_costVector.resize(size, 0.0);
    m_variables.resize(size, Variable::createPlusTypeVariable("", 0.0));
    m_constraints.resize(size, Constraint());
    unsigned int rowIndex;
    for (rowIndex = 0; rowIndex < size; rowIndex++) {
        double rhs = 0.0;
        unsigned int columnIndex;
        for (columnIndex = size - 1; columnIndex < size; columnIndex--) {
            rhs += 1.0 / (columnIndex + rowIndex + 1.0);
        }
        m_constraints[rowIndex] = Constraint::createEqualityTypeConstraint("", rhs);
    }
}

void HilbertModelBuilder::setName(const std::string &name)
{
    m_name = name;
}

void HilbertModelBuilder::setObjectiveFunctionConstant(double value)
{
    m_objectiveConstant = value;
}

void HilbertModelBuilder::setCostCoefficient(unsigned int index, double cost)
{
    m_costVector[index] = cost;
}

void HilbertModelBuilder::setObjectiveType(OBJECTIVE_TYPE type)
{
    m_objectiveType = type;
}

unsigned int HilbertModelBuilder::getColumnCount() const
{
    return m_size;
}

unsigned int HilbertModelBuilder::getRowCount() const
{
    return m_size;
}

const Variable &HilbertModelBuilder::getVariable(unsigned int index) const
{
    return m_variables[index];
}

const Constraint &HilbertModelBuilder::getConstraint(unsigned int index) const
{
    return m_constraints[index];
}

void HilbertModelBuilder::buildRow(unsigned int index, SparseVector *rowVector, std::vector<unsigned int> *nonzeros) const
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
        rowVector->newNonZero(1.0 / (columnIndex + index + 1.0), columnIndex);
    }    
}

void HilbertModelBuilder::buildColumn(unsigned int index, SparseVector *columnVector, std::vector<unsigned int> *nonzeros) const
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
        columnVector->newNonZero(1.0 / (rowIndex + index + 1.0), rowIndex);
    }
}

void HilbertModelBuilder::buildCostVector(DenseVector *costVector) const
{
    costVector->resize(m_size);
    unsigned int index;
    for (index = 0; index < m_size; index++) {
        costVector->set(index, m_costVector[index]);
    }
}

double HilbertModelBuilder::getObjectiveConstant() const
{
    return m_objectiveConstant;
}

std::string HilbertModelBuilder::getName() const
{
    return m_name;
}

bool HilbertModelBuilder::hasRowwiseRepresentation() const
{
    return true;
}

bool HilbertModelBuilder::hasColumnwiseRepresentation() const
{
    return true;
}

OBJECTIVE_TYPE HilbertModelBuilder::getObjectiveType() const
{
    return m_objectiveType;
}
