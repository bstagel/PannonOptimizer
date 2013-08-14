/**
 * @file simplexmodel.h
 */

#ifndef SIMPLEXMODEL_H
#define	SIMPLEXMODEL_H

#include "globals.h"

#include "lp/model.h"

class SimplexModel : public Model
{
public:

    SimplexModel();

    inline const Vector & getRhs() const {return m_rhs;}

    /**
     * Prints the output of the createComputationalForm (CF3) algorithm.
     * (The generated model with RHS and logical variables.)
     */
    void print(std::ostream out = std::cout) const;

private:
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

