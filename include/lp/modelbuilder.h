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
#include <linalg/vector.h>
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
    virtual Numerical::Double getObjectiveConstant() const = 0;
    
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

