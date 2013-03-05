/* 
 * File:   modelbuilder.h
 * Author: smidla
 *
 * Created on 2013. february 14., 12:25
 */

#ifndef MODELBUILDER_H
#define	MODELBUILDER_H

#include <lp/variable.h>
#include <lp/constraint.h>
#include <utils/numerical.h>
#include <linalg/vector.h>
#include <string>

/**
 */
class ModelBuilder
{
public:
    /**
     * 
     * @return 
     */
    virtual unsigned int getVariableCount() const = 0;

    /**
     * 
     * @return 
     */
    virtual unsigned int getConstraintCount() const = 0;

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
     * @param index
     * @param rowVector
     */
    virtual void buildRow(unsigned int index, Vector * rowVector) const = 0;

    /**
     * 
     * @param index
     * @param columnVector
     */
    virtual void buildColumn(unsigned int index, Vector * columnVector) const = 0;

    /**
     * 
     * @param costVector
     */
    virtual void buildCostVector(Vector * costVector) const = 0;

    /**
     * 
     * @return 
     */
    virtual Numerical::Double getObjectiveConstant() const = 0;
    
    /**
     * 
     * @return 
     */
    virtual std::string getName() const = 0;
    
    
};

#endif	/* MODELBUILDER_H */

