/* 
 * File:   model.h
 * Author: smidla
 *
 * Created on 2013. march 5., 17:46
 */

#ifndef MODEL_H
#define	MODEL_H

#include <vector>
#include <lp/variable.h>
#include <lp/constraint.h>
#include <linalg/vector.h>
#include <linalg/matrix.h>
#include <lp/modelbuilder.h>

class Model
{
    friend class ModelTestSuite;
public:
    Model();

    void build(const ModelBuilder & builder);

    inline unsigned int variableCount() const;
    inline unsigned int constraintCount() const;
    inline const Variable & getVariable(unsigned int index) const;
    inline const Constraint & getConstraint(unsigned int index) const;
    inline const Vector & getCostVector() const;
    inline const std::string & getName() const;
    inline Numerical::Double getCostConstant() const;
private:

    Matrix m_matrix;

    std::vector<Variable> m_variables;

    std::vector<Constraint> m_constraints;

    Vector m_costVector;

    Numerical::Double m_costConstant;

    std::string m_name;
};

unsigned int Model::variableCount() const
{
    return m_variables.size();
}

unsigned int Model::constraintCount() const
{
    return m_constraints.size();
}

const Variable & Model::getVariable(unsigned int index) const
{
    return m_variables[index];
}

const Constraint & Model::getConstraint(unsigned int index) const
{
    return m_constraints[index];
}

const Vector & Model::getCostVector() const
{
    return m_costVector;
}

const std::string & Model::getName() const
{
    return m_name;
}

Numerical::Double Model::getCostConstant() const
{
    return m_costConstant;
}

#endif	/* MODEL_H */

