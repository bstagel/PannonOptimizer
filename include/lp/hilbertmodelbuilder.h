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

#ifndef HILBERTMODELBUILDER_H
#define HILBERTMODELBUILDER_H

#include <globals.h>

#include <lp/modelbuilder.h>
#include <linalg/densevector.h>
#include <linalg/sparsevector.h>
#include <list>
#include <vector>

class HilbertModelBuilder : public ModelBuilder
{
    friend class HilbertModelBuilderTestSuite;
public:
    HilbertModelBuilder(unsigned int size = 0);

    /************************************************
     *
     * Interface for the Hilbert model construction
     *
     * *********************************************/

    void setSize(unsigned int size);

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

    unsigned int m_size;

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
     * Vector of the coefficients of the objective function
     */
    std::vector<double> m_costVector;

    std::vector<Variable> m_variables;

    std::vector<Constraint> m_constraints;

    mutable std::vector<double> m_tempData;

};
#endif // HILBERTMODELBUILDER_H
