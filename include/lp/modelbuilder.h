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
 * @file modelbuilder.h
 */


#ifndef MODELBUILDER_H
#define	MODELBUILDER_H

#include <globals.h>

#include <linalg/matrixbuilder.h>
#include <lp/variable.h>
#include <lp/constraint.h>
#include <utils/numerical.h>
#include <string>

/**
 */
class ModelBuilder: public MatrixBuilder
{
public:
   
    virtual ~ModelBuilder() {}

    /**
     * 
     * @param index
     * @return 
     */
    virtual const Variable & getVariable(unsigned int index) const = 0;

    /**
     * 
     * @param index
     * @return 
     */
    virtual const Constraint & getConstraint(unsigned int index) const = 0;

    /**
     * 
     * @param costVector
     */
    virtual void buildCostVector(DenseVector * costVector) const = 0;

    /**
     * 
     * @return 
     */
    virtual double getObjectiveConstant() const = 0;
    
    /**
     * 
     * @return 
     */
    virtual std::string getName() const = 0;
    
    /**
     */
    virtual OBJECTIVE_TYPE getObjectiveType() const = 0;
};

#endif	/* MODELBUILDER_H */

