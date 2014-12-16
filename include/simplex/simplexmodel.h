/**
 * @file simplexmodel.h This file contains the API of the SimplexModel class.
 */

#ifndef SIMPLEXMODEL_H
#define	SIMPLEXMODEL_H

#include <globals.h>

#include <lp/model.h>

/**
 * This class describes a general simplex model.
 * It stores the model object, the cost vector, the variables and the rhs vector.
 *
 * @class SimplexModel
 */
class SimplexModel
{
public:

    /**
     * Initializing constructor of the class, it calls the SimplexModel::makeComputationalForm() function.
     *
     * @param model The model of the LP problem.
     *
     * @constructor
     */
    SimplexModel(const Model & model);

    /**
     * Getter of the rhs vector member.
     *
     * @return SimplexModel::m_rhs
     */
    inline const DenseVector & getRhs() const {return m_rhs;}

    /**
     * Prints the output of the createComputationalForm (CF3) algorithm.
     * (The generated model with RHS and logical variables.)
     *
     * @param out The output stream.
     */
    void print(std::ostream& out = std::cout) const;

    /**
     * Getter of the LP problem's matrix.
     *
     * @see Model::getMatrix()
     */
    inline const Matrix & getMatrix()const {return m_model.getMatrix();}

    /**
     * Getter of the number of rows in the matrix.
     *
     * @see Model::getMatrix().rowCount().
     */
    inline unsigned int getRowCount() const {return m_model.getMatrix().rowCount();}

    /**
     * Getter of the number of columns in the matrix,
     *
     * @see Model::getMatrix().columnCount().
     */
    inline unsigned int getColumnCount() const {return m_model.getMatrix().columnCount();}

    /**
     * Getter of a contraint.
     *
     * @see Model::getConstraint()
     */
    inline const Constraint & getConstraint(unsigned int index) const {return m_model.getConstraint(index);}

    /**
     * Getter of all contraints.
     *
     * @see Model::getConstraints()
     */
    inline const std::vector<Constraint> & getConstraints() const {return m_model.getConstraints();}

    /**
     * Getter of problem's name.
     *
     * @see Model::getName()
     */
    inline const std::string & getName() const {return m_model.getName();}

    /**
     * Getter of the cost constant.
     *
     * @see Model::getCostConstant()
     */
    inline const Numerical::Double & getCostConstant() const {return m_model.getCostConstant();}

    /**
     * Getter of the objective type.
     *
     * @see Model::getObjectiveType()
     */
    inline OBJECTIVE_TYPE getObjectiveType() const {return m_model.getObjectiveType();}

    /**
     * Getter of objective row name.
     *
     * @see Model::getObjectiveRowname()
     */
    inline const std::string & getObjectiveRowname() const {return m_model.getObjectiveRowname();}

    /**
     * Getter function of the variables.
     *
     * @param index The index of the variable to be returned.
     * @return The selected variable as a const reference.
     */
    inline const Variable & getVariable(unsigned int index) const { return m_variables[index]; }

    /**
     * Returns a vector containing both the structural and logical variables.
     *
     * @return SimplexModel::m_variables
     */
    inline const std::vector<Variable> & getVariables() const {return m_variables;}

    /**
     * Getter of the cost vector.
     *
     * @return SimplexModel::m_costVector
     */
    inline const DenseVector & getCostVector() const {return m_costVector;}//m_model.getCostVector();}

    /**
     * Getter of the model member.
     *
     * @return The model of the LP problem.
     */
    inline const Model & getModel() const { return m_model; }

    /**
     * This function perturbs the cost vector with random epislon values and so prevents degeneracy.
     * Thanks to the perturbation of the cost vector the reduced costs are also perturbed. This is
     * an efficenty strategy for the dual algorithm.
     * @param initializeEngine number of times random engine should be initialized
     */
    void perturbCostVector(int initializeEngine);

    /**
     * The perturbation of the RHS vector with small epsilon values leads to antidegeneracy in the primal algorithm.
     * The basicvariable values are computed from the RHS, so those values are
     * also perturbed this way.
     */
    void perturbRHS();

    /**
     * This function shifts the bounds of the structural variables with random epsilon values.
     * By doing so it leads to antidegeneracy in the primal algorithm.
     */
    void shiftBounds();

    /**
     * This function resets the model corresponding to the original cost vector, rhs and bounds if perturbations were done.
     */
    void resetModel();

private:

    /**
     * Reference of the LP's mathematical model.
     */
    const Model & m_model;

    /**
     * The cost vector (c) of the LP problem.
     */
    DenseVector m_costVector;

    /**
     * Vector containing structural and logical variables.
     */
    std::vector<Variable> m_variables;

    /**
     * The right hand side vector (b) of the LP problem.
     */
    DenseVector m_rhs;

    /**
     * This vector is the original (not perturbed) cost vector.
     */
    DenseVector m_originalCostVector;

    /**
     * This vector is the original (not perturbed) rhs vector.
     */
    DenseVector m_originalRhs;

    /**
     * True if the cost vector was perturbed.
     */
    bool m_perturbedCostVector;

    /**
     * True if the rhs vector was perturbed.
     */
    bool m_perturbedRhs;

    /**
     * True if the bounds of the structural variables were perturbed.
     */
    bool m_perturbedBounds;

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

