#include <lp/model.h>

Model::Model()
{
    m_costConstant = 0.0;
}

void Model::build(const ModelBuilder & builder)
{
    unsigned int index;
    m_name = builder.getName();
    m_costConstant = builder.getObjectiveConstant();
    builder.buildCostVector(&m_costVector);

    m_constraints.resize(builder.getRowCount());
    m_variables.resize(builder.getColumnCount());

    m_matrix.build(builder);

    for (index = 0; index < m_variables.size(); index++) {
        m_variables[index] = builder.getVariable(index);
        m_variables[index].setVector( m_matrix.column(index) );
    }

    for (index = 0; index < m_constraints.size(); index++) {
        m_constraints[index] = builder.getConstraint(index);
        m_constraints[index].setVector( m_matrix.row(index) );
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

Model::Model(const Model & original)
{
    copy(original);
}

Model::~Model()
{
    clear();
}

Model & Model::operator=(const Model & original)
{
    if (this == &original) {
        return *this;
    }
    clear();
    copy(original);
    return *this;
}

void Model::clear()
{
    m_matrix.clear();
    m_variables.clear();
    m_constraints.clear();
    m_costVector.clear();
    m_costConstant = 0;
    m_name = "";
}

void Model::copy(const Model & original)
{
    m_matrix = original.m_matrix;
    m_variables = original.m_variables;
    m_constraints = original.m_constraints;
    m_costVector = original.m_costVector;
    m_costConstant = original.m_costConstant;
    m_name = original.m_name;
}