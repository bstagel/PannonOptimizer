//=================================================================================================
/*!
//  This file is part of the Pannon Optimizer library. 
//  This library is free software; you can redistribute it and/or modify it under the 
//  terms of the GNU Lesser General Public License as published by the Free Software 
//  Foundation; either version 3.0, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
//  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//  See the GNU General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License; see the file 
//  COPYING. If not, see http://www.gnu.org/licenses/.
*/
//=================================================================================================

/**
 * @file manualmodelbuilder.h
 */


#ifndef MANUALBUILDER_H
#define	MANUALBUILDER_H

#include <globals.h>

#include <lp/modelbuilder.h>
#include <linalg/densevector.h>
#include <linalg/sparsevector.h>
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
    void setObjectiveFunctionConstant(double value);

    /**
     * 
     * @param variable
     * @param costCoefficient
     * @param nonzeros
     * @param values
     * @param indices
     */
    void addVariable(const Variable & variable,
        double costCoefficient,
        unsigned int nonzeros,
        const double * values,
        const unsigned int * indices);

    /**
     * 
     * @param variable
     * @param costCoefficient
     * @param vector
     */
    void addVariable(const Variable & variable,
        double costCoefficient,
        const SparseVector & vector);

    /**
     * 
     * @param variable
     * @param costCoefficient
     * @param nonzeros
     * @param ...
     */
    void addVariable(const Variable & variable,
        double costCoefficient,
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
                       const double * values,
                       const unsigned int * indices,
                       unsigned int nonzeros,
                       Variable * defaultVariablePtr = nullptr
                       );

    /**
     * 
     * @param constraint
     * @param vector
     */
    void addConstraint(const Constraint & constraint,
        const SparseVector & vector);

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

    /**
     * 
     * @param index
     * @param cost
     */
    void setCostCoefficient(unsigned int index, double cost);

    /**
     * 
     * @param type
     */
    void setObjectiveType(OBJECTIVE_TYPE type);
    
    /************************************************
     * 
     * Interface for the Model
     * 
     * *********************************************/

    /**
     * 
     * @return 
     */
    unsigned int getColumnCount() const;

    /**
     * 
     * @return 
     */
    unsigned int getRowCount() const;

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
     */
    void buildRow(unsigned int index, SparseVector * rowVector,
        std::vector<unsigned int> * nonzeros) const;

    /**
     * 
     * @param index
     * @param columnVector
     */
    void buildColumn(unsigned int index, SparseVector * columnVector,
        std::vector<unsigned int> * nonzeros) const;

    /**
     * 
     * @param costVector
     */
    void buildCostVector(DenseVector * costVector) const;

    /**
     * 
     * @return 
     */
    double getObjectiveConstant() const;

    /**
     * 
     * @return 
     */
    std::string getName() const;

    /**
     * 
     * @return 
     */
    bool hasRowwiseRepresentation() const;

    /**
     * 
     * @return 
     */
    bool hasColumnwiseRepresentation() const;

    /**
     * 
     * @return 
     */
    OBJECTIVE_TYPE getObjectiveType() const;
    
private:

    struct IndexValuePair
    {
        double m_value;
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

        friend std::ostream& operator<<(std::ostream& os, const IndexValuePair& pair){
            os << "[idx: " <<pair.m_index << " : " << pair.m_value << "]";
            return os;
        }
    };

    OBJECTIVE_TYPE m_objectiveType;
    
    /**
     * Model name.
     */
    std::string m_name;

    /**
     * Constant of the objective function
     */
    double m_objectiveConstant;

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
    std::vector<double> m_costVector;

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
    static IndexValuePair createPair(double value, unsigned int index)
    {
        IndexValuePair pair;
        pair.m_index = index;
        pair.m_value = value;
        return pair;
    }
};

#endif	/* MANUALBUILDER_H */

