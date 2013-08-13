/**
 * @file variable.h
 */


#ifndef VARIABLE_H
#define	VARIABLE_H

#include "globals.h"

#include <iostream>
#include <utils/numerical.h>
#include <utils/exceptions.h>
#include <linalg/vector.h>

class Variable;

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
     * The class expresses an exception about a variable. It contains
     * the copy of the wrong variable.
     */
    class VariableException : public Exception
    {
    public:
        /**
         * Constructor of the VariableException. It gets the original wrong
         * variable, and creates its copy. Moreover, it gets a message about
         * the error.
         * 
         * @param variable The wrong variable object.
         * @param message The human readable message about the error.
         */
        VariableException(const Variable & variable,
            const std::string & message);

        /**
         * Destructor of the VariableException.
         */
        virtual ~VariableException();

        /**
         * Returns with the address of copy of the wrong variable.
         * 
         * @return Address of the wrong variable.
         */
        const Variable * getVariable() const;
    private:

        /**
         * Address of the wrong variable.
         */
        Variable * m_variable;
    };

    /**
     * The class expresses an exception about a variable. This
     * exception indicates that the upper bound of the variable
     * is invalid. It contains the copy of the wrong variable.
     */
    class InvalidUpperBoundException : public VariableException
    {
    public:
        /**
         * Constructor of the InvalidUpperBoundException. It gets the original wrong
         * variable, and creates its copy. Moreover, it gets a message about
         * the error.
         * 
         * @param variable The wrong variable object.
         * @param message The human readable message about the error.
         */
        InvalidUpperBoundException(const Variable & variable,
            const std::string & message);
    };

    /**
     * The class expresses an exception about a variable. This
     * exception indicates that the lower bound of the variable
     * is invalid. It contains the copy of the wrong variable.
     */
    class InvalidLowerBoundException : public VariableException
    {
    public:
        /**
         * Constructor of the InvalidLowerBoundException. It gets the original wrong
         * variable, and creates its copy. Moreover, it gets a message about
         * the error.
         * 
         * @param variable The wrong variable object.
         * @param message The human readable message about the error.
         */
        InvalidLowerBoundException(const Variable & variable,
            const std::string & message);
    };

    /**
     * The class expresses an exception about a variable. This
     * exception indicates that the bounds of the variable are invalid. 
     * It contains the copy of the wrong variable.
     */
    class InvalidBoundsException : public VariableException
    {
    public:
        /**
         * Constructor of the InvalidBoundsException. It gets the original wrong
         * variable, and creates its copy. Moreover, it gets a message about
         * the error.
         * 
         * @param variable The wrong variable object.
         * @param message The human readable message about the error.
         */
        InvalidBoundsException(const Variable & variable,
            const std::string & message);
    };

    /**
     * Creates a PLUS type variable, with 0 lower and infinity upper
     * bound. The value is 0, and the variable has no name.
     * 
     */
    inline Variable();

    /**
     * Creates a plus type variable, with a given value and lower bound.
     * The upper bound will be + infinity. If the lowerBound is - infinity,
     * then the variable will be free type variable.
     * 
     * If the lowerBound is + infinity, the function throws an
     * Variable::InvalidLowerBoundException.
     * 
     * @param name The name of the variable
     * @param value The value of the variable
     * @param lowerBound The lower bound of the variable
     * @return The requested variable
     */
    static Variable createPlusTypeVariable(const char * name,
        Numerical::Double value,
        Numerical::Double lowerBound) throw (InvalidLowerBoundException);

    /**
     * Creates a minus type variable, with a given value and upper bound.
     * The lower bound will be -infinity. If the upperBound is +infinity,
     * then the variable will be free type variable.
     * 
     * If the upperBound is -infinity, the function throws an
     * Variable::InvalidUpperBoundException.
     * 
     * @param name The name of the variable
     * @param value The value of the variable
     * @param upperBound The upper bound of the variable
     * @return The requested variable 
     */
    static Variable createMinusTypeVariable(const char * name,
        Numerical::Double value,
        Numerical::Double upperBound);

    /**
     * Creates a bounded type variable, with a given value and bounds.
     * If the bounds are the same, the result will be a fixed type variable,
     * and if the bounds are not finite, the result can be plus, minus, or 
     * free type also.
     * 
     * If the upperBound is -infinity, the function throws a 
     * Variable::InvalidUpperBoundException.
     * If the lowerBound is infinity, the function throws a 
     * Variable::InvalidLowerBoundException.
     * If the upperBound is less than the lowerBound, the function throws a 
     * Variable::InvalidBoundsException.
     * 
     * @param name The name of the variable
     * @param value The value of the variable
     * @param lowerBound The lower bound of the variable
     * @param upperBound The upper bound of the variable
     * @return The requested variable 
     */
    static Variable createBoundedTypeVariable(const char * name,
        Numerical::Double value,
        Numerical::Double lowerBound,
        Numerical::Double upperBound);

    /**
     * Creates a fixed type variable, with a given value.
     *
     * @param name The name of the variable
     * @param value The value of the variable
     * @return The requested variable  
     */
    static Variable createFixedTypeVariable(const char * name,
        Numerical::Double value);

    /**
     * Creates a free type variable with a given value.
     * 
     * @param name The name of the variable
     * @param value The value of the variable
     * @return The requested variable  
     */
    static Variable createFreeTypeVariable(const char * name,
        Numerical::Double value);

    /**
     * Returns with the lower bound of the variable.
     * 
     * @return The lower bound of the variable.
     */
    inline Numerical::Double getLowerBound() const;

    /**
     * Sets the lower bound of the variable.
     * If the lower bound is invalid, the function throws a
     * Variable::InvalidLowerBoundException, and if the lower bound is
     * greater than the upper bound, the function throws a
     * Variable::InvalidBoundsException.
     * 
     * @param lowerBound The lower bound of the variable.
     */
    inline void setLowerBound(Numerical::Double lowerBound);

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
    inline TYPE getType() const;

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
     * @param variable
     * @return 
     */
    inline bool operator==(const Variable & variable) const;

    /**
     * 
     */
    friend std::ostream & operator<<(std::ostream & os, const Variable & var);

