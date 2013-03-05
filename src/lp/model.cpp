#include <lp/model.h>

Model::Model()
{

}

void Model::build(const ModelBuilder & builder)
{
    m_name = builder.getName();
    m_costConstant = builder.getObjectiveConstant();
    builder.buildCostVector(&m_costVector);

    m_constraints.resize(builder.getConstraintCount());
    m_variables.resize(builder.getVariableCount());
    m_matrix.reInit(builder.getConstraintCount(), builder.getVariableCount(), false);

    unsigned int rowIndex;
    for (rowIndex = 0; rowIndex < builder.getConstraintCount(); rowIndex++) {
        Vector * rowVector = new Vector;
        m_matrix.m_rowWise[rowIndex] = rowVector;
        builder.buildRow(rowIndex, rowVector);
        m_constraints[rowIndex].setVector(*rowVector);
    }

    unsigned int columnIndex;
    for (columnIndex = 0; columnIndex < builder.getVariableCount(); columnIndex++) {
        Vector * columnVector = new Vector;
        m_matrix.m_columnWise[columnIndex] = columnVector;
        builder.buildColumn(columnIndex, columnVector);
        m_constraints[columnIndex].setVector(*columnVector);
    }
}
