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

    enum OBJECTIVE_TYPE
    {
        MINIMIZE, MAXIMIZE
    };

    Model();
    virtual ~Model();

    
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
    inline const Numerical::Double & getCostConstant() const {return m_costConstant;}
    inline OBJECTIVE_TYPE getObjectiveType() const {return m_objectiveType;}
    inline const std::string & getObjectiveRowname() const {return m_objectiveRowName;}

    virtual void print(std::ostream out = std::cout) const;

protected:
    Matrix m_matrix;
    std::vector<Variable> m_variables;
    std::vector<Constraint> m_constraints;
    Vector m_costVector;
    Numerical::Double m_costConstant;
    std::string m_objectiveRowName;
    std::string m_name;

    OBJECTIVE_TYPE m_objectiveType;
    //TODO Build the objective type

    void clear();

};

#endif	/* MODEL_H */

