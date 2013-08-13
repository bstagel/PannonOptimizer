/**
 * @file model.h
 */


#ifndef MODEL_H
#define	MODEL_H

#include "globals.h"

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
    virtual ~Model();


//    NOTE: Not necessary now, there's no dynamic member in the class
//    Model(const Model & original);
//    Model & operator=(const Model & original);
//    bool operator==(const Model & other);
//    bool operator!=(const Model & other);
    
    void build(const ModelBuilder & builder);

    inline unsigned int variableCount() const {return m_variables.size();}
    inline unsigned int constraintCount() const {return m_constraints.size();}
    inline const Matrix & getMatrix() const {return m_matrix;}
    inline const Variable & getVariable(unsigned int index) const {return m_variables[index];}
    inline const std::vector<Variable> & getVariables() const {return m_variables;}
    inline const Constraint & getConstraint(unsigned int index) const {return m_constraints[index];}
    inline const std::vector<Constraint> & getConstraints() const {return m_constraints;}
    inline const Vector & getCostVector() const {return m_costVector;}
    inline const std::string & getName() const {return m_name;}
    inline Numerical::Double getCostConstant() const {return m_costConstant;}

protected:
    Matrix m_matrix;
    std::vector<Variable> m_variables;
    std::vector<Constraint> m_constraints;
    Vector m_costVector;
    Numerical::Double m_costConstant;
    std::string m_name;

    void clear();

//NOTE: Not necessary now, there's no dynamic member in the class
//private:
//    void copy(const Model & original);
};

#endif	/* MODEL_H */

