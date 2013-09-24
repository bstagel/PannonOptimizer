/**
 * @file simplexmodel.h
 */

#ifndef SIMPLEXMODEL_H
#define	SIMPLEXMODEL_H

#include "globals.h"

#include "lp/model.h"

class SimplexModel
{
public:

    SimplexModel(const Model & model);

    inline const Vector & getRhs() const {return m_rhs;}

    /**
     * Prints the output of the createComputationalForm (CF3) algorithm.
     * (The generated model with RHS and logical variables.)
     */
    void print(std::ostream& out = std::cout) const;

    inline const Matrix & getMatrix()const {return m_model.getMatrix();}
    inline unsigned int getRowCount() const {return m_model.getMatrix().rowCount();}
    inline unsigned int getColumnCount() const {return m_model.getMatrix().columnCount();}
    inline const Variable & getVariable(unsigned int index) const {
        unsigned int rowCount = m_model.getMatrix().rowCount();
        return index<rowCount?m_model.getVariable(index):m_logicalVariables[index-rowCount];
    }
    inline const std::vector<Variable> & getStructuralVariables() const {return m_model.getVariables();}
    inline const std::vector<Variable> & getLogicalVariables() const {return m_logicalVariables;}
//    inline const Constraint & getConstraint(unsigned int index) const {return m_constraints[index];}
    inline const std::vector<Constraint> & getConstraints() const {return m_model.getConstraints();}
    inline const Vector & getCostVector() const {return m_model.getCostVector();}
    inline const std::string & getName() const {return m_model.getName();}
    inline const Numerical::Double & getCostConstant() const {return m_model.getCostConstant();}
    inline OBJECTIVE_TYPE getObjectiveType() const {return m_model.getObjectiveType();}
    inline const std::string & getObjectiveRowname() const {return m_model.getObjectiveRowname();}

private:
    const Model & m_model;
    std::vector<Variable> m_logicalVariables;
    Vector m_rhs;

    /**
     * Starts the computational form maker algorithm.
     * It will call the corresponding private function depending on
     * the selected CF type.
     * This function will modify the assigned model during its operation.
     * (RHS values will be set and logical variables will be added.)
     */
    void makeComputationalForm();
};

#endif	/* SIMPLEXMODEL_H */

