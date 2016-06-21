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
 * @file model.h This file contains the API of the Model class.
 */


#ifndef MODEL_H
#define	MODEL_H

#include <globals.h>

#include <vector>
#include <lp/variable.h>
#include <lp/constraint.h>
#include <lp/modelbuilder.h>
#include <lp/abstractmodel.h>
#include <linalg/densevector.h>
#include <linalg/matrix.h>
#include <lp/scaler.h>

/**
 * Describes a general LP model
 */
class Model : public AbstractModel
{
    friend class ModelTestSuite;
    friend class DualRatiotestTestSuite;
public:

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
    inline const DenseVector & getCostVector() const {return m_costVector;}
    inline const std::string & getName() const {return m_name;}
    inline const Numerical::Double & getCostConstant() const {return m_costConstant;}
    inline OBJECTIVE_TYPE getObjectiveType() const {return m_objectiveType;}
    inline const std::string & getObjectiveRowname() const {return m_objectiveRowName;}
    inline const std::vector<DenseVector*> * getSubstituteVectors() const {return m_substituteVectors;}

    virtual void addVariable(const Variable & variable, const SparseVector &column);
    virtual void addConstraint(const Constraint & constraint, const SparseVector & row);
    
    virtual void print(std::ostream& out = std::cout) const;

    void scale();

    std::string getHash() const;

    const std::vector<std::string> & getHistory() const {
        return m_history;
    }

    bool isScaled() const {
        return m_scaled;
    }

    bool isPresolved() const {
        return m_presolved;
    }

    const std::vector<Numerical::Double> & getRowMultipliers() const {
        return m_rowMultipliers;
    }

    const std::vector<Numerical::Double> & getColumnMultipliers() const {
        return m_columnMultipliers;
    }

    /********************************************************************
     * PRESOLVER API
     *******************************************************************/

    void removeVariable(unsigned int index);

    void removeConstraint(unsigned int index);

    void addToConstraint(unsigned int dest, unsigned int source, Numerical::Double lambda);

    void addToCostVector(unsigned int source, Numerical::Double lambda);

    void addToCostCoefficient(unsigned int index, Numerical::Double value);

    void setCostConstant(Numerical::Double value);

    inline std::vector<Variable> * getVariables() {return &m_variables;}
    inline std::vector<Constraint> * getConstraints() {return &m_constraints;}

    void setSubstitueVectors(std::vector<DenseVector *> *substituteVectors);

    /********************************************************************
     * CLP Like API for integration purposes
     *******************************************************************/

    void changeRowName(int rowIndex, std::string& name);

    void changeColumnName(int columnIndex, std::string& name);

    void changeConstraintLower(const double* rowLower);

    void changeConstraintUpper(const double* rowUpper);

    void changeVariableLower(const double* colLower);

    void changeVariableUpper(const double* colUpper);

    void changeObjectiveCoefficients(const double* objCoefficients);

public:
    Matrix m_matrix;
    std::vector<Variable> m_variables;
    std::vector<Constraint> m_constraints;
    DenseVector m_costVector;
    Numerical::Double m_costConstant;
    std::string m_objectiveRowName;
    std::string m_name;

    OBJECTIVE_TYPE m_objectiveType;

    void clear();

    std::vector<std::string> m_history;

    std::vector<Numerical::Double> m_rowMultipliers;

    std::vector<Numerical::Double> m_columnMultipliers;

    bool m_presolved;
    std::vector<DenseVector*> * m_substituteVectors;

    bool m_scaled;

    // const getter functions can change this value
    // reason: it is unnecessary to compute in most cases
    mutable std::string m_hash;

};

#endif	/* MODEL_H */

