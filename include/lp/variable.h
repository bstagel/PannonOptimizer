/* 
 * File:   variable.h
 * Author: smidla
 *
 * Created on 2011. februar 9., 17:45
 */

#ifndef VARIABLE_H
#define	VARIABLE_H

#include <iostream>
#include <utils/numerical.h>
#include <linalg/vector.h>

/**
 * Describes a variable
 */
class Variable
{
    friend class VariableTestSuite;
    friend class Model;
public:

    /**
     * 
     */
    enum TYPE
    {
        PLUS, /* Finite lower bound, infinity upper bound */
        MINUS, /* -Infinity lower bound, finite upper bound */
        BOUNDED, /* Finite lower and upper bounds */
        FIXED, /* The finite lower and upper bounds are the same */
        FREE /* The variable has no finite bounds */
    };

    /**
     * Creates a PLUS type variable, with 0 lower and infinity upper
     * bound. The value is 0, and the variable has no name.
     * 
     */
    inline Variable();

    /**
     * Creates a plus type variable, with a given value and lower bound.
     * 
     * @param name The name of the variable
     * @param value
     * @param lowerBound
     * @return 
     */
    static Variable createPlusTypeVariable(const char * name,
        Numerical::Double value,
        Numerical::Double lowerBound);

    /**
     * 
     * @param name
     * @param value
     * @param upperBound
     * @return 
     */
    static Variable createMinusTypeVariable(const char * name,
        Numerical::Double value,
        Numerical::Double upperBound);

    /**
     * 
     * @param name
     * @param value
     * @param lowerBound
     * @param upperBound
     * @return 
     */
    static Variable createBoundedTypeVariable(const char * name,
        Numerical::Double value,
        Numerical::Double lowerBound,
        Numerical::Double upperBound);

    /**
     * 
     * @param name
     * @param value
     * @return 
     */
    static Variable createFixedTypeVariable(const char * name,
        Numerical::Double value);

    /**
     * 
     * @param name
     * @param value
     * @return 
     */
    static Variable createFreeTypeVariable(const char * name,
        Numerical::Double value);

    /**
     *
     * @return
     */
    inline Numerical::Double getLowerBound() const;

    /**
     *
     * @param lowerBound
     */
    inline void setLowerBound(Numerical::Double lowerBound);

    /**
     *
     * @return
     */
    inline TYPE getType() const;

    /**
     *
     * @return
     */
    inline Numerical::Double getUpperBound() const;

    /**
     *
     * @param upperBound
     */
    inline void setUpperBound(Numerical::Double upperBound);

    /**
     *
     * @return
     */
    inline Numerical::Double getValue() const;

    /**
     *
     * @param value
     */
    inline void setValue(Numerical::Double value);

    /**
     *
     * @return
     */
    inline void setName(const char * name);

    /**
     *
     * @return
     */
    inline const char * getName() const;

    /**
     * 
     * @return 
     */
    inline const Vector * getVector() const;

    /**
     * 
     */
    friend std::ostream & operator<<(std::ostream & os, const Variable & var);

private:
    /**
     * @author Jozsef Smidla <smidla@dcs.uni-pannon.hu>
     */
    Numerical::Double m_lowerBound;

    /**
     * @author Jozsef Smidla <smidla@dcs.uni-pannon.hu>
     */
    Numerical::Double m_upperBound;

    /**
     * @author Jozsef Smidla <smidla@dcs.uni-pannon.hu>
     */
    Numerical::Double m_value;

    /**
     * @author Jozsef Smidla <smidla@dcs.uni-pannon.hu>
     */
    TYPE m_type;

    /**
     * @author Jozsef Smidla <smidla@dcs.uni-pannon.hu>
     */
    std::string m_name;

    /**
     */
    const Vector * m_vector;

    /**
     * 
     * @param lowerBound
     * @param upperBound
     * @param value
     * @param name
     * @param vector
     */
    inline Variable(Numerical::Double lowerBound,
        Numerical::Double upperBound,
        Numerical::Double value,
        const char * name);

    /**
     * 
     */
    inline void adjustType();
    
    /**
     * 
     * @param vector
     */
    inline void setVector(const Vector & vector);   

};

inline Variable::Variable()
{
    m_lowerBound = 0;
    m_upperBound = infinity;
    m_value = 0;
    m_type = PLUS;
    m_vector = 0;
}

inline Variable::Variable(Numerical::Double lowerBound,
    Numerical::Double upperBound,
    Numerical::Double value,
    const char * name)
{
    m_lowerBound = lowerBound;
    m_upperBound = upperBound;
    m_value = value;
    if (name != 0) {
        m_name = name;
    }
    m_vector = 0;
    adjustType();
}

inline Numerical::Double Variable::getLowerBound() const
{
    return m_lowerBound;
}

inline void Variable::setLowerBound(Numerical::Double lowerBound)
{
    m_lowerBound = lowerBound;
    adjustType();
}

inline Variable::TYPE Variable::getType() const
{
    return m_type;
}

inline Numerical::Double Variable::getUpperBound() const
{
    return m_upperBound;
}

inline void Variable::setUpperBound(Numerical::Double upperBound)
{
    m_upperBound = upperBound;
    adjustType();
}

inline Numerical::Double Variable::getValue() const
{
    return m_value;
}

inline void Variable::setValue(Numerical::Double value)
{
    m_value = value;
}

inline void Variable::setName(const char * name)
{
    m_name = name;
}

inline const char * Variable::getName() const
{
    if (m_name.length() > 0) {
        return m_name.c_str();
    }
    return "<NO NAME>";
}

inline void Variable::setVector(const Vector & vector) {
    m_vector = &vector;
}

inline const Vector * Variable::getVector() const
{
    return m_vector;
}

inline void Variable::adjustType()
{
    if (m_lowerBound == m_upperBound) {
        m_type = FIXED;
    } else if (m_lowerBound == -infinity) {
        if (m_upperBound == infinity) {
            m_type = FREE;
        } else {
            m_type = MINUS;
        }
    } else if (m_upperBound == infinity) {
        m_type = PLUS;
    } else {
        m_type = BOUNDED;
    }
}

#endif	/* VARIABLE_H */