private:
    Numerical::Double m_lowerBound;

    Numerical::Double m_upperBound;

    Numerical::Double m_value;

    TYPE m_type;

    std::string m_name;

    const Vector * m_vector;

    /**
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

    inline void adjustType();

    /**
     * @param vector
     */
    inline void setVector(const Vector & vector);

    /**
     * Checks the properties of the variable, and if the object
     * violates some conditions, it throws an exception.
     */
    void check() const;
};

inline Variable::Variable()
{
    m_lowerBound = 0;
    m_upperBound = PInfinity;
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
    check();
    adjustType();
}

inline Numerical::Double Variable::getLowerBound() const
{
    return m_lowerBound;
}

inline void Variable::setLowerBound(Numerical::Double lowerBound)
{
    m_lowerBound = lowerBound;
    check();
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
    check();
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

inline void Variable::setVector(const Vector & vector)
{
    m_vector = &vector;
}

inline const Vector * Variable::getVector() const
{
    return m_vector;
}

inline bool Variable::operator==(const Variable & variable) const
{
    if (m_lowerBound != variable.m_lowerBound) {
        return false;
    }
    if (m_name != variable.m_name) {
        return false;
    }
    if (m_type != variable.m_type) {
        return false;
    }
    if (m_upperBound != variable.m_upperBound) {
        return false;
    }
    if (m_value != variable.m_value) {
        return false;
    }
    if (m_vector != variable.m_vector) {
        return false;
    }
    return true;
}

inline void Variable::adjustType()
{
    if (m_lowerBound == m_upperBound) {
        m_type = FIXED;
    } else if (m_lowerBound == -PInfinity) {
        if (m_upperBound == PInfinity) {
            m_type = FREE;
        } else {
            m_type = MINUS;
        }
    } else if (m_upperBound == PInfinity) {
        m_type = PLUS;
    } else {
        m_type = BOUNDED;
    }
}

#endif	/* VARIABLE_H */
