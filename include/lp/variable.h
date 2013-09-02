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
     * Creates a variable, with a given value and bounds.
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
     * @return 
     */
    static Variable createVariable(const char * name,
        Numerical::Double value,
        Numerical::Double lowerBound,
        Numerical::Double upperBound) throw (InvalidLowerBoundException, 
                                             InvalidUpperBoundException,
                                             InvalidBoundsException);

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
        Numerical::Double upperBound) throw (InvalidUpperBoundException);

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
        Numerical::Double upperBound) throw (InvalidLowerBoundException, 
                                             InvalidUpperBoundException,
                                             InvalidBoundsException);

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
    inline void setLowerBound(Numerical::Double lowerBound) throw (InvalidLowerBoundException,
                                                                   InvalidBoundsException);

    /**
     * Returns with the upper bound of the variable.
     *
     * @return The upper bound of the variable.
     */
    inline Numerical::Double getUpperBound() const;

    /**
     * Sets the upper bound of the variable.
     * If the upper bound is invalid, the function throws a
     * Variable::InvalidUpperBoundException, and if the lower bound is
     * greater than the upper bound, the function throws a
     * Variable::InvalidBoundsException.
     *
     * @param upperBound The upper bound of the variable.
     */
    inline void setUpperBound(Numerical::Double upperBound) throw (InvalidUpperBoundException,
                                                                   InvalidBoundsException);

    /**
     * Returns with the type of the variable.
     *
     * @return The type of the variable.
     */
    inline TYPE getType() const;

    /**
     * Returns with the value of the variable.
     *
     * @return The value of the variable.
     */
    inline Numerical::Double getValue() const;

    /**
     * Sets the value of the variable.
     *
     * @param value The variable's new value.
     */
    inline void setValue(Numerical::Double value);

    /**
     * Sets the variable's name.
     *
     * @param name The variable's new name.
     */
    inline void setName(const char * name);

    /**
     * Returns with the name of the variable.
     *
     * @return The variable's name.
     */
    inline const char * getName() const;

    /**
     * Returns with the pointer of the corresponding vector.
     *
     * @return The pointer of the corresponding vector.
     */
    inline const Vector * getVector() const;

    /**
     * Returns with true when each data members are equal,
     * otherwise false.
     *
     * @return The equality of the two variables.
     */
    inline bool operator==(const Variable & variable) const;

    /**
     * Writes the properties of the variable to the ostream
     * object.
     *
     * @return The reference of the ostream object.
     */
    friend std::ostream & operator<<(std::ostream & os, const Variable & var);

private:
    /**
     * Represents the lower bound of the variable. It can be - infinity also.
     * When a function sets this variable to + infinity, the function throws
     * and InvalidLowerBoundException.
     */
    Numerical::Double m_lowerBound;

    /**
     * Represents the upper bound of the variable. It can be + infinity also.
     * When a function sets this variable to - infinity, the function throws
     * and InvalidUpperBoundException.
     */
    Numerical::Double m_upperBound;

    /**
     * The value of the variable. It can be lower then the lower bound or
     * greater than the upper bound.
     */
    Numerical::Double m_value;

    /**
     * Represents the type of the variable. It is modified by the adjustType()
     * function.
     */
    TYPE m_type;

    /**
     * Represents the name of the variable. The default name is <NO NAME>.
     */
    std::string m_name;

    /**
     * Represents the coefficient vector of the variable. The class Model fills
     * this value.
     */
    const Vector * m_vector;

    /**
     * General constructor of the class. It creates a variable with the given
     * properties, and throws exception when the bounds are invalid. This is the
     * helper function of the public variable creator functions.
     *
     * @param lowerBound The lower bound of the variable.
     * @param upperBound The upper bound of the variable.
     * @param name The name of the variable.
     */
    inline Variable(Numerical::Double lowerBound,
        Numerical::Double upperBound,
        Numerical::Double value,
        const char * name) throw (InvalidLowerBoundException,
                                  InvalidUpperBoundException,
                                  InvalidBoundsException);

    /**
     * This function adjustes the m_type variable considering the m_lowerBound
     * and m_upperBound. It supposes that the bounds are correct.
     */
    inline void adjustType();

    /**
     * Sets the pointer of the corresponding coefficient vector. This is a private
     * function, because only the friend classes can call it: The class Model
     * uses this function when builds the model.
     *
     * @param vector The vector of the coefficient values.
     */
    inline void setVector(const Vector & vector);

    /**
     * Checks the validity of the bounds. If the lower bound is + infinity, it
     * thorws an InvalidLowerBoundException, if the upper bound is - infinity, it
     * throws an InvalidUpperBoundException, and if the lower bound is greater than
     * the upper bound, it throws an InvalidBoundsException.
     */
    void check() const throw (InvalidLowerBoundException,
                              InvalidUpperBoundException,
                              InvalidBoundsException);
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
    const char * name) throw (InvalidLowerBoundException,
                              InvalidUpperBoundException,
                              InvalidBoundsException)
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

inline void Variable::setLowerBound(Numerical::Double lowerBound) throw (InvalidLowerBoundException,
                                                                         InvalidBoundsException)
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

inline void Variable::setUpperBound(Numerical::Double upperBound) throw (InvalidUpperBoundException,
                                                                         InvalidBoundsException)
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
