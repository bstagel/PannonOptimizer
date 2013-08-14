/**
 * @file simplex.h
 */


#ifndef SIMPLEX_H
#define	SIMPLEX_H

#include "globals.h"

#include "lp/simplexmodel.h"
#include "utils/numerical.h"

class Simplex
{
public:
    Simplex();
    virtual ~Simplex();

    inline const Numerical::Double & getObjectiveValue() const {return m_objectiveValue;}
    inline const SimplexModel * getModel() const {return m_model;}


    inline void setModel(SimplexModel * model) {m_model = model;}

private:
    Numerical::Double m_objectiveValue;
    SimplexModel * m_model;

};

#endif /* SIMPLEX_H */
