#include <lp/manualmodelbuilder.h>

ManualModelBuilder::ManualModelBuilder()
{
    m_objectiveConstant = 0.0;
}

/************************************************
 * 
 * Interface for the manual model construction
 * 
 * *********************************************/

void ManualModelBuilder::setName(const std::string & name)
{
    m_name = name;
}

void ManualModelBuilder::setObjectiveFunctionConstant(Numerical::Double value)
{
    m_objectiveConstant = value;
}

void ManualModelBuilder::addVariable(const Variable & variable,
    Numerical::Double costCoefficient,
    unsigned int nonzeros,
    const Numerical::Double * values,
    const unsigned int * indices)
{
    m_variables.push_back(variable);
    m_costVector.push_back(costCoefficient);

    // creating the new column
    std::list<IndexValuePair> newColumnList;
    m_columns.push_back(newColumnList);
    const unsigned int lastIndex = m_columns.size() - 1;
    unsigned int index;
    unsigned int maxIndex = 0;
    for (index = 0; index < nonzeros; index++) {
        IndexValuePair pair = {values[index], indices[index]};
        m_columns[lastIndex].push_back(pair);
        if (maxIndex < indices[index]) {
            maxIndex = indices[index];
        }
    }
    m_nonZerosInColumns.push_back(nonzeros);
    
    // creating the necessary constraints
    for (index = m_constraints.size(); index < maxIndex; index++) {
        m_constraints.push_back(Constraint::createGreaterTypeConstraint(0, 0.0));
    }

}

void ManualModelBuilder::addVariable(const Variable & variable,
    Numerical::Double costCoefficient,
    const Vector & vector)
{

}

void ManualModelBuilder::addVariable(const Variable & variable,
    Numerical::Double costCoefficient,
    unsigned int nonzeros,
    ...)
{

}

/************************************************
 * 
 * Interface for the Model
 * 
 * *********************************************/

unsigned int ManualModelBuilder::getVariableCount() const
{
    return m_variables.size();
}

unsigned int ManualModelBuilder::getConstraintCount() const
{
    return m_constraints.size();
}

const Variable & ManualModelBuilder::getVariable(unsigned int index) const
{
    return m_variables[index];
}

const Constraint & ManualModelBuilder::getConstraint(unsigned int index) const
{
    return m_constraints[index];
}

void ManualModelBuilder::rowBuild(unsigned int index, Vector * rowVector) const
{

}

void ManualModelBuilder::columnBuild(unsigned int index, Vector * columnVector) const
{

}

void ManualModelBuilder::costVectorBuild(Vector * costVector) const
{

}

Numerical::Double ManualModelBuilder::getObjectiveConstant() const
{
    return m_objectiveConstant;
}

std::string ManualModelBuilder::getName() const
{
    return m_name;
}
