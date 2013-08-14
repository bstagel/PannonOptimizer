/**
 * @file model.cpp
 */

#include <lp/model.h>

Model::Model()
{
    m_costConstant = 0.0;
    m_objectiveType = MINIMIZE;
}

void Model::build(const ModelBuilder & builder)
{
    unsigned int index;
    m_name = builder.getName();
    m_objectiveType = builder.getObjectiveType();
    m_costConstant = builder.getObjectiveConstant();
    builder.buildCostVector(&m_costVector);

    m_constraints.resize(builder.getRowCount());
    m_variables.resize(builder.getColumnCount());

    m_matrix.build(builder);

    for (index = 0; index < m_variables.size(); index++) {
        m_variables[index] = builder.getVariable(index);
        m_variables[index].setVector(m_matrix.column(index));
    }

    for (index = 0; index < m_constraints.size(); index++) {
        m_constraints[index] = builder.getConstraint(index);
        m_constraints[index].setVector(m_matrix.row(index));
    }

    /*m_matrix.reInit(builder.getRowCount(), builder.getColumnCount(), false);

    unsigned int rowIndex;
    for (rowIndex = 0; rowIndex < builder.getRowCount(); rowIndex++) {
        Vector * rowVector = new Vector;
        m_matrix.m_rowWise[rowIndex] = rowVector;
        builder.buildRow(rowIndex, rowVector);
        m_constraints[rowIndex].setVector(*rowVector);
    }

    unsigned int columnIndex;
    for (columnIndex = 0; columnIndex < builder.getColumnCount(); columnIndex++) {
        Vector * columnVector = new Vector;
        m_matrix.m_columnWise[columnIndex] = columnVector;
        builder.buildColumn(columnIndex, columnVector);
        m_constraints[columnIndex].setVector(*columnVector);
    }*/
}

Model::~Model()
{

}

void Model::print(std::ostream out) const
{
    out << (m_objectiveType == MINIMIZE ? "min" : "max");
    Vector::Iterator it = m_costVector.begin();
    Vector::Iterator end = m_costVector.end();
    for (unsigned int i = 0; it < end; i++, it++) {
        out << (i == 0 ? " " : "+ ") << *it << "*x" << i << " \n";
    }
    out << "\n s.t.: \n";
    for (unsigned int i = 0; i < m_matrix.rowCount(); i++) {
        out << m_constraints[i].getLowerBound() << " <= ";
        const Vector & row = m_matrix.row(i);
        Vector::Iterator it = row.begin();
        Vector::Iterator end = row.end();
        for (unsigned int j = 0; it < end; j++, it++) {
            out << (j == 0 ? " " : "+ ") << *it << "*x" << j << " ";
        }
        out << "<= " << m_constraints[i].getUpperBound() << "\n";
    }
    out << "\n";
    for (unsigned int i = 0; i < m_variables.size(); i++) {
        const Variable & variable = m_variables[i];
        out << variable.getLowerBound() << " x" << i << " " << variable.getUpperBound();
    }
}

void Model::clear()
{
    m_matrix.clear();
    m_variables.clear();
    m_constraints.clear();

}

void Model::addVariable(const Variable & variable, const Vector & column)
{
    m_variables.push_back(variable);
    m_matrix.appendColumn(column);
}

void Model::addConstraint(const Constraint & constraint, const Vector & row)
{
    m_constraints.push_back(constraint);
    m_matrix.appendRow(row);
}
