/* 
 * File:   manualbuilder.h
 * Author: smidla
 *
 * Created on 2013. february 14., 13:13
 */

#ifndef MANUALBUILDER_H
#define	MANUALBUILDER_H

#include <lp/modelbuilder.h>
#include <list>
#include <vector>

class ManualModelBuilder : public ModelBuilder
{
    friend class ManualModelBuilderTestSuite;
public:
    ManualModelBuilder();

    /************************************************
     * 
     * Interface for the manual model construction
     * 
     * *********************************************/

    /**
     * 
     * @param name
     */
    void setName(const std::string & name);

    /**
     * 
     * @param value
     */
    void setObjectiveFunctionConstant(Numerical::Double value);

    /**
     * 
     * @param variable
     * @param costCoefficient
     * @param nonzeros
     * @param values
     * @param indices
     */
    void addVariable(const Variable & variable,
        Numerical::Double costCoefficient,
        unsigned int nonzeros,
        const Numerical::Double * values,
        const unsigned int * indices);

    /**
     * 
     * @param variable
     * @param costCoefficient
     * @param vector
     */
    void addVariable(const Variable & variable,
        Numerical::Double costCoefficient,
        const Vector & vector);

    /**
     * 
     * @param variable
     * @param costCoefficient
     * @param nonzeros
     * @param ...
     */
    void addVariable(const Variable & variable,
        Numerical::Double costCoefficient,
        unsigned int nonzeros,
        ...);

    /**
     * 
     * @param index
     * @param constraint
     */
    void setConstraint(unsigned int index, const Constraint & constraint);

    /**
     * 
     * @param constraint
     * @param nonzeros
     * @param values
     * @param indices
     */
    void addConstraint(const Constraint & constraint,
        unsigned int nonzeros,
        const Numerical::Double * values,
        const unsigned int * indices);

    /**
     * 
     * @param constraint
     * @param vector
     */
    void addConstraint(const Constraint & constraint,
        const Vector & vector);

    /**
     * 
     * @param constraint
     * @param nonzeros
     * @param ...
     */
    void addConstraint(const Constraint & constraint,
        unsigned int nonzeros,
        ...);

    /**
     * 
     * @param index
     * @param variable
     */
    void setVariable(unsigned int index, const Variable & variable);

    /************************************************
     * 
     * Interface for the Model
     * 
     * *********************************************/

    /**
     * 
     * @return 
     */
    unsigned int getVariableCount() const;

    /**
     * 
     * @return 
     */
    unsigned int getConstraintCount() const;

    /**
     * 
     * @param index
     * @return 
     */
    const Variable & getVariable(unsigned int index) const;

    /**
     * 
     * @param index
     * @return 
     */
    const Constraint & getConstraint(unsigned int index) const;

    /**
     * 
     * @param index
     * @param rowVector
     * TODO: buildRow
     */
    void rowBuild(unsigned int index, Vector * rowVector) const;

    /**
     * 
     * @param index
     * @param columnVector
     */
    void columnBuild(unsigned int index, Vector * columnVector) const;

    /**
     * 
     * @param costVector
     */
    void costVectorBuild(Vector * costVector) const;

    /**
     * 
     * @return 
     */
    Numerical::Double getObjectiveConstant() const;

    /**
     * 
     * @return 
     */
    std::string getName() const;

private:

    struct IndexValuePair
    {
        Numerical::Double m_value;
        unsigned int m_index;

        bool operator==(const IndexValuePair & pair) const
        {
            if (m_value != pair.m_value) {
                return false;
            }
            if (m_index != pair.m_index) {
                return false;
            }
            return true;
        }
    };

    /**
     * Model name.
     */
    std::string m_name;

    /**
     * Constant of the objective function
     */
    Numerical::Double m_objectiveConstant;

    /**
     * List of the variables
     */
    std::vector<Variable> m_variables;

    /**
     * List of the constraints
     */
    std::vector<Constraint> m_constraints;

    /**
     * Vector of the coefficients of the objective function
     */
    std::vector<Numerical::Double> m_costVector;

    /**
     */
    std::vector< std::list<IndexValuePair> > m_columns;

    /**
     */
    std::vector<unsigned int> m_nonZerosInColumns;

    /**
     */
    std::vector< std::list<IndexValuePair> > m_rows;

    /**
     */
    std::vector<unsigned int> m_nonZerosInRows;

    /**
     * 
     * @param value
     * @param index
     * @return 
     */
    static IndexValuePair createPair(Numerical::Double value, unsigned int index)
    {
        IndexValuePair pair;
        pair.m_index = index;
        pair.m_value = value;
        return pair;
    }
};

#endif	/* MANUALBUILDER_H */

